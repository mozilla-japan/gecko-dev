/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim:expandtab:shiftwidth=4:tabstop=4:
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __nsClipboardX11_h_
#define __nsClipboardX11_h_

#include "nsIClipboard.h"
#include <gtk/gtk.h>

class nsRetrievalContextX11 : public nsRetrievalContext
{
public:
    enum State { INITIAL, COMPLETED, TIMED_OUT };

    nsRetrievalContextX11();
    virtual guchar* WaitForClipboardContext(const char* aMimeType,
                                            int32_t aWhichClipboard,
                                            uint32_t* aContentLength) override;
    virtual GdkAtom* GetTargets(int32_t aWhichClipboard,
                                int* aTargetNum) override;

    // Call this when data has been retrieved.
    void Complete(GtkSelectionData* aData);
    virtual ~nsRetrievalContextX11() override;

private:
    GtkSelectionData* WaitForContents(GtkClipboard *clipboard,
                                      const char *aMimeType);
    /**
     * Spins X event loop until timing out or being completed. Returns
     * null if we time out, otherwise returns the completed data (passing
     * ownership to caller).
     */
    void *Wait();

    State mState;
    void* mData;
};

#endif /* __nsClipboardX11_h_ */
