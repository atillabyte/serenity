/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <LibGUI/GDesktop.h>
#include <LibGUI/GDialog.h>
#include <LibGUI/GEvent.h>

GDialog::GDialog(CObject* parent)
    : GWindow(parent)
{
    set_modal(true);
}

GDialog::~GDialog()
{
}

int GDialog::exec()
{
    ASSERT(!m_event_loop);
    m_event_loop = make<CEventLoop>();
    auto new_rect = rect();
    if (parent() && parent()->is_window()) {
        auto& parent_window = *static_cast<GWindow*>(parent());
        new_rect.center_within(parent_window.rect());
    } else {
        new_rect.center_within(GDesktop::the().rect());
    }
    set_rect(new_rect);
    show();
    auto result = m_event_loop->exec();
    m_event_loop = nullptr;
    dbgprintf("%s: event loop returned with result %d\n", class_name(), result);
    remove_from_parent();
    return result;
}

void GDialog::done(int result)
{
    if (!m_event_loop)
        return;
    m_result = result;
    dbgprintf("%s: quit event loop with result %d\n", class_name(), result);
    m_event_loop->quit(result);
}

void GDialog::event(CEvent& event)
{
    if (event.type() == GEvent::KeyUp) {
        auto& key_event = static_cast<GKeyEvent&>(event);
        if (key_event.key() == KeyCode::Key_Escape) {
            done(ExecCancel);
            return;
        }
    }

    GWindow::event(event);
}

void GDialog::close()
{
    GWindow::close();
    m_event_loop->quit(ExecCancel);
}
