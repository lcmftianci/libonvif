#include "H264_2_RGB.h"

#include <opencv2/opencv.hpp>
extern "C"
{
//	#include "sdl1/SDL.h"
}

extern IplImage * MyResizeImage(IplImage* pSrc, double dScale);

extern FILE* g_fp;

AVCodec         *pCodec = NULL;
AVCodecContext  *pCodecCtx = NULL;
SwsContext      *img_convert_ctx = NULL;
AVFrame         *pFrame = NULL;
AVFrame         *pFrameRGB = NULL;
AVPacket		pkt;

FILE * fw;

//初始化ffmpeg相关组件
int H264_Init(void)
{
	avcodec_register_all();
	av_register_all();
	AVCodecID avCode;
	avCode = AV_CODEC_ID_H264;

	pCodec = avcodec_find_decoder(avCode);
	pCodecCtx = avcodec_alloc_context3(pCodec);
	
	pFrame = av_frame_alloc();
	if (pFrame == NULL)
		return -1;
	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL)
		return -1;
	av_init_packet(&pkt);

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
		return -1;

	if (fw == NULL)
		fw = fopen("a.mp4", "wb");

	return 0;
}

int g_init = 0;

//转码函数
int H264_2_RGB(unsigned char *inputbuf, int frame_size, unsigned char *outputbuf, unsigned int*outsize, int *nWidth, int *nHeight)
{
	int             av_result = 0;
	printf("Video decoding\n");
	int nGetPic;
	uint8_t *buffer_stream;
	buffer_stream = (uint8_t*)malloc(1024 * 1024 * 4);
	uint8_t* buffer_rgb = NULL;

	pkt.data = inputbuf;
	pkt.size = frame_size;

	av_result = avcodec_decode_video2(pCodecCtx, pFrame, &nGetPic, &pkt);
	if (av_result < 0)
	{
		fprintf(stderr, "decode failed: inputbuf = 0x%x , input_framesize = %d\n", inputbuf, frame_size);
		return -1;
	}
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
		return -1;

	if (avcodec_open2(pCodecCtx, pCodec, NULL)<0)
		return -1; 
	
	if (nGetPic)
	{
		int bytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
		buffer_rgb = (uint8_t *)av_malloc(bytes);
		avpicture_fill((AVPicture *)pFrameRGB, buffer_rgb, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

		img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
			pCodecCtx->width,
			pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
		if (img_convert_ctx == NULL)
		{

			printf("can't init convert context!\n");
			return -1;
		}
		sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

		*outsize = pCodecCtx->width * 3 * pCodecCtx->height;
		*nWidth = pCodecCtx->width;
		*nHeight = pCodecCtx->height;
		
		IplImage *pRgbImg = cvCreateImage(cvSize(*nWidth, *nHeight), 8, 3);
		memcpy(pRgbImg->imageData, buffer_rgb, *nWidth*3**nHeight);
		IplImage* pDes = MyResizeImage(pRgbImg, 0.4);
		cvShowImage("GB28181Show", pDes);
		cvWaitKey(5);
		cvReleaseImage(&pDes);
		sws_freeContext(img_convert_ctx);
		av_free(buffer_rgb);
		free(buffer_stream);
	}
	
	return 0;
}

void H264_Release(void)
{
	avcodec_close(pCodecCtx);
	av_free(pCodecCtx);
	av_free(pFrame);
	av_free(pFrameRGB);
	if (fw != NULL)
	{
		fclose(fw);
		fw = NULL;
	}
	//av_free(pkt);
}
