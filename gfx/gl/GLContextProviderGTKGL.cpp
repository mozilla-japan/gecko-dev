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

#define GET_NATIVE_WINDOW(aWidget) ((GdkWindow*) aWidget->GetNativeData(NS_NATIVE_WINDOW))

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

already_AddRefed<GLContextGTKGL>
GLContextGTKGL::CreateGLContext(CreateContextFlags flags, const SurfaceCaps& caps,
                                GdkWindow* aWindow, bool isOffscreen)
{
    RefPtr<GLContextGTKGL> glContext;
    GdkGLContext context;
    GError* error = nullptr;
    context = gdk_window_create_gl_context(window, &error);
    if (!context) {
        NS_WARNING("Failed to create GdkGLContext!");
        g_error_free(error);
        return nullptr;
    }
    glContext = new GLContextGTKGL(flags, caps,
                                   context, isOffscreen,
                                   profile);
    return glContext.forget();
}

already_AddRefed<GLContext>
GLContextProviderGTKGL::CreateForCompositorWidget(CompositorWidget* aCompositorWidget, bool aForceAccelerated)
{
    return CreateForWindow(aCompositorWidget->RealWidget(), aForceAccelerated);
}

already_AddRefed<GLContext>
GLContextProviderGTKGL::CreateForWindow(nsIWidget* aWidget, bool aForceAccelerated)
{
    SurfaceCaps caps = SurfaceCaps::Any();
    GdkWindow *aWindow = GET_NATIVE_WINDOW(aWidget);
    RefPtr<GdkGLContext> gl = GLContextGTKGL::CreateGLContext(CreateContextFlags::NONE,
                                                              caps, aWindow, false)
    return gl.forget();
}

/*static*/ already_AddRefed<GLContext>
GLContextProviderGTKGL::CreateOffscreen(const gfx::IntSize&,
                                       const SurfaceCaps&,
                                       CreateContextFlags,
                                       nsACString* const out_failureId)
{
    *out_failureId = NS_LITERAL_CSTRING("FEATURE_FAILURE_GTKGL");
    return nullptr;
}

/*static*/ already_AddRefed<GLContext>
GLContextProviderGTKGL::CreateHeadless(CreateContextFlags, nsACString* const out_failureId)
{
    *out_failureId = NS_LITERAL_CSTRING("FEATURE_FAILURE_GTKGL");
    return nullptr;
}

/*static*/ GLContext*
GLContextProviderGTKGL::GetGlobalContext()
{
    return nullptr;
}

/*static*/ void
GLContextProviderGTKGL::Shutdown()
{
}

} /* namespace gl */
} /* namespace mozilla */
