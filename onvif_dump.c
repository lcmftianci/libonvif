#include "onvif_dump.h"

#ifdef ONVIF_DEBUG

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

enum LOG_TYPE {
    log_pos = 0,                        // 打印地址值
    log_int,                            // 打印整形
    log_str,                            // 打印字符串(char *)
    log_strp,                           // 打印字符串(char **)
    log_float,                          // 打印浮点数
    log_bool,                           // 打印布尔值
    log_int64,                          // 打印64位整形
};
#define INDENTATION         3           // 打印时每层缩进的空格数

#define log_fmt             printf
#define log_func_in         log_fmt("\n================= + %s + >>>\n", __FUNCTION__)
#define log_func_out        log_fmt("================= - %s - <<<\n", __FUNCTION__)

static void log_val(const char *prestr, enum LOG_TYPE type, const void *val)
{
#ifdef WIN32
#define POSFORMAT   "(0x%p)"
#else
#define POSFORMAT   "(%p)"
#endif

    if (NULL == val) {
        log_fmt("%s(null)\n", prestr);
        return ;
    }

    switch (type)
    {
    case log_pos:
        log_fmt("%s" POSFORMAT "\n", prestr, val);
        break;
    case log_int:
        log_fmt("%s%d\n", prestr, *((int*)val));
        break;
    case log_str:
        log_fmt("%s%s\n", prestr, (char*)val);
        break;
    case log_strp:
        log_fmt("%s%s\n", prestr, *((char**)val));
        break;
    case log_float:
        log_fmt("%s%f\n", prestr, *((float*)val));
        break;
    case log_bool:
        log_fmt("%s%s\n", prestr, (*((enum xsd__boolean*)val)) ? "true" : "false" );
        break;
    case log_int64:
        log_fmt("%s%lld\n", prestr, *((long long*)val));
        break;
    default:
        assert(0);
        break;
    }

    return ;
}

static void log_indentation(unsigned int level)
{
    if (level > 0) {
        log_fmt("%*s%s", level * INDENTATION, "", "|- ");
    }
    return ;
}

/************************************************************************
**函数：log_level_fmt
**功能：打印具有层级结构的日志信息
**参数：
        [in] level  - 层级
        [in] fmt    - 格式化字符串
**返回：
**备注：
************************************************************************/
static void log_level_fmt(unsigned int level, const char *fmt, ...)
{
    va_list ap;

    log_indentation(level);

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    return ;
}

/************************************************************************
**函数：log_level
**功能：打印具有层级结构的日志信息
**参数：
        [in] level  - 层级
        [in] prestr - 附加的前缀日志信息字符串
        [in] type   - 要打印的参数类型
        [in] val    - 要打印的参数地址
**返回：
**备注：
************************************************************************/
static void log_level_val(unsigned int level, const char *prestr, enum LOG_TYPE type, const void *val)
{
    log_indentation(level);
    log_val(prestr, type, val);
    return ;
}

static const char *dump_enum2str_OSDType(enum tt__OSDType t)
{
    switch(t) {
    case tt__OSDType__Text:     return "Text";
    case tt__OSDType__Image:    return "Image";
    case tt__OSDType__Extended: return "Extended";
    }
    return "unknown";
}

static const char *dump_enum2str_VideoEncoding(enum tt__VideoEncoding e)
{
    switch(e) {
    case tt__VideoEncoding__JPEG:  return "JPEG";
    case tt__VideoEncoding__MPEG4: return "MPEG4";
    case tt__VideoEncoding__H264:  return "H264";
    }
    return "unknown";
}

static const char *dump_enum2str_AudioEncoding(enum tt__AudioEncoding e)
{
    switch (e) {
    case tt__AudioEncoding__G711: return "G711";
    case tt__AudioEncoding__G726: return "G726";
    case tt__AudioEncoding__AAC:  return "AAC";
    }
    return "unknown";
}

static const char *dump_enum2str_H264Profile(enum tt__H264Profile e)
{
    switch (e) {
    case tt__H264Profile__Baseline: return "Baseline";
    case tt__H264Profile__Main:     return "Main";
    case tt__H264Profile__Extended: return "Extended";
    case tt__H264Profile__High:     return "High";
    }
    return "unknown";
}

static void log_level_struct_tt__IntRange(unsigned int level, struct tt__IntRange *range)
{
    if (NULL != range) {
        log_level_val(level, "Min: ", log_int,  &range->Min);
        log_level_val(level, "Max: ", log_int,  &range->Max);
    }
}

static void log_level_struct_tt__VideoEncoderConfiguration(unsigned int level, struct tt__VideoEncoderConfiguration *vecfg)
{
    if (NULL == vecfg) {
        return;
    }

    log_level_val(level, "Name: ",                          log_str,    vecfg->Name);
    log_level_val(level, "UseCount: ",                      log_int,   &vecfg->UseCount);
    log_level_val(level, "token: ",                         log_str,    vecfg->token);
    log_level_val(level, "Encoding: ",                      log_str,    dump_enum2str_VideoEncoding(vecfg->Encoding));
    log_level_val(level, "Resolution: ",                    log_pos,    vecfg->Resolution);
    if (NULL != vecfg->Resolution) {
        log_level_val(level + 1, "Width: ",                 log_int,   &vecfg->Resolution->Width);
        log_level_val(level + 1, "Height: ",                log_int,   &vecfg->Resolution->Height);
    }
    log_level_val(level, "Quality: ",                       log_float, &vecfg->Quality);
    log_level_val(level, "RateControl: ",                   log_pos,    vecfg->RateControl);
    if (NULL !=  vecfg->RateControl) {
        log_level_val(level + 1, "FrameRateLimit: ",        log_int,   &vecfg->RateControl->FrameRateLimit);
        log_level_val(level + 1, "EncodingInterval: ",      log_int,   &vecfg->RateControl->EncodingInterval);
        log_level_val(level + 1, "BitrateLimit: ",          log_int,   &vecfg->RateControl->BitrateLimit);
    }
    log_level_val(level, "MPEG4: ",                         log_pos,    vecfg->MPEG4);
    log_level_val(level, "H264: ",                          log_pos,    vecfg->H264);
    log_level_val(level, "Multicast: ",                     log_pos,    vecfg->Multicast);
    log_level_val(level, "SessionTimeout: ",                log_int64, &vecfg->SessionTimeout);
}

static void log_level_struct_wsa__EndpointReferenceType(unsigned int level, struct wsa__EndpointReferenceType *a)
{
    int i;

    if (NULL == a) {
        return;
    }

    log_level_val(level, "Address: ",               log_str,    a->Address);
    log_level_val(level, "ReferenceProperties: ",   log_pos,    a->ReferenceProperties);
    if (NULL != a->ReferenceProperties) {
        log_level_val(level + 1, "__size: ",        log_int,   &a->ReferenceProperties->__size);
        log_level_val(level + 1, "__any: ",         log_pos,    a->ReferenceProperties->__any);
        for (i = 0; i < a->ReferenceProperties->__size; i++) {
            log_level_val(level + 2, "",            log_str,    a->ReferenceProperties->__any[i]);
        }
    }
    log_level_val(level, "ReferenceParameters: ",   log_pos,    a->ReferenceParameters);
    if (NULL != a->ReferenceParameters) {
        log_level_val(level + 1, "__size: ",        log_int,   &a->ReferenceParameters->__size);
        log_level_val(level + 1, "__any: ",         log_pos,    a->ReferenceParameters->__any);
        for (i = 0; i < a->ReferenceParameters->__size; i++) {
            log_level_val(level + 2, "",            log_str,    a->ReferenceParameters->__any[i]);
        }
    }
    log_level_val(level, "PortType: ",              log_strp,   a->PortType);
    log_level_val(level, "ServiceName: ",           log_pos,    a->ServiceName);
    if (NULL != a->ServiceName) {
        log_level_val(level + 1, "ServiceName: ",   log_str,    a->ServiceName->__item);
        log_level_val(level + 1, "PortName: ",      log_str,    a->ServiceName->PortName);
        log_level_val(level + 1, "__anyAttribute: ",log_str,    a->ServiceName->__anyAttribute);
    }
    log_level_val(level, "__size: ",                log_int,   &a->__size);
    log_level_val(level, "__any: ",                 log_pos,    a->__any);
    for (i = 0; i < a->__size; i++) {
        log_level_val(level + 1, "",                log_str,    a->__any[i]);
    }
    log_level_val(level, "__anyAttribute: ",        log_str,    a->__anyAttribute);
}

void dump__wsdd__ProbeMatches(struct __wsdd__ProbeMatches *rep)
{
    int i;
    struct wsdd__ProbeMatchType* probeMatch;

    assert(NULL != rep);
    log_func_in;
    log_level_val(0, "wsdd__ProbeMatches: ",                log_pos,    rep->wsdd__ProbeMatches);
    if (NULL != rep->wsdd__ProbeMatches) {
        log_level_val(1, "__sizeProbeMatch: ",              log_int,   &rep->wsdd__ProbeMatches->__sizeProbeMatch);
        log_level_val(1, "ProbeMatch: ",                    log_pos,   &rep->wsdd__ProbeMatches->ProbeMatch);
        for(i = 0; i < rep->wsdd__ProbeMatches->__sizeProbeMatch; i++) {
            probeMatch = rep->wsdd__ProbeMatches->ProbeMatch + i;
            log_level_fmt(2, "%d\n", i);
            log_level_val(3, "wsa__EndpointReference: ",    log_pos,   &probeMatch->wsa__EndpointReference);
            log_level_struct_wsa__EndpointReferenceType(4,             &probeMatch->wsa__EndpointReference);
            log_level_val(3, "Types: ",                     log_str,    probeMatch->Types);
            log_level_val(3, "Scopes: ",                    log_pos,    probeMatch->Scopes);
            if (NULL != probeMatch->Scopes) {
                log_level_val(4, "__item: ",                log_str,    probeMatch->Scopes->__item);
                log_level_val(4, "MatchBy: ",               log_str,    probeMatch->Scopes->MatchBy);
            }
            log_level_val(3, "XAddrs: ",                    log_str,    probeMatch->XAddrs);
            log_level_val(3, "MetadataVersion: ",           log_int,   &probeMatch->MetadataVersion);
        }
    }
    log_func_out;
}

static void log_level_struct_tt__SystemCapabilities(unsigned int level, struct tt__SystemCapabilities *a)
{
    int i;
    if (NULL == a) {
        return;
    }

    log_level_val(level, "DiscoveryResolve: ",              log_bool,   &a->DiscoveryResolve);
    log_level_val(level, "DiscoveryBye: ",                  log_bool,   &a->DiscoveryBye);
    log_level_val(level, "RemoteDiscovery: ",               log_bool,   &a->RemoteDiscovery);
    log_level_val(level, "SystemBackup: ",                  log_bool,   &a->SystemBackup);
    log_level_val(level, "SystemLogging: ",                 log_bool,   &a->SystemLogging);
    log_level_val(level, "FirmwareUpgrade: ",               log_bool,   &a->FirmwareUpgrade);
    log_level_val(level, "__sizeSupportedVersions: ",       log_int,    &a->__sizeSupportedVersions);
    log_level_val(level, "SupportedVersions: ",             log_pos,     a->SupportedVersions);
    for (i = 0; i < a->__sizeSupportedVersions; i++) {
        log_level_fmt(level + 1, "%d.%d\n",                              a->SupportedVersions[i].Major,
                                                                         a->SupportedVersions[i].Minor);
    }
}

static void log_level_struct_tt__DeviceCapabilities(unsigned int level, struct tt__DeviceCapabilities *a)
{
    if (NULL == a) {
        return;
    }

    log_level_val(level, "XAddr: ",             log_str, a->XAddr);
    log_level_val(level, "Network: ",           log_pos, a->Network);
    log_level_val(level, "System: ",            log_pos, a->System);
    log_level_struct_tt__SystemCapabilities(level + 1,   a->System);
    log_level_val(level, "IO: ",                log_pos, a->IO);
    log_level_val(level, "Security: ",          log_pos, a->Security);
    log_level_val(level, "Extension: ",         log_pos, a->Extension);
}

static void log_level_struct_tt__MediaCapabilities(unsigned int level, struct tt__MediaCapabilities *a)
{
    if (NULL == a) {
        return;
    }

    log_level_val(level, "XAddr: ",                 log_str, a->XAddr);
    log_level_val(level, "StreamingCapabilities: ", log_pos, a->StreamingCapabilities);
    log_level_val(level, "Extension: ",             log_pos, a->Extension);
}

void dump_tds__GetCapabilitiesResponse(struct _tds__GetCapabilitiesResponse *rep)
{
    assert(NULL != rep);
    log_func_in;
    log_level_val(0, "Capabilities: ",      log_pos, rep->Capabilities);
    if (NULL != rep->Capabilities) {
        log_level_val(1, "Analytics: ",     log_pos, rep->Capabilities->Analytics);
        if (NULL != rep->Capabilities->Analytics) {
            log_level_val(2, "XAddr: ",     log_str, rep->Capabilities->Analytics->XAddr);
        }

        log_level_val(1, "Device: ",        log_pos, rep->Capabilities->Device);
        if (NULL != rep->Capabilities->Device) {
            log_level_val(2, "XAddr: ",     log_str, rep->Capabilities->Device->XAddr);
        }

        log_level_val(1, "Events: ",        log_pos, rep->Capabilities->Events);
        if (NULL != rep->Capabilities->Events) {
            log_level_val(2, "XAddr: ",     log_str, rep->Capabilities->Events->XAddr);
        }

        log_level_val(1, "Imaging: ",       log_pos, rep->Capabilities->Imaging);
        if (NULL != rep->Capabilities->Imaging) {
            log_level_val(2, "XAddr: ",     log_str, rep->Capabilities->Imaging->XAddr);
        }

        log_level_val(1, "Media: ",         log_pos, rep->Capabilities->Media);
        if (NULL != rep->Capabilities->Media) {
            log_level_val(2, "XAddr: ",     log_str, rep->Capabilities->Media->XAddr);
        }

        log_level_val(1, "PTZ: ",           log_pos, rep->Capabilities->PTZ);
        if (NULL != rep->Capabilities->PTZ) {
            log_level_val(2, "XAddr: ",     log_str, rep->Capabilities->PTZ->XAddr);
        }

        log_level_val(1, "Extension: ",     log_pos, rep->Capabilities->Extension);
        if (NULL != rep->Capabilities->Extension) {
            log_level_val(2, "DeviceIO: ",  log_pos, rep->Capabilities->Extension->DeviceIO);
            if (NULL != rep->Capabilities->Extension->DeviceIO) {
                log_level_val(3, "XAddr: ", log_str, rep->Capabilities->Extension->DeviceIO->XAddr);
            }

            log_level_val(2, "Display: ",  log_pos, rep->Capabilities->Extension->Display);
            if (NULL != rep->Capabilities->Extension->Display) {
                log_level_val(3, "XAddr: ", log_str, rep->Capabilities->Extension->Display->XAddr);
            }

            log_level_val(2, "Recording: ",  log_pos, rep->Capabilities->Extension->Recording);
            if (NULL != rep->Capabilities->Extension->Recording) {
                log_level_val(3, "XAddr: ", log_str, rep->Capabilities->Extension->Recording->XAddr);
            }
            
            log_level_val(2, "Search: ",  log_pos, rep->Capabilities->Extension->Search);
            if (NULL != rep->Capabilities->Extension->Search) {
                log_level_val(3, "XAddr: ", log_str, rep->Capabilities->Extension->Search->XAddr);
            }

            log_level_val(2, "Replay: ",  log_pos, rep->Capabilities->Extension->Replay);
            if (NULL != rep->Capabilities->Extension->Replay) {
                log_level_val(3, "XAddr: ", log_str, rep->Capabilities->Extension->Replay->XAddr);
            }

            log_level_val(2, "Receiver: ",  log_pos, rep->Capabilities->Extension->Receiver);
            if (NULL != rep->Capabilities->Extension->Receiver) {
                log_level_val(3, "XAddr: ", log_str, rep->Capabilities->Extension->Receiver->XAddr);
            }

            log_level_val(2, "AnalyticsDevice: ",  log_pos, rep->Capabilities->Extension->AnalyticsDevice);
            if (NULL != rep->Capabilities->Extension->AnalyticsDevice) {
                log_level_val(3, "XAddr: ", log_str, rep->Capabilities->Extension->AnalyticsDevice->XAddr);
            }
        }
    }
    log_func_out;
}

void dump_tds__GetDeviceInformationResponse(struct _tds__GetDeviceInformationResponse *rep)
{
    assert(NULL != rep);
    log_func_in;
    log_val("Manufacturer:       ", log_str, rep->Manufacturer);
    log_val("Model:              ", log_str, rep->Model);
    log_val("Serial Number:      ", log_str, rep->SerialNumber);
    log_val("Hardware Id:        ", log_str, rep->HardwareId);
    log_val("Firmware Version:   ", log_str, rep->FirmwareVersion);
    log_func_out;
}

void dump_trt__GetProfilesResponse(struct _trt__GetProfilesResponse *rep)
{
    int i;

    assert(NULL != rep);

    log_func_in;

    log_level_val(0, "sizeProfiles: ", log_int, &rep->__sizeProfiles);
    log_level_val(0, "Profiles: ",     log_pos,  rep->Profiles);
    for(i=0; i<rep->__sizeProfiles; i++) {
        struct tt__Profile *profiles = &rep->Profiles[i];
        log_level_fmt(1, "%d\n", i);
        log_level_val(2, "Name: ",                     log_str,  profiles->Name);
        log_level_val(2, "token: ",                    log_str,  profiles->token);
        log_level_val(2, "fixed: ",                    log_bool, profiles->fixed);
        log_level_val(2, "VideoSourceConfiguration: ", log_pos,  profiles->VideoSourceConfiguration);
        if (NULL != profiles->VideoSourceConfiguration) {
            struct tt__VideoSourceConfiguration *vsc = profiles->VideoSourceConfiguration;

            log_level_val(3, "Name: ",        log_str,  vsc->Name);
            log_level_val(3, "UseCount: ",    log_int, &vsc->UseCount);
            log_level_val(3, "token: ",       log_str,  vsc->token);
            log_level_val(3, "SourceToken: ", log_str,  vsc->SourceToken);
            log_level_val(3, "Bounds: ",      log_pos,  vsc->Bounds);
            if (NULL != vsc->Bounds) {
                log_level_val(4, "x: ",       log_int, &vsc->Bounds->x);
                log_level_val(4, "y: ",       log_int, &vsc->Bounds->y);
                log_level_val(4, "width: ",   log_int, &vsc->Bounds->width);
                log_level_val(4, "height: ",  log_int, &vsc->Bounds->height);
            }
        }
        log_level_val(2, "VideoEncoderConfiguration: ", log_pos, profiles->VideoEncoderConfiguration);
        if (NULL != profiles->VideoEncoderConfiguration) {
            struct tt__VideoEncoderConfiguration *vec = profiles->VideoEncoderConfiguration;
            log_level_val(3, "Name: ",       log_str,    vec->Name);
            log_level_val(3, "UseCount: ",   log_int,   &vec->UseCount);
            log_level_val(3, "token: ",      log_str,    vec->token);
            log_level_val(3, "Encoding: ",   log_str,    dump_enum2str_VideoEncoding(vec->Encoding));
            log_level_val(3, "Quality: ",    log_float, &vec->Quality);
            log_level_val(3, "Resolution: ", log_pos,    vec->Resolution);
            if (NULL != vec->Resolution) {
                log_level_val(4, "Width: ",  log_int,   &vec->Resolution->Width);
                log_level_val(4, "Height: ", log_int,   &vec->Resolution->Height);
            }
            log_level_val(3, "RateControl: ",log_pos,    vec->RateControl);
            if (NULL !=  vec->RateControl) {
                log_level_val(4, "FrameRateLimit: ",   log_int, &vec->RateControl->FrameRateLimit);
                log_level_val(4, "EncodingInterval: ", log_int, &vec->RateControl->EncodingInterval);
                log_level_val(4, "BitrateLimit: ",     log_int, &vec->RateControl->BitrateLimit);
            }
        }
        log_level_val(2, "AudioSourceConfiguration: ", log_pos, profiles->AudioSourceConfiguration);
        if (NULL != profiles->AudioSourceConfiguration) {
            struct tt__AudioSourceConfiguration *asc = profiles->AudioSourceConfiguration;
            log_level_val(3, "Name: ",        log_str,  asc->Name);
            log_level_val(3, "UseCount: ",    log_int, &asc->UseCount);
            log_level_val(3, "token: ",       log_str,  asc->token);
            log_level_val(3, "SourceToken: ", log_str,  asc->SourceToken);
        }
        log_level_val(2, "AudioEncoderConfiguration: ", log_pos, profiles->AudioEncoderConfiguration);
        if (NULL != profiles->AudioEncoderConfiguration) {
            struct tt__AudioEncoderConfiguration *aec = profiles->AudioEncoderConfiguration;
            log_level_val(3, "Name: ",       log_str,  aec->Name);
            log_level_val(3, "UseCount: ",   log_int, &aec->UseCount);
            log_level_val(3, "token: ",      log_str,  aec->token);
            log_level_val(3, "Bitrate: ",    log_int, &aec->Bitrate);
            log_level_val(3, "SampleRate: ", log_int, &aec->SampleRate);
            log_level_val(3, "Encoding: ",   log_str,  dump_enum2str_AudioEncoding(aec->Encoding));
        }

    }
    log_func_out;
}

void dump_trt__GetStreamUriResponse(struct _trt__GetStreamUriResponse *rep)
{
    assert(NULL != rep);
    log_func_in;
    log_level_val(0, "MediaUri: ", log_pos, rep->MediaUri);
    if (NULL != rep->MediaUri) {
        log_level_val(1, "Uri:                 ", log_str,    rep->MediaUri->Uri);
        log_level_val(1, "InvalidAfterConnect: ", log_bool,  &rep->MediaUri->InvalidAfterConnect);
        log_level_val(1, "InvalidAfterReboot:  ", log_bool,  &rep->MediaUri->InvalidAfterReboot);
        log_level_val(1, "Timeout:             ", log_int64, &rep->MediaUri->Timeout);
    }
    log_func_out;
}

void dump_tds__GetServicesResponse(struct _tds__GetServicesResponse *rep)
{
    int i;

    assert(NULL != rep);
    log_func_in;
    log_level_val(0, "sizeService: ",           log_int,   &rep->__sizeService);
    log_level_val(0, "Service: ",               log_pos,    rep->Service);
    if (NULL != rep->Service) {
        for(i = 0; i < rep->__sizeService; i++) {
            struct tds__Service *s = rep->Service + i;
            log_level_fmt(1, "%d\n", i);
            log_level_val(2, "Namespace: ",     log_str,    s->Namespace);
            log_level_val(2, "XAddr: ",         log_str,    s->XAddr);
            log_level_val(2, "Version: ",       log_pos,    s->Version);
            if (NULL != s->Version) {
                log_level_fmt(3, "%d.%d\n",                 s->Version->Major, s->Version->Minor);
            }
        }
    }
    log_func_out;
}

void dump_tds__GetSystemDateAndTime(struct _tds__GetSystemDateAndTimeResponse *rep)
{
    struct tt__SystemDateTime *systm;

    assert(NULL != rep);
    assert(NULL != rep->SystemDateAndTime);
    systm = rep->SystemDateAndTime;

    log_func_in;
    log_val("DateTimeType:      ", log_int, &systm->DateTimeType);
    log_val("DaylightSavings:   ", log_int, &systm->DaylightSavings);
    if (NULL == systm->TimeZone) {
        log_val("TimeZone:          ", log_pos, systm->TimeZone);
    } else {
        log_val("TimeZone:          ", log_str, systm->TimeZone->TZ);
    }
    if (NULL == systm->UTCDateTime) {
        log_val("UTCDateTime:       ", log_pos, systm->UTCDateTime);
    } else {
        assert(NULL != systm->UTCDateTime->Date);
        assert(NULL != systm->UTCDateTime->Time);
        log_fmt("UTCDateTime:       %04d-%02d-%02d %02d:%02d:%02d \n",
            systm->UTCDateTime->Date->Year,
            systm->UTCDateTime->Date->Month,
            systm->UTCDateTime->Date->Day,
            systm->UTCDateTime->Time->Hour,
            systm->UTCDateTime->Time->Minute,
            systm->UTCDateTime->Time->Second);
    }
    if (NULL == systm->LocalDateTime) {
        log_val("LocalDateTime:     ", log_pos, systm->LocalDateTime);
    } else {
        assert(NULL != systm->LocalDateTime->Date);
        assert(NULL != systm->LocalDateTime->Time);
        log_fmt("LocalDateTime:     %04d-%02d-%02d %02d:%02d:%02d \n",
            systm->LocalDateTime->Date->Year,
            systm->LocalDateTime->Date->Month,
            systm->LocalDateTime->Date->Day,
            systm->LocalDateTime->Time->Hour,
            systm->LocalDateTime->Time->Minute,
            systm->LocalDateTime->Time->Second);
    }
    log_func_out;
}

void dump_trt__GetOSDOptions(struct _trt__GetOSDOptionsResponse *rep)
{
    int i;
    struct tt__OSDConfigurationOptions *p;

    assert(NULL != rep);
    assert(NULL != rep->OSDOptions);
    p = rep->OSDOptions;

    log_func_in;

    log_level_val(0, "MaximumNumberOfOSDs: ",   log_pos,    p->MaximumNumberOfOSDs);
    if (NULL != p->MaximumNumberOfOSDs) {
        struct tt__MaximumNumberOfOSDs *m = p->MaximumNumberOfOSDs;
        log_level_val(1, "Total: ",             log_int,    &(m->Total));
        log_level_val(1, "Image: ",             log_int,    m->Image);
        log_level_val(1, "PlainText: ",         log_int,    m->PlainText);
        log_level_val(1, "Date: ",              log_int,    m->Date);
        log_level_val(1, "Time: ",              log_int,    m->Time);
        log_level_val(1, "DateAndTime: ",       log_int,    m->DateAndTime);
    }

    log_level_val(0, "sizeType: ",              log_int,    &p->__sizeType);
    log_level_val(0, "Type: ",                  log_pos,    p->Type);
    for (i = 0; i < p->__sizeType; i++) {
        log_level_fmt(1, "%s\n", dump_enum2str_OSDType(p->Type[i]));
    }

    log_level_val(0, "sizePositionOption: ",    log_int,    &p->__sizePositionOption);
    log_level_val(0, "PositionOption: ",        log_pos,    p->PositionOption);
    for (i = 0; i < p->__sizePositionOption; i++) {
        log_level_fmt(1, "%s\n", p->PositionOption[i]);
    }

    log_level_val(0, "TextOption: ",            log_pos,    p->TextOption);
    if (NULL != p->TextOption) {
        struct tt__OSDTextOptions *to = p->TextOption;
        log_level_val(1, "sizeType: ",          log_int,    &(to->__sizeType));
        log_level_val(1, "Type: ",              log_pos,    to->Type);
        for (i = 0; i < to->__sizeType; i++) {
            log_level_fmt(2, "%s\n", to->Type[i]);
        }

        log_level_val(1, "FontSizeRange: ",     log_pos,    to->FontSizeRange);
        log_level_struct_tt__IntRange(2, to->FontSizeRange);

        log_level_val(1, "sizeDateFormat: ",    log_int,    &(to->__sizeDateFormat));
        log_level_val(1, "DateFormat: ",        log_pos,    to->DateFormat);
        for (i = 0; i < to->__sizeDateFormat; i++) {
            log_level_fmt(2, "%s\n", to->DateFormat[i]);
        }
        
        log_level_val(1, "sizeTimeFormat: ",    log_int,    &(to->__sizeTimeFormat));
        log_level_val(1, "TimeFormat: ",        log_pos,    to->TimeFormat);
        for (i = 0; i < to->__sizeTimeFormat; i++) {
            log_level_fmt(2, "%s\n", to->TimeFormat[i]);
        }

        log_level_val(1, "FontColor: ",         log_pos,    to->FontColor);
        log_level_val(1, "BackgroundColor: ",   log_pos,    to->BackgroundColor);
    }
    
    log_level_val(0, "ImageOption: ",           log_pos,    p->ImageOption);

    log_func_out;
}

void dump_trt__GetOSDs(struct _trt__GetOSDsResponse *rep)
{
    int i;
    assert(NULL != rep);

    log_func_in;
    log_level_val(0, "sizeOSDs: ",                  log_int,    &rep->__sizeOSDs);
    log_level_val(0, "OSDs: ",                      log_pos,    rep->OSDs);
    for (i = 0; i < rep->__sizeOSDs; i++) {
        struct tt__OSDConfiguration *o = rep->OSDs + i;
        log_level_fmt(1, "%d\n", i);
        log_level_val(2, "token: ",                 log_str,    o->token);
        log_level_val(2, "VideoSourceConfigurationToken: ", log_pos, o->VideoSourceConfigurationToken);
        if (NULL != o->VideoSourceConfigurationToken) {
            log_level_val(3, "item: ",              log_str,    o->VideoSourceConfigurationToken->__item);
        }
        
        log_level_val(2, "Type: ",                  log_str,    dump_enum2str_OSDType(o->Type));
        log_level_val(2, "Position: ",              log_pos,    o->Position);
        if (NULL != o->Position) {
            log_level_val(3, "Type: ",              log_str,    o->Position->Type);
            log_level_val(3, "Pos: ",               log_pos,    o->Position->Pos);
            if (NULL != o->Position->Pos) {
                log_level_val(4, "x: ",             log_float,  o->Position->Pos->x);
                log_level_val(4, "y: ",             log_float,  o->Position->Pos->y);
            }
        }

        log_level_val(2, "TextString: ",            log_pos,    o->TextString);
        if (NULL != o->TextString) {
            log_level_val(3, "Type: ",              log_str,    o->TextString->Type);
            log_level_val(3, "DateFormat: ",        log_str,    o->TextString->DateFormat);
            log_level_val(3, "TimeFormat: ",        log_str,    o->TextString->TimeFormat);
            log_level_val(3, "FontSize: ",          log_int,    o->TextString->FontSize);
            log_level_val(3, "PlainText: ",         log_str,    o->TextString->PlainText);
            log_level_val(3, "FontColor: ",         log_pos,    o->TextString->FontColor);
            log_level_val(3, "BackgroundColor: ",   log_pos,    o->TextString->BackgroundColor);
        }

        log_level_val(2, "Image: ",                 log_pos,    o->Image);
    }
    log_func_out;
}

void dump_trt__GetVideoSourcesResponse(struct _trt__GetVideoSourcesResponse *rep)
{
    int i;

    assert(NULL != rep);

    log_func_in;
    log_level_val(0, "sizeVideoSources: ", log_int, &rep->__sizeVideoSources);
    log_level_val(0, "VideoSources: ",     log_pos,  rep->VideoSources);
    for (i = 0; i < rep->__sizeVideoSources; i++) {
        struct tt__VideoSource *vs = rep->VideoSources + i;
        log_level_fmt(1, "%d\n", i);
        log_level_val(2, "token: ",      log_str,    vs->token);
        log_level_val(2, "Framerate: ",  log_float, &vs->Framerate);
        log_level_val(2, "Resolution: ", log_pos,    vs->Resolution);
        log_level_val(3, "Width: ",      log_int,   &vs->Resolution->Width);
        log_level_val(3, "Height: ",     log_int,   &vs->Resolution->Height);
        log_level_val(2, "Imaging: ",    log_pos,    vs->Imaging);
        log_level_val(2, "Extension: ",  log_pos,    vs->Extension);
    }
    log_func_out;
}

void dump_trt__GetVideoSourceConfigurationsResponse(struct _trt__GetVideoSourceConfigurationsResponse *rep)
{
    int i;

    assert(NULL != rep);

    log_func_in;
    log_level_val(0, "sizeConfigurations: ",    log_int,   &rep->__sizeConfigurations);
    log_level_val(0, "Configurations: ",        log_pos,    rep->Configurations);
    for (i = 0; i < rep->__sizeConfigurations; i++) {
        struct tt__VideoSourceConfiguration *cfg = rep->Configurations + i;
        log_level_fmt(1, "%d\n", i);
        log_level_val(2, "Name: ",              log_str,    cfg->Name);
        log_level_val(2, "UseCount: ",          log_int,   &cfg->UseCount);
        log_level_val(2, "token: ",             log_str,    cfg->token);
        log_level_val(2, "SourceToken: ",       log_str,    cfg->SourceToken);
        log_level_val(2, "Bounds: ",            log_pos,    cfg->Bounds);
        log_level_val(3, "x: ",                 log_int,   &cfg->Bounds->x);
        log_level_val(3, "y: ",                 log_int,   &cfg->Bounds->y);
        log_level_val(3, "width: ",             log_int,   &cfg->Bounds->width);
        log_level_val(3, "height: ",            log_int,   &cfg->Bounds->height);
        log_level_val(2, "Extension: ",         log_pos,    cfg->Extension);
    }
    log_func_out;
}

void dump_trt__GetSnapshotUriResponse(struct _trt__GetSnapshotUriResponse *rep)
{
    assert(NULL != rep);

    log_func_in;

    log_level_val(0, "MediaUri: ",                log_pos,    rep->MediaUri);
    if (NULL != rep->MediaUri) {
        log_level_val(1, "Uri: ",                 log_str,    rep->MediaUri->Uri);
        log_level_val(1, "InvalidAfterConnect: ", log_bool,  &rep->MediaUri->InvalidAfterConnect);
        log_level_val(1, "InvalidAfterReboot: ",  log_bool,  &rep->MediaUri->InvalidAfterReboot);
        log_level_val(1, "Timeout: ",             log_int64, &rep->MediaUri->Timeout);
    }

    log_func_out;
}

void dump_tds__GetNetworkInterfaces(struct _tds__GetNetworkInterfacesResponse *rep)
{
    int i, j;
    assert(NULL != rep);

    log_func_in;

    log_level_val(0, "sizeNetworkInterfaces: ",         log_int,  &rep->__sizeNetworkInterfaces);
    log_level_val(0, "NetworkInterfaces: ",             log_pos,   rep->NetworkInterfaces);
    for (i=0; i < rep->__sizeNetworkInterfaces; i++) {
        struct tt__NetworkInterface *ni = rep->NetworkInterfaces + i;
        log_level_fmt(1, "%d\n", i);
        log_level_val(2, "token: ",                     log_str,   ni->token);
        log_level_val(2, "Enabled: ",                   log_bool, &ni->Enabled);
        log_level_val(2, "Info: ",                      log_pos,   ni->Info);
        if (NULL != ni->Info) {
            log_level_val(3, "Name: ",                  log_str,   ni->Info->Name);
            log_level_val(3, "HwAddress: ",             log_str,   ni->Info->HwAddress);
            log_level_val(3, "MTU: ",                   log_int,   ni->Info->MTU);
        }
        log_level_val(1, "Link: ",                      log_pos,   ni->Link);
        if (NULL != ni->Link) {
            log_level_val(3, "AdminSettings: ",         log_pos,   ni->Link->AdminSettings);
            if (NULL != ni->Link->AdminSettings) {
                log_level_val(4, "AutoNegotiation: ",   log_bool, &ni->Link->AdminSettings->AutoNegotiation);
                log_level_val(4, "Speed: ",             log_int,  &ni->Link->AdminSettings->Speed);
                log_level_val(4, "Duplex: ",            log_int,  &ni->Link->AdminSettings->Duplex);
            }
            log_level_val(3, "OperSettings: ",          log_pos,   ni->Link->OperSettings);
            if (NULL != ni->Link->OperSettings) {
                log_level_val(4, "AutoNegotiation: ",   log_bool, &ni->Link->AdminSettings->AutoNegotiation);
                log_level_val(4, "Speed: ",             log_int,  &ni->Link->AdminSettings->Speed);
                log_level_val(4, "Duplex: ",            log_int,  &ni->Link->AdminSettings->Duplex);
            }
            log_level_val(3, "InterfaceType: ",         log_int,  &ni->Link->InterfaceType);
        }
        log_level_val(1, "IPv4: ",                      log_pos,   ni->IPv4);
        if (NULL != ni->IPv4) {
            log_level_val(3, "Enabled: ",               log_bool, &ni->IPv4->Enabled);
            log_level_val(3, "Config: ",                log_pos,   ni->IPv4->Config);
            if (NULL != ni->IPv4->Config) {
                log_level_val(4, "sizeManual: ",        log_int,  &ni->IPv4->Config->__sizeManual);
                log_level_val(4, "Manual: ",            log_pos,   ni->IPv4->Config->Manual);
                for (j=0; j < ni->IPv4->Config->__sizeManual; j++) {
                    log_level_fmt(5, "%d\n", j);
                    log_level_val(6, "Address: ",       log_str,   ni->IPv4->Config->Manual[j].Address);
                    log_level_val(6, "PrefixLength: ",  log_int,  &ni->IPv4->Config->Manual[j].PrefixLength);
                }
                log_level_val(3, "LinkLocal: ",         log_pos,   ni->IPv4->Config->LinkLocal);
                if (NULL != ni->IPv4->Config->LinkLocal) {
                    log_level_val(5, "Address: ",       log_str,   ni->IPv4->Config->LinkLocal->Address);
                    log_level_val(5, "PrefixLength: ",  log_int,  &ni->IPv4->Config->LinkLocal->PrefixLength);
                }
                log_level_val(3, "FromDHCP: ",          log_pos,   ni->IPv4->Config->FromDHCP);
                if (NULL != ni->IPv4->Config->FromDHCP) {
                    log_level_val(5, "Address: ",       log_str,   ni->IPv4->Config->FromDHCP->Address);
                    log_level_val(5, "PrefixLength: ",  log_int,  &ni->IPv4->Config->FromDHCP->PrefixLength);
                }
                log_level_val(4, "DHCP: ",              log_bool, &ni->IPv4->Config->DHCP);
            }
            log_level_val(1, "IPv6: ",                  log_pos,   ni->IPv6);
            if (NULL != ni->IPv6) {
                log_level_val(3, "Enabled: ",           log_bool, &ni->IPv6->Enabled);
                log_level_val(3, "Config: ",            log_pos,   ni->IPv6->Config);
            }
            log_level_val(1, "Extension: ",             log_pos,   ni->Extension);
        }
    }

    log_func_out;
}

void dump_trt__GetServiceCapabilitiesResponse(struct _trt__GetServiceCapabilitiesResponse *rep)
{
    assert(NULL != rep);

    log_func_in;

    log_level_val(0, "Capabilities: ",                      log_pos,     rep->Capabilities);
    if (NULL != rep->Capabilities) {
        log_level_val(1, "ProfileCapabilities: ",           log_pos,     rep->Capabilities->ProfileCapabilities);
        if (NULL != rep->Capabilities->ProfileCapabilities) {
            log_level_val(2, "MaximumNumberOfProfiles: ",   log_int,     rep->Capabilities->ProfileCapabilities->MaximumNumberOfProfiles);
            
        }
        log_level_val(1, "StreamingCapabilities: ",         log_pos,     rep->Capabilities->StreamingCapabilities);
        if (NULL != rep->Capabilities->StreamingCapabilities) {
            log_level_val(2, "RTPMulticast: ",              log_bool,    rep->Capabilities->StreamingCapabilities->RTPMulticast);
            log_level_val(2, "RTP_USCORETCP: ",             log_bool,    rep->Capabilities->StreamingCapabilities->RTP_USCORETCP);
            log_level_val(2, "RTP_USCORERTSP_USCORETCP: ",  log_bool,    rep->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP);
            log_level_val(2, "NonAggregateControl: ",       log_bool,    rep->Capabilities->StreamingCapabilities->NonAggregateControl);
            log_level_val(2, "NoRTSPStreaming: ",           log_bool,    rep->Capabilities->StreamingCapabilities->NoRTSPStreaming);
        }
        log_level_val(1, "SnapshotUri: ",                   log_bool,    rep->Capabilities->SnapshotUri);
        log_level_val(1, "Rotation: ",                      log_bool,    rep->Capabilities->Rotation);
        log_level_val(1, "VideoSourceMode: ",               log_bool,    rep->Capabilities->VideoSourceMode);
        log_level_val(1, "OSD: ",                           log_bool,    rep->Capabilities->OSD);
        log_level_val(1, "EXICompression: ",                log_bool,    rep->Capabilities->EXICompression);
    }

    log_func_out;
}

void dump_trt__GetVideoEncoderConfigurationResponse(struct _trt__GetVideoEncoderConfigurationResponse *rep)
{
    assert(NULL != rep);

    log_func_in;
    log_level_struct_tt__VideoEncoderConfiguration(1, rep->Configuration);
    log_func_out;
}

void dump_trt__GetVideoEncoderConfigurationsResponse(struct _trt__GetVideoEncoderConfigurationsResponse *rep)
{
    int i;
    assert(NULL != rep);
    
    log_func_in;

    log_level_val(0, "__sizeConfigurations: ",              log_int,   &rep->__sizeConfigurations);
    log_level_val(0, "Configurations: ",                    log_pos,    rep->Configurations);
    if (NULL != rep->Configurations) {
        for (i=0; i < rep->__sizeConfigurations; i++) {
            struct tt__VideoEncoderConfiguration *pcfg = rep->Configurations + i;
            log_level_fmt(1, "%d\n", i);
            log_level_struct_tt__VideoEncoderConfiguration(2, pcfg);
        }
    }

    log_func_out;
}

void dump_trt__GetCompatibleVideoEncoderConfigurationsResponse(struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *rep)
{
    int i;
    assert(NULL != rep);

    log_func_in;

    log_level_val(0, "__sizeConfigurations: ",              log_int,   &rep->__sizeConfigurations);
    log_level_val(0, "Configurations: ",                    log_pos,    rep->Configurations);
    if (NULL != rep->Configurations) {
        for (i=0; i < rep->__sizeConfigurations; i++) {
            struct tt__VideoEncoderConfiguration *pcfg = rep->Configurations + i;
            log_level_fmt(1, "%d\n", i);
            log_level_struct_tt__VideoEncoderConfiguration(2, pcfg);
        }
    }

    log_func_out;
}

void dump_trt__GetVideoEncoderConfigurationOptionsResponse(struct _trt__GetVideoEncoderConfigurationOptionsResponse *rep)
{
    int i;
    assert(NULL != rep);

    log_func_in;

    log_level_val(0, "Options: ",                           log_pos,    rep->Options);
    if (NULL != rep->Options) {
        log_level_val(1, "QualityRange: ",                  log_pos,    rep->Options->QualityRange);
        log_level_struct_tt__IntRange(2,                                rep->Options->QualityRange);
        log_level_val(1, "JPEG: ",                          log_pos,    rep->Options->JPEG);
        log_level_val(1, "MPEG4: ",                         log_pos,    rep->Options->MPEG4);
        log_level_val(1, "H264: ",                          log_pos,    rep->Options->H264);
        if (NULL != rep->Options->H264) {
            struct tt__H264Options *pH264 = rep->Options->H264;
            log_level_val(2, "sizeResolutionsAvailable: ",  log_int,   &pH264->__sizeResolutionsAvailable);
            log_level_val(2, "ResolutionsAvailable: ",      log_pos,    pH264->ResolutionsAvailable);
            for (i = 0; i < pH264->__sizeResolutionsAvailable; i++) {
                log_level_fmt(3, "%d * %d\n",                           pH264->ResolutionsAvailable[i].Width,
                                                                        pH264->ResolutionsAvailable[i].Height);
            }
            log_level_val(2, "GovLengthRange: ",            log_pos,    pH264->GovLengthRange);
            log_level_struct_tt__IntRange(3,                            pH264->GovLengthRange);
            log_level_val(2, "FrameRateRange: ",            log_pos,    pH264->FrameRateRange);
            log_level_struct_tt__IntRange(3,                            pH264->FrameRateRange);
            log_level_val(2, "EncodingIntervalRange: ",     log_pos,    pH264->EncodingIntervalRange);
            log_level_struct_tt__IntRange(3,                            pH264->EncodingIntervalRange);
            log_level_val(2, "sizeH264ProfilesSupported: ", log_int,   &pH264->__sizeH264ProfilesSupported);
            log_level_val(2, "H264ProfilesSupported: ",     log_pos,   &pH264->H264ProfilesSupported);
            for (i = 0; i < pH264->__sizeH264ProfilesSupported; i++) {
                log_level_fmt(3, "%s\n",                                dump_enum2str_H264Profile(pH264->H264ProfilesSupported[i]));
            }
        }
        log_level_val(1, "Extension: ",                     log_pos,    rep->Options->Extension);
    }

    log_func_out;
}

#endif