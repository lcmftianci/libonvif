#ifndef _ONVIF_INTERFACE_H__
#define _ONVIF_INTERFACE_H__

#define MY_SOAP_SOCK_TIMEOUT    (10)                                               // socket超时时间（单秒秒）

#define MY_ONVIF_ADDRESS_SIZE   (128)                                              // URI地址长度
#define MY_ONVIF_TOKEN_SIZE     (65)                                               // token长度

#ifdef _WIN32
#define EXPORT_ONVIF _declspec(dllexport)
#else
#define EXPORT_ONVIF
#endif

EXPORT_ONVIF void MYONVIF_DetectDevice(void(*cb)(char *DeviceXAddr));

EXPORT_ONVIF int MYONVIF_GetCapabilities(const char *DeviceXAddr, struct tagCapabilities *capa);

EXPORT_ONVIF int my_make_uri_withauth(char *src_uri, char *username, char *password, char *dest_uri, unsigned int size_dest_uri);

EXPORT_ONVIF int MYONVIF_GetProfiles(const char *MediaXAddr, struct tagProfile **profiles);
#if 1
EXPORT_ONVIF int MYONVIF_GetStreamUri(const char *MediaXAddr, char *ProfileToken, char *uri, unsigned int sizeuri);
#endif
#endif
