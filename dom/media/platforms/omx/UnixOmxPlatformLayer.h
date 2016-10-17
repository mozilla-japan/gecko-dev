/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#if !defined(UnixOmxPlatformLayer_h_)
#define UnixOmxPlatformLayer_h_

#include "OmxPlatformLayer.h"

namespace mozilla {

class UnixOmxPlatformLayer : public OmxPlatformLayer {
public:
  static void Init(void);

  UnixOmxPlatformLayer(OmxDataDecoder* aDataDecoder,
                       OmxPromiseLayer* aPromiseLayer,
                       TaskQueue* aTaskQueue);

  virtual ~UnixOmxPlatformLayer();

  virtual OMX_ERRORTYPE InitOmxToStateLoaded(const TrackInfo* aInfo) override;

  virtual OMX_ERRORTYPE EmptyThisBuffer(BufferData* aData) override;

  virtual OMX_ERRORTYPE FillThisBuffer(BufferData* aData) override;

  virtual OMX_ERRORTYPE SendCommand(OMX_COMMANDTYPE aCmd,
                                    OMX_U32 aParam1,
                                    OMX_PTR aCmdData) override;

  virtual nsresult AllocateOmxBuffer(OMX_DIRTYPE aType, BUFFERLIST* aBufferList) override;

  virtual nsresult ReleaseOmxBuffer(OMX_DIRTYPE aType, BUFFERLIST* aBufferList) override;

  virtual OMX_ERRORTYPE GetState(OMX_STATETYPE* aType) override;

  virtual OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE aParamIndex,
                                     OMX_PTR aComponentParameterStructure,
                                     OMX_U32 aComponentParameterSize) override;

  virtual OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
                                     OMX_PTR aComponentParameterStructure,
                                     OMX_U32 aComponentParameterSize) override;

  virtual nsresult Shutdown() override;

protected:
  OMX_HANDLETYPE mComponent;
};

}

#endif // UnixOmxPlatformLayer_h_
