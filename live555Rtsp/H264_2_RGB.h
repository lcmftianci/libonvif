extern "C"
{
	#include <libavcodec/avcodec.h>  
	#include <libavformat/avformat.h>  
	#include <libavutil/avutil.h>  
	#include <libswscale/swscale.h>  
};



//初始化ffmpeg相关组件
int H264_Init(void);

//转码函数
int H264_2_RGB(unsigned char *inputbuf, int frame_size, unsigned char *outputbuf, unsigned int*outsize, int *nWidth, int *nHeight);

//释放资源
void H264_Release(void);
