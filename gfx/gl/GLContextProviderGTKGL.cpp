/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "GLContextProvider.h"
#include "GLContextGTKGL.h"
#include "nsDebug.h"
#include "nsIWidget.h"
#include <gdk/gdk.h>
#include "gfxFailure.h"
#include "gfxPrefs.h"
#include "prenv.h"
#include "GeckoProfiler.h"
#include "mozilla/widget/CompositorWidget.h"

namespace mozilla {
namespace gl {

using namespace mozilla::gfx;
using namespace mozilla::widget;

GLContextGTKGL::GLContextGTKGL(CreateContextFlags flags, const SurfaceCaps& caps,
                               GdkGLContext* context, bool isOffscreen,
                               ContextProfile profile)
    : GLContext(flags, caps, nullptr, isOffscreen)
    , mContext(context),
      mOwnsContext(true)
{
    // TODO: We assume that this context should be GLESv2
    SetProfileVersion(ContextProfile::OpenGLES, 200);
}

GLContextGTKGL::~GLContextGTKGL()
{
    MarkDestroyed();

    // Wrapped context should not destroy glxContext/Surface
    if (!mOwnsContext) {
        return;
    }
}

bool
GLContextGTKGL::Init()
{
    if (!InitWithPrefix("gl", true))
        return false;

    return true;
}

bool
GLContextGTKGL::MakeCurrentImpl(bool aForce)
{
    bool succeeded = true;

    if (aForce || gdk_gl_context_get_current() != mContext) {
        gdk_gl_context_make_current(mContext);
    }

    return succeeded;
}

bool
GLContextGTKGL::IsCurrent()
{
    return gdk_gl_context_get_current() == mContext;
}

bool
GLContextGTKGL::SetupLookupFunction()
{
    return false;
}

bool
GLContextGTKGL::IsDoubleBuffered() const
{
    return false;
}

} /* namespace gl */
} /* namespace mozilla */
