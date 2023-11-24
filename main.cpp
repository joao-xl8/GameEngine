#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <variant>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
typedef std::variant<sf::CircleShape, sf::RectangleShape> TypeShape;
class Object
{
private:
public:
    std::shared_ptr<TypeShape> shape;
    std::string name;
    sf::Vector2f vel;
    bool exists = true;
    sf::Color fill_color = sf::Color(255, 255, 255);
    sf::Color outline_color = sf::Color(0, 0, 0);

    template <typename T>
    Object(const std::string &init_name, const sf::Vector2f &init_vel, T &&init_shape, const sf::Color &init_fill_color)
        : name{init_name}, vel{init_vel}, fill_color{init_fill_color}
    {
        shape = std::make_shared<TypeShape>(std::forward<T>(init_shape));
    }
    ~Object(){};
};

// Helper function to create an Object based on TypeShape
template <typename T>
std::shared_ptr<Object> makeObject(std::string name, sf::Vector2f vel, T &&shape, sf::Color color)
{
    auto object = std::shared_ptr<Object>(new Object(std::move(name), std::move(vel), std::forward<T>(shape), std::move(color)));
    return object;
}

enum Options
{
    Window,
    Font,
    Rectangle,
    Circle,
    Invalid
};

Options resolveOption(std::string input)
{
    if (input == "Window")
        return Options::Window;
    if (input == "Font")
        return Options::Font;
    if (input == "Rectangle")
        return Options::Rectangle;
    if (input == "Circle")
        return Options::Circle;

    return Options::Invalid;
}

class GameManager
{
private:
    std::vector<std::shared_ptr<Object>> m_objs;

    int current_index = 0;
    std::shared_ptr<sf::Font> font;
    int fontSize;
    sf::Color fontColor;

    sf::Color bgColor;
    sf::RenderWindow window;
    int selectedShapeIndex = -1;
    int windowWidth;
    int windowHeight;
    float velMult = 4.f;
    bool stopPhysics = false;
    bool isMouseButtonDown = false;

public:
    GameManager() : windowWidth{400}, windowHeight{400}
    {
        loadConfigFromFile("config.txt");
    };

    ~GameManager()
    {
        ImGui::SFML::Shutdown();
    };

    void setMouseButtonDown(bool state)
    {
        isMouseButtonDown = state;
    }

    void addObject(const std::shared_ptr<Object> &object)
    {
        m_objs.push_back(object);
    }
    void loadConfigFromFile(const std::string &filename)
    {
        std::ifstream fin(filename);
        std::string type;

        while (fin >> type)
        {
            std::cout << type << std::endl;
            switch (resolveOption(type))
            {
            case Options::Window:
            {
                fin >> windowWidth >> windowHeight;

                window.create(sf::VideoMode(windowWidth, windowHeight), "Game in SFML and ImGUI");
                window.setFramerateLimit(60);
                ImGui::SFML::Init(window);
                break;
            }
            case Options::Font:
            {
                std::string fontSource;
                fin >> fontSource;

                font = std::make_shared<sf::Font>();
                if (!font->loadFromFile(fontSource))
                {
                    std::cerr << "Error while trying to load font! \n";
                }
                fin >> fontSize;

                int r, g, b;

                fin >> r >> g >> b;
                fontColor = sf::Color(r, g, b);
                break;
            }
            case Options::Rectangle:
            {
                std::string name;
                sf::Vector2f init_pos;
                sf::Vector2f init_vel;
                int red;
                int green;
                int blue;
                sf::Vector2f size;
                fin >> name >> init_pos.x >> init_pos.y >> init_vel.x >> init_vel.y >> red >> green >> blue >> size.x >> size.y;

                sf::RectangleShape rectangle_shape(size);

                rectangle_shape.setPosition(init_pos);
                m_objs.push_back(makeObject(name, init_vel, rectangle_shape, sf::Color(red, green, blue)));
                break;
            }
            case Options::Circle:
            {
                std::string name;
                sf::Vector2f init_pos;
                sf::Vector2f init_vel;
                int red;
                int green;
                int blue;
                float radius;

                fin >> name >> init_pos.x >> init_pos.y >> init_vel.x >> init_vel.y >> red >> green >> blue >> radius;

                sf::CircleShape circle_shape(radius, 1000.0f);

                circle_shape.setPosition(init_pos);
                m_objs.push_back(makeObject(name, init_vel, circle_shape, sf::Color(red, green, blue)));
                break;
            }
            case Options::Invalid:
                break;
            default:
                break;
            }
        }
    };

    void moveObjects()
    {
        if (stopPhysics)
            return;
        for (const auto &o : m_objs)
        {

            // change vel dir if hit bounding box aka: "bounce"

            if (std::holds_alternative<sf::CircleShape>(*o->shape))
            {
                const sf::CircleShape &circle = std::get<sf::CircleShape>(*o->shape);
                if (circle.getGlobalBounds().top <= 0.f || circle.getGlobalBounds().top + circle.getGlobalBounds().height >= windowHeight)
                {
                    o->vel.y *= -1;
                }
                if (circle.getGlobalBounds().left <= 0.f || circle.getGlobalBounds().left + circle.getGlobalBounds().width >= windowWidth)
                {
                    o->vel.x *= -1;
                }
                std::get<sf::CircleShape>(*o->shape).setPosition(circle.getPosition() + o->vel / velMult);
            }
            if (std::holds_alternative<sf::RectangleShape>(*o->shape))
            {
                const sf::RectangleShape &rect = std::get<sf::RectangleShape>(*o->shape);
                if (rect.getGlobalBounds().top <= 0.f || rect.getGlobalBounds().top + rect.getGlobalBounds().height >= windowHeight)
                {
                    o->vel.y *= -1;
                }
                if (rect.getGlobalBounds().left <= 0.f || rect.getGlobalBounds().left + rect.getGlobalBounds().width >= windowWidth)
                {
                    o->vel.x *= -1;
                }
                std::get<sf::RectangleShape>(*o->shape).setPosition(rect.getPosition() + o->vel / velMult);
            }
        }
    };

    void updateUI()
    {

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (!isMouseButtonDown)
            {

                setMouseButtonDown(true);
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                for (int i = static_cast<int>(m_objs.size()) - 1; i >= 0; --i)
                {
                    const Object &obj = *m_objs[i];

                    if (obj.exists)
                    {
                        if (std::holds_alternative<sf::CircleShape>(*obj.shape))
                        {
                            const sf::CircleShape &circle = std::get<sf::CircleShape>(*obj.shape);
                            if (circle.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition)))
                            {
                                selectedShapeIndex = i;
                                break;
                            }
                        }
                        else if (std::holds_alternative<sf::RectangleShape>(*obj.shape))
                        {
                            const sf::RectangleShape &rectangle = std::get<sf::RectangleShape>(*obj.shape);
                            if (rectangle.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition)))
                            {
                                selectedShapeIndex = i;
                                break;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            setMouseButtonDown(false);
        }

        ImGui::Begin("Params");
        // GLOBAL
        ImGui::Text("Global");
        ImGui::SameLine();
        ImGui::Checkbox("Stop Physics ", &stopPhysics);
        ImGui::SliderFloat("Global Velocity Scale", &velMult, 1.f, 10.f);
        // Shape selection combo box
        const char *shapeNames[m_objs.size()];
        for (size_t i = 0; i < m_objs.size(); ++i)
        {
            shapeNames[i] = m_objs[i]->name.c_str();
        }
        //          LOCAL
        ImGui::Combo("Shape", &selectedShapeIndex, shapeNames, static_cast<int>(m_objs.size()));
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            selectedShapeIndex = -1;
        }
        if (!m_objs.empty() && selectedShapeIndex >= 0)
        {
            Object &selectedObject = *m_objs[static_cast<size_t>(selectedShapeIndex)];
            ImGui::Text("Selected Shape: %s", selectedObject.name.c_str());

            // Add more UI elements related to the selected shape if needed...

            ImGui::Checkbox(("Show " + selectedObject.name).c_str(), &selectedObject.exists);

            char newNameBuffer[256];
            strcpy(newNameBuffer, selectedObject.name.c_str());
            if (ImGui::InputText("New Name", newNameBuffer, IM_ARRAYSIZE(newNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                selectedObject.name = std::string(newNameBuffer);
            }
            ImGui::SliderFloat("X Velocity", &selectedObject.vel.x, -8.f, 8.0f);

            ImGui::SliderFloat("Y Velocity", &selectedObject.vel.y, -8.f, 8.0f);

            if (ImGui::Button("Edit Color"))
            {
                ImGui::OpenPopup("ColorPicker");
            }

            // Color picker popup
            if (ImGui::BeginPopup("ColorPicker"))
            {
                // Use ColorEdit3 to edit RGB color

                ImVec4 fill_color = ImVec4(
                    selectedObject.fill_color.r / 255.0f,
                    selectedObject.fill_color.g / 255.0f,
                    selectedObject.fill_color.b / 255.0f,
                    1.0f);
                ImVec4 outline_color = ImVec4(
                    selectedObject.outline_color.r / 255.0f,
                    selectedObject.outline_color.g / 255.0f,
                    selectedObject.outline_color.b / 255.0f,
                    1.0f);
                // Use ColorEdit3 to edit RGB color
                if (ImGui::ColorEdit3("Fill Color", (float *)&fill_color))
                {
                    selectedObject.fill_color.r = static_cast<uint8_t>(fill_color.x * 255.0f);
                    selectedObject.fill_color.g = static_cast<uint8_t>(fill_color.y * 255.0f);
                    selectedObject.fill_color.b = static_cast<uint8_t>(fill_color.z * 255.0f);
                }

                if (ImGui::ColorEdit3("Outline Color", (float *)&outline_color))
                {
                    selectedObject.outline_color.r = static_cast<uint8_t>(outline_color.x * 255.0f);
                    selectedObject.outline_color.g = static_cast<uint8_t>(outline_color.y * 255.0f);
                    selectedObject.outline_color.b = static_cast<uint8_t>(outline_color.z * 255.0f);
                }
                // Button to apply the selected color to the shape
                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup(); // Close the color picker popup
                }

                ImGui::EndPopup();
            }
            // For example, if you want to adjust the scale of the circle shape
            if (std::holds_alternative<sf::CircleShape>(*selectedObject.shape))
            {
                sf::CircleShape &circle = std::get<sf::CircleShape>(*selectedObject.shape);

                float circleScale = circle.getScale().x;

                ImGui::SliderFloat("Circle Scale", &circleScale, 0.1f, 4.0f);
                // Apply the scale to the selected circle shape
                circle.setScale(circleScale, circleScale);
            }
            // For example, if you want to adjust the scale of the RectangleShape
            if (std::holds_alternative<sf::RectangleShape>(*selectedObject.shape))
            {
                sf::RectangleShape &rect = std::get<sf::RectangleShape>(*selectedObject.shape);

                float rectScale = rect.getScale().x;

                ImGui::SliderFloat("RectangleShape Scale", &rectScale, 0.1f, 4.0f);
                // Apply the scale to the selected RectangleShape
                rect.setScale(rectScale, rectScale);
            }
        }
        else
        {
            ImGui::Text("You can select a shape by clicking it!");
        }

        ImGui::SameLine();
        ImGui::End();

        moveObjects();
        // update new values
    };

    void drawShapes()
    {
        if (!m_objs.empty())
        {
            for (size_t i = 0; i < m_objs.size(); ++i)
            {
                const auto &s = m_objs[i];

                if (s->exists)
                {
                    sf::Text label;
                    label.setFont(*font);
                    label.setFillColor(fontColor);
                    label.setCharacterSize(fontSize);
                    label.setString(s->name);
                    label.setOrigin(label.getGlobalBounds().getSize() / 2.f + label.getGlobalBounds().getPosition());

                    if (std::holds_alternative<sf::CircleShape>(*s->shape))
                    {
                        sf::CircleShape &circle = std::get<sf::CircleShape>(*s->shape);
                        circle.setFillColor(s->fill_color);
                        window.draw(circle);
                        label.setPosition(circle.getPosition() + (circle.getGlobalBounds().getSize() / 2.f));
                    }
                    else if (std::holds_alternative<sf::RectangleShape>(*s->shape))
                    {
                        sf::RectangleShape &rect = std::get<sf::RectangleShape>(*s->shape);
                        rect.setFillColor(s->fill_color);
                        window.draw(rect);
                        label.setPosition(rect.getPosition() + (rect.getGlobalBounds().getSize() / 2.f));
                    }

                    window.draw(label);

                    if (i == static_cast<size_t>(selectedShapeIndex))
                    {
                        sf::Color highlightColor(255, 255, 0); // Yellow color for highlight
                        if (std::holds_alternative<sf::CircleShape>(*s->shape))
                        {
                            sf::CircleShape &circle = std::get<sf::CircleShape>(*s->shape);
                            circle.setOutlineColor(highlightColor);
                            circle.setOutlineThickness(2.0f);
                        }
                        else if (std::holds_alternative<sf::RectangleShape>(*s->shape))
                        {
                            sf::RectangleShape &rect = std::get<sf::RectangleShape>(*s->shape);
                            rect.setOutlineColor(highlightColor);
                            rect.setOutlineThickness(2.0f);
                        }
                    }
                    else
                    {
                        if (std::holds_alternative<sf::CircleShape>(*s->shape))
                        {
                            sf::CircleShape &circle = const_cast<sf::CircleShape &>(std::get<sf::CircleShape>(*s->shape));
                            circle.setOutlineColor(s->outline_color);
                        }
                        else if (std::holds_alternative<sf::RectangleShape>(*s->shape))
                        {
                            sf::RectangleShape &rect = const_cast<sf::RectangleShape &>(std::get<sf::RectangleShape>(*s->shape));
                            rect.setOutlineColor(s->outline_color);
                        }
                    }
                }
            }
        }
    }

    void gameLoop()
    {
        sf::Clock deltaClock;
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(window, event);
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            ImGui::SFML::Update(window, deltaClock.restart());

            updateUI();

            window.clear(sf::Color(0, 0, 0)); // Color background

            drawShapes();

            ImGui::SFML::Render(window);
            window.display();
        }
    };
};

int main()
{
    GameManager gameManager;

    gameManager.gameLoop();

    return 0;
}