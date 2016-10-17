/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "OmxDataDecoder.h"
#include "OmxPromiseLayer.h"
#include "UnixOmxPlatformLayer.h"

#ifdef LOG
#undef LOG
#endif

extern mozilla::LogModule* GetPDMLog();

#define LOG(arg, ...) MOZ_LOG(GetPDMLog(), mozilla::LogLevel::Debug, ("UnixOmxPlatformLayer(%p)::%s: " arg, this, __func__, ##__VA_ARGS__))

namespace mozilla {

UnixOmxPlatformLayer::UnixOmxPlatformLayer(OmxDataDecoder* aDataDecoder,
                                           OmxPromiseLayer* aPromiseLayer,
                                           TaskQueue* aTaskQueue)
  : mComponent(nullptr)
{
  LOG("");
}

UnixOmxPlatformLayer::~UnixOmxPlatformLayer()
{
  LOG("");
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::InitOmxToStateLoaded(const TrackInfo* aInfo)
{
  LOG("");
  return OMX_ErrorUndefined;
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
  LOG("");
  return OMX_SendCommand(mComponent, aCmd, aParam1, aCmdData);
}

nsresult
UnixOmxPlatformLayer::AllocateOmxBuffer(OMX_DIRTYPE aType,
                                        BUFFERLIST* aBufferList)
{
  LOG("");
  return NS_ERROR_NOT_IMPLEMENTED;
}

nsresult
UnixOmxPlatformLayer::ReleaseOmxBuffer(OMX_DIRTYPE aType,
                                       BUFFERLIST* aBufferList)
{
  LOG("");
  return NS_ERROR_NOT_IMPLEMENTED;
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::GetState(OMX_STATETYPE* aType)
{
  LOG("");
  return OMX_GetState(mComponent, aType);
}

OMX_ERRORTYPE
UnixOmxPlatformLayer::GetParameter(OMX_INDEXTYPE aParamIndex,
                                   OMX_PTR aComponentParameterStructure,
                                   OMX_U32 aComponentParameterSize)
{
  LOG("");
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
  LOG("");
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

}
