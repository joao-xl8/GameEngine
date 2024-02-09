#pragma once
#include <string>
class Action {
private:
    std::string name;
    std::string type;
public:
    Action(std::string name, std::string type) : name(name), type(type) {};
    const std::string& getName() const { return name; };
    const std::string& getType() const { return type; };
};