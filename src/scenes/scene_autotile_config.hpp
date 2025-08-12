#pragma once

#include "scene.hpp"
#include "../systems/auto_tiling_manager.hpp"
#include "../systems/auto_tiling_rule.hpp"
#include "../ui/command_overlay.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <map>

// Custom comparator for sf::Vector2i to use as map key
struct Vector2iComparator {
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }
};

enum class AssetSlicingFocus {
    ASSET_TREE,
    SLICING_FORM,
    PREVIEW_PANEL
};

enum class Panel {
    MENU,           // Left accordion menu
    CONTENT,        // Right content area
    TILESET,        // Legacy support
    RULE_GRID,      // Legacy support
    PREVIEW         // Legacy support
};

enum class AccordionSection {
    ASSET_SLICING = 0   // Configure how to slice the tileset - only section needed
};

class Scene_AutoTileConfig : public Scene {
private:
    static const int MIN_PANEL_WIDTH = 200;
    static const int ACCORDION_HEADER_HEIGHT = 35;
    static const int ACCORDION_ITEM_HEIGHT = 25;
    
    // Core systems
    std::unique_ptr<AutoTilingManager> m_autoTilingManager;
    
    // Current state
    std::string m_currentTileType;
    sf::Vector2i m_selectedTilePos;
    Panel m_currentPanel;
    AccordionSection m_currentSection;
    AssetSlicingFocus m_assetSlicingFocus;
    int m_menuSelection;
    int m_selectedFormField;
    int m_selectedFileOperation;
    int m_selectedAssetIndex;
    bool m_inRuleGridArea;  // Track if we're navigating in the rule grid area of slicing form
    
    // Asset slicing data
    std::vector<std::string> m_availableAssets;
    sf::Texture m_currentAssetTexture;
    sf::Sprite m_currentAssetSprite;
    
    // Slicing parameters
    int m_tileWidth;
    int m_tileHeight;
    int m_marginX;
    int m_marginY;
    int m_spacingX;
    int m_spacingY;
    int m_offsetX;      // Starting X position within texture
    int m_offsetY;      // Starting Y position within texture
    int m_gridLineThickness; // Grid line thickness in pixels
    
    // Preview panel navigation
    float m_previewScrollX; // Scroll offset for large assets
    float m_previewScrollY; // Scroll offset for large assets
    float m_currentPreviewWidth;  // Current preview area width
    float m_currentPreviewHeight; // Current preview area height
    
    // Scrollable content offsets
    float m_assetTreeScrollY;     // Scroll offset for asset tree
    float m_slicingFormScrollY;   // Scroll offset for slicing form
    
    // Rule editing state
    std::vector<RuleCondition> m_currentRule;
    bool m_editingRule;
    int m_ruleGridSelection;
    sf::Vector2i m_tilesetSelection;
    
    // Tile navigation and per-tile rules
    sf::Vector2i m_selectedTileInPreview;  // Currently selected tile in preview
    int m_tilesX, m_tilesY;                // Number of tiles in X and Y directions
    std::map<sf::Vector2i, std::vector<RuleCondition>, Vector2iComparator> m_tileRules;  // Rules for each tile position
    sf::Sprite m_selectedTileSprite;       // Sprite for displaying selected tile
    sf::Texture m_selectedTileTexture;     // Texture for selected tile
    
    // Layout dimensions
    int m_menuPanelWidth;
    int m_contentPanelWidth;
    int m_panelHeight;
    
    // UI Elements - Base
    sf::RectangleShape m_background;
    sf::Text m_titleText;
    sf::Text m_currentTileTypeText;
    
    // UI Elements - Menu Panel (Left)
    sf::RectangleShape m_menuPanel;
    std::vector<sf::RectangleShape> m_accordionHeaders;
    std::vector<sf::Text> m_accordionHeaderTexts;
    std::vector<bool> m_accordionExpanded;
    std::vector<std::vector<sf::RectangleShape>> m_accordionItems;
    std::vector<std::vector<sf::Text>> m_accordionItemTexts;
    
    // UI Elements - Content Panel (Right)
    sf::RectangleShape m_contentPanel;
    sf::Text m_contentTitle;
    
    // Legacy UI elements (for compatibility)
    sf::RectangleShape m_tilesetPanel;
    sf::RectangleShape m_rulesPanel;
    sf::RectangleShape m_previewPanel;
    
    // Tileset display
    sf::Sprite m_tilesetSprite;
    sf::RectangleShape m_tilesetSelector;
    
    // Rule configuration
    std::vector<sf::RectangleShape> m_ruleGrid;
    std::vector<sf::Text> m_ruleGridLabels;
    
    // Tile Type UI
    std::vector<sf::RectangleShape> m_tileTypeButtons;
    std::vector<sf::Text> m_tileTypeButtonTexts;
    
    // File Operations UI
    std::vector<sf::RectangleShape> m_fileOpButtons;
    std::vector<sf::Text> m_fileOpButtonTexts;

public:
    Scene_AutoTileConfig(GameEngine* gameEngine);
    
    void init() override;
    void onEnd() override;
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;

private:
    // Layout and UI setup
    void calculateResponsiveLayout();
    void setupUI();
    void setupAccordionMenu();
    int setupAccordionItems(AccordionSection section, int yPos);
    void setupContentPanel();
    void updateContentPanel();
    
    // Accordion menu management
    void toggleAccordionSection(AccordionSection section);
    void expandAccordionSection(AccordionSection section);
    void collapseAccordionSection(AccordionSection section);
    void updateOverlayCommands();
    
    // Content panel drawing methods
    void drawCurrentSectionContent(float x, float y, float width, float height);
    void drawAssetSlicingPanel(float x, float y, float width, float height);
    
    // Content panel management
    
    // Navigation and input
    void handleNavigation(const std::string& direction);
    void handleAssetSlicingNavigation(const std::string& direction);
    void handleSelection();
    void handleConfirmation();
    void switchPanel();
    void switchPanelInCurrentTab();
    void switchPanelInCurrentTabReverse();

    
    // Asset slicing methods
    void loadSelectedAsset();
    void initializeAvailableAssets();
    std::string getFormFieldName(int fieldIndex);
    std::string getFileOperationName(int operationIndex);
    void updateSlicingParameters();
    void adjustSlicingParameter(int delta);
    void drawAssetTree(float x, float y, float width, float height);
    void drawSlicingForm(float x, float y, float width, float height);
    void drawAssetPreview(float x, float y, float width, float height);
    void drawSlicingGrid(float x, float y, float scale);
    void drawSlicingGridToTexture(sf::RenderTexture& texture, float x, float y, float scale);
    
    // Helper function to get tile content rectangle (inside grid lines)
    sf::IntRect getTileContentRect(int tileX, int tileY) const;
    
    // Tileset operations

    void selectTileInTileset(sf::Vector2i position);
    
    // Rule operations
    void createNewRule();
    void deleteCurrentRule();
    void saveCurrentRule();
    void loadRule(int index);
    void loadExistingRulesForTile(sf::Vector2i tilePos);
    void displayNoConfigMessage();
    void displayNoRuleMessage(sf::Vector2i tilePos);
    void cycleRuleCondition(int cellIndex);
    
    // Tile Type operations
    void switchTileType(int direction);
    
    // Asset Configuration operations
    void createDefaultAssetConfig(const std::string& assetName);
    void loadAssetConfig(const std::string& assetName);
    void saveAssetConfig(const std::string& assetName);
    std::string getAssetConfigPath(const std::string& assetName);
    void updateSlicingParametersFromConfig();
    void applyCurrentSlicingParameters();
    
    // Tile Navigation and Rule Management
    void handleTileNavigation(const std::string& direction);
    void handlePreviewPanelNavigation(const std::string& direction);
    void selectTileInPreview(sf::Vector2i tilePos);
    void updateTileGridDimensions();
    void extractSelectedTile();
    void loadRulesForSelectedTile();
    void saveRulesForSelectedTile();
    void drawSelectedTileInfo(float x, float y, float width, float height);
    void drawTileRuleGrid(float x, float y, float width, float height);
    void drawTileRuleGridInForm(float x, float y, float width, float height);
    sf::Vector2i getTilePositionFromPixel(sf::Vector2f pixelPos);
    bool isValidTilePosition(sf::Vector2i tilePos) const;
    
    // Utility functions
    sf::Vector2i getTilesetGridPosition(sf::Vector2i mousePos);
    int getRuleGridCell(sf::Vector2i mousePos);
    int countActiveConditions() const;
    sf::Color getRuleConditionColor(RuleCondition condition) const;
    std::string ruleConditionToString(RuleCondition condition) const;
    std::string getPositionName(int row, int col) const;
    std::string getSectionName(AccordionSection section);
};
