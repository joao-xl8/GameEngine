#pragma once

#include "scene.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

class Scene_Loading : public Scene
{
private:
    // Loading state
    std::string m_nextSceneName;
    std::shared_ptr<Scene> m_nextScene;
    std::function<std::shared_ptr<Scene>()> m_sceneFactory;
    
    // Asset loading
    std::vector<std::string> m_assetsToLoad;
    std::vector<std::string> m_soundsToLoad;
    size_t m_currentAssetIndex = 0;
    size_t m_totalAssets = 0;
    bool m_loadingComplete = false;
    
    // Visual elements
    sf::Text m_loadingText;
    sf::Text m_progressText;
    sf::RectangleShape m_progressBarBackground;
    sf::RectangleShape m_progressBarFill;
    sf::RectangleShape m_background;
    
    // Timing
    float m_loadingTimer = 0.0f;
    float m_minLoadingTime = 1.0f; // Minimum time to show loading screen
    
    // Asset loading methods
    void loadNextAsset();
    void updateProgress();
    void setupVisuals();
    void loadAssetsFromConfig(const std::string& sceneName);
    
public:
    Scene_Loading(GameEngine* game, const std::string& nextSceneName, 
                  std::function<std::shared_ptr<Scene>()> sceneFactory,
                  const std::vector<std::string>& assetsToLoad = {},
                  const std::vector<std::string>& soundsToLoad = {});
    
    void init() override;
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
    void onEnd() override;
    
    // Static helper methods for common scene transitions
    static void loadPlayScene(GameEngine* game, const std::string& levelPath);
    static void loadMenuScene(GameEngine* game);
    static void loadMapEditorScene(GameEngine* game);
};
