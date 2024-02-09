#include "scene.hpp"
#include "game_engine.hpp"

// void Scene::simulate(int frame)
// {
//     m_entityManager.update();
//     if (m_paused)
//     {
//         return;
//     }
//     update();
//     for (auto &action : m_game->getAssets().getActions())
//     {
//         if (action.frame == frame)
//         {
//             doAction(action);
//         }
//     }
//     sRender();
// }

// void Scene::doAction(const Action &action)
// {
//     if (action.getType() == "START")
//     {
//         // m_entityManager.startAction(action);
//     }
//     else if (action.getType() == "END")
//     {
//         // m_entityManager.endAction(action);
//     }
// }

void Scene::registerAction(int inputKey, std::string actionName)
{
    std::printf("Registering action %s for key %d\n", actionName.c_str(), inputKey);
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

// void Scene::drawLine(const sf::Vector2f &start, const sf::Vector2f &end, const sf::Color &color)
// {
//     sf::Vertex line[] = {
//         sf::Vertex(start, color),
//         sf::Vertex(end, color)};
//     m_game->window().draw(line, 2, sf::Lines);
// }

void Scene::setPaused(bool paused)
{
    m_paused = paused;
}
