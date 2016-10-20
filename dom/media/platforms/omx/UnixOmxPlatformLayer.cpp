/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "OmxDataDecoder.h"
#include "OmxPromiseLayer.h"
#include "UnixOmxPlatformLayer.h"
#include "OmxCoreLibLinker.h"

#ifdef LOG
#undef LOG
#endif

extern mozilla::LogModule* GetPDMLog();

#define LOG(arg, ...) MOZ_LOG(GetPDMLog(), mozilla::LogLevel::Debug, ("UnixOmxPlatformLayer(%p)::%s: " arg, this, __func__, ##__VA_ARGS__))

namespace mozilla {

#define OMX_FUNC(func) extern typeof(func)* func;
#include "OmxFunctionList.h"
#undef OMX_FUNC

/* static */ void
UnixOmxPlatformLayer::Init(void)
{
  OmxCoreLibLinker::Link();
  OMX_ERRORTYPE err = OMX_Init();
  if (err != OMX_ErrorNone) {
    MOZ_LOG(GetPDMLog(), mozilla::LogLevel::Debug,
            ("UnixOmxPlatformLayer::%s: Failed to initialize OMXCore: 0x%08x",
             __func__, err));
  }
}

/* static */ OMX_ERRORTYPE
UnixOmxPlatformLayer::EventHandler(OMX_HANDLETYPE hComponent,
                                   OMX_PTR pAppData,
                                   OMX_EVENTTYPE eEvent,
                                   OMX_U32 nData1,
                                   OMX_U32 nData2,
                                   OMX_PTR pEventData)
{
  return OMX_ErrorUndefined;
}

/* static */ OMX_ERRORTYPE
UnixOmxPlatformLayer::EmptyBufferDone(OMX_HANDLETYPE hComponent,
                                      OMX_IN OMX_PTR pAppData,
                                      OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
  return OMX_ErrorUndefined;
}

/* static */ OMX_ERRORTYPE
UnixOmxPlatformLayer::FillBufferDone(OMX_OUT OMX_HANDLETYPE hComponent,
                                     OMX_OUT OMX_PTR pAppData,
                                     OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
{
  return OMX_ErrorUndefined;
}

/* static */ OMX_CALLBACKTYPE UnixOmxPlatformLayer::callbacks =
  { EventHandler, EmptyBufferDone, FillBufferDone };

UnixOmxPlatformLayer::UnixOmxPlatformLayer(OmxDataDecoder* aDataDecoder,
                                           OmxPromiseLayer* aPromiseLayer,
                                           TaskQueue* aTaskQueue,
                                           layers::ImageContainer* aImageContainer)
  : mComponent(nullptr)
  , mDataDecoder(aDataDecoder)
  , mPromiseLayer(aPromiseLayer)
  , mTaskQueue(aTaskQueue)
  , mImageContainer(aImageContainer)
  , mDummyMode(false)
{
  LOG("");
}

UnixOmxPlatformLayer::~UnixOmxPlatformLayer()
{
  LOG("");
  if (mComponent)
    OMX_FreeHandle(mComponent);
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::InitOmxToStateLoaded(const TrackInfo* aInfo)
{
  LOG("");

  if (!aInfo)
    return OMX_ErrorUndefined;
  mInfo = aInfo;

  return CreateComponentRenesas();
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::EmptyThisBuffer(BufferData* aData)
{
  LOG("");
  return OMX_ErrorUndefined;
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::FillThisBuffer(BufferData* aData)
{
  LOG("");
  return OMX_ErrorUndefined;
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::SendCommand(OMX_COMMANDTYPE aCmd,
                                  OMX_U32 aParam1,
                                  OMX_PTR aCmdData)
{
  LOG("aCmd: 0x%08x", aCmd);
  if (!mComponent)
    return OMX_ErrorUndefined;
  return OMX_SendCommand(mComponent, aCmd, aParam1, aCmdData);
}

nsresult
UnixOmxPlatformLayer::FindPortDefinition(OMX_DIRTYPE aType, OMX_PARAM_PORTDEFINITIONTYPE& portDef)
{
  nsTArray<uint32_t> portIndex;
  GetPortIndices(portIndex);
  for (auto idx : portIndex) {
    InitOmxParameter(&portDef);
    portDef.nPortIndex = idx;

    OMX_ERRORTYPE err = GetParameter(OMX_IndexParamPortDefinition,
                                     &portDef,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
    if (err != OMX_ErrorNone) {
      return NS_ERROR_FAILURE;
    } else if (portDef.eDir == aType) {
      LOG("Found OMX_IndexParamPortDefinition: port: %d, type: %d",
          portDef.nPortIndex, portDef.eDir);
      return NS_OK;
    }
  }
}

nsresult
UnixOmxPlatformLayer::AllocateOmxBuffer(OMX_DIRTYPE aType,
                                        BUFFERLIST* aBufferList)
{
  LOG("aType: %d", aType);

  OMX_PARAM_PORTDEFINITIONTYPE portDef;
  nsresult result = FindPortDefinition(aType, portDef);
  if (result != NS_OK)
    return result;

  LOG("nBufferCountActual: %d, nBufferSize: %d",
      portDef.nBufferCountActual, portDef.nBufferSize);

  /* TODO: Allocate buffers */

  return NS_ERROR_NOT_IMPLEMENTED;
}

nsresult
UnixOmxPlatformLayer::ReleaseOmxBuffer(OMX_DIRTYPE aType,
                                       BUFFERLIST* aBufferList)
{
  LOG("aType: 0x%08x");
  return NS_ERROR_NOT_IMPLEMENTED;
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::GetState(OMX_STATETYPE* aType)
{
  LOG("");

  if (mComponent)
    return OMX_GetState(mComponent, aType);

  if (mDummyMode) {
    if (aType)
      *aType = OMX_StateLoaded;
    return OMX_ErrorNone;
  }
  return OMX_ErrorUndefined;
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::GetParameter(OMX_INDEXTYPE aParamIndex,
                                   OMX_PTR aComponentParameterStructure,
                                   OMX_U32 aComponentParameterSize)
{
  LOG("aParamIndex: 0x%08x", aParamIndex);

  if (!mComponent) {
    if (mDummyMode)
      return OMX_ErrorNone;
    return OMX_ErrorUndefined;
  }

  // TODO: Should check the struct size?
  return OMX_GetParameter(mComponent,
                          aParamIndex,
                          aComponentParameterStructure);
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::SetParameter(OMX_INDEXTYPE aParamIndex,
                                   OMX_PTR aComponentParameterStructure,
                                   OMX_U32 aComponentParameterSize)
{
  LOG("aParamIndex: 0x%08x", aParamIndex);

  if (!mComponent) {
    if (mDummyMode)
      return OMX_ErrorNone;
    return OMX_ErrorUndefined;
  }

  // TODO: Should check the struct size?
  return OMX_SetParameter(mComponent,
                          aParamIndex,
                          aComponentParameterStructure);
}

nsresult
UnixOmxPlatformLayer::Shutdown()
{
  LOG("");
  return NS_ERROR_NOT_IMPLEMENTED;
}

bool
UnixOmxPlatformLayer::SupportsMimeType(const nsACString& aMimeType)
{
  return SupportsMimeTypeRenesas(aMimeType);
}

bool
UnixOmxPlatformLayer::SupportsMimeTypeRenesas(const nsACString& aMimeType)
{
  const char* mime = aMimeType.Data();
  MOZ_LOG(GetPDMLog(), mozilla::LogLevel::Debug,
          ("OmxPlatformLayer::%s: aMimeType: %s",
           __func__, mime));

  if (aMimeType.EqualsLiteral("video/avc") ||
      aMimeType.EqualsLiteral("video/mp4") ||
      aMimeType.EqualsLiteral("video/mp4v-es")) {
    return true;
  }

  if (aMimeType.EqualsLiteral("audio/mp4a-latm")) {
    return true;
  }

  return false;
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::CreateComponentRenesas(void)
{
  OMX_ERRORTYPE err = OMX_ErrorUndefined;
  const char* mime = mInfo->mMimeType.Data();

  if (mInfo->GetAsVideoInfo()) {
    // This is video decoding.
    if (mInfo->mMimeType.EqualsLiteral("video/avc") ||
        mInfo->mMimeType.EqualsLiteral("video/mp4") ||
        mInfo->mMimeType.EqualsLiteral("video/mp4v-es")) {
      err = OMX_GetHandle(&mComponent,
                          "OMX.RENESAS.VIDEO.DECODER.H264",
                          this,
                          &callbacks);
    }
  } else if (mInfo->GetAsAudioInfo()) {
    // This is audio decoding.
    if (mInfo->mMimeType.EqualsLiteral("audio/mp4a-latm")) {
      err = OMX_GetHandle(&mComponent,
                          "OMX.RENESAS.AUDIO.DECODER.AAC",
                          this,
                          &callbacks);
      // TODO:
      // Although we want to demonstrate video decoding performance of RZ/G1
      // series, most H.264 files aren't detected as playable because the
      // board doesn't have OpenMAX IL component of AAC decoder. To avoid it we
      // will use blnak data for audio at this moment.
      // In the futuer we may have to implement other way (use GStreamer?).
      if (err != OMX_ErrorNone) {
        mDummyMode = true;
        err = OMX_ErrorNone;
        LOG("Use dummy mode for %s: 0x%08x", mime, err);
      }
    }
  }

  if (err == OMX_ErrorNone) {
    LOG("Succeeded to create the component for %s", mime);
  } else {
    LOG("Failed to create the component for %s: 0x%08x", mime, err);
  }

  return err;
}

}
