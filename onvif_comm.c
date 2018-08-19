#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "wsseapi.h"
#include "onvif_comm.h"
#include "onvif_dump.h"

void soap_perror(struct soap *soap, const char *str)
{
    if (NULL == str) {
        SOAP_DBGERR("[soap] error: %d, %s, %s\n", soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    } else {
        SOAP_DBGERR("[soap] %s error: %d, %s, %s\n", str, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    }
    return;
}

void* ONVIF_soap_malloc(struct soap *soap, unsigned int n)
{
    void *p = NULL;

    if (n > 0) {
        p = soap_malloc(soap, n);
        SOAP_ASSERT(NULL != p);
        memset(p, 0x00 ,n);
    }
    return p;
}

struct soap *ONVIF_soap_new(int timeout)
{
    struct soap *soap = NULL;                                                   // soap环境变量
	soap = soap_new();

	SOAP_ASSERT(NULL != soap);
    //SOAP_ASSERT(NULL != (soap = soap_new()));

    soap_set_namespaces(soap, namespaces);                                      // 设置soap的namespaces
    soap->recv_timeout    = timeout;                                            // 设置超时（超过指定时间没有数据就退出）
    soap->send_timeout    = timeout;
    soap->connect_timeout = timeout;

#if defined(__linux__) || defined(__linux)                                      // 参考https://www.genivia.com/dev.html#client-c的修改：
    soap->socket_flags = MSG_NOSIGNAL;                                          // To prevent connection reset errors
#endif

    soap_set_mode(soap, SOAP_C_UTFSTRING);                                      // 设置为UTF-8编码，否则叠加中文OSD会乱码

    return soap;
}

void ONVIF_soap_delete(struct soap *soap)
{
    soap_destroy(soap);                                                         // remove deserialized class instances (C++ only)
    soap_end(soap);                                                             // Clean up deserialized data (except class instances) and temporary data
    soap_done(soap);                                                            // Reset, close communications, and remove callbacks
    soap_free(soap);                                                            // Reset and deallocate the context created with soap_new or soap_copy
}

/************************************************************************
**函数：ONVIF_SetAuthInfo
**功能：设置认证信息
**参数：
        [in] soap     - soap环境变量
        [in] username - 用户名
        [in] password - 密码
**返回：
        0表明成功，非0表明失败
**备注：
************************************************************************/
int ONVIF_SetAuthInfo(struct soap *soap, const char *username, const char *password)
{
    int result = 0;

    SOAP_ASSERT(NULL != username);
    SOAP_ASSERT(NULL != password);

    result = soap_wsse_add_UsernameTokenDigest(soap, NULL, username, password);
    SOAP_CHECK_ERROR(result, soap, "add_UsernameTokenDigest");

EXIT:

    return result;
}

/************************************************************************
**函数：ONVIF_init_header
**功能：初始化soap描述消息头
**参数：
        [in] soap - soap环境变量
**返回：无
**备注：
    1). 在本函数内部通过ONVIF_soap_malloc分配的内存，将在ONVIF_soap_delete中被释放
************************************************************************/
void ONVIF_init_header(struct soap *soap)
{
    struct SOAP_ENV__Header *header = NULL;

    SOAP_ASSERT(NULL != soap);

    header = (struct SOAP_ENV__Header *)ONVIF_soap_malloc(soap, sizeof(struct SOAP_ENV__Header));
    soap_default_SOAP_ENV__Header(soap, header);
    header->wsa__MessageID = (char*)soap_wsa_rand_uuid(soap);
    header->wsa__To        = (char*)ONVIF_soap_malloc(soap, strlen(SOAP_TO) + 1);
    header->wsa__Action    = (char*)ONVIF_soap_malloc(soap, strlen(SOAP_ACTION) + 1);
    strcpy(header->wsa__To, SOAP_TO);
    strcpy(header->wsa__Action, SOAP_ACTION);
    soap->header = header;

    return;
}

/************************************************************************
**函数：ONVIF_init_ProbeType
**功能：初始化探测设备的范围和类型
**参数：
        [in]  soap  - soap环境变量
        [out] probe - 填充要探测的设备范围和类型
**返回：
        0表明探测到，非0表明未探测到
**备注：
    1). 在本函数内部通过ONVIF_soap_malloc分配的内存，将在ONVIF_soap_delete中被释放
************************************************************************/
void ONVIF_init_ProbeType(struct soap *soap, struct wsdd__ProbeType *probe)
{
    struct wsdd__ScopesType *scope = NULL;                                      // 用于描述查找哪类的Web服务

    SOAP_ASSERT(NULL != soap);
    SOAP_ASSERT(NULL != probe);

    scope = (struct wsdd__ScopesType *)ONVIF_soap_malloc(soap, sizeof(struct wsdd__ScopesType));
    soap_default_wsdd__ScopesType(soap, scope);                                 // 设置寻找设备的范围
    scope->__item = (char*)ONVIF_soap_malloc(soap, strlen(SOAP_ITEM) + 1);
    strcpy(scope->__item, SOAP_ITEM);

    memset(probe, 0x00, sizeof(struct wsdd__ProbeType));
    soap_default_wsdd__ProbeType(soap, probe);
    probe->Scopes = scope;
    probe->Types  = (char*)ONVIF_soap_malloc(soap, strlen(SOAP_TYPES) + 1);     // 设置寻找设备的类型
    strcpy(probe->Types, SOAP_TYPES);

    return;
}

void ONVIF_DetectDevice(void (*cb)(char *DeviceXAddr))
{
    int i;
    int result = 0;
    unsigned int count = 0;                                                     // 搜索到的设备个数
    struct soap *soap = NULL;                                                   // soap环境变量
    struct wsdd__ProbeType      req;                                            // 用于发送Probe消息
    struct __wsdd__ProbeMatches rep;                                            // 用于接收Probe应答
    struct wsdd__ProbeMatchType *probeMatch;

	soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT);

	SOAP_ASSERT(NULL != soap);
   //SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

    ONVIF_init_header(soap);                                                    // 设置消息头描述
    ONVIF_init_ProbeType(soap, &req);                                           // 设置寻找的设备的范围和类型
    result = soap_send___wsdd__Probe(soap, SOAP_MCAST_ADDR, NULL, &req);        // 向组播地址广播Probe消息
    while (SOAP_OK == result)                                                   // 开始循环接收设备发送过来的消息
    {
        memset(&rep, 0x00, sizeof(rep));
        result = soap_recv___wsdd__ProbeMatches(soap, &rep);
        if (SOAP_OK == result) {
            if (soap->error) {
                soap_perror(soap, "ProbeMatches");
            } else {                                                            // 成功接收到设备的应答消息
                dump__wsdd__ProbeMatches(&rep);

                if (NULL != rep.wsdd__ProbeMatches) {
                    count += rep.wsdd__ProbeMatches->__sizeProbeMatch;
                    for(i = 0; i < rep.wsdd__ProbeMatches->__sizeProbeMatch; i++) {
                        probeMatch = rep.wsdd__ProbeMatches->ProbeMatch + i;
                        if (NULL != cb) {
                            cb(probeMatch->XAddrs);                             // 使用设备服务地址执行函数回调
                        }
                    }
                }
            }
        } else if (soap->error) {
            break;
        }
    }

    SOAP_DBGLOG("\ndetect end! It has detected %d devices!\n", count);

    if (NULL != soap) {
        ONVIF_soap_delete(soap);
    }

    return ;
}

/************************************************************************
**函数：ONVIF_GetProfiles
**功能：获取设备的音视频码流配置信息
**参数：
        [in] MediaXAddr - 媒体服务地址
        [out] profiles  - 返回的设备音视频码流配置信息列表，调用者有责任使用free释放该缓存
**返回：
        返回设备可支持的码流数量（通常是主/辅码流），即使profiles列表个数
**备注：
        1). 注意：一个码流（如主码流）可以包含视频和音频数据，也可以仅仅包含视频数据。
************************************************************************/
int ONVIF_GetProfiles(const char *MediaXAddr, struct tagProfile **profiles)
{
    int i = 0;
    int result = 0;
    struct soap *soap = NULL;
    struct _trt__GetProfiles            req;
    struct _trt__GetProfilesResponse    rep;

    SOAP_ASSERT(NULL != MediaXAddr);

	soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT);
	SOAP_ASSERT(NULL != soap);
    //SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

    ONVIF_SetAuthInfo(soap, USERNAME, PASSWORD);

    memset(&req, 0x00, sizeof(req));
    memset(&rep, 0x00, sizeof(rep));
    result = soap_call___trt__GetProfiles(soap, MediaXAddr, NULL, &req, &rep);
    SOAP_CHECK_ERROR(result, soap, "GetProfiles");

    dump_trt__GetProfilesResponse(&rep);

    if (rep.__sizeProfiles > 0) {                                               // 分配缓存
        (*profiles) = (struct tagProfile *)malloc(rep.__sizeProfiles * sizeof(struct tagProfile));
        SOAP_ASSERT(NULL != (*profiles));
        memset((*profiles), 0x00, rep.__sizeProfiles * sizeof(struct tagProfile));
    }

    for(i = 0; i < rep.__sizeProfiles; i++) {                                   // 提取所有配置文件信息（我们所关心的）
        struct tt__Profile *ttProfile = &rep.Profiles[i];
        struct tagProfile *plst = &(*profiles)[i];

        if (NULL != ttProfile->token) {                                         // 配置文件Token
            strncpy(plst->token, ttProfile->token, sizeof(plst->token) - 1);
        }

        if (NULL != ttProfile->VideoEncoderConfiguration) {                     // 视频编码器配置信息
            if (NULL != ttProfile->VideoEncoderConfiguration->token) {          // 视频编码器Token
                strncpy(plst->venc.token, ttProfile->VideoEncoderConfiguration->token, sizeof(plst->venc.token) - 1);
            }
            if (NULL != ttProfile->VideoEncoderConfiguration->Resolution) {     // 视频编码器分辨率
                plst->venc.Width  = ttProfile->VideoEncoderConfiguration->Resolution->Width;
                plst->venc.Height = ttProfile->VideoEncoderConfiguration->Resolution->Height;
            }
        }
    }

EXIT:

    if (NULL != soap) {
        ONVIF_soap_delete(soap);
    }

    return rep.__sizeProfiles;
}

/************************************************************************
**函数：ONVIF_GetCapabilities
**功能：获取设备能力信息
**参数：
        [in] DeviceXAddr - 设备服务地址
        [out] capa       - 返回设备能力信息信息
**返回：
        0表明成功，非0表明失败
**备注：
    1). 其中最主要的参数之一是媒体服务地址
************************************************************************/
int ONVIF_GetCapabilities(const char *DeviceXAddr, struct tagCapabilities *capa)
{
    int result = 0;
    struct soap *soap = NULL;
    struct _tds__GetCapabilities            req;
    struct _tds__GetCapabilitiesResponse    rep;

    SOAP_ASSERT(NULL != DeviceXAddr);
    SOAP_ASSERT(NULL != capa);
	soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT);
	SOAP_ASSERT(NULL != soap);
    //SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

    ONVIF_SetAuthInfo(soap, USERNAME, PASSWORD);

    memset(&req, 0x00, sizeof(req));
    memset(&rep, 0x00, sizeof(rep));
    result = soap_call___tds__GetCapabilities(soap, DeviceXAddr, NULL, &req, &rep);
    SOAP_CHECK_ERROR(result, soap, "GetCapabilities");

    dump_tds__GetCapabilitiesResponse(&rep);

    memset(capa, 0x00, sizeof(struct tagCapabilities));
    if (NULL != rep.Capabilities) {
        if (NULL != rep.Capabilities->Media) {
            if (NULL != rep.Capabilities->Media->XAddr) {
                strncpy(capa->MediaXAddr, rep.Capabilities->Media->XAddr, sizeof(capa->MediaXAddr) - 1);
            }
        }
        if (NULL != rep.Capabilities->Events) {
            if (NULL != rep.Capabilities->Events->XAddr) {
                strncpy(capa->EventXAddr, rep.Capabilities->Events->XAddr, sizeof(capa->EventXAddr) - 1);
            }
        }
    }

EXIT:

    if (NULL != soap) {
        ONVIF_soap_delete(soap);
    }

    return result;
}

/************************************************************************
**函数：make_uri_withauth
**功能：构造带有认证信息的URI地址
**参数：
        [in]  src_uri       - 未带认证信息的URI地址
        [in]  username      - 用户名
        [in]  password      - 密码
        [out] dest_uri      - 返回的带认证信息的URI地址
        [in]  size_dest_uri - dest_uri缓存大小
**返回：
        0成功，非0失败
**备注：
    1). 例子：
    无认证信息的uri：rtsp://100.100.100.140:554/av0_0
    带认证信息的uri：rtsp://username:password@100.100.100.140:554/av0_0
************************************************************************/
int make_uri_withauth(char *src_uri, char *username, char *password, char *dest_uri, unsigned int size_dest_uri)
{
    int result = 0;
    unsigned int needBufSize = 0;

    SOAP_ASSERT(NULL != src_uri);
    SOAP_ASSERT(NULL != username);
    SOAP_ASSERT(NULL != password);
    SOAP_ASSERT(NULL != dest_uri);
    memset(dest_uri, 0x00, size_dest_uri);

    needBufSize = strlen(src_uri) + strlen(username) + strlen(password) + 3;    // 检查缓存是否足够，包括‘:’和‘@’和字符串结束符
    if (size_dest_uri < needBufSize) {
        SOAP_DBGERR("dest uri buf size is not enough.\n");
        result = -1;
        goto EXIT;
    }

    if (0 == strlen(username) && 0 == strlen(password)) {                       // 生成新的uri地址
        strcpy(dest_uri, src_uri);
    } else {
        char *p = strstr(src_uri, "//");
        if (NULL == p) {
            SOAP_DBGERR("can't found '//', src uri is: %s.\n", src_uri);
            result = -1;
            goto EXIT;
        }
        p += 2;

        memcpy(dest_uri, src_uri, p - src_uri);
        sprintf(dest_uri + strlen(dest_uri), "%s:%s@", username, password);
        strcat(dest_uri, p);
    }

EXIT:

    return result;
}
