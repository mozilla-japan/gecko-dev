/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set ts=8 sts=4 et sw=4 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GLCONTEXTGTKGL_H_
#define GLCONTEXTGTKGL_H_

#include "GLContext.h"

#include <gdk/gdk.h>

namespace mozilla {
namespace gl {

class GLContextGTKGL : public GLContext
{
    friend class GLContextProviderGTKGL;

    static already_AddRefed<GLContextGTKGL>
    CreateGLContext(CreateContextFlags flags, const SurfaceCaps& caps,
                    GdkWindow* aWindow, bool isOffscreen);
public:
    MOZ_DECLARE_REFCOUNTED_VIRTUAL_TYPENAME(GLContextGTKGL, override)
    GLContextGTKGL(CreateContextFlags flags, const SurfaceCaps& caps,
                   GdkGLContext* context, bool isOffscreen);

    ~GLContextGTKGL();

    virtual GLContextType GetContextType() const override { return GLContextType::GTKGL; }

    bool Init() override;

    GdkGLContext* GetGTKGLContext() const { return mContext; }

    virtual bool MakeCurrentImpl(bool aForce) override;

    virtual bool IsCurrent() override;

    virtual bool SetupLookupFunction() override;

    virtual bool IsDoubleBuffered() const override;

    virtual bool SupportsRobustness() const override { return false; }

    virtual bool SwapBuffers() override { return false; };

private:
    GdkGLContext* mContext;
    bool mOwnsContext;

};

} // namespace gl
} // namespace mozilla

#endif // GLCONTEXTGTKGL_H_
