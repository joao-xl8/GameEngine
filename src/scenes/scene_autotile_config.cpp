#include "scene_autotile_config.hpp"
#include "../game_engine.hpp"
#include "../action_types.hpp"
#include "scene_loading.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

Scene_AutoTileConfig::Scene_AutoTileConfig(GameEngine* gameEngine) 
    : Scene(gameEngine), 
      m_currentTileType("Wall"),
      m_selectedTilePos(0, 0), 
      m_currentPanel(Panel::MENU), 
      m_currentSection(AccordionSection::ASSET_SLICING),
      m_menuSelection(0),
      m_inRuleGridArea(false),
      m_currentRule(9, RuleCondition::IGNORE),
      m_editingRule(false),
      m_ruleGridSelection(0), 
      m_tilesetSelection(0, 0),
      m_selectedTileInPreview(0, 0),
      m_tilesX(0),
      m_tilesY(0) {
    
    m_autoTilingManager = std::make_unique<AutoTilingManager>(gameEngine);
    
    // Initialize accordion state - start with Asset Slicing expanded
    m_accordionExpanded.resize(3, false);
    m_accordionExpanded[static_cast<int>(AccordionSection::ASSET_SLICING)] = true;
    
    init();
}

void Scene_AutoTileConfig::init() {
    
    // Standard navigation controls (required for input handling)
    // Note: These are obvious to users and shouldn't clutter the overlay
    registerAction(sf::Keyboard::Escape, ActionTypes::BACK);
    registerAction(sf::Keyboard::W, ActionTypes::UP);
    registerAction(sf::Keyboard::S, ActionTypes::DOWN);
    registerAction(sf::Keyboard::A, ActionTypes::LEFT);
    registerAction(sf::Keyboard::D, ActionTypes::RIGHT);
    registerAction(sf::Keyboard::Space, ActionTypes::SELECT);
    registerAction(sf::Keyboard::Space, ActionTypes::CONFIRM);
    
    // Accordion interface controls (show these - they're specific to this scene)
    registerAction(sf::Keyboard::Tab, "SWITCH_PANEL");           // Switch between Menu and Content
    registerAction(sf::Keyboard::Enter, "TOGGLE_SECTION");       // Expand/Collapse accordion section
    registerAction(sf::Keyboard::Num1, "GOTO_ASSET_SLICING");    // Jump to Asset Slicing
    
    // Context-sensitive controls (change based on current section)
    registerAction(sf::Keyboard::N, "NEW_ITEM");                 // New rule/tile type/etc
    registerAction(sf::Keyboard::C, "DELETE_ITEM");              // Delete/Cancel
    registerAction(sf::Keyboard::R, "SAVE_ITEM");                // Save rule/config/etc
    registerAction(sf::Keyboard::I, "INFO");                     // Show info/details
    registerAction(sf::Keyboard::T, "SWITCH_TILE_TYPE");         // Switch tile type
    registerAction(sf::Keyboard::L, "LOAD");                     // Load configuration
    registerAction(sf::Keyboard::E, "EXPORT");                   // Export configuration
    
    // Initialize selection state
    m_currentPanel = Panel::CONTENT; // Start focused on content
    m_currentSection = AccordionSection::ASSET_SLICING;
    m_assetSlicingFocus = AssetSlicingFocus::ASSET_TREE;
    m_tilesetSelection = sf::Vector2i(0, 0);
    m_ruleGridSelection = 0;
    m_menuSelection = 0;
    m_selectedFormField = 0;
    m_selectedFileOperation = 0;
    m_selectedAssetIndex = 0;
    
    // Initialize slicing parameters with defaults
    m_tileWidth = 64;
    m_tileHeight = 64;
    m_marginX = 0;
    m_marginY = 0;
    m_spacingX = 0;
    m_spacingY = 0;
    m_offsetX = 0;
    m_offsetY = 0;
    m_gridLineThickness = 1; // Default 1 pixel grid lines
    m_previewScrollX = 0;
    m_previewScrollY = 0;
    m_currentPreviewWidth = 0;
    m_currentPreviewHeight = 0;
    m_assetTreeScrollY = 0;
    m_slicingFormScrollY = 0;
    
    initializeAvailableAssets();
    
    calculateResponsiveLayout();
    
    setupUI();
    
    setupAccordionMenu();
    
    setupContentPanel();
    
}

void Scene_AutoTileConfig::onEnd() {
    // Cleanup when scene ends
}

void Scene_AutoTileConfig::update() {
    sRender();
}

void Scene_AutoTileConfig::calculateResponsiveLayout() {
    sf::Vector2u windowSize = m_game->window().getSize();
    
    // Use full width for content (no left menu)
    m_panelHeight = windowSize.y - 120; // Leave space for title and bottom margin
    m_contentPanelWidth = windowSize.x - 40; // Full width minus margins
    m_menuPanelWidth = 0; // No menu panel
}

void Scene_AutoTileConfig::setupUI() {
    // Background - make sure it covers the entire window
    sf::Vector2u windowSize = m_game->window().getSize();
    m_background.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    m_background.setFillColor(sf::Color(30, 30, 35));
    
    // Title - positioned at top center
    m_titleText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_titleText.setString("Auto-Tile Configuration - Unity-Style Inspector");
    m_titleText.setCharacterSize(20);
    m_titleText.setFillColor(sf::Color::White);
    
    // Center the title
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setPosition((windowSize.x - titleBounds.width) / 2, 10);
    
    // Current tile type text - positioned below title
    m_currentTileTypeText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_currentTileTypeText.setString("Current: " + m_currentTileType);
    m_currentTileTypeText.setCharacterSize(14);
    m_currentTileTypeText.setFillColor(sf::Color(200, 200, 200));
    
    // Center the tile type text
    sf::FloatRect tileTypeBounds = m_currentTileTypeText.getLocalBounds();
    m_currentTileTypeText.setPosition((windowSize.x - tileTypeBounds.width) / 2, 35);
}

void Scene_AutoTileConfig::setupAccordionMenu() {
    // Menu panel background - positioned on the left side
    m_menuPanel.setSize(sf::Vector2f(m_menuPanelWidth, m_panelHeight));
    m_menuPanel.setPosition(10, 60); // Start lower to avoid title overlap
    m_menuPanel.setFillColor(sf::Color(45, 45, 50));
    m_menuPanel.setOutlineThickness(2);
    m_menuPanel.setOutlineColor(sf::Color(100, 100, 100));
    
    // Setup accordion sections - now only 3 sections
    const std::vector<std::string> sectionNames = {
        "Asset Slicing",
        "Tile Types",
        "File Operations"
    };
    
    const std::vector<sf::Color> sectionColors = {
        sf::Color(80, 140, 200),  // Brighter blue for Asset Slicing
        sf::Color(140, 200, 80),  // Brighter green for Tile Types
        sf::Color(200, 80, 140)   // Brighter purple for File Operations
    };
    
    m_accordionHeaders.resize(3);
    m_accordionHeaderTexts.resize(3);
    
    int yPos = 70; // Start position for headers
    for (int i = 0; i < 3; i++) {
        // Header background with section-specific color
        m_accordionHeaders[i].setSize(sf::Vector2f(m_menuPanelWidth - 20, ACCORDION_HEADER_HEIGHT));
        m_accordionHeaders[i].setPosition(20, yPos);
        
        // Use brighter colors for better visibility
        if (m_accordionExpanded[i]) {
            m_accordionHeaders[i].setFillColor(sectionColors[i]);
            m_accordionHeaders[i].setOutlineThickness(3);
            m_accordionHeaders[i].setOutlineColor(sf::Color::White);
        } else {
            sf::Color dimmedColor = sectionColors[i];
            dimmedColor.r = dimmedColor.r * 0.7f;
            dimmedColor.g = dimmedColor.g * 0.7f; 
            dimmedColor.b = dimmedColor.b * 0.7f;
            m_accordionHeaders[i].setFillColor(dimmedColor);
            m_accordionHeaders[i].setOutlineThickness(1);
            m_accordionHeaders[i].setOutlineColor(sf::Color(150, 150, 150));
        }
        
        // Header text
        m_accordionHeaderTexts[i].setFont(m_game->getAssets().getFont("ShareTech"));
        m_accordionHeaderTexts[i].setString(sectionNames[i]);
        m_accordionHeaderTexts[i].setCharacterSize(16);
        m_accordionHeaderTexts[i].setFillColor(sf::Color::White);
        m_accordionHeaderTexts[i].setPosition(25, yPos + 8);
        
        yPos += ACCORDION_HEADER_HEIGHT + 5;
        
        // Add items if expanded
        if (m_accordionExpanded[i]) {
            yPos += setupAccordionItems(static_cast<AccordionSection>(i), yPos);
        }
    }
}

int Scene_AutoTileConfig::setupAccordionItems(AccordionSection section, int yPos) {
    int sectionIndex = static_cast<int>(section);
    std::vector<std::string> itemNames;
    std::vector<sf::Color> itemColors;
    
    switch (section) {
        case AccordionSection::ASSET_SLICING:
            itemNames = {"Configure Tile Size", "Set Grid Layout", "Adjust Margins", "Preview Slicing"};
            itemColors = {sf::Color(80, 140, 200), sf::Color(90, 150, 210), sf::Color(100, 160, 220), sf::Color(110, 170, 230)};
            break;
//         case AccordionSection::TILE_TYPES:
            itemNames = {"Wall", "Ground", "Water", "Add New Type"};
            itemColors = {sf::Color(140, 200, 80), sf::Color(150, 210, 90), sf::Color(160, 220, 100), sf::Color(170, 230, 110)};
            break;
//         case AccordionSection::FILE_OPERATIONS:
            itemNames = {"Save Current", "Load Config", "Export All", "Import Config"};
            itemColors = {sf::Color(200, 80, 140), sf::Color(210, 90, 150), sf::Color(220, 100, 160), sf::Color(230, 110, 170)};
            break;
    }
    
    // Clear existing items
    if (static_cast<size_t>(sectionIndex) < m_accordionItems.size()) {
        m_accordionItems[sectionIndex].clear();
        m_accordionItemTexts[sectionIndex].clear();
    } else {
        m_accordionItems.resize(4);
        m_accordionItemTexts.resize(4);
    }
    
    m_accordionItems[sectionIndex].resize(itemNames.size());
    m_accordionItemTexts[sectionIndex].resize(itemNames.size());
    
    int startY = yPos;
    for (size_t i = 0; i < itemNames.size(); i++) {
        // Item background with gradient effect
        m_accordionItems[sectionIndex][i].setSize(sf::Vector2f(m_menuPanelWidth - 20, ACCORDION_ITEM_HEIGHT));
        m_accordionItems[sectionIndex][i].setPosition(30, yPos);
        m_accordionItems[sectionIndex][i].setFillColor(itemColors[i]);
        m_accordionItems[sectionIndex][i].setOutlineThickness(1);
        m_accordionItems[sectionIndex][i].setOutlineColor(sf::Color(65, 65, 70));
        
        // Item text with proper indentation
        m_accordionItemTexts[sectionIndex][i].setFont(m_game->getAssets().getFont("ShareTech"));
        m_accordionItemTexts[sectionIndex][i].setString("  " + itemNames[i]);
        m_accordionItemTexts[sectionIndex][i].setCharacterSize(14);
        m_accordionItemTexts[sectionIndex][i].setFillColor(sf::Color::White);
        m_accordionItemTexts[sectionIndex][i].setPosition(35, yPos + 5);
        
        yPos += ACCORDION_ITEM_HEIGHT + 2;
    }
    
    return yPos - startY; // Return height used
}

void Scene_AutoTileConfig::setupContentPanel() {
    // Content panel background - positioned to the right of menu panel
    int contentX = 20 + m_menuPanelWidth + 10; // Menu panel + gap
    m_contentPanel.setSize(sf::Vector2f(m_contentPanelWidth, m_panelHeight));
    m_contentPanel.setPosition(contentX, 60);
    m_contentPanel.setFillColor(sf::Color(50, 50, 55));
    m_contentPanel.setOutlineThickness(2);
    m_contentPanel.setOutlineColor(sf::Color(100, 100, 100));
    
    // Content title
    m_contentTitle.setFont(m_game->getAssets().getFont("ShareTech"));
    m_contentTitle.setCharacterSize(18);
    m_contentTitle.setFillColor(sf::Color::White);
    m_contentTitle.setPosition(contentX + 10, 70);
    
    // Setup initial content based on expanded section
    updateContentPanel();
}

void Scene_AutoTileConfig::updateContentPanel() {
    switch (m_currentSection) {
        case AccordionSection::ASSET_SLICING:
            m_contentTitle.setString("📐 Asset Slicing & Selection");
            break;
        default:
            // Only asset slicing is supported
            m_contentTitle.setString("📐 Asset Slicing & Selection");
            break;
    }
}

void Scene_AutoTileConfig::sDoAction(const Action& action) {
    if (action.getType() == "START") {
        if (action.getName() == ActionTypes::BACK) {
            Scene_Loading::loadMenuScene(m_game);
        }
        // 1-3 Keys: Switch tabs
        else if (action.getName() == "GOTO_ASSET_SLICING") {
            expandAccordionSection(AccordionSection::ASSET_SLICING);
        }
        // Tab Key: Switch panels within current tab (check for Shift modifier)
        else if (action.getName() == "SWITCH_PANEL") {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
                switchPanelInCurrentTabReverse();
            } else {
                switchPanelInCurrentTab();
            }
        }
        // WASD: Navigate within current panel
        else if (action.getName() == ActionTypes::UP) {
            handleNavigation("UP");
        }
        else if (action.getName() == ActionTypes::DOWN) {
            handleNavigation("DOWN");
        }
        else if (action.getName() == ActionTypes::LEFT) {
            handleNavigation("LEFT");
        }
        else if (action.getName() == ActionTypes::RIGHT) {
            handleNavigation("RIGHT");
        }
        // Space/Enter: Select/Confirm
        else if (action.getName() == ActionTypes::SELECT || action.getName() == ActionTypes::CONFIRM) {
            handleSelection();
        }
        else if (action.getName() == "SAVE_ITEM") {
            // Context-sensitive saving based on current section
            switch (m_currentSection) {
                case AccordionSection::ASSET_SLICING:
                    if (m_assetSlicingFocus == AssetSlicingFocus::PREVIEW_PANEL) {
                        // Save rules for the currently selected tile
                        saveRulesForSelectedTile();
                        // Also save the entire asset configuration including all tile rules
                        if (m_selectedAssetIndex >= 0 && m_selectedAssetIndex < static_cast<int>(m_availableAssets.size())) {
                            std::string assetName = m_availableAssets[m_selectedAssetIndex];
                            saveAssetConfig(assetName);
                        }
                    } else {
                        // Save current asset configuration (slicing parameters)
                        if (m_selectedAssetIndex >= 0 && m_selectedAssetIndex < static_cast<int>(m_availableAssets.size())) {
                            std::string assetName = m_availableAssets[m_selectedAssetIndex];
                            saveAssetConfig(assetName);
                        } else {
                        }
                    }
                    break;
            }
        }
        else if (action.getName() == "LOAD") {
        }
        else {
            std::cout << "Unhandled action: " << action.getName() << std::endl;
        }
    }
}

void Scene_AutoTileConfig::sRender() {
    // Get game view for proper positioning
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    // Draw background using game view
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    background.setFillColor(sf::Color(25, 25, 30));
    m_game->window().draw(background);
    
    // Use full width for content (no navigation panel)
    float contentX = viewCenter.x - viewSize.x / 2 + 10; // Small margin
    float contentY = viewCenter.y - viewSize.y / 2 + 10; // Small margin
    float contentWidth = viewSize.x - 20; // Full width minus margins
    float contentHeight = viewSize.y - 20; // Full height minus margins
    
    // Draw content based on current section (always Asset Slicing for now)
    drawCurrentSectionContent(contentX, contentY, contentWidth, contentHeight);
}

void Scene_AutoTileConfig::updateOverlayCommands() {
    // Create a context-specific command list that will be shown in the overlay
    // This provides dynamic, context-aware help for the user
    
    // The base class will handle the actual overlay rendering
    // but we can influence what gets shown through the registered actions
    updateCommandOverlay();
}

void Scene_AutoTileConfig::drawCurrentSectionContent(float x, float y, float width, float height) {
    // Always show Asset Slicing (remove menu navigation)
    drawAssetSlicingPanel(x, y, width, height);
}

void Scene_AutoTileConfig::drawAssetSlicingPanel(float x, float y, float width, float height) {
    // Title
    sf::Text title;
    title.setFont(m_game->getAssets().getFont("ShareTech"));
    title.setString("Asset Slicing & Configuration");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color(80, 140, 200));
    title.setPosition(x, y);
    m_game->window().draw(title);
    
    // Save button (right side of title area)
    float buttonWidth = 120;
    float buttonHeight = 30;
    float buttonX = x + width - buttonWidth - 20;
    float buttonY = y + 5;
    
    sf::RectangleShape saveButton;
    saveButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    saveButton.setPosition(buttonX, buttonY);
    saveButton.setFillColor(sf::Color(60, 120, 60)); // Green background
    saveButton.setOutlineThickness(2);
    saveButton.setOutlineColor(sf::Color(80, 160, 80)); // Lighter green border
    m_game->window().draw(saveButton);
    
    sf::Text saveButtonText;
    saveButtonText.setFont(m_game->getAssets().getFont("ShareTech"));
    saveButtonText.setString("Save | R");
    saveButtonText.setCharacterSize(16);
    saveButtonText.setFillColor(sf::Color::White);
    
    // Center the text in the button
    sf::FloatRect textBounds = saveButtonText.getLocalBounds();
    saveButtonText.setPosition(
        buttonX + (buttonWidth - textBounds.width) / 2,
        buttonY + (buttonHeight - textBounds.height) / 2 - 2
    );
    m_game->window().draw(saveButtonText);
    
    // Layout: Asset Tree (left) | Slicing Form (center) | Preview (right)
    // Add padding between panels and adjust sizes accordingly
    float panelPadding = 10.0f; // Padding between panels
    float totalPadding = panelPadding * 2; // Two gaps between three panels
    float availableWidth = width - totalPadding; // Width available for panels after padding
    
    // Calculate panel widths from available width (not total width)
    float assetTreeWidth = availableWidth * 0.2f;   // 20% of available width
    float slicingFormWidth = availableWidth * 0.35f; // 35% of available width
    float previewWidth = availableWidth * 0.45f;     // 45% of available width
    float panelHeight = height - 40; // Leave space for title
    
    // Store actual preview dimensions for navigation (set BEFORE drawing preview)
    m_currentPreviewWidth = previewWidth;
    m_currentPreviewHeight = panelHeight;

    // Asset Tree Panel (Left) - starts at content edge
    float assetTreeX = x;
    drawAssetTree(assetTreeX, y + 40, assetTreeWidth, panelHeight);
    
    // Slicing Form Panel (Center) - with padding after asset tree
    float slicingFormX = assetTreeX + assetTreeWidth + panelPadding;
    drawSlicingForm(slicingFormX, y + 40, slicingFormWidth, panelHeight);
    
    // Preview Panel (Right) - with padding after slicing form
    float previewX = slicingFormX + slicingFormWidth + panelPadding;

    drawAssetPreview(previewX, y + 40, previewWidth, panelHeight);
}

void Scene_AutoTileConfig::drawAssetTree(float x, float y, float width, float height) {
    // Asset Tree Panel Background
    sf::RectangleShape treePanel;
    treePanel.setSize(sf::Vector2f(width, height));
    treePanel.setPosition(x, y);
    treePanel.setFillColor(sf::Color(45, 45, 50));
    treePanel.setOutlineThickness(m_assetSlicingFocus == AssetSlicingFocus::ASSET_TREE ? 3 : 1);
    treePanel.setOutlineColor(m_assetSlicingFocus == AssetSlicingFocus::ASSET_TREE ? sf::Color::Yellow : sf::Color(70, 70, 75));
    m_game->window().draw(treePanel);
    
    // Tree Title (always visible)
    sf::Text treeTitle;
    treeTitle.setFont(m_game->getAssets().getFont("ShareTech"));
    treeTitle.setString("Available Assets");
    treeTitle.setCharacterSize(16);
    treeTitle.setFillColor(sf::Color::White);
    treeTitle.setPosition(x + 10, y + 10);
    m_game->window().draw(treeTitle);
    
    // Asset List (scrollable with simple bounds checking)
    float itemY = y + 40 - m_assetTreeScrollY;
    float itemHeight = 30;
    float scrollAreaTop = y + 40;
    float scrollAreaBottom = y + height;
    
    for (size_t i = 0; i < m_availableAssets.size(); i++) {
        // Only draw items that are visible in the scroll area
        if (itemY + itemHeight >= scrollAreaTop && itemY <= scrollAreaBottom) {
            // Asset item background
            sf::RectangleShape itemBg;
            itemBg.setSize(sf::Vector2f(width - 20, itemHeight));
            itemBg.setPosition(x + 10, itemY);
            
            // Highlight selected asset
            if (static_cast<int>(i) == m_selectedAssetIndex) {
                itemBg.setFillColor(sf::Color(80, 140, 200, 150));
                itemBg.setOutlineThickness(2);
                itemBg.setOutlineColor(sf::Color::White);
            } else {
                itemBg.setFillColor(sf::Color(55, 55, 60));
                itemBg.setOutlineThickness(1);
                itemBg.setOutlineColor(sf::Color(65, 65, 70));
            }
            
            m_game->window().draw(itemBg);
            
            // Asset name
            sf::Text assetName;
            assetName.setFont(m_game->getAssets().getFont("ShareTech"));
            assetName.setString(m_availableAssets[i]);
            assetName.setCharacterSize(14);
            assetName.setFillColor(sf::Color::White);
            assetName.setPosition(x + 15, itemY + 8);
            m_game->window().draw(assetName);
        }
        
        itemY += itemHeight + 5;
    }
    
    // Scroll indicator if content is scrollable (drawn in original view)
    float totalContentHeight = m_availableAssets.size() * (itemHeight + 5);
    float availableHeight = height - 40;
    if (totalContentHeight > availableHeight) {
        // Draw scroll bar
        float scrollBarHeight = (availableHeight / totalContentHeight) * availableHeight;
        float scrollBarY = y + 40 + (m_assetTreeScrollY / totalContentHeight) * availableHeight;
        
        sf::RectangleShape scrollBar;
        scrollBar.setSize(sf::Vector2f(4, scrollBarHeight));
        scrollBar.setPosition(x + width - 8, scrollBarY);
        scrollBar.setFillColor(sf::Color(150, 150, 150));
        m_game->window().draw(scrollBar);
    }
}

void Scene_AutoTileConfig::drawSlicingForm(float x, float y, float width, float height) {
    // Slicing Form Panel Background
    sf::RectangleShape formPanel;
    formPanel.setSize(sf::Vector2f(width, height));
    formPanel.setPosition(x, y);
    formPanel.setFillColor(sf::Color(50, 50, 55));
    formPanel.setOutlineThickness(m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM ? 3 : 1);
    formPanel.setOutlineColor(m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM ? sf::Color::Yellow : sf::Color(70, 70, 75));
    m_game->window().draw(formPanel);
    
    // Form Title
    sf::Text formTitle;
    formTitle.setFont(m_game->getAssets().getFont("ShareTech"));
    formTitle.setString("Slicing Parameters");
    formTitle.setCharacterSize(16);
    formTitle.setFillColor(sf::Color::White);
    formTitle.setPosition(x + 10, y + 10);
    m_game->window().draw(formTitle);
    
    // Form Fields - Two column layout for better space utilization
    std::vector<std::string> fieldNames = {"Tile Width:", "Tile Height:", "Margin X:", "Margin Y:", "Spacing X:", "Spacing Y:", "Offset X:", "Offset Y:", "Grid Line:"};
    std::vector<int*> fieldValues = {&m_tileWidth, &m_tileHeight, &m_marginX, &m_marginY, &m_spacingX, &m_spacingY, &m_offsetX, &m_offsetY, &m_gridLineThickness};
    
    float upperHeight = height * 0.50f; // Reduced space to close gap with rule block
    float fieldStartY = y + 40;
    float fieldHeight = 34;
    float columnWidth = (width - 30) / 2; // Two columns with margins
    
    for (size_t i = 0; i < fieldNames.size(); i++) {
        int col = i % 2; // 0 for left column, 1 for right column
        int row = i / 2; // Row within the column
        
        float fieldX = x + 10 + col * (columnWidth + 10);
        float fieldY = fieldStartY + row * (fieldHeight + 5);
        
        // Field background with 2px padding
        sf::RectangleShape fieldBg;
        fieldBg.setSize(sf::Vector2f(columnWidth - 4, fieldHeight - 4)); // 2px padding on each side
        fieldBg.setPosition(fieldX + 2, fieldY + 2); // 2px offset for padding
        
        // Highlight selected field (only if not in rule grid area)
        if (static_cast<int>(i) == m_selectedFormField && m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM && !m_inRuleGridArea) {
            fieldBg.setFillColor(sf::Color(80, 140, 200, 100));
            fieldBg.setOutlineThickness(2);
            fieldBg.setOutlineColor(sf::Color::Yellow);
        } else {
            fieldBg.setFillColor(sf::Color(60, 60, 65));
            fieldBg.setOutlineThickness(1);
            fieldBg.setOutlineColor(sf::Color(75, 75, 80));
        }
        
        m_game->window().draw(fieldBg);
        
        // Field label with padding
        sf::Text fieldLabel;
        fieldLabel.setFont(m_game->getAssets().getFont("ShareTech"));
        fieldLabel.setString(fieldNames[i]);
        fieldLabel.setCharacterSize(11);
        fieldLabel.setFillColor(sf::Color::White);
        fieldLabel.setPosition(fieldX + 7, fieldY + 5); // Account for padding
        m_game->window().draw(fieldLabel);
        
        // Field value with adjustment indicators and padding
        sf::Text fieldValue;
        fieldValue.setFont(m_game->getAssets().getFont("ShareTech"));
        std::string valueText = std::to_string(*fieldValues[i]) + "px";
        
        // Add adjustment indicators when field is selected (only if not in rule grid area)
        if (static_cast<int>(i) == m_selectedFormField && m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM && !m_inRuleGridArea) {
            valueText = "< " + valueText + " >";
            fieldValue.setFillColor(sf::Color::Yellow);
        } else {
            fieldValue.setFillColor(sf::Color(200, 200, 255));
        }
        
        fieldValue.setString(valueText);
        fieldValue.setCharacterSize(11);
        fieldValue.setPosition(fieldX + 7, fieldY + 17); // Account for padding
        m_game->window().draw(fieldValue);
    }
    
    // Separator line - closer to parameters
    float separatorY = y + upperHeight; // Reduced gap
    sf::RectangleShape separator;
    separator.setSize(sf::Vector2f(width - 20, 2));
    separator.setPosition(x + 10, separatorY);
    separator.setFillColor(sf::Color(100, 100, 100));
    m_game->window().draw(separator);
    
    // Selected tile info and rule grid (lower portion - 60% of height)
    float lowerY = separatorY + 10;
    float lowerHeight = height - (lowerY - y); // Full available height
    
    if (m_currentAssetTexture.getSize().x > 0) {
        // Selected tile info
        sf::Text tileInfoTitle;
        tileInfoTitle.setFont(m_game->getAssets().getFont("ShareTech"));
        tileInfoTitle.setString("Selected Tile Rules");
        tileInfoTitle.setCharacterSize(14);
        tileInfoTitle.setFillColor(sf::Color(200, 140, 80));
        tileInfoTitle.setPosition(x + 10, lowerY);
        m_game->window().draw(tileInfoTitle);
        
        // Tile position and info
        sf::Text tileInfo;
        tileInfo.setFont(m_game->getAssets().getFont("ShareTech"));
        tileInfo.setString("Tile: (" + std::to_string(m_selectedTileInPreview.x) + ", " + 
                          std::to_string(m_selectedTileInPreview.y) + ") | " +
                          std::to_string(m_tileWidth) + "x" + std::to_string(m_tileHeight) + "px");
        tileInfo.setCharacterSize(12);
        tileInfo.setFillColor(sf::Color::White);
        tileInfo.setPosition(x + 10, lowerY + 20);
        m_game->window().draw(tileInfo);
        
        // 3x3 Rule grid
        drawTileRuleGridInForm(x, lowerY + 45, width, lowerHeight - 45);
    } else {
        sf::Text noTileInfo;
        noTileInfo.setFont(m_game->getAssets().getFont("ShareTech"));
        noTileInfo.setString("No asset loaded\nSelect an asset to configure tile rules");
        noTileInfo.setCharacterSize(12);
        noTileInfo.setFillColor(sf::Color(120, 120, 120));
        noTileInfo.setPosition(x + 10, lowerY + 20);
        m_game->window().draw(noTileInfo);
    }
}

void Scene_AutoTileConfig::drawAssetPreview(float x, float y, float width, float height) {
    // Preview Panel Background
    sf::RectangleShape previewPanel;
    previewPanel.setSize(sf::Vector2f(width, height));
    previewPanel.setPosition(x, y);
    previewPanel.setFillColor(sf::Color(40, 40, 45));
    previewPanel.setOutlineThickness(m_assetSlicingFocus == AssetSlicingFocus::PREVIEW_PANEL ? 3 : 1);
    previewPanel.setOutlineColor(m_assetSlicingFocus == AssetSlicingFocus::PREVIEW_PANEL ? sf::Color::Yellow : sf::Color(70, 70, 75));
    m_game->window().draw(previewPanel);
    
    // Preview Title
    sf::Text previewTitle;
    previewTitle.setFont(m_game->getAssets().getFont("ShareTech"));
    previewTitle.setString("Asset Preview");
    previewTitle.setCharacterSize(16);
    previewTitle.setFillColor(sf::Color::White);
    previewTitle.setPosition(x + 10, y + 10);
    m_game->window().draw(previewTitle);
    
    // Draw asset if loaded
    if (m_currentAssetTexture.getSize().x > 0) {
        // Update tile grid dimensions
        updateTileGridDimensions();
        
        // Use the full panel area for preview but account for title
        float previewAreaX = x + 5; // Small margin from panel edge
        float previewAreaY = y + 35; // Account for title
        float previewAreaWidth = width - 10; // Small margins on both sides
        float previewAreaHeight = height - 50; // Account for title and bottom margin
        
        sf::Vector2u textureSize = m_currentAssetTexture.getSize();
     
        // Draw a visible border around the preview area (inside the panel)
        sf::RectangleShape previewBorder;
        previewBorder.setSize(sf::Vector2f(previewAreaWidth - 2, previewAreaHeight - 2)); // Account for border thickness
        previewBorder.setPosition(previewAreaX + 1, previewAreaY + 1); // Center the border
        previewBorder.setFillColor(sf::Color::Transparent);
        previewBorder.setOutlineThickness(1);
        previewBorder.setOutlineColor(sf::Color::Red); // Red border to show exact preview area
        m_game->window().draw(previewBorder);
        
        // Pixel-perfect display - no scaling, show actual pixels
        float scale = 1.0f; // Always 1:1 pixel ratio
        
        // Apply scroll offsets for navigation
        float assetX = previewAreaX - m_previewScrollX;
        float assetY = previewAreaY - m_previewScrollY;
        
        // Create a render texture for clipping
        sf::RenderTexture previewTexture;
        if (previewTexture.create(previewAreaWidth, previewAreaHeight)) {
            previewTexture.clear(sf::Color::Transparent);
            
            // Draw asset sprite to render texture (with scroll offset)
            m_currentAssetSprite.setScale(scale, scale);
            m_currentAssetSprite.setPosition(-m_previewScrollX, -m_previewScrollY);
            previewTexture.draw(m_currentAssetSprite);
            
            // Draw slicing grid overlay to render texture
            drawSlicingGridToTexture(previewTexture, -m_previewScrollX, -m_previewScrollY, scale);
            
            // Draw selected tile highlight to render texture
            if (m_assetSlicingFocus == AssetSlicingFocus::PREVIEW_PANEL && isValidTilePosition(m_selectedTileInPreview)) {
                float tilePixelX = -m_previewScrollX + m_offsetX + m_marginX + m_selectedTileInPreview.x * (m_tileWidth + m_spacingX);
                float tilePixelY = -m_previewScrollY + m_offsetY + m_marginY + m_selectedTileInPreview.y * (m_tileHeight + m_spacingY);
                
                // Adjust position to be inside grid lines
                float adjustedX = tilePixelX + m_gridLineThickness;
                float adjustedY = tilePixelY + m_gridLineThickness;
                float adjustedWidth = m_tileWidth - m_gridLineThickness;
                float adjustedHeight = m_tileHeight - m_gridLineThickness;
                
                sf::RectangleShape tileHighlight;
                tileHighlight.setSize(sf::Vector2f(adjustedWidth, adjustedHeight));
                tileHighlight.setPosition(adjustedX, adjustedY);
                tileHighlight.setFillColor(sf::Color(255, 255, 0, 100)); // Semi-transparent yellow
                tileHighlight.setOutlineThickness(1);
                tileHighlight.setOutlineColor(sf::Color::Yellow);
                previewTexture.draw(tileHighlight);
            }
            
            previewTexture.display();
            
            // Draw the render texture to the main window

            sf::Sprite previewSprite(previewTexture.getTexture());
            previewSprite.setPosition(previewAreaX, previewAreaY);
            m_game->window().draw(previewSprite);
            
            // Asset info (drawn at bottom of panel, outside clipped area)
            sf::Text assetInfo;
            assetInfo.setFont(m_game->getAssets().getFont("ShareTech"));
            assetInfo.setString("Size: " + std::to_string(textureSize.x) + "x" + std::to_string(textureSize.y) + 
                    " | Tiles: " + std::to_string(m_tilesX) + "x" + std::to_string(m_tilesY) +
                    " | Preview: " + std::to_string((int)previewAreaWidth) + "x" + std::to_string((int)previewAreaHeight) +
                    " | Scroll: " + std::to_string((int)m_previewScrollX) + "," + std::to_string((int)m_previewScrollY));
            assetInfo.setCharacterSize(16);
            assetInfo.setFillColor(sf::Color(180, 180, 180));
            assetInfo.setPosition(x + 120, y + 10);
            m_game->window().draw(assetInfo);
        } else {
            // Fallback: draw without clipping if render texture creation fails
            m_currentAssetSprite.setScale(scale, scale);
            m_currentAssetSprite.setPosition(assetX, assetY);
            m_game->window().draw(m_currentAssetSprite);
        }

    } else {
        // No asset loaded message
        sf::Text noAsset;
        noAsset.setFont(m_game->getAssets().getFont("ShareTech"));
        noAsset.setString("No asset loaded\nSelect an asset from the tree");
        noAsset.setCharacterSize(14);
        noAsset.setFillColor(sf::Color(120, 120, 120));
        noAsset.setPosition(x + width/2 - 80, y + height/2 - 20);
        m_game->window().draw(noAsset);
    }
}

void Scene_AutoTileConfig::drawSlicingGrid(float x, float y, float scale) {
    if (m_currentAssetTexture.getSize().x == 0) return;
    float scaledTileWidth = m_tileWidth * scale;
    float scaledTileHeight = m_tileHeight * scale;
    float scaledMarginX = m_marginX * scale;
    float scaledMarginY = m_marginY * scale;
    float scaledSpacingX = m_spacingX * scale;
    float scaledSpacingY = m_spacingY * scale;
    float scaledOffsetX = m_offsetX * scale;
    float scaledOffsetY = m_offsetY * scale;
    
    // Use the already calculated tile dimensions
    int tilesX = m_tilesX;
    int tilesY = m_tilesY;
    
    // Only draw grid lines if they're visible (scale is reasonable)
    if (scale < 0.02f) return; // Skip grid for very small scales
    
    // Calculate the actual drawable area starting from offset
    float gridStartX = x + scaledOffsetX + scaledMarginX;
    float gridStartY = y + scaledOffsetY + scaledMarginY;
    
    // Vertical lines
    for (int i = 0; i <= tilesX; i++) {
        float lineX = gridStartX + i * (scaledTileWidth + scaledSpacingX);
        sf::RectangleShape vLine;
        vLine.setSize(sf::Vector2f(m_gridLineThickness, tilesY * (scaledTileHeight + scaledSpacingY) - scaledSpacingY));
        vLine.setPosition(lineX, gridStartY);
        vLine.setFillColor(sf::Color(255, 255, 0, 180)); // Semi-transparent yellow
        m_game->window().draw(vLine);
    }
    
    // Horizontal lines
    for (int i = 0; i <= tilesY; i++) {
        float lineY = gridStartY + i * (scaledTileHeight + scaledSpacingY);
        sf::RectangleShape hLine;
        hLine.setSize(sf::Vector2f(tilesX * (scaledTileWidth + scaledSpacingX) - scaledSpacingX, m_gridLineThickness));
        hLine.setPosition(gridStartX, lineY);
        hLine.setFillColor(sf::Color(255, 255, 0, 180)); // Semi-transparent yellow
        m_game->window().draw(hLine);
    }
    
    // Draw tile numbers for better navigation (only if scale is large enough and grid is reasonable)
    if (scale > 0.2f && tilesX <= 20 && tilesY <= 20) {
        for (int tileY = 0; tileY < tilesY; tileY++) {
            for (int tileX = 0; tileX < tilesX; tileX++) {
                float tilePixelX = gridStartX + tileX * (scaledTileWidth + scaledSpacingX);
                float tilePixelY = gridStartY + tileY * (scaledTileHeight + scaledSpacingY);
                
                sf::Text tileNumber;
                tileNumber.setFont(m_game->getAssets().getFont("ShareTech"));
                tileNumber.setString(std::to_string(tileX) + "," + std::to_string(tileY));
                tileNumber.setCharacterSize(std::max(8, (int)(12 * scale)));
                tileNumber.setFillColor(sf::Color(255, 255, 255, 200));
                tileNumber.setPosition(tilePixelX + 2, tilePixelY + 2);
                m_game->window().draw(tileNumber);
            }
        }
    }
}

void Scene_AutoTileConfig::drawSlicingGridToTexture(sf::RenderTexture& texture, float x, float y, float scale) {
    if (m_currentAssetTexture.getSize().x == 0) return;
    float scaledTileWidth = m_tileWidth * scale;
    float scaledTileHeight = m_tileHeight * scale;
    float scaledMarginX = m_marginX * scale;
    float scaledMarginY = m_marginY * scale;
    float scaledSpacingX = m_spacingX * scale;
    float scaledSpacingY = m_spacingY * scale;
    float scaledOffsetX = m_offsetX * scale;
    float scaledOffsetY = m_offsetY * scale;
    
    // Use the already calculated tile dimensions
    int tilesX = m_tilesX;
    int tilesY = m_tilesY;
    
    // Only draw grid lines if they're visible (scale is reasonable)
    if (scale < 0.02f) return; // Skip grid for very small scales
    
    // Calculate the actual drawable area starting from offset
    float gridStartX = x + scaledOffsetX + scaledMarginX;
    float gridStartY = y + scaledOffsetY + scaledMarginY;
    
    // Vertical lines
    for (int i = 0; i <= tilesX; i++) {
        float lineX = gridStartX + i * (scaledTileWidth + scaledSpacingX);
        sf::RectangleShape vLine;
        vLine.setSize(sf::Vector2f(m_gridLineThickness, tilesY * (scaledTileHeight + scaledSpacingY) - scaledSpacingY));
        vLine.setPosition(lineX, gridStartY);
        vLine.setFillColor(sf::Color(255, 255, 0, 180)); // Semi-transparent yellow
        texture.draw(vLine);
    }
    
    // Horizontal lines
    for (int i = 0; i <= tilesY; i++) {
        float lineY = gridStartY + i * (scaledTileHeight + scaledSpacingY);
        sf::RectangleShape hLine;
        hLine.setSize(sf::Vector2f(tilesX * (scaledTileWidth + scaledSpacingX) - scaledSpacingX, m_gridLineThickness));
        hLine.setPosition(gridStartX, lineY);
        hLine.setFillColor(sf::Color(255, 255, 0, 180)); // Semi-transparent yellow
        texture.draw(hLine);
    }
    
    // Draw tile numbers for better navigation (only if scale is large enough and grid is reasonable)
    if (scale > 0.2f && tilesX <= 20 && tilesY <= 20) {
        for (int tileY = 0; tileY < tilesY; tileY++) {
            for (int tileX = 0; tileX < tilesX; tileX++) {
                float tilePixelX = gridStartX + tileX * (scaledTileWidth + scaledSpacingX);
                float tilePixelY = gridStartY + tileY * (scaledTileHeight + scaledSpacingY);
                
                sf::Text tileNumber;
                tileNumber.setFont(m_game->getAssets().getFont("ShareTech"));
                tileNumber.setString(std::to_string(tileX) + "," + std::to_string(tileY));
                tileNumber.setCharacterSize(std::max(8, (int)(12 * scale)));
                tileNumber.setFillColor(sf::Color(255, 255, 255, 200));
                tileNumber.setPosition(tilePixelX + 2, tilePixelY + 2);
                texture.draw(tileNumber);
            }
        }
    }
}

sf::IntRect Scene_AutoTileConfig::getTileContentRect(int tileX, int tileY) const {
    if (!isValidTilePosition(sf::Vector2i(tileX, tileY))) {
        return sf::IntRect(0, 0, 0, 0); // Invalid rectangle
    }
    
    // Calculate the pixel position of the tile with offset
    int tilePixelX = m_offsetX + m_marginX + tileX * (m_tileWidth + m_spacingX);
    int tilePixelY = m_offsetY + m_marginY + tileY * (m_tileHeight + m_spacingY);
    
    // Adjust for grid line thickness - return only content area inside grid lines
    int contentStartX = tilePixelX + m_gridLineThickness;
    int contentStartY = tilePixelY + m_gridLineThickness;
    int contentWidth = m_tileWidth - (2 * m_gridLineThickness);
    int contentHeight = m_tileHeight - (2 * m_gridLineThickness);
    
    // Ensure content dimensions are valid
    contentWidth = std::max(1, contentWidth);
    contentHeight = std::max(1, contentHeight);
    
    return sf::IntRect(contentStartX, contentStartY, contentWidth, contentHeight);
}

// Accordion management methods
void Scene_AutoTileConfig::toggleAccordionSection(AccordionSection section) {
    int sectionIndex = static_cast<int>(section);
    
    if (m_accordionExpanded[sectionIndex]) {
        collapseAccordionSection(section);
    } else {
        // Collapse all other sections first (accordion behavior)
        for (int i = 0; i < 3; i++) {
            if (i != sectionIndex && m_accordionExpanded[i]) {
                collapseAccordionSection(static_cast<AccordionSection>(i));
            }
        }
        expandAccordionSection(section);
    }
}

void Scene_AutoTileConfig::expandAccordionSection(AccordionSection section) {
    int sectionIndex = static_cast<int>(section);
    m_accordionExpanded[sectionIndex] = true;
    m_currentSection = section;
    
    // Update overlay commands for new section
    updateOverlayCommands();
    
    // Switch content panel to match expanded section
    updateContentPanel();
    
    // Rebuild accordion menu with new expanded state
    setupAccordionMenu();
    
}

void Scene_AutoTileConfig::collapseAccordionSection(AccordionSection section) {
    int sectionIndex = static_cast<int>(section);
    m_accordionExpanded[sectionIndex] = false;
    
    // Clear items for collapsed section
    if (static_cast<size_t>(sectionIndex) < m_accordionItems.size()) {
        m_accordionItems[sectionIndex].clear();
        m_accordionItemTexts[sectionIndex].clear();
    }
}

void Scene_AutoTileConfig::switchPanel() {
    switch (m_currentPanel) {
        case Panel::MENU:
            m_currentPanel = Panel::CONTENT;
            break;
        case Panel::CONTENT:
            m_currentPanel = Panel::MENU;
            break;
        default:
            m_currentPanel = Panel::MENU;
            break;
    }
}

std::string Scene_AutoTileConfig::getSectionName(AccordionSection section) {
    switch (section) {
        case AccordionSection::ASSET_SLICING: return "Asset Slicing";
        default: return "Unknown";
    }
}

// Minimal implementations for required methods
void Scene_AutoTileConfig::handleNavigation(const std::string& direction) {
    // WASD now navigates within the current panel based on current section
    switch (m_currentSection) {
        case AccordionSection::ASSET_SLICING:
            handleAssetSlicingNavigation(direction);
            break;
        default:
            // Only asset slicing is supported
            handleAssetSlicingNavigation(direction);
            break;
    }
}

void Scene_AutoTileConfig::switchPanelInCurrentTab() {
    switch (m_currentSection) {
        case AccordionSection::ASSET_SLICING:
            // Cycle through: ASSET_TREE -> SLICING_FORM -> PREVIEW_PANEL -> ASSET_TREE
            switch (m_assetSlicingFocus) {
                case AssetSlicingFocus::ASSET_TREE:
                    m_assetSlicingFocus = AssetSlicingFocus::SLICING_FORM;
                    m_inRuleGridArea = false; // Start in parameter area
                    break;
                case AssetSlicingFocus::SLICING_FORM:
                    m_assetSlicingFocus = AssetSlicingFocus::PREVIEW_PANEL;
                    m_inRuleGridArea = false;
                    break;
                case AssetSlicingFocus::PREVIEW_PANEL:
                    m_assetSlicingFocus = AssetSlicingFocus::ASSET_TREE;
                    m_inRuleGridArea = false;
                    break;
            }
            // Update overlay commands for new panel context
            updateOverlayCommands();
            break;
            
//         case AccordionSection::TILE_TYPES:
            // For tile types, Tab could switch between type list and properties
            break;
            
//         case AccordionSection::FILE_OPERATIONS:
            // For file ops, Tab could switch between operations and status
            break;
    }
}

void Scene_AutoTileConfig::switchPanelInCurrentTabReverse() {
    switch (m_currentSection) {
        case AccordionSection::ASSET_SLICING:
            // Cycle through in reverse: ASSET_TREE <- SLICING_FORM <- PREVIEW_PANEL <- ASSET_TREE
            switch (m_assetSlicingFocus) {
                case AssetSlicingFocus::ASSET_TREE:
                    m_assetSlicingFocus = AssetSlicingFocus::PREVIEW_PANEL;
                    m_inRuleGridArea = false;
                    break;
                case AssetSlicingFocus::SLICING_FORM:
                    m_assetSlicingFocus = AssetSlicingFocus::ASSET_TREE;
                    m_inRuleGridArea = false;
                    break;
                case AssetSlicingFocus::PREVIEW_PANEL:
                    m_assetSlicingFocus = AssetSlicingFocus::SLICING_FORM;
                    m_inRuleGridArea = false; // Start in parameter area
                    break;
            }
            // Update overlay commands for new panel context
            updateOverlayCommands();
            break;
            
//         case AccordionSection::TILE_TYPES:
            // For tile types, Shift+Tab could switch in reverse direction
            break;
            
//         case AccordionSection::FILE_OPERATIONS:
            // For file ops, Shift+Tab could switch in reverse direction
            break;
    }
}

void Scene_AutoTileConfig::handleAssetSlicingNavigation(const std::string& direction) {
    if (m_assetSlicingFocus == AssetSlicingFocus::PREVIEW_PANEL) {
        // Handle preview panel navigation (scrolling and tile selection)
        handlePreviewPanelNavigation(direction);
        return;
    }
    
    if (direction == "UP") {
        if (m_assetSlicingFocus == AssetSlicingFocus::ASSET_TREE) {
            // Navigate up in asset tree
            if (m_selectedAssetIndex > 0) {
                m_selectedAssetIndex--;
                loadSelectedAsset();
                
                // Auto-scroll to keep selected item visible
                float itemHeight = 35; // 30 + 5 spacing
                float selectedItemY = m_selectedAssetIndex * itemHeight;
                if (selectedItemY < m_assetTreeScrollY) {
                    m_assetTreeScrollY = selectedItemY;
                }
            } else {
                // Scroll up if at top of list
                m_assetTreeScrollY = std::max(0.0f, m_assetTreeScrollY - 35);
            }
        }
        else if (m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM) {
            if (m_inRuleGridArea) {
                // Navigate up in rule grid
                if (m_ruleGridSelection >= 3) {
                    m_ruleGridSelection -= 3; // Move up one row
                    // Safety check
                    if (m_ruleGridSelection < 0) m_ruleGridSelection = 0;
                } else {
                    // Jump back to parameters (last parameter)
                    m_inRuleGridArea = false;
                    m_selectedFormField = 8; // Last parameter (Grid Line)
                    
                    // Update overlay commands for parameter context
                    updateOverlayCommands();
                }
            } else {
                // Navigate up in form fields
                if (m_selectedFormField > 0) {
                    m_selectedFormField--;
                }
            }
        }
    }
    else if (direction == "DOWN") {
        if (m_assetSlicingFocus == AssetSlicingFocus::ASSET_TREE) {
            // Navigate down in asset tree
            if (m_selectedAssetIndex < static_cast<int>(m_availableAssets.size()) - 1) {
                m_selectedAssetIndex++;
                loadSelectedAsset();
                
                // Auto-scroll to keep selected item visible
                float itemHeight = 35; // 30 + 5 spacing
                float selectedItemY = m_selectedAssetIndex * itemHeight;
                float availableHeight = 300; // Approximate, should be calculated properly
                if (selectedItemY + itemHeight > m_assetTreeScrollY + availableHeight) {
                    m_assetTreeScrollY = selectedItemY + itemHeight - availableHeight;
                }
            } else {
                // Scroll down if at bottom of list
                float totalContentHeight = m_availableAssets.size() * 35;
                float availableHeight = 300; // Approximate
                float maxScroll = std::max(0.0f, totalContentHeight - availableHeight);
                m_assetTreeScrollY = std::min(maxScroll, m_assetTreeScrollY + 35);
            }
        }
        else if (m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM) {
            if (m_inRuleGridArea) {
                // Navigate down in rule grid
                if (m_ruleGridSelection <= 5) {
                    m_ruleGridSelection += 3; // Move down one row
                    // Safety check
                    if (m_ruleGridSelection >= 9) m_ruleGridSelection = 8;
                }
            } else {
                // Navigate down in form fields
                if (m_selectedFormField < 8) { // 9 form fields (0-8)
                    m_selectedFormField++;
                } else {
                    // Jump down to rule grid
                    m_inRuleGridArea = true;
                    m_ruleGridSelection = 1; // Start at top-center of rule grid
                    m_selectedFormField = -1; // Clear form field selection when in rule area
                    
                    // Safety check: ensure rule grid selection is valid
                    if (m_ruleGridSelection < 0 || m_ruleGridSelection >= 9) {
                        m_ruleGridSelection = 1;
                    }
                    
                    // Update overlay commands for rule grid context
                    updateOverlayCommands();
                }
            }
        }
    }
    else if (direction == "LEFT") {
        if (m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM) {
            if (m_inRuleGridArea) {
                // Navigate left in rule grid
                if (m_ruleGridSelection % 3 != 0) {
                    m_ruleGridSelection--;
                    // Safety check
                    if (m_ruleGridSelection < 0) m_ruleGridSelection = 0;
                }
            } else {
                // Decrease parameter value
                adjustSlicingParameter(-1);
            }
        }
    }
    else if (direction == "RIGHT") {
        if (m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM) {
            if (m_inRuleGridArea) {
                // Navigate right in rule grid
                if (m_ruleGridSelection % 3 != 2) {
                    m_ruleGridSelection++;
                    // Safety check
                    if (m_ruleGridSelection >= 9) m_ruleGridSelection = 8;
                }
            } else {
                // Increase parameter value
                adjustSlicingParameter(1);
            }
        }
    }
}

void Scene_AutoTileConfig::adjustSlicingParameter(int delta) {
    std::vector<int*> fieldValues = {&m_tileWidth, &m_tileHeight, &m_marginX, &m_marginY, &m_spacingX, &m_spacingY, &m_offsetX, &m_offsetY, &m_gridLineThickness};
    
    if (m_selectedFormField >= 0 && m_selectedFormField < static_cast<int>(fieldValues.size())) {
        int* currentValue = fieldValues[m_selectedFormField];
        int oldValue = *currentValue;
        
        // Apply delta with reasonable limits
        *currentValue += delta;
        
        // Set reasonable limits for each parameter
        switch (m_selectedFormField) {
            case 0: // Tile Width
            case 1: // Tile Height
                *currentValue = std::max(8, std::min(512, *currentValue));
                break;
            case 2: // Margin X
            case 3: // Margin Y
            case 4: // Spacing X
            case 5: // Spacing Y
            case 6: // Offset X
            case 7: // Offset Y
                *currentValue = std::max(0, std::min(512, *currentValue)); // Allow larger offsets
                break;
            case 8: // Grid Line Thickness
                *currentValue = std::max(1, std::min(10, *currentValue)); // 1-10 pixel range
                break;
        }
        
        if (*currentValue != oldValue) {
            updateSlicingParameters();
        }
    }
}

void Scene_AutoTileConfig::handleSelection() {
    switch (m_currentSection) {
        case AccordionSection::ASSET_SLICING:
            if (m_assetSlicingFocus == AssetSlicingFocus::ASSET_TREE) {
                // Select current asset
                loadSelectedAsset();
            }
            else if (m_assetSlicingFocus == AssetSlicingFocus::SLICING_FORM) {
                if (m_inRuleGridArea) {
                    // Cycle rule condition for selected cell
                    if (m_ruleGridSelection != 4) { // Don't change center cell
                        cycleRuleCondition(m_ruleGridSelection);
                        saveRulesForSelectedTile(); // Auto-save rule changes
                    }
                } else {
                    // Apply current slicing parameters
                    updateSlicingParameters();
                }
            }
            break;
    }
}



void Scene_AutoTileConfig::selectTileInTileset(sf::Vector2i position) {
    m_selectedTilePos = position;
}

void Scene_AutoTileConfig::cycleRuleCondition(int cellIndex) {
    if (cellIndex < 0 || cellIndex >= static_cast<int>(m_currentRule.size()) || cellIndex == 4) {
        return;
    }
    
    // Cycle through: IGNORE -> SAME -> DIFFERENT -> EMPTY -> IGNORE
    switch (m_currentRule[cellIndex]) {
        case RuleCondition::IGNORE:
            m_currentRule[cellIndex] = RuleCondition::SAME;
            break;
        case RuleCondition::SAME:
            m_currentRule[cellIndex] = RuleCondition::DIFFERENT;
            break;
        case RuleCondition::DIFFERENT:
            m_currentRule[cellIndex] = RuleCondition::EMPTY;
            break;
        case RuleCondition::EMPTY:
            m_currentRule[cellIndex] = RuleCondition::IGNORE;
            break;
    }
}

void Scene_AutoTileConfig::switchTileType(int direction) {
    std::vector<std::string> types = {"Wall", "Ground", "Water"};
    auto it = std::find(types.begin(), types.end(), m_currentTileType);
    if (it != types.end()) {
        int index = std::distance(types.begin(), it);
        index = (index + direction) % types.size();
        if (index < 0) index = types.size() - 1;
        m_currentTileType = types[index];
        m_currentTileTypeText.setString("Current: " + m_currentTileType);
    }
}

int Scene_AutoTileConfig::getRuleGridCell(sf::Vector2i mousePos) {
    (void)mousePos; // Suppress unused parameter warning
    return 0;
}

sf::Vector2i Scene_AutoTileConfig::getTilePositionFromPixel(sf::Vector2f pixelPos) {
    // This function converts pixel coordinates to tile coordinates in the preview panel
    // For now, return invalid position - this would need preview panel bounds to work properly
    (void)pixelPos; // Suppress unused parameter warning
    return sf::Vector2i(-1, -1);
}

sf::Vector2i Scene_AutoTileConfig::getTilesetGridPosition(sf::Vector2i mousePos) {
    (void)mousePos; // Suppress unused parameter warning
    return sf::Vector2i(-1, -1);
}

int Scene_AutoTileConfig::countActiveConditions() const {
    return 0;
}

sf::Color Scene_AutoTileConfig::getRuleConditionColor(RuleCondition condition) const {
    switch (condition) {
        case RuleCondition::SAME: return sf::Color::Green;
        case RuleCondition::DIFFERENT: return sf::Color::Red;
        default: return sf::Color(128, 128, 128); // Gray color
    }
}

std::string Scene_AutoTileConfig::ruleConditionToString(RuleCondition condition) const {
    switch (condition) {
        case RuleCondition::SAME: return "SAME";
        case RuleCondition::DIFFERENT: return "DIFF";
        default: return "IGNORE";
    }
}

std::string Scene_AutoTileConfig::getPositionName(int row, int col) const {
    return "Position(" + std::to_string(row) + "," + std::to_string(col) + ")";
}

void Scene_AutoTileConfig::initializeAvailableAssets() {
    m_availableAssets.clear();
    
    std::string assetsPath = "assets/imgs/autotiles/";
    
    // Create directory if it doesn't exist
    if (!std::filesystem::exists(assetsPath)) {
        std::filesystem::create_directories(assetsPath);
        return;
    }
    
    // Scan directory for image files
    try {
        for (const auto& entry : std::filesystem::directory_iterator(assetsPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                
                // Check for supported image formats
                if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || 
                    extension == ".bmp" || extension == ".tga") {
                    m_availableAssets.push_back(filename);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cout << "Error scanning assets directory: " << e.what() << std::endl;
    }
    
    // Sort assets alphabetically for consistent ordering
    std::sort(m_availableAssets.begin(), m_availableAssets.end());
    
    // Load the first asset by default
    if (!m_availableAssets.empty()) {
        loadSelectedAsset();
    }
}

void Scene_AutoTileConfig::loadSelectedAsset() {
    if (m_selectedAssetIndex >= 0 && m_selectedAssetIndex < static_cast<int>(m_availableAssets.size())) {
        std::string assetName = m_availableAssets[m_selectedAssetIndex];
        std::string assetPath = "assets/imgs/autotiles/" + assetName;
  
        if (m_currentAssetTexture.loadFromFile(assetPath)) {
            m_currentAssetSprite.setTexture(m_currentAssetTexture);
            
            // Load the asset configuration
            loadAssetConfig(assetName);
            
            // Initialize tile selection
            m_selectedTileInPreview = sf::Vector2i(0, 0);
            updateTileGridDimensions();
            extractSelectedTile();
            loadRulesForSelectedTile();
            
        } else {
            std::cout << "Failed to load texture: " << assetPath << std::endl;
        }
    }
}

std::string Scene_AutoTileConfig::getFormFieldName(int fieldIndex) {
    switch (fieldIndex) {
        case 0: return "Tile Width";
        case 1: return "Tile Height";
        case 2: return "Margin X";
        case 3: return "Margin Y";
        case 4: return "Spacing X";
        case 5: return "Spacing Y";
        case 6: return "Offset X";
        case 7: return "Offset Y";
        default: return "Unknown";
    }
}

std::string Scene_AutoTileConfig::getFileOperationName(int operationIndex) {
    switch (operationIndex) {
        case 0: return "Save Current Tile";
        case 1: return "Save All Configs";
        case 2: return "Load Configuration";
        case 3: return "Export All";
        case 4: return "Import Config";
        default: return "Unknown";
    }
}

void Scene_AutoTileConfig::updateSlicingParameters() {
    // This will be called when form values change
    // Update tile grid dimensions based on new parameters
    updateTileGridDimensions();
    
    // Ensure selected tile is within new bounds
    if (m_selectedTileInPreview.x >= m_tilesX) {
        m_selectedTileInPreview.x = std::max(0, m_tilesX - 1);
    }
    if (m_selectedTileInPreview.y >= m_tilesY) {
        m_selectedTileInPreview.y = std::max(0, m_tilesY - 1);
    }
    
    // Extract the currently selected tile with new parameters
    extractSelectedTile();
    
    // Reload rules for the selected tile (in case tile position changed)
    loadRulesForSelectedTile();
  }

// Asset Configuration Methods
void Scene_AutoTileConfig::createDefaultAssetConfig(const std::string& assetName) {
    using json = nlohmann::json;
    
    json config;
    config["asset_name"] = assetName;
    config["tile_width"] = 64;
    config["tile_height"] = 64;
    config["margin_x"] = 0;
    config["margin_y"] = 0;
    config["spacing_x"] = 0;
    config["spacing_y"] = 0;
    config["offset_x"] = 0;
    config["offset_y"] = 0;
    config["created_date"] = std::time(nullptr);
    config["last_modified"] = std::time(nullptr);
    
    // Generic defaults - no hardcoded asset-specific values
    // Users can adjust parameters as needed for their specific assets
    
    std::string configPath = getAssetConfigPath(assetName);
    
    // Create metadata directory if it doesn't exist
    std::string metadataDir = "metadata/autotiling";
    if (!std::filesystem::exists(metadataDir)) {
        std::filesystem::create_directories(metadataDir);
    }
    
    std::ofstream file(configPath);
    if (file.is_open()) {
        file << config.dump(4); // Pretty print with 4 spaces
        file.close();
    } else {
        std::cout << "Failed to create config file: " << configPath << std::endl;
    }
}

void Scene_AutoTileConfig::loadAssetConfig(const std::string& assetName) {
    using json = nlohmann::json;
    
    std::string configPath = getAssetConfigPath(assetName);
    
    // Check if config file exists
    if (!std::filesystem::exists(configPath)) {
        createDefaultAssetConfig(assetName);
    }
    
    // Load the config
    std::ifstream file(configPath);
    if (file.is_open()) {
        try {
            json config;
            file >> config;
            file.close();
            
            // Apply the loaded configuration
            m_tileWidth = config.value("tile_width", 64);
            m_tileHeight = config.value("tile_height", 64);
            m_marginX = config.value("margin_x", 0);
            m_marginY = config.value("margin_y", 0);
            m_spacingX = config.value("spacing_x", 0);
            m_spacingY = config.value("spacing_y", 0);
            m_offsetX = config.value("offset_x", 0);
            m_offsetY = config.value("offset_y", 0);
            m_gridLineThickness = config.value("grid_line_thickness", 1);
            
            // Load tile rules if they exist
            m_tileRules.clear();
            if (config.contains("tile_rules")) {
                json tileRulesJson = config["tile_rules"];
                for (auto& [tileKey, ruleArray] : tileRulesJson.items()) {
                    // Parse tile position from key "x,y"
                    size_t commaPos = tileKey.find(',');
                    if (commaPos != std::string::npos) {
                        int x = std::stoi(tileKey.substr(0, commaPos));
                        int y = std::stoi(tileKey.substr(commaPos + 1));
                        sf::Vector2i tilePos(x, y);
                        
                        // Parse rule conditions
                        std::vector<RuleCondition> conditions;
                        for (const auto& conditionStr : ruleArray) {
                            std::string condStr = conditionStr.get<std::string>();
                            if (condStr == "SAME") {
                                conditions.push_back(RuleCondition::SAME);
                            } else if (condStr == "DIFFERENT") {
                                conditions.push_back(RuleCondition::DIFFERENT);
                            } else if (condStr == "EMPTY") {
                                conditions.push_back(RuleCondition::EMPTY);
                            } else {
                                conditions.push_back(RuleCondition::IGNORE);
                            }
                        }
                        
                        if (conditions.size() == 9) { // Ensure we have all 9 conditions
                            m_tileRules[tilePos] = conditions;
                        }
                    }
                }
            }
            
            // Update the preview and grid after loading parameters
            updateSlicingParameters();
            
        } catch (const json::exception& e) {
            createDefaultAssetConfig(assetName);
            loadAssetConfig(assetName); // Retry with default config
        }
    } else {
    }
}

void Scene_AutoTileConfig::saveAssetConfig(const std::string& assetName) {
    using json = nlohmann::json;
    
    json config;
    config["asset_name"] = assetName;
    config["tile_width"] = m_tileWidth;
    config["tile_height"] = m_tileHeight;
    config["margin_x"] = m_marginX;
    config["margin_y"] = m_marginY;
    config["spacing_x"] = m_spacingX;
    config["spacing_y"] = m_spacingY;
    config["offset_x"] = m_offsetX;
    config["offset_y"] = m_offsetY;
    config["grid_line_thickness"] = m_gridLineThickness;
    config["last_modified"] = std::time(nullptr);
    
    // Save tile rules
    json tileRulesJson = json::object();
    for (const auto& pair : m_tileRules) {
        std::string tileKey = std::to_string(pair.first.x) + "," + std::to_string(pair.first.y);
        json ruleArray = json::array();
        
        for (const auto& condition : pair.second) {
            switch (condition) {
                case RuleCondition::IGNORE:
                    ruleArray.push_back("IGNORE");
                    break;
                case RuleCondition::SAME:
                    ruleArray.push_back("SAME");
                    break;
                case RuleCondition::DIFFERENT:
                    ruleArray.push_back("DIFFERENT");
                    break;
                case RuleCondition::EMPTY:
                    ruleArray.push_back("EMPTY");
                    break;
            }
        }
        tileRulesJson[tileKey] = ruleArray;
    }
    config["tile_rules"] = tileRulesJson;
    
    // Preserve creation date if it exists
    std::string configPath = getAssetConfigPath(assetName);
    if (std::filesystem::exists(configPath)) {
        std::ifstream existingFile(configPath);
        if (existingFile.is_open()) {
            try {
                json existingConfig;
                existingFile >> existingConfig;
                existingFile.close();
                
                if (existingConfig.contains("created_date")) {
                    config["created_date"] = existingConfig["created_date"];
                }
            } catch (const json::exception& e) {
                // If we can't read the existing file, just use current time
                config["created_date"] = std::time(nullptr);
            }
        }
    } else {
        config["created_date"] = std::time(nullptr);
    }
    
    std::ofstream file(configPath);
    if (file.is_open()) {
        file << config.dump(4); // Pretty print with 4 spaces
        file.close();
    } else {
    }
}

std::string Scene_AutoTileConfig::getAssetConfigPath(const std::string& assetName) {
    // Remove file extension and create JSON config name
    std::string baseName = assetName;
    size_t dotPos = baseName.find_last_of('.');
    if (dotPos != std::string::npos) {
        baseName = baseName.substr(0, dotPos);
    }
    
    return "metadata/autotiling/" + baseName + ".json";
}

void Scene_AutoTileConfig::updateSlicingParametersFromConfig() {
    // This method is called when slicing parameters are updated
    // It triggers a recalculation of the slicing grid
    updateSlicingParameters();
}

void Scene_AutoTileConfig::applyCurrentSlicingParameters() {
    // This method applies the current slicing parameters
    // It's called when switching assets or loading configs
    updateSlicingParameters();
}

// Tile Navigation and Rule Management Methods
void Scene_AutoTileConfig::handlePreviewPanelNavigation(const std::string& direction) {
    if (m_currentAssetTexture.getSize().x == 0) return;
    
    const float scrollSpeed = 32.0f; // Scroll by 32 pixels at a time
    sf::Vector2u textureSize = m_currentAssetTexture.getSize();
    
    // Use the actual preview area dimensions stored during drawing
    float previewAreaWidth = m_currentPreviewWidth;
    float previewAreaHeight = m_currentPreviewHeight;
    
    // Fallback to reasonable defaults if dimensions not set yet
    if (previewAreaWidth <= 0) previewAreaWidth = 400;
    if (previewAreaHeight <= 0) previewAreaHeight = 300;
    
    if (direction == "UP") {
        // Try to move selected tile up first
        if (m_selectedTileInPreview.y > 0) {
            sf::Vector2i newPos = m_selectedTileInPreview;
            newPos.y--;
            selectTileInPreview(newPos);
            
            // Adjust scroll if tile is out of view
            float tileY = m_offsetY + m_marginY + newPos.y * (m_tileHeight + m_spacingY);
            if (tileY < m_previewScrollY) {
                m_previewScrollY = std::max(0.0f, tileY - scrollSpeed);
            }
        } else {
            // Scroll up
            m_previewScrollY = std::max(0.0f, m_previewScrollY - scrollSpeed);
        }
    }
    else if (direction == "DOWN") {
        // Try to move selected tile down first
        if (m_selectedTileInPreview.y < m_tilesY - 1) {
            sf::Vector2i newPos = m_selectedTileInPreview;
            newPos.y++;
            selectTileInPreview(newPos);
            
            // Adjust scroll if tile is out of view
            float tileY = m_offsetY + m_marginY + newPos.y * (m_tileHeight + m_spacingY);
            if (tileY + m_tileHeight > m_previewScrollY + previewAreaHeight) {
                m_previewScrollY = tileY + m_tileHeight - previewAreaHeight + scrollSpeed;
            }
        } else {
            // Scroll down
            float maxScrollY = std::max(0.0f, (float)textureSize.y - previewAreaHeight);
            m_previewScrollY = std::min(maxScrollY, m_previewScrollY + scrollSpeed);
        }
    }
    else if (direction == "LEFT") {
        // Try to move selected tile left first
        if (m_selectedTileInPreview.x > 0) {
            sf::Vector2i newPos = m_selectedTileInPreview;
            newPos.x--;
            selectTileInPreview(newPos);
            
            // Adjust scroll if tile is out of view
            float tileX = m_offsetX + m_marginX + newPos.x * (m_tileWidth + m_spacingX);
            if (tileX < m_previewScrollX) {
                m_previewScrollX = std::max(0.0f, tileX - scrollSpeed);
            }
        } else {
            // Scroll left
            m_previewScrollX = std::max(0.0f, m_previewScrollX - scrollSpeed);
        }
    }
    else if (direction == "RIGHT") {
        // Try to move selected tile right first
        if (m_selectedTileInPreview.x < m_tilesX - 1) {
            sf::Vector2i newPos = m_selectedTileInPreview;
            newPos.x++;
            selectTileInPreview(newPos);
            
            // Adjust scroll if tile is out of view
            float tileX = m_offsetX + m_marginX + newPos.x * (m_tileWidth + m_spacingX);
            if (tileX + m_tileWidth > m_previewScrollX + previewAreaWidth) {
                m_previewScrollX = tileX + m_tileWidth - previewAreaWidth + scrollSpeed;
            }
        } else {
            // Scroll right
            float maxScrollX = std::max(0.0f, (float)textureSize.x - previewAreaWidth);
            m_previewScrollX = std::min(maxScrollX, m_previewScrollX + scrollSpeed);
        }
    }
}

void Scene_AutoTileConfig::handleTileNavigation(const std::string& direction) {
    updateTileGridDimensions(); // Ensure we have current dimensions
    
    sf::Vector2i newPos = m_selectedTileInPreview;
    
    if (direction == "UP" && newPos.y > 0) {
        newPos.y--;
    }
    else if (direction == "DOWN" && newPos.y < m_tilesY - 1) {
        newPos.y++;
    }
    else if (direction == "LEFT" && newPos.x > 0) {
        newPos.x--;
    }
    else if (direction == "RIGHT" && newPos.x < m_tilesX - 1) {
        newPos.x++;
    }
    
    if (newPos != m_selectedTileInPreview) {
        selectTileInPreview(newPos);
    }
}

void Scene_AutoTileConfig::selectTileInPreview(sf::Vector2i tilePos) {
    if (!isValidTilePosition(tilePos)) {
        return;
    }
    
    m_selectedTileInPreview = tilePos;
    extractSelectedTile();
    loadRulesForSelectedTile();
}

void Scene_AutoTileConfig::updateTileGridDimensions() {
    if (m_currentAssetTexture.getSize().x == 0) {
        m_tilesX = m_tilesY = 0;
        return;
    }
    
    sf::Vector2u textureSize = m_currentAssetTexture.getSize();

    // Calculate available area after offset and margins
    int availableWidth = textureSize.x - m_offsetX - 2 * m_marginX;
    int availableHeight = textureSize.y - m_offsetY - 2 * m_marginY;
    
    // Calculate number of tiles based on current slicing parameters
    if (availableWidth > 0 && availableHeight > 0) {
        m_tilesX = (availableWidth + m_spacingX) / (m_tileWidth + m_spacingX);
        m_tilesY = (availableHeight + m_spacingY) / (m_tileHeight + m_spacingY);
    } else {
        m_tilesX = m_tilesY = 0;
    }
 
    // Ensure selected tile is within bounds
    if (m_selectedTileInPreview.x >= m_tilesX) {
        m_selectedTileInPreview.x = std::max(0, m_tilesX - 1);
    }
    if (m_selectedTileInPreview.y >= m_tilesY) {
        m_selectedTileInPreview.y = std::max(0, m_tilesY - 1);
    }
}

void Scene_AutoTileConfig::extractSelectedTile() {
    if (m_currentAssetTexture.getSize().x == 0 || !isValidTilePosition(m_selectedTileInPreview)) {
        return;
    }
    
    // Calculate the pixel position of the selected tile with offset
    int tilePixelX = m_offsetX + m_marginX + m_selectedTileInPreview.x * (m_tileWidth + m_spacingX);
    int tilePixelY = m_offsetY + m_marginY + m_selectedTileInPreview.y * (m_tileHeight + m_spacingY);
    
    // Adjust for grid line thickness - extract only content inside grid lines
    // Since assets have at least 1px gap, we can safely use grid line thickness as offset
    int contentStartX = tilePixelX + m_gridLineThickness;
    int contentStartY = tilePixelY + m_gridLineThickness;
    int contentWidth = m_tileWidth - (2 * m_gridLineThickness); // Remove grid lines from both sides
    int contentHeight = m_tileHeight - (2 * m_gridLineThickness); // Remove grid lines from both sides
    
    // Ensure content dimensions are valid
    contentWidth = std::max(1, contentWidth);
    contentHeight = std::max(1, contentHeight);
    
    // Create a texture for the selected tile content (without grid lines)
    sf::Image fullImage = m_currentAssetTexture.copyToImage();
    sf::Image tileImage;
    tileImage.create(contentWidth, contentHeight);
    
    // Copy only the content pixels (inside grid lines)
    for (int y = 0; y < contentHeight; y++) {
        for (int x = 0; x < contentWidth; x++) {
            if (contentStartX + x < static_cast<int>(fullImage.getSize().x) && 
                contentStartY + y < static_cast<int>(fullImage.getSize().y)) {
                tileImage.setPixel(x, y, fullImage.getPixel(contentStartX + x, contentStartY + y));
            }
        }
    }
    
    // Load the extracted tile content into the sprite
    m_selectedTileTexture.loadFromImage(tileImage);
    m_selectedTileSprite.setTexture(m_selectedTileTexture);
}

void Scene_AutoTileConfig::loadRulesForSelectedTile() {
    // Load existing rules for this tile position, or create default
    auto it = m_tileRules.find(m_selectedTileInPreview);
    if (it != m_tileRules.end()) {
        m_currentRule = it->second;
    } else {
        // Create default rule (all IGNORE except center)
        m_currentRule = std::vector<RuleCondition>(9, RuleCondition::IGNORE);
        m_currentRule[4] = RuleCondition::SAME; // Center tile
    }
}

void Scene_AutoTileConfig::saveRulesForSelectedTile() {
    m_tileRules[m_selectedTileInPreview] = m_currentRule;
}

void Scene_AutoTileConfig::drawSelectedTileInfo(float x, float y, float width, float height) {
    // Draw separator line
    sf::RectangleShape separator;
    separator.setSize(sf::Vector2f(width - 20, 2));
    separator.setPosition(x + 10, y + height * 0.4f);
    separator.setFillColor(sf::Color(100, 100, 100));
    m_game->window().draw(separator);
    
    // Draw selected tile info
    sf::Text tileInfo;
    tileInfo.setFont(m_game->getAssets().getFont("ShareTech"));
    tileInfo.setString("Selected Tile: (" + std::to_string(m_selectedTileInPreview.x) + ", " + 
                      std::to_string(m_selectedTileInPreview.y) + ")\nSize: " + 
                      std::to_string(m_tileWidth) + "x" + std::to_string(m_tileHeight) + "px");
    tileInfo.setCharacterSize(14);
    tileInfo.setFillColor(sf::Color::White);
    tileInfo.setPosition(x + 10, y + height * 0.45f);
    m_game->window().draw(tileInfo);
}

void Scene_AutoTileConfig::drawTileRuleGrid(float x, float y, float width, float height) {
    float startY = y + height * 0.65f;
    float availableHeight = height * 0.35f - 10;
    
    // Title for rule section
    sf::Text ruleTitle;
    ruleTitle.setFont(m_game->getAssets().getFont("ShareTech"));
    ruleTitle.setString("Tile Rules:");
    ruleTitle.setCharacterSize(16);
    ruleTitle.setFillColor(sf::Color(200, 200, 200));
    ruleTitle.setPosition(x + 10, startY);
    m_game->window().draw(ruleTitle);
    
    // 3x3 Rule grid - smaller cells for better space usage
    float maxCellSize = std::min(availableHeight - 30, (width - 40) / 3);
    float gridSize = std::min(maxCellSize, 35.0f); // Limit cell size to 35px maximum
    float gridStartX = x + (width - gridSize * 3) / 2;
    float gridStartY = startY + 25;
    
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int cellIndex = row * 3 + col;
            sf::RectangleShape cell;
            cell.setSize(sf::Vector2f(gridSize - 2, gridSize - 2));
            cell.setPosition(gridStartX + col * gridSize, gridStartY + row * gridSize);
            
            // Color based on rule condition
            RuleCondition condition = (cellIndex < static_cast<int>(m_currentRule.size())) ? m_currentRule[cellIndex] : RuleCondition::IGNORE;
            sf::Color cellColor;
            
            if (row == 1 && col == 1) {
                // Center cell - make background transparent to show the current previewed tile
                cellColor = sf::Color(40, 40, 45, 100); // Semi-transparent dark background
                cell.setFillColor(cellColor);
                
                // Draw the current previewed tile in the center if available
                if (m_selectedTileTexture.getSize().x > 0) {
                    float tileScale = (gridSize - 4) / std::max(m_tileWidth, m_tileHeight);
                    m_selectedTileSprite.setScale(tileScale, tileScale);
                    m_selectedTileSprite.setPosition(gridStartX + col * gridSize + 2, gridStartY + row * gridSize + 2);
                    m_game->window().draw(m_selectedTileSprite);
                }
            } else {
                switch (condition) {
                    case RuleCondition::SAME:
                        cellColor = sf::Color(100, 200, 100); // Green
                        break;
                    case RuleCondition::DIFFERENT:
                        cellColor = sf::Color(200, 100, 100); // Red
                        break;
                    case RuleCondition::EMPTY:
                        cellColor = sf::Color(100, 100, 200); // Blue
                        break;
                    case RuleCondition::IGNORE:
                    default:
                        cellColor = sf::Color(80, 80, 85); // Gray
                        break;
                }
                cell.setFillColor(cellColor);
            }
            
            // Highlight selected cell
            if (cellIndex == m_ruleGridSelection) {
                cell.setOutlineThickness(3);
                cell.setOutlineColor(sf::Color::Yellow);
            } else {
                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color::White);
            }
            
            m_game->window().draw(cell);
        }
    }
}


bool Scene_AutoTileConfig::isValidTilePosition(sf::Vector2i tilePos) const {
    return tilePos.x >= 0 && tilePos.x < m_tilesX && tilePos.y >= 0 && tilePos.y < m_tilesY;
}

void Scene_AutoTileConfig::drawTileRuleGridInForm(float x, float y, float width, float height) {
    // Safety check: ensure we have a valid game instance
    if (!m_game) {
        return;
    }
    
    // Safety check: ensure m_currentRule is properly sized
    if (m_currentRule.size() != 9) {
        m_currentRule = std::vector<RuleCondition>(9, RuleCondition::IGNORE);
        m_currentRule[4] = RuleCondition::SAME; // Center tile
    }
    
    // Safety check: ensure tile dimensions are valid
    if (m_tileWidth <= 0 || m_tileHeight <= 0) {
        m_tileWidth = 64;
        m_tileHeight = 64;
    }
    
    // 3x3 Rule grid with selected tile in center - Interactive, smaller cells
    float maxCellSize = std::min((height - 40) / 3, (width - 40) / 3);
    float gridSize = std::min(maxCellSize, 50.0f); // Limit cell size to 30px maximum for form
    float gridStartX = x + (width - gridSize * 3) / 2;
    float gridStartY = y + 25;
    
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int cellIndex = row * 3 + col;
            
            // Safety check: ensure cellIndex is valid
            if (cellIndex < 0 || cellIndex >= 9) {
                continue;
            }
            
            sf::RectangleShape cell;
            cell.setSize(sf::Vector2f(gridSize - 2, gridSize - 2));
            cell.setPosition(gridStartX + col * gridSize, gridStartY + row * gridSize);
            
            // Color based on rule condition
            RuleCondition condition = (cellIndex < static_cast<int>(m_currentRule.size())) ? m_currentRule[cellIndex] : RuleCondition::IGNORE;
            sf::Color cellColor;
            
            if (row == 1 && col == 1) {
                // Center cell - make background transparent to show the current previewed tile
                cellColor = sf::Color(40, 40, 45, 100); // Semi-transparent dark background
                cell.setFillColor(cellColor);
                
                // Draw the current previewed tile in the center if available
                if (m_selectedTileTexture.getSize().x > 0) {
                    float tileScale = (gridSize - 4) / std::max(m_tileWidth, m_tileHeight);
                    m_selectedTileSprite.setScale(tileScale, tileScale);
                    m_selectedTileSprite.setPosition(gridStartX + col * gridSize + 2, gridStartY + row * gridSize + 2);
                    m_game->window().draw(m_selectedTileSprite);
                }
                
            } else {
                switch (condition) {
                    case RuleCondition::SAME:
                        cellColor = sf::Color(100, 200, 100); // Green
                        break;
                    case RuleCondition::DIFFERENT:
                        cellColor = sf::Color(200, 100, 100); // Red
                        break;
                    case RuleCondition::EMPTY:
                        cellColor = sf::Color(100, 100, 200); // Blue
                        break;
                    case RuleCondition::IGNORE:
                    default:
                        cellColor = sf::Color(80, 80, 85); // Gray
                        break;
                }
                cell.setFillColor(cellColor);
                
                // Condition label
                sf::Text conditionLabel;
                conditionLabel.setFont(m_game->getAssets().getFont("ShareTech"));
                std::string conditionText;
                sf::Color textColor = sf::Color::White;
                
                switch (condition) {
                    case RuleCondition::SAME: 
                        conditionText = "SAME"; 
                        textColor = sf::Color::White;
                        break;
                    case RuleCondition::DIFFERENT: 
                        conditionText = "DIFF"; 
                        textColor = sf::Color::White;
                        break;
                    case RuleCondition::EMPTY: 
                        conditionText = "EMPTY"; 
                        textColor = sf::Color::White;
                        break;
                    case RuleCondition::IGNORE: 
                        conditionText = "IGN"; 
                        textColor = sf::Color(200, 200, 200);
                        break;
                }
                
                conditionLabel.setString(conditionText);
                conditionLabel.setCharacterSize(std::max(8, static_cast<int>(gridSize / 10)));
                conditionLabel.setFillColor(textColor);
                
                sf::FloatRect textBounds = conditionLabel.getLocalBounds();
                conditionLabel.setPosition(
                    gridStartX + col * gridSize + (gridSize - textBounds.width) / 2,
                    gridStartY + row * gridSize + (gridSize - textBounds.height) / 2
                );
                m_game->window().draw(conditionLabel);
            }
            
            // Highlight selected cell for editing (only when in rule grid area)
            if (cellIndex == m_ruleGridSelection && m_inRuleGridArea && m_ruleGridSelection >= 0 && m_ruleGridSelection < 9) {
                cell.setOutlineThickness(4);
                cell.setOutlineColor(sf::Color::Yellow);
            } else {
                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color::White);
            }
            
            m_game->window().draw(cell);
        }
    }
    
    // Rule editing instructions
    sf::Text ruleInstructions;
    ruleInstructions.setFont(m_game->getAssets().getFont("ShareTech"));
    
    // Show current selection info
    std::string selectionInfo = "Selected: ";
    if (m_ruleGridSelection == 4) {
        selectionInfo += "CENTER (tile)";
    } else {
        selectionInfo += "Position " + std::to_string(m_ruleGridSelection);
        RuleCondition condition = (m_ruleGridSelection < static_cast<int>(m_currentRule.size())) ? m_currentRule[m_ruleGridSelection] : RuleCondition::IGNORE;
        selectionInfo += " - ";
        switch (condition) {
            case RuleCondition::SAME: selectionInfo += "SAME"; break;
            case RuleCondition::DIFFERENT: selectionInfo += "DIFFERENT"; break;
            case RuleCondition::EMPTY: selectionInfo += "EMPTY"; break;
            case RuleCondition::IGNORE: selectionInfo += "IGNORE"; break;
        }
    }
    
    ruleInstructions.setString(selectionInfo);
    ruleInstructions.setCharacterSize(10);
    ruleInstructions.setFillColor(sf::Color(150, 150, 150));
    ruleInstructions.setPosition(x + 10, gridStartY + gridSize * 3 + 5);
    m_game->window().draw(ruleInstructions);
}
