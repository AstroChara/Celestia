// textinput.h
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

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <celutil/array_view.h>

class Simulation;

namespace celestia
{

enum class CharEnteredResult
{
    Normal,
    Finished,
    Cancelled,
};

class TextInput
{
public:
    std::string_view getTypedText() const;
    util::array_view<std::string> getCompletion() const;
    int getCompletionIndex() const;

    CharEnteredResult charEntered(const Simulation*, std::string_view, bool withLocations);
    void appendText(const Simulation*, std::string_view, bool withLocations);
    void reset();

private:
    std::string m_text;
    std::vector<std::string> m_completion;
    int m_completionIdx{ -1 };
};

}
