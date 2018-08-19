#include <iostream>
#include "onvif_interface.h"
#include "onvif_comm.h"

#define __STDC_CONSTANT_MACROS  

/*opencv库*/
#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
//#include"opencv2/face.hpp"
//#include"opencv2/face/facerec.hpp"
//#include"opencv2/objdetect.hpp"
//#include"opencv2/core/base.hpp"
//#include"opencv2/xfeatures2d.hpp"

extern "C"   /*这里必须要使用C方式导入*/
{
	#include "libavcodec/avcodec.h"
	#include "libavdevice/avdevice.h"
	#include "libavformat/avformat.h"
	#include "libavfilter/avfilter.h"
	#include "libavutil/avutil.h"
	#include "libavutil/time.h"
	#include "libswscale/swscale.h"
	#include "libavutil/pixdesc.h"
}


#if 0
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "postproc.lib")
#endif

//#include "soapH.h"
//#include "stdsoap2.h"
//#include "soapStub.h"
//#include "base64.h"
//
//#pragma comment(lib, "UsageEnvironment.lib")
//#pragma comment(lib, "liveMedia.lib")
//#pragma comment(lib, "groupsock.lib")
//#pragma comment(lib, "BasicUsageEnvironment.lib")
//#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace cv;
//using namespace cv::xfeatures2d;
//using namespace cv::ml;
//using namespace face;

//String faceCascadeFileName = "C:\\Anaconda3\\Library\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml";
//String eyesCasecadeFileName = "C:\\Anaconda3\\Library\\etc\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";

//CascadeClassifier faceCascade;
//CascadeClassifier eyesCascade;

IplImage * MyResizeImage(IplImage* pSrc, double dScale)
{
	CvSize nSize;
	nSize.width = pSrc->width*dScale;
	nSize.height = pSrc->height*dScale;
	IplImage* pDes = cvCreateImage(nSize, pSrc->depth, pSrc->nChannels);
	cvResize(pSrc, pDes, CV_INTER_CUBIC);
	cvReleaseImage(&pSrc);
	return pDes;
} 
//人脸检测
#if 0
void DetectFace(Mat frame)
{
	vector<Rect> faces;
	Mat frameGray;
	cvtColor(frame, frameGray, COLOR_BGR2GRAY);
	equalizeHist(frameGray, frameGray);
#if 0
	faceCascade.detectMultiScale(frameGray, faces, 1.1, 3, 3);
	for each (Rect var in faces)
	{
		rectangle(frame, var, Scalar(255, 0, 0), 2, 8, 0);
		Mat faceROI = frameGray(var);
		vector<Rect> eyes;
		eyesCascade.detectMultiScale(faceROI, eyes, 1.1, 1, 0);
		for each(Rect tr in eyes)
		{
			Rect rect(var.x + tr.x, var.y + tr.y, tr.width, tr.height);
			rectangle(frame, rect, Scalar(0, 244, 0), 2, 8, 0);
		}
	}
#endif
	namedWindow("face", 1);
	imshow("face", frame);
	waitKey(5);
}
#endif


void open_rtsp(const char *rtsp)
{
	unsigned int    i;
	int             ret;
	int             video_st_index = -1;
	int             audio_st_index = -1;
	AVFormatContext *ifmt_ctx = NULL;
	AVPacket        pkt;
	AVStream        *st = NULL;
	char            errbuf[64];
	AVDictionary *optionsDict = NULL;
	av_register_all();                                                          // Register all codecs and formats so that they can be used.
	avformat_network_init();                                                    // Initialization of network components
	av_dict_set(&optionsDict, "rtsp_transport", "tcp", 0);                //采用tcp传输
	av_dict_set(&optionsDict, "stimeout", "2000000", 0);                  //如果没有设置stimeout，那么把ipc网线拔掉，av_read_frame会阻塞（时间单位是微妙）
	if ((ret = avformat_open_input(&ifmt_ctx, rtsp, 0, &optionsDict)) < 0) {            // Open the input file for reading.
		printf("Could not open input file '%s' (error '%s')\n", rtsp, av_make_error_string(errbuf, sizeof(errbuf), ret));
		//goto EXIT;
	}

	if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {                // Get information on the input file (number of streams etc.).
		printf("Could not open find stream info (error '%s')\n", av_make_error_string(errbuf, sizeof(errbuf), ret));
		//goto EXIT;
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {                                // dump information
		av_dump_format(ifmt_ctx, i, rtsp, 0);
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {                                // find video stream index
		st = ifmt_ctx->streams[i];
		switch (st->codec->codec_type) {
		case AVMEDIA_TYPE_AUDIO: audio_st_index = i; break;
		case AVMEDIA_TYPE_VIDEO: video_st_index = i; break;
		default: break;
		}
	}
	if (-1 == video_st_index) {
		printf("No H.264 video stream in the input file\n");
		//goto EXIT;
	}

	av_init_packet(&pkt);                                                       // initialize packet.
	pkt.data = NULL;
	pkt.size = 0;
	bool nRestart = false;
	int videoindex = -1;
	int audioindex = -1;
	AVStream *pVst;
	uint8_t* buffer_rgb = NULL;
	AVCodecContext *pVideoCodecCtx = NULL;
	AVFrame         *pFrame = av_frame_alloc();
	AVFrame         *pFrameRGB = av_frame_alloc();
	int got_picture;
	SwsContext      *img_convert_ctx = NULL;
	AVCodec *pVideoCodec = NULL;


	//if (!faceCascade.load(faceCascadeFileName))
	{
		cout << "脸模型文件加载失败" << endl;
		//return;
	}
	//if (!eyesCascade.load(eyesCasecadeFileName))
	{
		cout << "人眼模型文件加载失败" << endl;
		//return;
	}

	while (1)
	{
		do {
			ret = av_read_frame(ifmt_ctx, &pkt);                                // read frames
#if 1
			cout << pkt.size << endl;
																				//decode stream
			if (!nRestart)
			{
				for (int i = 0; i < ifmt_ctx->nb_streams; i++)
				{
					if ((ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) && (videoindex < 0))
					{
						videoindex = i;
					}
					if ((ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) && (audioindex < 0))
					{
						audioindex = i;
					}
				}
				pVst = ifmt_ctx->streams[videoindex];
				pVideoCodecCtx = pVst->codec;
				pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
				if (pVideoCodec == NULL)
					return;
				//pVideoCodecCtx = avcodec_alloc_context3(pVideoCodec);

				if (avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL) < 0)
					return;

				nRestart = true;
				printf("first frame \n");
			}

			if (pkt.stream_index == videoindex)
			{
				//fprintf(stdout, "pkt.size=%d,pkt.pts=%lld, pkt.data=0x%x.", pkt.size, pkt.pts, (unsigned int)pkt.data);
				fprintf(stdout, "pkt.size=%d,pkt.pts=%lld\n", pkt.size, pkt.pts);
				int av_result = avcodec_decode_video2(pVideoCodecCtx, pFrame, &got_picture, &pkt);

				if (got_picture)
				{
					fprintf(stdout, "decode one video frame!\n");
				}

				if (av_result < 0)
				{
					fprintf(stderr, "decode failed: inputbuf = 0x%x , input_framesize = %d\n", pkt.data, pkt.size);
					return;
				}
				printf("get pic %d--->%d--->%d\n", got_picture, pVideoCodecCtx->width, pVideoCodecCtx->height);
#if 1
				if (got_picture)
				{
					int bytes = avpicture_get_size(AV_PIX_FMT_RGB24, pVideoCodecCtx->width, pVideoCodecCtx->height);
					buffer_rgb = (uint8_t *)av_malloc(bytes);
					avpicture_fill((AVPicture *)pFrameRGB, buffer_rgb, AV_PIX_FMT_RGB24, pVideoCodecCtx->width, pVideoCodecCtx->height);

					img_convert_ctx = sws_getContext(pVideoCodecCtx->width, pVideoCodecCtx->height, pVideoCodecCtx->pix_fmt,
						pVideoCodecCtx->width, pVideoCodecCtx->height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
					if (img_convert_ctx == NULL)
					{

						printf("can't init convert context!\n");
						return;
					}
					sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pVideoCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
					printf("iplimage-=-====-=-\n");
#if 1
					IplImage *pRgbImg = cvCreateImage(cvSize(pVideoCodecCtx->width, pVideoCodecCtx->height), 8, 3);
					memcpy(pRgbImg->imageData, buffer_rgb, pVideoCodecCtx->width * 3 * pVideoCodecCtx->height);
					Mat Img = cvarrToMat(pRgbImg, true);
					
					IplImage *pDesImage = MyResizeImage(pRgbImg, 0.3);
					cvShowImage("GB28181Show", pDesImage);
					cvWaitKey(3);
					//DetectFace(Img);
					//cvWaitKey(10);
					cvReleaseImage(&pDesImage);
#endif
					sws_freeContext(img_convert_ctx);
					av_free(buffer_rgb);
				}
#endif
			}

#endif
		} while (ret == AVERROR(EAGAIN));

		if (ret < 0) {
			printf("Could not read frame ---(error '%s')\n", av_make_error_string(errbuf, sizeof(errbuf), ret));
			//goto EXIT;
		}

		if (pkt.stream_index == video_st_index) {                               // video frame
			printf("Video Packet size = %d\n", pkt.size);
		}
		else if (pkt.stream_index == audio_st_index) {                         // audio frame
			printf("Audio Packet size = %d\n", pkt.size);
		}
		else {
			printf("Unknow Packet size = %d\n", pkt.size);
		}

		av_packet_unref(&pkt);
	}

EXIT:

	if (NULL != ifmt_ctx) {
		avformat_close_input(&ifmt_ctx);
		ifmt_ctx = NULL;
	}

	return;
}

int PushStream(const char* strInputUrl, const char* strOutputUrl)
{
	AVOutputFormat *ofmt = NULL;
	//输入对应一个AVFormatContext，输出对应一个AVFormatContext  
	//（Input AVFormatContext and Output AVFormatContext）  
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	const char *in_filename, *out_filename;
	int ret, i;
	int videoindex = -1;
	int frame_index = 0;
	int64_t start_time = 0;
	//in_filename  = "cuc_ieschool.mov";  
	//in_filename  = "cuc_ieschool.mkv";  
	//in_filename  = "cuc_ieschool.ts";  
	//in_filename  = "cuc_ieschool.mp4";  
	//in_filename  = "cuc_ieschool.h264";  
	in_filename = strInputUrl;//输入URL（Input file URL）  
										//in_filename  = "shanghai03_p.h264";  

	out_filename = strOutputUrl;//输出 URL（Output URL）[RTMP]  
																//out_filename = "rtp://233.233.233.233:6666";//输出 URL（Output URL）[UDP]  
	av_register_all();
	//Network  
	avformat_network_init();
	//输入（Input）  
	if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		printf("Could not open input file.");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		goto end;
	}

	for (i = 0; i<ifmt_ctx->nb_streams; i++)
		if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}

	av_dump_format(ifmt_ctx, 0, in_filename, 0);

	//输出（Output）  

	avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", out_filename); //RTMP  
																			//avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP  

	if (!ofmt_ctx) {
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		//根据输入流创建输出流（Create output AVStream according to input AVStream）  
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
		if (!out_stream) {
			printf("Failed allocating output stream\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		//复制AVCodecContext的设置（Copy the settings of AVCodecContext）  
		ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		if (ret < 0) {
			printf("Failed to copy context from input to output stream codec context\n");
			goto end;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
	//Dump Format------------------  
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	//打开输出URL（Open output URL）  
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			printf("Could not open output URL '%s'", out_filename);
			goto end;
		}
	}
	//写文件头（Write file header）  
	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		printf("Error occurred when opening output URL\n");
		goto end;
	}

	start_time = av_gettime();
	while (1) {
		AVStream *in_stream, *out_stream;
		//获取一个AVPacket（Get an AVPacket）  
		ret = av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
			break;
		//FIX：No PTS (Example: Raw H.264)  
		//Simple Write PTS  
		if (pkt.pts == AV_NOPTS_VALUE) {
			//Write PTS  
			AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
			//Duration between 2 frames (us)  
			int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
			//Parameters  
			pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
			pkt.dts = pkt.pts;
			pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
		}
		//Important:Delay  
		if (pkt.stream_index == videoindex) {
			AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
			AVRational time_base_q = { 1,AV_TIME_BASE };
			int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
			int64_t now_time = av_gettime() - start_time;
			if (pts_time > now_time)
				av_usleep(pts_time - now_time);

		}

		in_stream = ifmt_ctx->streams[pkt.stream_index];
		out_stream = ofmt_ctx->streams[pkt.stream_index];
		/* copy packet */
		//转换PTS/DTS（Convert PTS/DTS）  
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;
		//Print to Screen  
		if (pkt.stream_index == videoindex) {
			printf("Send %8d video frames to output URL\n", frame_index);
			frame_index++;
		}
		//ret = av_write_frame(ofmt_ctx, &pkt);  
		ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

		if (ret < 0) {
			printf("Error muxing packet\n");
			break;
		}

		av_free_packet(&pkt);

	}
	//写文件尾（Write file trailer）  
	av_write_trailer(ofmt_ctx);
end:
	avformat_close_input(&ifmt_ctx);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		printf("Error occurred.\n");
		return -1;
	}
	return 0;
}

void cb_discovery(char* DeviceXAddr)
{
	int stmno = 0;                                                              // 码流序号，0为主码流，1为辅码流
	int profile_cnt = 0;                                                        // 设备配置文件个数
	struct tagProfile *profiles = NULL;                                         // 设备配置文件列表
	struct tagCapabilities capa;                                                // 设备能力信息

	char uri[MY_ONVIF_ADDRESS_SIZE] = { 0 };                                         // 不带认证信息的URI地址
	char uri_auth[MY_ONVIF_TOKEN_SIZE + 50] = { 0 };                               // 带有认证信息的URI地址

	MYONVIF_GetCapabilities(DeviceXAddr, &capa);                                  // 获取设备能力信息（获取媒体服务地址）

	profile_cnt = MYONVIF_GetProfiles(capa.MediaXAddr, &profiles);                // 获取媒体配置信息（主/辅码流配置信息）

	if (profile_cnt > stmno) {
		MYONVIF_GetStreamUri(capa.MediaXAddr, profiles[stmno].token, uri, sizeof(uri)); // 获取RTSP地址

		//my_make_uri_withauth(uri, USERNAME, PASSWORD, uri_auth, sizeof(uri_auth)); // 生成带认证信息的URI（有的IPC要求认证）
		char chName[6] = "admin";
		my_make_uri_withauth(uri, chName, chName, uri_auth, sizeof(uri_auth)); // 生成带认证信息的URI（有的IPC要求认证）

		open_rtsp(uri_auth);                                                    // 读取主码流的音视频数据
	}

	if (NULL != profiles) {
		free(profiles);
		profiles = NULL;
	}
}


int main(void)
{
	MYONVIF_DetectDevice(cb_discovery);
	getchar();
	while (1)
	{
		//通过urlcode获取rtsp地址
		//string strUrl = RtspGetUrl("00029240000000000101#f8f5858576b74f1bb31cc1406a729986");
		open_rtsp("rtsp://192.168.17.1/Titanic.mkv");
	}

	//PushStream("D:\\BaiduNetdiskDownload\\test.mp4", "rtmp://10.125.190.15:1935/live/room");

	cout << "hello world" << endl;
	getchar();
	return 0;
}
