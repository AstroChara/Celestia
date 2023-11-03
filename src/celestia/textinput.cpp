// textinput.cpp
//
// Copyright (C) 2023, the Celestia Development Team
//
// Split out from celestiacore.h/celestiacore.cpp
// Copyright (C) 2001-2009, the Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "textinput.h"

#include <cstddef>
#include <cstdint>
#include <cwctype>

#include <celengine/overlay.h>
#include <celengine/rectangle.h>
#include <celengine/simulation.h>
#include <celutil/gettext.h>
#include <celutil/utf8.h>
#include "hud.h"
#include "windowmetrics.h"

namespace celestia
{

std::string_view
TextInput::getTypedText() const
{
    return m_text;
}

util::array_view<std::string>
TextInput::getCompletion() const
{
    return m_completion;
}

int
TextInput::getCompletionIndex() const
{
    return m_completionIdx;
}

CharEnteredResult
TextInput::charEntered(const Simulation* sim, std::string_view c_p, bool withLocations)
{
    char c = c_p.front();
    if (std::int32_t uc = 0;
        UTF8Decode(c_p, uc) && !(uc <= WCHAR_MAX && std::iswcntrl(static_cast<std::wint_t>(uc))))
    {
        appendText(sim, c_p, withLocations);
        return CharEnteredResult::Normal;
    }

    switch (c)
    {
    case '\b':
        doBackspace(sim, withLocations);
        break;
    case '\t':
        doTab();
        break;
    case '\n':
    case '\r':
        return CharEnteredResult::Finished;
    case '\033':
        return CharEnteredResult::Cancelled; // ESC
    case '\177':
        doBackTab();
        break;
    }

    return CharEnteredResult::Normal;
}

void
TextInput::doBackspace(const Simulation* sim, bool withLocations)
{
    m_completionIdx = -1;
    if (m_text.empty())
        return;

#ifdef AUTO_COMPLETION
    do
    {
#endif
        for (;;)
        {
            auto ch = static_cast<std::byte>(m_text.back());
            m_text.pop_back();
            // If the string is empty, or the removed character was
            // not a UTF-8 continuation byte 0b10xx_xxxx then we're
            // done.
            if (m_text.empty() || (ch & std::byte(0xc0)) != std::byte(0x80))
                break;
        }

        m_completion.clear();
        if (!m_text.empty())
                sim->getObjectCompletion(m_completion, m_text, withLocations);
#ifdef AUTO_COMPLETION
    } while (!typedText.empty() && typedTextCompletion.size() == 1);
#endif
}

void
TextInput::doTab()
{
    if (m_completionIdx + 1 < (int) m_completion.size())
        m_completionIdx++;
    else if ((int) m_completion.size() > 0 && m_completionIdx + 1 == (int) m_completion.size())
        m_completionIdx = 0;

    if (m_completionIdx >= 0)
    {
        auto pos = m_text.rfind('/');
        m_text.resize(pos == std::string::npos ? 0 : (pos + 1));
        m_text.append(m_completion[m_completionIdx]);
    }
}

void
TextInput::doBackTab()
{
    if (m_completionIdx > 0)
        m_completionIdx--;
    else if (m_completionIdx == 0 || m_completion.size() > 0)
        m_completionIdx = m_completion.size() - 1;

    if (m_completionIdx >= 0)
    {
        auto pos = m_text.rfind('/');
        m_text.resize(pos == std::string::npos ? 0 : (pos + 1));
        m_text.append(m_completion[m_completionIdx]);
    }
}

void
TextInput::appendText(const Simulation* sim, std::string_view sv, bool withLocations)
{
    m_text.append(sv);
    m_completion.clear();
    sim->getObjectCompletion(m_completion, m_text, withLocations);
    m_completionIdx = -1;
#ifdef AUTO_COMPLETION
    if (m_completion.size() == 1)
    {
        auto pos = m_text.rfind('/');
        m_text.resize(pos == std::string::npos ? 0 : (pos + 1));
        m_text.append(m_completion.front());
    }
#endif
}

void
TextInput::reset()
{
    m_text.clear();
    m_completion.clear();
    m_completionIdx = -1;
}

void
TextInput::render(Overlay* overlay,
                  const HudFonts& hudFonts,
                  const WindowMetrics& metrics,
                  const Color& consoleColor) const
{
    overlay->setFont(hudFonts.titleFont());
    overlay->savePos();
    int rectHeight = hudFonts.fontHeight() * 3.0f + metrics.screenDpi / 25.4f * 9.3f + hudFonts.titleFontHeight();
    celestia::Rect r(0, 0, metrics.width, metrics.insetBottom + rectHeight);
    r.setColor(consoleColor);
    overlay->drawRectangle(r);
    overlay->moveBy(metrics.getSafeAreaStart(), metrics.getSafeAreaBottom(rectHeight - hudFonts.titleFontHeight()));
    overlay->setColor(0.6f, 0.6f, 1.0f, 1.0f);
    overlay->beginText();
    overlay->print(_("Target name: {}"), m_text);
    overlay->endText();
    overlay->setFont(hudFonts.font());
    if (!m_completion.empty())
        renderCompletion(overlay, metrics, hudFonts.fontHeight());

    overlay->restorePos();
    overlay->setFont(hudFonts.font());
}

void
TextInput::renderCompletion(Overlay* overlay, const WindowMetrics& metrics, int fontHeight) const
{
    constexpr int nb_cols = 4;
    constexpr int nb_lines = 3;
    constexpr int spacing = 3;
    int start = 0;
    overlay->moveBy(spacing, -fontHeight - spacing);
    auto iter = m_completion.begin();
    auto typedTextCompletionIdx = m_completionIdx;
    if (typedTextCompletionIdx >= nb_cols * nb_lines)
    {
        start = (typedTextCompletionIdx / nb_lines + 1 - nb_cols) * nb_lines;
        iter += start;
    }

    int columnWidth = metrics.getSafeAreaWidth() / nb_cols;
    for (int i = 0; iter < m_completion.end() && i < nb_cols; i++)
    {
        overlay->savePos();
        overlay->beginText();
        for (int j = 0; iter < m_completion.end() && j < nb_lines; iter++, j++)
        {
            if (i * nb_lines + j == typedTextCompletionIdx - start)
                overlay->setColor(1.0f, 0.6f, 0.6f, 1);
            else
                overlay->setColor(0.6f, 0.6f, 1.0f, 1);
            overlay->print(*iter);
            overlay->print("\n");
        }
        overlay->endText();
        overlay->restorePos();
        overlay->moveBy(metrics.layoutDirection == LayoutDirection::RightToLeft ? -columnWidth : columnWidth, 0);
    }
}

} // end namespace celestia
