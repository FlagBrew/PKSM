#include "pp2d.h"

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projectionTop;
static C3D_Mtx projectionBot;
static C3D_Tex* glyphSheets;
static textVertex_s* textVtxArray;
static int textVtxArrayPos;
static C3D_RenderTarget* top;
static C3D_RenderTarget* bot;

static struct {
	C3D_Tex tex;
	u32 width;
	u32 height;
	bool allocated;
} textures[MAX_TEXTURES];

static struct {
	size_t id;
	int startIndex;
	bool succeeded;
} fastData;

static void pp2d_add_text_vertex(float vx, float vy, float tx, float ty);
static void pp2d_blend(u32 color);
static void pp2d_draw_vbo(size_t id, int x, int y, int height, int width, u32 color, float left, float right, float top, float bottom);
static bool pp2d_fast_draw_vbo(int x, int y, int height, int width, float left, float right, float top, float bottom);
static void pp2d_free_texture(size_t id);
static u32 pp2d_get_next_pow2(u32 n);
static void pp2d_get_text_size_internal(float* width, float* height, float scaleX, float scaleY, const char* text);
static void pp2d_set_text_color(u32 color);

static void pp2d_add_text_vertex(float vx, float vy, float tx, float ty)
{
	textVertex_s* vtx = &textVtxArray[textVtxArrayPos++];
	vtx->position[0] = vx;
	vtx->position[1] = vy;
	vtx->position[2] = 0.5f;
	vtx->texcoord[0] = tx;
	vtx->texcoord[1] = ty;
}

void pp2d_begin_draw(gfxScreen_t target)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	textVtxArrayPos = 0;
	pp2d_draw_on(target);
}

static void pp2d_blend(u32 color)
{
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_CONSTANT, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
	C3D_TexEnvColor(env, color);
}

void pp2d_draw_on(gfxScreen_t target)
{
	C3D_FrameDrawOn(target == GFX_TOP ? top : bot);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, target == GFX_TOP ? &projectionTop : &projectionBot);
}

void pp2d_draw_rectangle(int x, int y, int width, int height, u32 color)
{
	pp2d_set_text_color(color);
	if (pp2d_fast_draw_vbo(x, y, height, width, 0, 0, 0, 0))
	{
		C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);
	}
	pp2d_blend(PP2D_NEUTRAL);
}

void pp2d_draw_text(float x, float y, float scaleX, float scaleY, u32 color, const char* text)
{
	if (text == NULL)
		return;
	
	pp2d_set_text_color(color);

	ssize_t  units;
	uint32_t code;
	const uint8_t* p = (const uint8_t*)text;
	float firstX = x;
	int lastSheet = -1;
	
	do
	{
		if (!*p) break;
		units = decode_utf8(&code, p);
		if (units == -1)
			break;
		p += units;
		if (code == '\n')
		{
			x = firstX;
			y += scaleY*fontGetInfo()->lineFeed;
		}
		else if (code > 0)
		{
			int glyphIdx = fontGlyphIndexFromCodePoint(code);
			fontGlyphPos_s data;
			fontCalcGlyphPos(&data, glyphIdx, GLYPH_POS_CALC_VTXCOORD, scaleX, scaleY);

			if (data.sheetIndex != lastSheet)
			{
				lastSheet = data.sheetIndex;
				C3D_TexBind(0, &glyphSheets[lastSheet]);
			}

			if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
				break;

			pp2d_add_text_vertex(x+data.vtxcoord.left,  y+data.vtxcoord.bottom, data.texcoord.left,  data.texcoord.bottom);
			pp2d_add_text_vertex(x+data.vtxcoord.right, y+data.vtxcoord.bottom, data.texcoord.right, data.texcoord.bottom);
			pp2d_add_text_vertex(x+data.vtxcoord.left,  y+data.vtxcoord.top,    data.texcoord.left,  data.texcoord.top);
			pp2d_add_text_vertex(x+data.vtxcoord.right, y+data.vtxcoord.top,    data.texcoord.right, data.texcoord.top);

			C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);

			x += data.xAdvance;
		}
	} while (code > 0);
}

void pp2d_draw_text_center(gfxScreen_t target, float y, float scaleX, float scaleY, u32 color, const char* text)
{
	float width = pp2d_get_text_width(text, scaleX, scaleY);
	float x = ((target == GFX_TOP ? TOP_WIDTH : BOTTOM_WIDTH) - width) / 2;
	pp2d_draw_text(x, y, scaleX, scaleY, color, text);
}

void pp2d_draw_textf(float x, float y, float scaleX, float scaleY, u32 color, const char* text, ...) 
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	pp2d_draw_text(x, y, scaleX, scaleY, color, buffer);
	va_end(args);
}

void pp2d_draw_texture(size_t id, int x, int y)
{
	pp2d_draw_texture_blend_flip(id, x, y, PP2D_NEUTRAL, NONE);
}

void pp2d_draw_texture_blend(size_t id, int x, int y, u32 color)
{
	pp2d_draw_texture_blend_flip(id, x, y, color, NONE);
}

void pp2d_draw_texture_blend_flip(size_t id, int x, int y, u32 color, flipType fliptype)
{
	if (id >= MAX_SPRITES)
		return;
	
	float left = 0.0f;
	float right = (float)textures[id].width / (float)textures[id].tex.width;
	float top = 1.0f;
	float bottom = (float)(textures[id].tex.height - textures[id].height) / (float)textures[id].tex.height;
	
	if (fliptype == HORIZONTAL || fliptype == BOTH)
	{
		float tmp = left;
		left = right;
		right = tmp;
	}
	
	if (fliptype == VERTICAL || fliptype == BOTH)
	{
		float tmp = top;
		top = bottom;
		bottom = tmp;
	}
	
	pp2d_draw_vbo(id, 
		x, y, 
		textures[id].height, textures[id].width,
		color,
		left, right, 
		top, bottom
	);
}

void pp2d_draw_texture_flip(size_t id, int x, int y, flipType fliptype)
{
	pp2d_draw_texture_blend_flip(id, x, y, PP2D_NEUTRAL, fliptype);
}

void pp2d_draw_texture_scale(size_t id, int x, int y, float scaleX, float scaleY)
{
	if (id >= MAX_SPRITES)
		return;
	
	pp2d_draw_vbo(id, 
		x, y,
		textures[id].height * scaleY, textures[id].width * scaleX,
		PP2D_NEUTRAL,
		0.0f, (float)textures[id].width / (float)textures[id].tex.width, 
		1.0f, (float)(textures[id].tex.height - textures[id].height) / (float)textures[id].tex.height
	);
}

void pp2d_draw_texture_scale_blend(size_t id, int x, int y, float scaleX, float scaleY, u32 color)
{
	if (id >= MAX_SPRITES)
		return;
	
	pp2d_draw_vbo(id, 
		x, y,
		textures[id].height * scaleY, textures[id].width * scaleX,
		color,
		0.0f, (float)textures[id].width / (float)textures[id].tex.width, 
		1.0f, (float)(textures[id].tex.height - textures[id].height) / (float)textures[id].tex.height
	);
}

void pp2d_draw_texture_part(size_t id, int x, int y, int xbegin, int ybegin, int width, int height)
{
	if (id >= MAX_SPRITES)
		return;
	
	pp2d_draw_vbo(id, 
		x, y, 
		height, width,
		PP2D_NEUTRAL,
		(float)xbegin / (float)textures[id].tex.width, (float)(xbegin + width) / (float)textures[id].tex.width, 
		(float)(textures[id].tex.height - ybegin) / (float)textures[id].tex.height, (float)(textures[id].tex.height - ybegin - height) / (float)textures[id].tex.height
	);
}

void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color)
{
	if (id >= MAX_SPRITES)
		return;
	
	pp2d_draw_vbo(id, 
		x, y, 
		height, width,
		color,
		(float)xbegin / (float)textures[id].tex.width, (float)(xbegin + width) / (float)textures[id].tex.width, 
		(float)(textures[id].tex.height - ybegin) / (float)textures[id].tex.height, (float)(textures[id].tex.height - ybegin - height) / (float)textures[id].tex.height
	);
}

void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY)
{
	if (id >= MAX_SPRITES)
		return;
	
	pp2d_draw_vbo(id, 
		x, y, 
		height * scaleY, width * scaleX,
		PP2D_NEUTRAL,
		(float)xbegin / (float)textures[id].tex.width, (float)(xbegin + width) / (float)textures[id].tex.width, 
		(float)(textures[id].tex.height - ybegin) / (float)textures[id].tex.height, (float)(textures[id].tex.height - ybegin - height) / (float)textures[id].tex.height
	);
}

void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color)
{
	if (id >= MAX_SPRITES)
		return;
	
	pp2d_draw_vbo(id, 
		x, y, 
		height * scaleY, width * scaleX,
		color,
		(float)xbegin / (float)textures[id].tex.width, (float)(xbegin + width) / (float)textures[id].tex.width, 
		(float)(textures[id].tex.height - ybegin) / (float)textures[id].tex.height, (float)(textures[id].tex.height - ybegin - height) / (float)textures[id].tex.height
	);
}

static void pp2d_draw_vbo(size_t id, int x, int y, int height, int width, u32 color, float left, float right, float top, float bottom)
{
	if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
		return;
	
	C3D_TexBind(0, &textures[id].tex);
	pp2d_blend(color);
	
	pp2d_add_text_vertex(        x, y + height,  left, bottom);
	pp2d_add_text_vertex(x + width, y + height, right, bottom);
	pp2d_add_text_vertex(        x,          y,  left,    top);
	pp2d_add_text_vertex(x + width,          y, right,    top);

	C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);
}

void pp2d_draw_wtext(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t* text) 
{
	if (text == NULL)
		return;
	
	u32 size = wcslen(text) * sizeof(wchar_t);
	char buf[size];
	memset(buf, 0, size);
	utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
	buf[size - 1] = '\0';	
	
	pp2d_draw_text(x, y, scaleX, scaleY, color, buf);
}

void pp2d_draw_wtext_center(gfxScreen_t target, float y, float scaleX, float scaleY, u32 color, const wchar_t* text)
{
	float width = pp2d_get_wtext_width(text, scaleX, scaleY);
	float x = ((target == GFX_TOP ? TOP_WIDTH : BOTTOM_WIDTH) - width) / 2;
	pp2d_draw_wtext(x, y, scaleX, scaleY, color, text);
}

void pp2d_draw_wtextf(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t* text, ...) 
{
	wchar_t buffer[256];
	va_list args;
	va_start(args, text);
	vswprintf(buffer, 256, text, args);
	pp2d_draw_wtext(x, y, scaleX, scaleY, color, buffer);
	va_end(args);
}

void pp2d_end_draw(void)
{
	C3D_FrameEnd(0);
}

void pp2d_exit(void)
{
	for (size_t id = 0; id < MAX_TEXTURES; id++)
	{
		if (textures[id].allocated)
		{
			pp2d_free_texture(id);
		}
	}
	
	linearFree(textVtxArray);
	free(glyphSheets);
	
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
	
	C3D_Fini();
	gfxExit();
}

void pp2d_fast_draw_init(size_t id)
{
	fastData.startIndex = textVtxArrayPos;
	fastData.id = id;
	fastData.succeeded = false;
	
	C3D_TexBind(0, &textures[id].tex);
	pp2d_blend(PP2D_NEUTRAL);
}

void pp2d_fast_draw_texture(int x, int y)
{
	size_t id = fastData.id;
	if (id >= MAX_SPRITES)
		return;

	fastData.succeeded = pp2d_fast_draw_vbo(x, y, 
		textures[id].height, textures[id].width, 
		0.0f, (float)textures[id].width / (float)textures[id].tex.width, 
		1.0f, (float)(textures[id].tex.height - textures[id].height) / (float)textures[id].tex.height
	);
}

void pp2d_fast_draw_texture_part(int x, int y, int xbegin, int ybegin, int width, int height)
{
	size_t id = fastData.id;
	if (id >= MAX_SPRITES)
		return;

	fastData.succeeded = pp2d_fast_draw_vbo(x, y, 
		height, width, 
		(float)xbegin / (float)textures[id].tex.width, (float)(xbegin + width) / (float)textures[id].tex.width, 
		(float)(textures[id].tex.height - ybegin) / (float)textures[id].tex.height, (float)(textures[id].tex.height - ybegin - height) / (float)textures[id].tex.height
	);
}

static bool pp2d_fast_draw_vbo(int x, int y, int height, int width, float left, float right, float top, float bottom)
{
	if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
		return false;
	
	pp2d_add_text_vertex(        x, y + height,  left, bottom);
	pp2d_add_text_vertex(x + width, y + height, right, bottom);
	pp2d_add_text_vertex(        x,          y,  left,    top);
	pp2d_add_text_vertex(x + width,          y, right,    top);
	
	return true;
}

void pp2d_fast_render(void)
{
	if (fastData.succeeded)
	{
		C3D_DrawArrays(GPU_TRIANGLE_STRIP, fastData.startIndex, textVtxArrayPos - fastData.startIndex);
	}
	
	fastData.succeeded = false;
	fastData.startIndex = -1;
}

static void pp2d_free_texture(size_t id)
{
	if (id >= MAX_TEXTURES)
		return;
	
	C3D_TexDelete(&textures[id].tex);
	textures[id].width = 0;
	textures[id].height = 0;
	textures[id].allocated = false;
}

Result pp2d_init(void)
{
	Result res = 0;
	
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	
	top = C3D_RenderTargetCreate(SCREEN_HEIGHT, TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetClear(top, C3D_CLEAR_ALL, BACKGROUND_COLOR, 0);
	C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	
	bot = C3D_RenderTargetCreate(SCREEN_HEIGHT, BOTTOM_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetClear(bot, C3D_CLEAR_ALL, BACKGROUND_COLOR, 0);
	C3D_RenderTargetSetOutput(bot, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	
	res = fontEnsureMapped();
	if (R_FAILED(res))
		return res;
	
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_len);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);

	Mtx_OrthoTilt(&projectionTop, 0, TOP_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, true);
	Mtx_OrthoTilt(&projectionBot, 0, BOTTOM_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, true);
	
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

	int i;
	TGLP_s* glyphInfo = fontGetGlyphInfo();
	glyphSheets = malloc(sizeof(C3D_Tex)*glyphInfo->nSheets);
	for (i = 0; i < glyphInfo->nSheets; i ++)
	{
		C3D_Tex* tex = &glyphSheets[i];
		tex->data = fontGetGlyphSheetTex(i);
		tex->fmt = glyphInfo->sheetFmt;
		tex->size = glyphInfo->sheetSize;
		tex->width = glyphInfo->sheetWidth;
		tex->height = glyphInfo->sheetHeight;
		tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)
			| GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_EDGE) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_EDGE);
		tex->border = 0;
		tex->lodParam = 0;
	}
	
	textVtxArray = (textVertex_s*)linearAlloc(sizeof(textVertex_s)*TEXT_VTX_ARRAY_COUNT);
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, textVtxArray, sizeof(textVertex_s), 2, 0x10);
	
	return 0;
}

static u32 pp2d_get_next_pow2(u32 v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v >= 64 ? v : 64;
}

float pp2d_get_text_height(const char* text, float scaleX, float scaleY)
{
	float height;
	pp2d_get_text_size_internal(NULL, &height, scaleX, scaleY, text);
	return height;
}

static void pp2d_get_text_size_internal(float* width, float* height, float scaleX, float scaleY, const char* text)
{
	float w = 0.0f;
	float h = 0.0f;
	
	ssize_t  units;
	uint32_t code;
	const uint8_t* p = (const uint8_t*)text;
	
	do
	{
		if (!*p) break;
		units = decode_utf8(&code, p);
		if (units == -1)
			break;
		p += units;
		if (code == '\n')
		{
			h += scaleY*fontGetInfo()->lineFeed;
		}
		else if (code > 0)
		{
			w += (scaleX * fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(code))->charWidth);
		}
	} while (code > 0);
	
	if (width)
	{
		*width = w;
	}
	
	if (height)
	{
		*height = h;
	}
}

float pp2d_get_text_width(const char* text, float scaleX, float scaleY)
{
	float width;
	pp2d_get_text_size_internal(&width, NULL, scaleX, scaleY, text);
	return width;
}

float pp2d_get_wtext_height(const wchar_t* text, float scaleX, float scaleY)
{
	u32 size = wcslen(text) * sizeof(wchar_t);
	char buf[size];
	memset(buf, 0, size);
	utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
	buf[size - 1] = '\0';
	
	float height;
	pp2d_get_text_size_internal(NULL, &height, scaleX, scaleY, buf);
	return height;
}

float pp2d_get_wtext_width(const wchar_t* text, float scaleX, float scaleY)
{
	u32 size = wcslen(text) * sizeof(wchar_t);
	char buf[size];
	memset(buf, 0, size);
	utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
	buf[size - 1] = '\0';
	
	float width;
	pp2d_get_text_size_internal(&width, NULL, scaleX, scaleY, buf);
	return width;
}

void pp2d_load_texture_png(size_t id, const char* path)
{
	if (id >= MAX_TEXTURES)
		return;
	
	u8* image;
	unsigned width;
	unsigned height;

	lodepng_decode32_file(&image, &width, &height, path);

	for (u32 i = 0; i < width; i++) 
	{
		for (u32 j = 0; j < height; j++) 
		{
			u32 p = (i + j*width) * 4;

			u8 r = *(u8*)(image + p);
			u8 g = *(u8*)(image + p + 1);
			u8 b = *(u8*)(image + p + 2);
			u8 a = *(u8*)(image + p + 3);

			*(image + p) = a;
			*(image + p + 1) = b;
			*(image + p + 2) = g;
			*(image + p + 3) = r;
		}
	}
	
	free(image);

	u32 w_pow2 = pp2d_get_next_pow2(width);
	u32 h_pow2 = pp2d_get_next_pow2(height);
	
	C3D_TexInit(&textures[id].tex, (u16)w_pow2, (u16)h_pow2, GPU_RGBA8);
	C3D_TexSetFilter(&textures[id].tex, GPU_LINEAR, GPU_NEAREST);
	
	textures[id].allocated = true;
	textures[id].width = width;
	textures[id].height = height;

	memset(textures[id].tex.data, 0, textures[id].tex.size);
	for (u32 i = 0; i < width; i++) 
	{
		for (u32 j = 0; j < height; j++) 
		{
			u32 dst = ((((j >> 3) * (w_pow2 >> 3) + (i >> 3)) << 6) + ((i & 1) | ((j & 1) << 1) | ((i & 2) << 1) | ((j & 2) << 2) | ((i & 4) << 2) | ((j & 4) << 3))) * 4;
			u32 src = (j * width + i) * 4;

			memcpy(textures[id].tex.data + dst, image + src, 4);
		}
	}

	C3D_TexFlush(&textures[id].tex);
}

void pp2d_set_screen_color(gfxScreen_t target, u32 color)
{
	C3D_RenderTargetSetClear(target == GFX_TOP ? top : bot, C3D_CLEAR_ALL, color, 0);
}

static void pp2d_set_text_color(u32 color)
{
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_RGB, GPU_CONSTANT, 0, 0);
	C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, GPU_CONSTANT, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
	C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
	C3D_TexEnvColor(env, color);
}