#include "scene.hpp"
#include "game_engine.hpp"

void Scene::registerAction(int inputKey, std::string actionName)
{
    // std::printf("Registering action %s for key %d\n", actionName.c_str(), inputKey);
    m_actionMap[inputKey] = actionName;
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
