#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>

Result http_download(httpcContext *context) {
	Result ret = 0;
	u32 statuscode = 0;
	u32 contentsize = 0;
	u8 *buf;
	
	ret = httpcBeginRequest(context);
	if (ret != 0) 
		return ret;

	ret = httpcGetResponseStatusCode(context, &statuscode, 0);
	if (ret != 0)
		return ret;

	if (statuscode != 200) 
		return -2;

	ret = httpcGetDownloadSizeState(context, NULL, &contentsize);
	if (ret != 0)
		return ret;

	gfxFlushBuffers();

	buf = (u8*)malloc(contentsize);
	if (buf == NULL)
		return -1;
	memset(buf, 0, contentsize);

	ret = httpcDownloadData(context, buf, contentsize, NULL);
	if(ret != 0) {
		free(buf);
		return ret;
	}

	printf("%s", buf);
	free(buf);
	return 0;
}

void getText(char *url) {
	Result ret = 0;
	httpcContext context;
	
	httpcInit();
	gfxFlushBuffers();

	ret = httpcOpenContext(&context, url, 1);
	gfxFlushBuffers();
	
	if (ret == 0) {
		ret = http_download(&context);
		gfxFlushBuffers();
		httpcCloseContext(&context);
	}
	httpcExit();
}