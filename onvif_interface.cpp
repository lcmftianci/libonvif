#include "onvif_interface.h"
#include "onvif_comm.h"
#include "onvif_dump.h"

void MYONVIF_DetectDevice(void(*cb)(char *DeviceXAddr))
{
	ONVIF_DetectDevice(cb);
}

int MYONVIF_GetCapabilities(const char *DeviceXAddr, struct tagCapabilities *capa)
{
	return ONVIF_GetCapabilities(DeviceXAddr, capa);
}

//»ñÈ¡rtspÂ·¾¶
int my_make_uri_withauth(char *src_uri, char *username, char *password, char *dest_uri, unsigned int size_dest_uri)
{
	return make_uri_withauth(src_uri, username, password, dest_uri, size_dest_uri);
}

int MYONVIF_GetProfiles(const char *MediaXAddr, struct tagProfile **profiles)
{
	return ONVIF_GetProfiles(MediaXAddr, profiles);
}

#if 1
int MYONVIF_GetStreamUri(const char *MediaXAddr, char *ProfileToken, char *uri, unsigned int sizeuri)
{
	int result = 0;
	struct soap *soap = NULL;
	struct tt__StreamSetup              ttStreamSetup;
	struct tt__Transport                ttTransport;
	struct _trt__GetStreamUri           req;
	struct _trt__GetStreamUriResponse   rep;

	SOAP_ASSERT(NULL != MediaXAddr);
	SOAP_ASSERT(NULL != uri);
	memset(uri, 0x00, sizeuri);
	soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT);
	SOAP_ASSERT(NULL != soap);
	//SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

	memset(&req, 0x00, sizeof(req));
	memset(&rep, 0x00, sizeof(rep));
	memset(&ttStreamSetup, 0x00, sizeof(ttStreamSetup));
	memset(&ttTransport, 0x00, sizeof(ttTransport));
	ttStreamSetup.Stream = tt__StreamType__RTP_Unicast;
	ttStreamSetup.Transport = &ttTransport;
	ttStreamSetup.Transport->Protocol = tt__TransportProtocol__RTSP;
	ttStreamSetup.Transport->Tunnel = NULL;
	req.StreamSetup = &ttStreamSetup;
	req.ProfileToken = ProfileToken;

	ONVIF_SetAuthInfo(soap, USERNAME, PASSWORD);
	result = soap_call___trt__GetStreamUri(soap, MediaXAddr, NULL, &req, &rep);
	SOAP_CHECK_ERROR(result, soap, "GetServices");

	dump_trt__GetStreamUriResponse(&rep);

	result = -1;
	if (NULL != rep.MediaUri) {
		if (NULL != rep.MediaUri->Uri) {
			if (sizeuri > strlen(rep.MediaUri->Uri)) {
				strcpy(uri, rep.MediaUri->Uri);
				result = 0;
			}
			else {
				SOAP_DBGERR("Not enough cache!\n");
			}
		}
	}

EXIT:

	if (NULL != soap) {
		ONVIF_soap_delete(soap);
	}

	return result;
	//return ONVIF_GetStreamUri(MediaXAddr, ProfileToken, uri, sizeuri);
}
#endif
