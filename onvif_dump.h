#ifndef __ONVIF_DUMP_H__
#define __ONVIF_DUMP_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//va_start
#include <stdarg.h>
#include "soapH.h"

/* 用于打开ONVIF日志信息输出 */
#define ONVIF_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ONVIF_DEBUG

void dump__wsdd__ProbeMatches(struct __wsdd__ProbeMatches *rep);
void dump_tds__GetCapabilitiesResponse(struct _tds__GetCapabilitiesResponse *rep);
void dump_tds__GetDeviceInformationResponse(struct _tds__GetDeviceInformationResponse *rep);
void dump_trt__GetProfilesResponse(struct _trt__GetProfilesResponse *rep);
void dump_trt__GetStreamUriResponse(struct _trt__GetStreamUriResponse *rep);
void dump_tds__GetServicesResponse(struct _tds__GetServicesResponse *rep);
void dump_tds__GetSystemDateAndTime(struct _tds__GetSystemDateAndTimeResponse *rep);
void dump_trt__GetOSDOptions(struct _trt__GetOSDOptionsResponse *rep);
void dump_trt__GetOSDs(struct _trt__GetOSDsResponse *rep);
void dump_trt__GetVideoSourcesResponse(struct _trt__GetVideoSourcesResponse *rep);
void dump_trt__GetVideoSourceConfigurationsResponse(struct _trt__GetVideoSourceConfigurationsResponse *rep);
void dump_trt__GetSnapshotUriResponse(struct _trt__GetSnapshotUriResponse *rep);
void dump_tds__GetNetworkInterfaces(struct _tds__GetNetworkInterfacesResponse *rep);
void dump_trt__GetServiceCapabilitiesResponse(struct _trt__GetServiceCapabilitiesResponse *rep);
void dump_trt__GetVideoEncoderConfigurationResponse(struct _trt__GetVideoEncoderConfigurationResponse *rep);
void dump_trt__GetVideoEncoderConfigurationsResponse(struct _trt__GetVideoEncoderConfigurationsResponse *rep);
void dump_trt__GetCompatibleVideoEncoderConfigurationsResponse(struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *rep);
void dump_trt__GetVideoEncoderConfigurationOptionsResponse(struct _trt__GetVideoEncoderConfigurationOptionsResponse *rep);

#else

#define dump__wsdd__ProbeMatches
#define dump_tds__GetCapabilitiesResponse
#define dump_tds__GetDeviceInformationResponse
#define dump_trt__GetProfilesResponse
#define dump_trt__GetStreamUriResponse
#define dump_tds__GetServicesResponse
#define dump_tds__GetSystemDateAndTime
#define dump_trt__GetOSDOptions
#define dump_trt__GetOSDs
#define dump_trt__GetVideoSourcesResponse
#define dump_trt__GetVideoSourceConfigurationsResponse
#define dump_trt__GetSnapshotUriResponse
#define dump_tds__GetNetworkInterfaces
#define dump_trt__GetServiceCapabilitiesResponse
#define dump_trt__GetVideoEncoderConfigurationResponse
#define dump_trt__GetVideoEncoderConfigurationsResponse
#define dump_trt__GetCompatibleVideoEncoderConfigurationsResponse
#define dump_trt__GetVideoEncoderConfigurationOptionsResponse

#endif

#ifdef __cplusplus
}
#endif

#endif
