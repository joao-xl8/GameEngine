#pragma once

#include "base_component.hpp"
#include <string>

class CSave : public Component
{
public:
    bool enabled = true;
    std::string savePointName = "";
    std::string displayText = "Save Game";
    
    CSave() = default;
    CSave(const std::string& name, const std::string& text = "Save Game")
        : savePointName(name), displayText(text) {}
};
