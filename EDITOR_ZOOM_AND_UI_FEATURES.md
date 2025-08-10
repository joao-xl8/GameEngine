# Grid Map Editor - Zoom and Collapsible UI Features

## Overview
Added zoom functionality with arrow keys and ImGui-style collapsible info panel to the grid map editor for better usability and screen space management.

## New Features

### 🔍 **Zoom Controls**
- **Up Arrow**: Zoom in (increase zoom level)
- **Down Arrow**: Zoom out (decrease zoom level)
- **Zoom Range**: 0.25x (zoomed out) to 4.0x (zoomed in)
- **Zoom Step**: 0.1x increments for smooth zooming
- **Visual Feedback**: Current zoom level displayed in header (e.g., "2.5x")

#### Implementation Details
```cpp
// Zoom variables
float m_zoomLevel = 1.0f;         // Current zoom level
float m_minZoom = 0.25f;          // Minimum zoom (zoomed out)
float m_maxZoom = 4.0f;           // Maximum zoom (zoomed in)
float m_zoomStep = 0.1f;          // Zoom increment/decrement

// Zoom controls
else if (action.getName() == "ZOOM_IN") {
    m_zoomLevel = std::min(m_maxZoom, m_zoomLevel + m_zoomStep);
    m_gameView.setSize(m_game->window().getSize().x / m_zoomLevel, 
                      m_game->window().getSize().y / m_zoomLevel);
}
```

### 📋 **Collapsible Info Panel**
- **I Key**: Toggle info panel visibility
- **ImGui-style Design**: `[-]` when expanded, `[+]` when collapsed
- **Header Always Visible**: Shows title and zoom level even when collapsed
- **Space Saving**: Collapsed panel only shows 40px header instead of 600px full panel

#### Implementation Details
```cpp
// UI control variable
bool m_showInfoPanel = true;      // Toggle for info panel visibility

// Collapsible UI rendering
void Scene_GridMapEditor::drawUI()
{
    // Always draw header with collapse indicator
    std::string collapseIndicator = m_showInfoPanel ? "[-] " : "[+] ";
    headerText.setString(collapseIndicator + "MAP EDITOR");
    
    // Only draw full panel if expanded
    if (!m_showInfoPanel) {
        return;
    }
    // ... rest of UI content
}
```

## User Experience Improvements

### 🎯 **Zoom Benefits**
- **Detail Work**: Zoom in (4.0x) for precise tile placement
- **Overview**: Zoom out (0.25x) to see large level layouts
- **Smooth Navigation**: 0.1x increments for fine control
- **Visual Feedback**: Always see current zoom level in header

### 🎛️ **Collapsible Panel Benefits**
- **More Screen Space**: Collapse panel to see more of the level
- **Quick Toggle**: Press 'I' to instantly show/hide info
- **Essential Info Always Visible**: Zoom level stays in header
- **ImGui-style Familiarity**: Standard `[-]`/`[+]` indicators

## Controls Summary

### New Controls
- **↑ (Up Arrow)**: Zoom in
- **↓ (Down Arrow)**: Zoom out  
- **I**: Toggle info panel

### Existing Controls (Updated Help)
- **WASD**: Move cursor
- **Space**: Place object
- **C**: Remove object
- **Q/E**: Change asset
- **Z/X**: Change type
- **R**: Rotate asset
- **T**: Toggle collision on cell
- **V**: Show/hide collision overlay
- **F**: Save level
- **L**: Open level selector
- **Escape**: Back to menu

## Technical Implementation

### Zoom System
```cpp
// Initialize view with zoom
m_gameView.setSize(m_game->window().getSize().x / m_zoomLevel, 
                  m_game->window().getSize().y / m_zoomLevel);

// Zoom controls with bounds checking
m_zoomLevel = std::min(m_maxZoom, m_zoomLevel + m_zoomStep);  // Zoom in
m_zoomLevel = std::max(m_minZoom, m_zoomLevel - m_zoomStep);  // Zoom out
```

### Collapsible UI System
```cpp
// Header always visible (40px height)
sf::RectangleShape headerBackground(sf::Vector2f(320, 40));

// Full panel only when expanded (560px height)
if (m_showInfoPanel) {
    sf::RectangleShape uiBackground(sf::Vector2f(320, 560));
    // ... draw full content
}
```

### Visual Indicators
```cpp
// Zoom level in header
std::ostringstream zoomStream;
zoomStream << std::fixed << std::setprecision(1) << m_zoomLevel << "x";
zoomText.setString(zoomStream.str());

// Collapse indicator
std::string collapseIndicator = m_showInfoPanel ? "[-] " : "[+] ";
```

## Benefits

### Productivity
- ✅ **Faster Navigation**: Zoom out to see entire level, zoom in for details
- ✅ **More Screen Space**: Collapse panel when not needed
- ✅ **Quick Access**: Single key toggles for both features
- ✅ **Visual Feedback**: Always know current zoom level

### Usability
- ✅ **Intuitive Controls**: Arrow keys for zoom (standard in many editors)
- ✅ **Familiar UI**: ImGui-style collapsible panels
- ✅ **Non-destructive**: Collapsing doesn't lose any information
- ✅ **Smooth Operation**: Incremental zoom for precise control

### Workflow
- ✅ **Level Overview**: Zoom out to plan overall layout
- ✅ **Detail Work**: Zoom in for precise tile placement
- ✅ **Clean Workspace**: Collapse panel for unobstructed view
- ✅ **Quick Reference**: Expand panel to check asset properties

## Files Modified
- `src/scenes/scene_grid_map_editor.hpp`: Added zoom and UI toggle variables
- `src/scenes/scene_grid_map_editor.cpp`: Implemented zoom controls and collapsible UI

These features significantly improve the editor's usability for both small detail work and large level design tasks.
