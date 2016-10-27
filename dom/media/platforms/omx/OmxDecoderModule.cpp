/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "OmxDecoderModule.h"

#include "OmxDataDecoder.h"
#include "OmxPlatformLayer.h"

#ifdef MOZ_WIDGET_GTK
#include "UnixOmxPlatformLayer.h"
#endif

namespace mozilla {

/* static */ bool
OmxDecoderModule::Init()
{
#ifdef MOZ_WIDGET_GTK
  return UnixOmxPlatformLayer::Init();
#endif
  return false;
}

OmxDecoderModule*
OmxDecoderModule::Create()
{
#ifdef MOZ_WIDGET_GTK
  if (!Init())
    return nullptr;
  return new OmxDecoderModule();
#endif
  return nullptr;
}

already_AddRefed<MediaDataDecoder>
OmxDecoderModule::CreateVideoDecoder(const VideoInfo& aConfig,
                                     mozilla::layers::LayersBackend aLayersBackend,
                                     mozilla::layers::ImageContainer* aImageContainer,
                                     FlushableTaskQueue* aVideoTaskQueue,
                                     MediaDataDecoderCallback* aCallback)
{
  RefPtr<OmxDataDecoder> decoder = new OmxDataDecoder(aConfig,
                                                      aCallback,
                                                      aImageContainer);
  return decoder.forget();
}

already_AddRefed<MediaDataDecoder>
OmxDecoderModule::CreateAudioDecoder(const AudioInfo& aConfig,
                                     FlushableTaskQueue* aAudioTaskQueue,
                                     MediaDataDecoderCallback* aCallback)
{
  RefPtr<OmxDataDecoder> decoder = new OmxDataDecoder(aConfig,
                                                      aCallback,
                                                      nullptr);
  return decoder.forget();
}

PlatformDecoderModule::ConversionRequired
OmxDecoderModule::DecoderNeedsConversion(const TrackInfo& aConfig) const
{
  return ConversionRequired::kNeedNone;
}

bool
OmxDecoderModule::SupportsMimeType(const nsACString& aMimeType) const
{
  return OmxPlatformLayer::SupportsMimeType(aMimeType);
}

}
