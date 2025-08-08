#pragma once
#include <map>
#include "../action.hpp"
#include "../entity_manager.hpp"
#include "../ui/command_overlay.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class GameEngine; 

class Scene {
protected:
    GameEngine* m_game;
    EntityManager m_entityManager;
    int m_currentScene;
    std::map<int, std::string> m_actionMap;
    std::map<std::string, std::vector<int>> m_registeredActions; // Track all registered actions
    std::unique_ptr<CommandOverlay> m_commandOverlay;
    bool m_paused = false;

    void setPaused(bool paused);
    void updateCommandOverlay(); // Update the command overlay with current actions
public:
    Scene() {}
    Scene(GameEngine* game) : m_game(game) {}

    virtual ~Scene() {}

    virtual void onEnd() = 0;
    virtual void update() = 0;
    virtual void sDoAction(const Action& action) = 0;
    virtual void sRender() = 0;
    virtual void init() = 0;

    void simulate(const size_t frame);
    void registerAction(const int inputKey, const std::string actionName);
    void renderCommandOverlay(); // Render the command overlay

    size_t width() const;
    size_t height() const;
    size_t currentFrame() const;

    std::map<int, std::string>& getActionMap();
    bool hasEnded() const;
};
