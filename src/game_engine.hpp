#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include "entity_manager.hpp"
#include "input_manager.hpp"

const unsigned int INIT_WINDOW_WIDTH = 400;
const unsigned int INIT_WINDOW_HEIGHT = 400;

enum Options
{
    Window,
    Font,
    Player,
    Enemy,
    Bullet,
    Invalid
};

class GameEngine 
{
    sf::RenderWindow m_window;
    EntityManager m_entities;
    InputManager m_inputs;
    bool m_paused = false;
    bool m_running = true;
    std::shared_ptr<sf::Font> font;
    int font_size;
    sf::Color font_color;
    sf::Color bg_color;
    int window_width;
    int window_height;
public:
    GameEngine() : window_width{400}, window_height{400} {
        init();
    };
    ~GameEngine(){
        ImGui::SFML::Shutdown();
    };

    void init() {
        loadConfigFromFile("config.txt");
    }

    Options resolveOption(std::string& input)
    {
        if (input == "Window")
            return Options::Window;
        if (input == "Font")
            return Options::Font;
        if (input == "Player")
            return Options::Player;
        if (input == "Enemy")
            return Options::Enemy;
        if (input == "Bullet")
            return Options::Bullet;
        return Options::Invalid;
    }

    void loadConfigFromFile(const std::string &filename)
    {
        std::ifstream fin(filename);
        std::string type;

        while (fin >> type)
        {
            switch (resolveOption(type))
            {
            case Options::Window:
            {
                fin >> window_width >> window_height;

                m_window.create(sf::VideoMode(window_width, window_height), "Game in SFML and ImGUI");
                m_window.setFramerateLimit(60);
                ImGui::SFML::Init(m_window);
                break;
            }
            case Options::Font:
            {
                std::string font_source;
                fin >> font_source;

                font = std::make_shared<sf::Font>();
                if (!font->loadFromFile(font_source))
                {
                    std::cerr << "Error while trying to load font! \n";
                }
                fin >> font_size;

                int r, g, b;

                fin >> r >> g >> b;
                font_color = sf::Color(r, g, b);
                break;
            }
            case Options::Player:
            {
                auto e = m_entities.addEntity("Player");
                
                float sx;
                float sy;
                int r,g,b;
                float rad;
                int v;

                fin >> sx >> sy >> r >> g >> b >> rad >> v;

                Vec2 i_speed = { sx, sy };
                Vec2 i_pos = {window_width/2.f, window_height/2.f};
                sf::Color color = sf::Color(r,g,b);

                e->cShape = std::make_shared<CCShape>();
                e->cShape->shape.setFillColor(color);
                e->cShape->shape.setRadius(rad);
                e->cShape->shape.setPointCount(v);
                e->cTransform = std::make_shared<CTransform>(i_pos, i_speed);
                
                break;
            }
            case Options::Enemy:
            {
                auto e = m_entities.addEntity("Enemy");

                int rs1;
                int rs2;
                int r,g,b;
                float rad;
                float sr;
                int rv1;
                int rv2;

                fin >> rs1 >> rs2 >> r >> g >> b >> rad >> sr >> rv1 >> rv2;

                float r_speed_value = (float)(rs1 + ( std::rand() % ( 1 + rs2 - rs1)));
                int r_num_sides = rv1 + ( std::rand() % ( 1 + rv2 - rv1));

                Vec2 i_speed = { r_speed_value, r_speed_value };
                Vec2 i_pos = {window_width/2.f, window_height/2.f};
                sf::Color color = sf::Color(r,g,b);

                e->cShape = std::make_shared<CCShape>();
                e->cShape->shape.setFillColor(color);
                e->cShape->shape.setRadius(rad);
                e->cShape->shape.setPointCount(r_num_sides);
                e->cTransform = std::make_shared<CTransform>(i_pos, i_speed);
                e->cLifespan = std::make_shared<CLifespan>(sr);


                break;
            }
            case Options::Bullet:
            {
                auto e = m_entities.addEntity("Bullet");

                float sx;
                float sy;
                int r,g,b;
                float rad;
                int v;
                int l;

                fin >> sx >> sy >> r >> g >> b >> rad >> v >> l;
                

                Vec2 i_speed = { sx, sy };
                Vec2 i_pos = {window_width/2.f, window_height/2.f};
                sf::Color color = sf::Color(r,g,b);

                e->cShape = std::make_shared<CCShape>();
                e->cShape->shape.setFillColor(color);
                e->cShape->shape.setRadius(rad);
                e->cShape->shape.setPointCount(v);
                e->cTransform = std::make_shared<CTransform>(i_pos, i_speed);
                e->cLifespan = std::make_shared<CLifespan>(l);

                break;
            }
            case Options::Invalid:
                break;
            default:
                break;
            }
        }
    }

    void update(){
        sf::Clock delta_clock;
        while (m_window.isOpen())
        {
            sf::Event event;
            while (m_window.pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(m_window, event);
                if (event.type == sf::Event::Closed)
                    m_window.close();
            }

            ImGui::SFML::Update(m_window, delta_clock.restart());

            // updateUI();

            // update entity manager
            m_entities.update();

            // handle user input
            m_inputs.update();

            // movement
            sMovement(m_entities.getEntities());

            // collision
            sCollisions(m_entities.getEntities());


            m_window.clear(sf::Color(0, 0, 0)); // Color background

            // render
            sRender(m_entities.getEntities(), m_window);

            ImGui::SFML::Render(m_window);
            m_window.display();
        }
    }

    void sMovement(EntityVec &entities);

    void sCollisions(EntityVec &entities);

    void sRender(EntityVec &entities, sf::RenderWindow& win);

    void sUserInput(EntityVec &entities) const; 

    void sEnemySpawner() {};
};
