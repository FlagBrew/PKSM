#include "lodepng.h"
#include "screen.h"
#include "vshader_shbin.h"
	
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	 GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	 GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

#define TEXT_VTX_ARRAY_COUNT (4*1024)

typedef struct { float position[3]; float texcoord[2]; } textVertex_s;

static bool c3d_initialized;

static bool shader_initialized;
static DVLB_s * vshader_dvlb;
static shaderProgram_s program;

static int uLoc_projection;
static int uLoc_transform;
static int uLoc_use_transform;

static C3D_RenderTarget * target_top;
static C3D_RenderTarget * target_bottom;

static C3D_Mtx projectionTop;
static C3D_Mtx projectionBot;

static C3D_Tex * glyphSheets;
static textVertex_s* textVtxArray;
static int textVtxArrayPos;

static u8 base_alpha = 0xFF;

static struct 
{
	bool allocated;
	C3D_Tex tex;
	u32 width;
	u32 height;
} textures[MAX_TEXTURES];

typedef struct 
{
	float x;
	float y;
	float z;
} vertex;

typedef struct 
{
	vertex position;
	u32 color;
} vertexData;

static void * mempool_addr = NULL;
static u32 mempool_index = 0;
static u32 mempool_size = 0;

static void screen_reset_mempool()
{
	mempool_index = 0;
}

void screen_reset_text_vtx_array_pos() {
	textVtxArrayPos = 0;
}

static void screen_set_blend(u32 color, bool rgb, bool alpha) 
{
	C3D_TexEnv * env = C3D_GetTexEnv(0);
	
	if(env == NULL)
		return;

	if(rgb) 
	{
		C3D_TexEnvSrc(env, C3D_RGB, GPU_CONSTANT, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
	} 
	else 
	{
		C3D_TexEnvSrc(env, C3D_RGB, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
	}

	if(alpha) 
	{
		C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, GPU_CONSTANT, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
	} 
	else 
	{
		C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
	}

	C3D_TexEnvColor(env, color);
}

void screen_init(void) 
{	
	// Initialize Citro3D
	if(C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 16))
		c3d_initialized = true;
	
	// Initialize the render targets
	target_top = C3D_RenderTargetCreate(SCREEN_HEIGHT, TOP_SCREEN_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_FrameBufClear(&target_top->frameBuf, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
	C3D_RenderTargetSetOutput(target_top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	target_bottom = C3D_RenderTargetCreate(SCREEN_HEIGHT, BOTTOM_SCREEN_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_FrameBufClear(&target_bottom->frameBuf, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
	C3D_RenderTargetSetOutput(target_bottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	
	mempool_addr = linearAlloc(0x80000);
	mempool_size = 0x80000;
	
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_len);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);
	
	// Get the location of the uniforms
	uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	uLoc_transform = shaderInstanceGetUniformLocation(program.vertexShader, "transform");
	uLoc_use_transform = shaderInstanceGetUniformLocation(program.vertexShader, "useTransform");

	// Configure attributes for use with the vertex shader
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0 = position
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v2 = texcoord
	
	// Compute the projection matrix (top and bottom screens)
	Mtx_OrthoTilt(&projectionTop, 0.0, TOP_SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 0.0, 1.0, true);
	Mtx_OrthoTilt(&projectionBot, 0.0, BOTTOM_SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 0.0, 1.0, true);

	C3D_CullFace(GPU_CULL_NONE);
	
	// Configure depth test to overwrite pixels with the same depth (needed to draw overlapping sprites)
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
	
	// Update the uniforms
	C3D_BoolUnifSet(GPU_VERTEX_SHADER, uLoc_use_transform, false);
	
	screen_set_blend(0, false, false);
	
	Result fontMapRes = fontEnsureMapped();
	if(R_FAILED(fontMapRes))
		return;

	// Load the glyph texture sheets
	int i;
	TGLP_s* glyphInfo = fontGetGlyphInfo();
	glyphSheets = malloc(sizeof(C3D_Tex) * glyphInfo->nSheets);
	for (i = 0; i < glyphInfo->nSheets; i ++)
	{
		C3D_Tex * tex = &glyphSheets[i];
		tex->data = fontGetGlyphSheetTex(i);
		tex->fmt = glyphInfo->sheetFmt;
		tex->size = glyphInfo->sheetSize;
		tex->width = glyphInfo->sheetWidth;
		tex->height = glyphInfo->sheetHeight;
		tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR) | GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_EDGE) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_EDGE);
		tex->border = 0;
		tex->lodParam = 0;
	}
	
	// Create the text vertex array
	textVtxArray = (textVertex_s*)linearAlloc(sizeof(textVertex_s)*TEXT_VTX_ARRAY_COUNT);
	
	screen_reset_mempool();
}

void screen_exit(void) 
{
	for(u32 id = 0; id < MAX_TEXTURES; id++)
        screen_unload_texture(id);

	if(glyphSheets != NULL) 
	{
		free(glyphSheets);
		glyphSheets = NULL;
	}

	if(shader_initialized) 
	{
		shaderProgramFree(&program);
		shader_initialized = false;
	}

	if(vshader_dvlb != NULL) 
	{
		DVLB_Free(vshader_dvlb);
		vshader_dvlb = NULL;
	}

	if(target_top != NULL) 
	{
		C3D_RenderTargetDelete(target_top);
		target_top = NULL;
	}

	if(target_bottom != NULL) 
	{
		C3D_RenderTargetDelete(target_bottom);
		target_bottom = NULL;
	}

	if(c3d_initialized) 
	{
		C3D_Fini();
		c3d_initialized = false;
	}
}

void screen_clear(gfxScreen_t screen, u32 color)
{	
	color = ((color>>24)&0x000000FF) | ((color>>8)&0x0000FF00) | ((color<<8)&0x00FF0000) | ((color<<24)&0xFF000000); // reverse byte order
	
	if (screen == GFX_TOP)
		C3D_RenderTargetSetClear(target_top, C3D_CLEAR_ALL, color, 0);
	else
		C3D_RenderTargetSetClear(target_bottom, C3D_CLEAR_ALL, color, 0);
}

void screen_set_base_alpha(u8 alpha) 
{
	base_alpha = alpha;
}

static u32 screen_next_pow_2(u32 i) 
{
	i--;
	i |= i >> 1;
	i |= i >> 2;
	i |= i >> 4;
	i |= i >> 8;
	i |= i >> 16;
	i++;

	return i;
}

u32 screen_allocate_free_texture(void) 
{
	u32 id = 0;
	for(u32 i = 1; i < MAX_TEXTURES; i++) 
	{
		if(!textures[i].allocated) 
		{
			textures[i].allocated = true;
			id = i;
			break;
		}
	}

	if(id == 0)
		return 0;

	return id;
}

static void screen_prepare_texture(u32* pow2WidthOut, u32* pow2HeightOut, u32 id, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter) 
{
	if(id >= MAX_TEXTURES) 
		return;

	u32 pow2Width = screen_next_pow_2(width);
	if(pow2Width < 64) 
		pow2Width = 64;
    

	u32 pow2Height = screen_next_pow_2(height);
	if(pow2Height < 64)
        pow2Height = 64;

	if(textures[id].tex.data != NULL && ((textures[id].tex.width != pow2Width) || (textures[id].tex.height != pow2Height) || (textures[id].tex.fmt != format))) 
	{
		C3D_TexDelete(&textures[id].tex);
		textures[id].tex.data = NULL;
	}

	if(textures[id].tex.data == NULL && !C3D_TexInit(&textures[id].tex, (u16) pow2Width, (u16) pow2Height, format)) 
		return;

	C3D_TexSetFilter(&textures[id].tex, linearFilter ? GPU_LINEAR : GPU_NEAREST, GPU_NEAREST);

	textures[id].allocated = true;
	textures[id].width = width;
	textures[id].height = height;

	if(pow2WidthOut != NULL)
		*pow2WidthOut = pow2Width;

	if(pow2HeightOut != NULL)
		*pow2HeightOut = pow2Height;
}

void screen_load_texture_tiled(u32 id, void* data, u32 size, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter) 
{
	u32 pow2Width = 0;
	u32 pow2Height = 0;
	screen_prepare_texture(&pow2Width, &pow2Height, id, width, height, format, linearFilter);

	if(width != pow2Width || height != pow2Height) 
	{
		u32 pixelSize = size / width / height;

		memset(textures[id].tex.data, 0, textures[id].tex.size);
		for(u32 y = 0; y < height; y += 8) 
		{
			u32 dstPos = y * pow2Width * pixelSize;
			u32 srcPos = y * width * pixelSize;

			memcpy(&((u8*) textures[id].tex.data)[dstPos], &((u8*) data)[srcPos], width * 8 * pixelSize);
		}
	} 
	else
		memcpy(textures[id].tex.data, data, textures[id].tex.size);

	C3D_TexFlush(&textures[id].tex);
}

void screen_load_texture_untiled(u32 id, void* data, u32 size, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter) 
{
	u32 pow2Width = 0;
	u32 pow2Height = 0;
	screen_prepare_texture(&pow2Width, &pow2Height, id, width, height, format, linearFilter);

	u32 pixelSize = size / width / height;

	memset(textures[id].tex.data, 0, textures[id].tex.size);
	for(u32 x = 0; x < width; x++) 
	{
		for(u32 y = 0; y < height; y++) 
		{
			u32 dstPos = ((((y >> 3) * (pow2Width >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * pixelSize;
			u32 srcPos = (y * width + x) * pixelSize;

			memcpy(&((u8*) textures[id].tex.data)[dstPos], &((u8*) data)[srcPos], pixelSize);
		}
	}

	C3D_TexFlush(&textures[id].tex);
}

void screen_load_texture_png(u32 id, const char* path, bool linearFilter) 
{
	if(id >= MAX_TEXTURES) 
		return;

	unsigned char * image;
	unsigned width = 0, height = 0;

	lodepng_decode32_file(&image, &width, &height, path);

	if(image == NULL)
		return;

	// lodepng outputs big endian rgba so we need to convert
	for(u32 x = 0; x < width; x++) 
	{
		for(u32 y = 0; y < height; y++) 
		{
			u32 pos = (y * width + x) * 4;

			u8 c1 = image[pos + 0];
			u8 c2 = image[pos + 1];
			u8 c3 = image[pos + 2];
			u8 c4 = image[pos + 3];

			image[pos + 0] = c4;
			image[pos + 1] = c3;
			image[pos + 2] = c2;
			image[pos + 3] = c1;
		}
	}

	screen_load_texture_untiled(id, image, (u32) (width * height * 4), (u32) width, (u32) height, GPU_RGBA8, linearFilter);

	free(image);
}

void screen_unload_texture(u32 id) 
{
	if(id >= MAX_TEXTURES)
		return;

	C3D_TexDelete(&textures[id].tex);

	textures[id].allocated = false;
	textures[id].width = 0;
	textures[id].height = 0;
}

void screen_get_texture_size(u32* width, u32* height, u32 id) 
{
	if(id >= MAX_TEXTURES)
		return;

	if(width)
        *width = textures[id].width;

	if(height)
		*height = textures[id].height;
}

void screen_begin_frame(void) 
{
	screen_reset_mempool();
	
	if(!C3D_FrameBegin(C3D_FRAME_SYNCDRAW))
		return;
	
	screen_reset_text_vtx_array_pos();
}

void screen_end_frame(void) 
{
	C3D_FrameEnd(0);
}

void screen_select(gfxScreen_t screen) 
{    
	if(!C3D_FrameDrawOn(screen == GFX_TOP ? target_top : target_bottom))
		return;

	// Get the location of the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, screen == GFX_TOP ? &projectionTop : &projectionBot);
}

static void screen_draw_quad(float x1, float y1, float x2, float y2, float left, float bottom, float right, float top) 
{
	C3D_ImmDrawBegin(GPU_TRIANGLE_STRIP);

	C3D_ImmSendAttrib(x1, y2, 0.5f, 0.0f);
	C3D_ImmSendAttrib(left, bottom, 0.0f, 0.0f);

	C3D_ImmSendAttrib(x2, y2, 0.5f, 0.0f);
	C3D_ImmSendAttrib(right, bottom, 0.0f, 0.0f);

	C3D_ImmSendAttrib(x1, y1, 0.5f, 0.0f);
	C3D_ImmSendAttrib(left, top, 0.0f, 0.0f);

	C3D_ImmSendAttrib(x2, y1, 0.5f, 0.0f);
	C3D_ImmSendAttrib(right, top, 0.0f, 0.0f);

	C3D_ImmDrawEnd();
}

void screen_draw_texture(u32 id, float x, float y) 
{
	if(id >= MAX_TEXTURES)
		return;

	if(textures[id].tex.data == NULL)
		return;
	
	if(base_alpha != 0xFF)
		screen_set_blend(base_alpha << 24, false, true);

	C3D_TexBind(0, &textures[id].tex);
	screen_draw_quad(x, y, x + textures[id].width, y + textures[id].height, 0, (float) (textures[id].tex.height - textures[id].height) / (float) textures[id].tex.height, (float) textures[id].width / (float) textures[id].tex.width, 1.0f);

	if(base_alpha != 0xFF)
		screen_set_blend(0, false, false);
}

void screen_draw_texture_part(u32 id, float x, float y, float xbegin, float ybegin, float w, float h) 
{
	if(id >= MAX_TEXTURES)
		return;

	if(textures[id].tex.data == NULL)
		return;
	
	if(base_alpha != 0xFF)
		screen_set_blend(base_alpha << 24, false, true);
	
	float left = ((float)textures[id].width - xbegin) / (float)textures[id].tex.width;
	float bottom = ((float)textures[id].tex.height - ybegin - h) / (float)textures[id].tex.height;
	float right = ((float)textures[id].width - xbegin - w) / (float)textures[id].tex.width;	
	float top = ((float)textures[id].tex.height - ybegin) / (float)textures[id].tex.height;
	
	C3D_TexBind(0, &textures[id].tex);
	screen_draw_quad(x, y, x + w, y + h, left, bottom, right, top);

	if(base_alpha != 0xFF)
		screen_set_blend(0, false, false);
}

void screen_draw_texture_crop(u32 id, float x, float y, float width, float height) 
{
	if(id >= MAX_TEXTURES)
		return;

	if(textures[id].tex.data == NULL) 
		return;

	if(base_alpha != 0xFF)
		screen_set_blend(base_alpha << 24, false, true);

	C3D_TexBind(0, &textures[id].tex);
	screen_draw_quad(x, y, x + width, y + height, 0, (float) (textures[id].tex.height - textures[id].height) / (float) textures[id].tex.height, width / (float) textures[id].tex.width, (textures[id].tex.height - textures[id].height + height) / (float) textures[id].tex.height);

	if(base_alpha != 0xFF)
		screen_set_blend(0, false, false);
}

int screen_get_texture_width(u32 id)
{
	if(textures[id].tex.data == NULL)
		return 0;
	
	return textures[id].width;
}

int screen_get_texture_height(u32 id)
{	
	if(textures[id].tex.data == NULL)
		return 0;
	
	return textures[id].height;
}

static void screen_get_string_size_internal(float * width, float * height, const char * text, float scaleX, float scaleY, bool oneLine, bool wrap, float wrapWidth) 
{
	float w = 0;
	float h = 0;
	float lineWidth = 0;

	if(text != NULL) 
	{
		h = scaleY * fontGetInfo()->lineFeed;

		const uint8_t* p = (const uint8_t*) text;
		const uint8_t* lastAlign = p;
		u32 code = 0;
		ssize_t units = -1;
		while(*p && (units = decode_utf8(&code, p)) != -1 && code > 0) 
		{
			p += units;

			if((code == '\n') || (wrap && lineWidth + scaleX * fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(code))->charWidth >= wrapWidth)) 
			{
				lastAlign = p;

				if(lineWidth > w)
					w = lineWidth;

				lineWidth = 0;

				if(oneLine)
					break;

				h += scaleY * fontGetInfo()->lineFeed;
			}

			if(code != '\n') 
			{
				u32 num = 1;
				if(code == '\t') 
				{
					code = ' ';
					num = 4 - (p - units - lastAlign) % 4;

					lastAlign = p;
				}

				lineWidth += (scaleX * fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(code))->charWidth) * num;
			}
		}
	}

	if(width)
		*width = lineWidth > w ? lineWidth : w;

	if(height)
		*height = h;
}

void screen_get_string_size(float * width, float * height, const char * text, float scaleX, float scaleY) 
{
	screen_get_string_size_internal(width, height, text, scaleX, scaleY, false, false, 0);
}

void screen_get_string_size_wrap(float * width, float * height, const char * text, float scaleX, float scaleY, float wrapWidth) 
{
	screen_get_string_size_internal(width, height, text, scaleX, scaleY, false, true, wrapWidth);
}

float screen_get_string_width(const char * text, float scaleX, float scaleY)
{
	float width = 0.0;
	screen_get_string_size(&width, NULL, text, scaleX, scaleY);
	
	return width;
}

float screen_get_string_height(const char * text, float scaleX, float scaleY)
{
	float height = 0.0;
	screen_get_string_size(NULL, &height, text, scaleX, scaleY);
	
	return height;
}

static void setTextColor(u32 color)
{
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_RGB, GPU_CONSTANT, 0, 0);
	C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, GPU_CONSTANT, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
	C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
	C3D_TexEnvColor(env, color);
}

static void screen_draw_string_internal(const char * text, float x, float y, float scaleX, float scaleY, u32 color, bool wrap, bool baseline, float wrapX) 
{
	if(text == NULL)
		return;
	
	setTextColor(color);

	float stringWidth;
	screen_get_string_size_internal(&stringWidth, NULL, text, scaleX, scaleY, false, wrap, wrapX - x);

	float lineWidth;
	screen_get_string_size_internal(&lineWidth, NULL, text, scaleX, scaleY, true, wrap, wrapX - x);

	ssize_t  units;
	uint32_t code;
	
	const uint8_t * p = (const uint8_t *)text;
	const uint8_t * lastAlign = p;
	float firstX = x;
	u32 flags = GLYPH_POS_CALC_VTXCOORD | (baseline ? GLYPH_POS_AT_BASELINE : 0);
	int lastSheet = -1;
	
	do
	{
		if (!*p) 
			break;
		
		units = decode_utf8(&code, p);
		
		if (units == -1)
			break;
		
		p += units;

		if((code == '\n') || (wrap && firstX + scaleX * fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(code))->charWidth >= wrapX)) 
		{
			lastAlign = p;

			screen_get_string_size_internal(&lineWidth, NULL, (const char*) p, scaleX, scaleY, true, wrap, wrapX - x);
			
			firstX = x;
			y += scaleY*fontGetInfo()->lineFeed;
		}

		if(code != '\n') 
		{
			u32 num = 1;
			
			if(code == '\t') 
			{
				code = ' ';
				num = 4 - (p - units - lastAlign) % 4;

				lastAlign = p;
			}

			fontGlyphPos_s data;
			fontCalcGlyphPos(&data, fontGlyphIndexFromCodePoint(code), flags, scaleX, scaleY);

			if(data.sheetIndex != lastSheet) 
			{
				lastSheet = data.sheetIndex;
				C3D_TexBind(0, &glyphSheets[lastSheet]);
			}

			for(u32 i = 0; i < num; i++) 
			{
				screen_draw_quad(firstX + data.vtxcoord.left, y + data.vtxcoord.top, firstX + data.vtxcoord.right, y + data.vtxcoord.bottom, data.texcoord.left, data.texcoord.bottom, data.texcoord.right, data.texcoord.top);

				firstX += data.xAdvance;
			}
		}
	} while (code > 0);
	
	screen_set_blend(0, false, false);
}

void screen_draw_string(float x, float y, float scaleX, float scaleY, u32 color, const char * text) 
{
	screen_draw_string_internal(text, x, y, scaleX, scaleY, color, false, false, 0);
}

void screen_draw_stringf(float x, float y, float scaleX, float scaleY, u32 color, const char * text, ...) 
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	screen_draw_string_internal(buffer, x, y, scaleX, scaleY, color, false, false, 0);
	va_end(args);
}

void screen_draw_string_wrap(float x, float y, float scaleX, float scaleY, u32 color, float wrapX, const char * text) 
{
	screen_draw_string_internal(text, x, y, scaleX, scaleY, color, true, false, wrapX);
}

void screen_draw_string_center(bool isTop, float y, float scaleX, float scaleY, u32 color, const char * text) 
{
	float width = 0;
	screen_get_string_size(&width, NULL, text, scaleX, scaleY);
	screen_draw_string_internal(text, ((isTop ? 400 : 320) - width) / 2, y, scaleX, scaleY, color, false, false, 0);
}

static void * screen_pool_memalign(u32 size, u32 alignment)
{
	u32 new_index = (mempool_index + alignment - 1) & ~(alignment - 1);
	
	if ((new_index + size) < mempool_size) 
	{
		void *addr = (void *)((u32)mempool_addr + new_index);
		mempool_index = new_index + size;
		return addr;
	}
	
	return NULL;
}

static void setupVertices(vertexData * vertices) 
{
	C3D_TexEnv * env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

	C3D_AttrInfo * attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
	AttrInfo_AddLoader(attrInfo, 1, GPU_UNSIGNED_BYTE, 4);

	C3D_BufInfo * bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, vertices, sizeof(vertexData), 2, 0x10);
}

void screen_draw_rect(float x, float y, float width, float height, u32 color)
{
	vertexData * vertices = (vertexData *)screen_pool_memalign(4 * sizeof(vertexData), 8);
	
	if (!vertices) 
		return;

	vertices[0].position = (vertex){x, y, 0.5f};
	vertices[1].position = (vertex){x + width, y, 0.5f};
	vertices[2].position = (vertex){x, y + height, 0.5f};
	vertices[3].position = (vertex){x + width, y + height, 0.5f};
	
	for (int i = 0; i <= 3; i++)
		vertices[i].color = color;

	setupVertices(vertices);

	C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
	
	screen_set_blend(color, false, false);
}

void screen_set_text_vertex_array_pos(u32 pos) {
	textVtxArrayPos = pos;
}

static void screen_draw_wstring_internal(const wchar_t * wtext, float x, float y, float scaleX, float scaleY, u32 color, bool wrap, bool baseline, float wrapX) 
{
	if (wtext == NULL)
		return;
	
	u32 size = wcslen(wtext) * sizeof(wchar_t);
	char text[size];
	memset(text, 0, size);
	utf32_to_utf8((uint8_t*)text, (uint32_t*)wtext, size);
	text[size - 1] = '\0';
	
	screen_draw_string(x, y, scaleX, scaleY, color, text);
}

void screen_draw_wstring(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t * wtext) 
{
	screen_draw_wstring_internal(wtext, x, y, scaleX, scaleY, color, false, false, 0);
}

static void screen_get_wstring_size_internal(float * w, float * h, const wchar_t * wtext, float scaleX, float scaleY, bool oneLine, bool wrap, float wrapWidth) 
{
	u32 size = wcslen(wtext) * sizeof(wchar_t);
	char text[size];
	memset(text, 0, size);
	utf32_to_utf8((uint8_t*)text, (uint32_t*)wtext, size);
	text[size - 1] = '\0';

	screen_get_string_size_internal(w, h, text, scaleX, scaleY, oneLine, wrap, wrapWidth);
}

void screen_get_wstring_size(float * width, float * height, const wchar_t * text, float scaleX, float scaleY) 
{
	screen_get_wstring_size_internal(width, height, text, scaleX, scaleY, false, false, 0);
}

void screen_draw_wstring_center(bool isTop, float y, float scaleX, float scaleY, u32 color, const wchar_t * wtext) 
{
	float width = 0;
	screen_get_wstring_size(&width, NULL, wtext, scaleX, scaleY);
	screen_draw_wstring_internal(wtext, ((isTop ? 400 : 320) - width) / 2, y, scaleX, scaleY, color, false, false, 0);
}

void screen_draw_wstringf(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t * text, ...) 
{
	wchar_t buffer[256];
	va_list args;
	va_start(args, text);
	vswprintf(buffer, 256, text, args);
	screen_draw_wstring(x, y, scaleX, scaleY, color, text);
	va_end(args);
}

float screen_get_wstring_width(const wchar_t * text, float scaleX, float scaleY)
{
	float width = 0.0;
	screen_get_wstring_size(&width, NULL, text, scaleX, scaleY);
	
	return width;
}

float screen_get_wstring_height(const wchar_t * text, float scaleX, float scaleY)
{
	float height = 0.0;
	screen_get_wstring_size(NULL, &height, text, scaleX, scaleY);
	
	return height;
}