#include "scene.hpp"
#include "../game_engine.hpp"

void Scene::registerAction(int inputKey, std::string actionName)
{
    // std::printf("Registering action %s for key %d\n", actionName.c_str(), inputKey);
    m_actionMap[inputKey] = actionName;
    
    // Track registered actions for command overlay
    m_registeredActions[actionName].push_back(inputKey);
    
    // Initialize command overlay if not already done and game is available
    if (!m_commandOverlay && m_game) {
        try {
            m_commandOverlay = std::make_unique<CommandOverlay>(m_game);
            // Update command overlay
            updateCommandOverlay();
        } catch (const std::exception& e) {
            // If command overlay fails to initialize, continue without it
            std::cout << "Warning: Command overlay initialization failed: " << e.what() << std::endl;
        }
    } else if (m_commandOverlay) {
        // Update command overlay
        updateCommandOverlay();
    }
}

void Scene::updateCommandOverlay()
{
    if (m_commandOverlay) {
        try {
            m_commandOverlay->update(m_registeredActions);
        } catch (const std::exception& e) {
            std::cout << "Warning: Command overlay update failed: " << e.what() << std::endl;
        }
    }
}

void Scene::renderCommandOverlay()
{
    if (m_commandOverlay) {
        try {
            m_commandOverlay->render();
        } catch (const std::exception& e) {
            std::cout << "Warning: Command overlay render failed: " << e.what() << std::endl;
        }
    }
}

size_t Scene::width() const
{
    return m_game->window().getSize().x;
}

size_t Scene::height() const
{
    return m_game->window().getSize().y;
}

size_t Scene::currentFrame() const
{
    // return m_game->getAssets().getFrame();
    return 0;
}

std::map<int, std::string> &Scene::getActionMap()
{
    return m_actionMap;
}

bool Scene::hasEnded() const
{
    return m_currentScene == -1;
}

void Scene::setPaused(bool paused)
{
    m_paused = paused;
}
