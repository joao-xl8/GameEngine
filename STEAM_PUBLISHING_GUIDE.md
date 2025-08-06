# Steam Publishing Guide for GameEngine

A comprehensive guide to publishing your C++/SFML game engine project to Steam.

## Table of Contents
1. [Pre-Publishing Requirements](#pre-publishing-requirements)
2. [Steam Developer Account Setup](#steam-developer-account-setup)
3. [Game Preparation](#game-preparation)
4. [Technical Requirements](#technical-requirements)
5. [Build System for Distribution](#build-system-for-distribution)
6. [Steam Integration](#steam-integration)
7. [Store Page Setup](#store-page-setup)
8. [Testing and QA](#testing-and-qa)
9. [Release Process](#release-process)
10. [Post-Launch Considerations](#post-launch-considerations)

---

## Pre-Publishing Requirements

### Legal and Business Setup
- [ ] **Business Entity**: Consider forming an LLC or corporation
- [ ] **Tax Information**: Prepare tax forms (W-9 for US, tax treaty forms for international)
- [ ] **Bank Account**: Business bank account for Steam payments
- [ ] **Legal Review**: Terms of service, privacy policy, EULA
- [ ] **Age Rating**: ESRB, PEGI, or other regional ratings if required

### Financial Requirements
- [ ] **Steam Direct Fee**: $100 USD per game (refundable after $1,000 in sales)
- [ ] **Revenue Share**: Steam takes 30% (reduces to 25% after $10M, 20% after $50M)
- [ ] **Payment Processing**: Steam handles this, but understand the fees

---

## Steam Developer Account Setup

### Account Creation
1. **Visit**: [Steamworks Partner Portal](https://partner.steamgames.com/)
2. **Create Account**: Use business email if available
3. **Pay Steam Direct Fee**: $100 USD per game
4. **Complete Tax Interview**: Provide tax information
5. **Wait for Approval**: Can take 1-30 days

### Required Documentation
- [ ] Government-issued ID
- [ ] Bank account information
- [ ] Tax forms (W-9, W-8BEN, etc.)
- [ ] Business registration (if applicable)

---

## Game Preparation

### Current Project Status
Your GameEngine project includes:
- ✅ **Core Game Loop**: Complete with scenes, entities, components
- ✅ **Graphics System**: SFML-based rendering with viewport scaling
- ✅ **Input System**: Keyboard controls with action mapping
- ✅ **Sound System**: Background music and sound effects
- ✅ **Animation System**: Sprite-based animations
- ✅ **Camera System**: Following camera with dead zones
- ✅ **Grid Movement**: Tile-based movement system
- ✅ **Menu System**: Main menu, options, scene management

### Game Content Requirements
- [ ] **Complete Gameplay**: Finish your game mechanics
- [ ] **Level Design**: Create engaging levels/content
- [ ] **Art Assets**: Professional-quality sprites, backgrounds, UI
- [ ] **Audio**: Music, sound effects, voice acting (if applicable)
- [ ] **Story/Narrative**: If your game includes story elements
- [ ] **Balancing**: Difficulty curves, progression systems

### Technical Polish
- [ ] **Performance Optimization**: 60+ FPS on target hardware
- [ ] **Memory Management**: No memory leaks
- [ ] **Error Handling**: Graceful handling of edge cases
- [ ] **Save System**: If applicable to your game
- [ ] **Settings Menu**: Graphics, audio, controls options
- [ ] **Accessibility**: Consider colorblind support, key remapping

---

## Technical Requirements

### Platform Support
Your current build system supports:
- ✅ **macOS**: Native ARM64 and x86_64 support
- ✅ **Linux**: Cross-compilation ready
- ⚠️ **Windows**: Needs testing and refinement

### Steam Platform Requirements
- [ ] **Windows Support**: Primary Steam platform (90%+ of users)
- [ ] **64-bit Builds**: Steam recommends 64-bit only
- [ ] **Multiple Resolutions**: Support common resolutions (1920x1080, 1366x768, etc.)
- [ ] **Windowed/Fullscreen**: Both modes supported
- [ ] **Controller Support**: Consider gamepad support
- [ ] **Steam Overlay**: Must not interfere with Steam overlay

### Minimum System Requirements
Define and test:
```
Minimum:
- OS: Windows 10 64-bit / macOS 10.15 / Ubuntu 18.04
- Processor: Intel i3-4130 / AMD FX-6300
- Memory: 4 GB RAM
- Graphics: DirectX 11 compatible / OpenGL 3.3
- Storage: 500 MB available space
- Sound Card: DirectX compatible

Recommended:
- OS: Windows 11 64-bit / macOS 12.0 / Ubuntu 20.04
- Processor: Intel i5-8400 / AMD Ryzen 5 2600
- Memory: 8 GB RAM
- Graphics: GTX 1060 / RX 580 / Integrated graphics
- Storage: 1 GB available space
- Sound Card: DirectX compatible
```

---

## Build System for Distribution

### Current Build System Enhancement
Your current Makefile needs distribution-ready builds:

```makefile
# Add to your Makefile
.PHONY: dist-windows dist-macos dist-linux package-all

# Windows distribution build
dist-windows:
	@echo "Building Windows distribution..."
	mkdir -p dist/windows
	$(CXX) $(CXXFLAGS) -DNDEBUG -O3 -static-libgcc -static-libstdc++ \
		$(SOURCES) $(INCLUDES) $(LIBS_WINDOWS) -o dist/windows/$(TARGET).exe
	cp -r assets dist/windows/
	cp README.md LICENSE dist/windows/

# macOS distribution build  
dist-macos:
	@echo "Building macOS distribution..."
	mkdir -p dist/macos/$(TARGET).app/Contents/MacOS
	mkdir -p dist/macos/$(TARGET).app/Contents/Resources
	$(CXX) $(CXXFLAGS) -DNDEBUG -O3 \
		$(SOURCES) $(INCLUDES) $(LIBS_MACOS) -o dist/macos/$(TARGET).app/Contents/MacOS/$(TARGET)
	cp -r assets dist/macos/$(TARGET).app/Contents/Resources/
	cp Info.plist dist/macos/$(TARGET).app/Contents/

# Linux distribution build
dist-linux:
	@echo "Building Linux distribution..."
	mkdir -p dist/linux
	$(CXX) $(CXXFLAGS) -DNDEBUG -O3 \
		$(SOURCES) $(INCLUDES) $(LIBS_LINUX) -o dist/linux/$(TARGET)
	cp -r assets dist/linux/
	cp README.md LICENSE dist/linux/

# Package all platforms
package-all: dist-windows dist-macos dist-linux
	@echo "Creating distribution packages..."
	cd dist && zip -r $(TARGET)-windows.zip windows/
	cd dist && tar -czf $(TARGET)-macos.tar.gz macos/
	cd dist && tar -czf $(TARGET)-linux.tar.gz linux/
```

### Distribution Checklist
- [ ] **Static Linking**: Minimize external dependencies
- [ ] **Asset Packaging**: All assets included and accessible
- [ ] **Performance Build**: Optimized release builds (-O3, -DNDEBUG)
- [ ] **Debug Symbols**: Strip debug info for smaller file size
- [ ] **Installer**: Consider creating installers for Windows
- [ ] **Code Signing**: Sign executables (especially for macOS/Windows)

---

## Steam Integration

### Steamworks SDK Integration
1. **Download SDK**: Get latest Steamworks SDK from partner portal
2. **Integration**: Add Steam API calls to your game

```cpp
// Add to your GameEngine class
#include "steam/steam_api.h"

class GameEngine {
private:
    bool m_steamInitialized = false;
    
public:
    bool initSteam() {
        if (!SteamAPI_Init()) {
            std::printf("Failed to initialize Steam API\n");
            return false;
        }
        m_steamInitialized = true;
        std::printf("Steam API initialized successfully\n");
        return true;
    }
    
    void shutdownSteam() {
        if (m_steamInitialized) {
            SteamAPI_Shutdown();
        }
    }
    
    void updateSteam() {
        if (m_steamInitialized) {
            SteamAPI_RunCallbacks();
        }
    }
};
```

### Steam Features to Consider
- [ ] **Achievements**: Define and implement achievements
- [ ] **Leaderboards**: If applicable to your game
- [ ] **Cloud Saves**: Sync save files across devices
- [ ] **Workshop**: User-generated content support
- [ ] **Trading Cards**: Collectible cards for your game
- [ ] **Rich Presence**: Show what player is doing in Steam
- [ ] **Screenshots**: Steam screenshot functionality

### Steam Configuration Files
Create `steam_appid.txt` in your game directory:
```
480  # Replace with your actual App ID
```

---

## Store Page Setup

### Required Assets
- [ ] **Header Capsule**: 460x215 pixels
- [ ] **Small Capsule**: 231x87 pixels  
- [ ] **Main Capsule**: 616x353 pixels
- [ ] **Vertical Capsule**: 374x448 pixels
- [ ] **Hero Capsule**: 1920x622 pixels
- [ ] **Screenshots**: At least 5, 1920x1080 recommended
- [ ] **Trailer Video**: 30-60 seconds, showcase gameplay
- [ ] **Logo**: High-resolution transparent PNG

### Store Page Content
- [ ] **Game Title**: Memorable and searchable
- [ ] **Short Description**: 1-2 sentences, appears in search
- [ ] **Detailed Description**: Full game description with features
- [ ] **System Requirements**: Minimum and recommended specs
- [ ] **Tags**: Genre, features, themes (choose wisely)
- [ ] **Categories**: Single-player, multiplayer, etc.
- [ ] **Languages**: Supported languages for interface/audio
- [ ] **Age Rating**: ESRB, PEGI ratings if required
- [ ] **Release Date**: Target launch date

### Pricing Strategy
- [ ] **Base Price**: Research similar games in your genre
- [ ] **Regional Pricing**: Steam suggests regional prices
- [ ] **Launch Discount**: Consider 10-20% launch discount
- [ ] **Future Sales**: Plan for seasonal Steam sales

---

## Testing and QA

### Pre-Release Testing
- [ ] **Internal Testing**: Thorough testing by development team
- [ ] **Beta Testing**: Closed beta with trusted users
- [ ] **Platform Testing**: Test on multiple Windows/Mac/Linux systems
- [ ] **Performance Testing**: Various hardware configurations
- [ ] **Steam Integration Testing**: All Steam features work correctly
- [ ] **Localization Testing**: If supporting multiple languages

### Steam-Specific Testing
- [ ] **Steam Overlay**: Ensure game works with overlay
- [ ] **Steam Input**: Test with Steam Controller/gamepad
- [ ] **Big Picture Mode**: Test in Steam's TV mode
- [ ] **Steam Deck**: Consider Steam Deck compatibility
- [ ] **Achievement Testing**: All achievements trigger correctly
- [ ] **Cloud Save Testing**: Save sync works properly

### Bug Tracking and Fixes
- [ ] **Bug Database**: Track and prioritize issues
- [ ] **Critical Bugs**: Fix game-breaking issues
- [ ] **Performance Issues**: Address framerate problems
- [ ] **Compatibility Issues**: Fix platform-specific problems
- [ ] **User Experience**: Polish UI/UX issues

---

## Release Process

### Pre-Launch (2-4 weeks before)
- [ ] **Store Page Live**: Make store page visible
- [ ] **Press Kit**: Create press materials
- [ ] **Marketing Campaign**: Social media, gaming sites
- [ ] **Influencer Outreach**: Send keys to streamers/YouTubers
- [ ] **Review Embargo**: Set review embargo date
- [ ] **Day-One Patch**: Prepare any critical fixes

### Launch Day
- [ ] **Release Build**: Upload final build to Steam
- [ ] **Store Page Update**: Final description/screenshots
- [ ] **Social Media**: Announce launch across platforms
- [ ] **Monitor Launch**: Watch for issues, player feedback
- [ ] **Community Management**: Respond to forums/reviews
- [ ] **Press Outreach**: Send launch announcements

### Post-Launch (First Week)
- [ ] **Monitor Reviews**: Respond to feedback
- [ ] **Bug Fixes**: Address critical issues quickly
- [ ] **Community Engagement**: Active in Steam forums
- [ ] **Sales Tracking**: Monitor sales performance
- [ ] **Marketing Adjustment**: Adjust marketing based on reception

---

## Post-Launch Considerations

### Ongoing Support
- [ ] **Regular Updates**: Bug fixes and improvements
- [ ] **Community Feedback**: Listen to player suggestions
- [ ] **DLC Planning**: Consider additional content
- [ ] **Seasonal Sales**: Participate in Steam sales
- [ ] **Achievement Additions**: Add new achievements over time

### Long-term Strategy
- [ ] **Sequel Planning**: If game is successful
- [ ] **Platform Expansion**: Console ports, mobile versions
- [ ] **Community Building**: Discord, social media presence
- [ ] **Merchandise**: Consider physical merchandise
- [ ] **Competitive Scene**: If applicable to your game type

### Analytics and Metrics
- [ ] **Steam Analytics**: Use Steamworks analytics
- [ ] **Player Behavior**: Understand how players play
- [ ] **Retention Metrics**: Track player engagement
- [ ] **Revenue Tracking**: Monitor sales and trends
- [ ] **Review Analysis**: Learn from player feedback

---

## Current Project Readiness Assessment

### ✅ **Ready Components**
- **Core Engine**: Solid foundation with ECS architecture
- **Graphics Pipeline**: SFML integration with viewport system
- **Audio System**: Background music and sound effects
- **Input Handling**: Robust action-based input system
- **Scene Management**: Menu and gameplay scene transitions
- **Build System**: Multi-platform build support

### ⚠️ **Needs Development**
- **Game Content**: Actual gameplay, levels, progression
- **Windows Build**: Thorough testing and optimization
- **Steam Integration**: Steamworks SDK implementation
- **Polish**: UI/UX improvements, visual effects
- **Performance**: Optimization for various hardware

### 🔄 **Requires Planning**
- **Game Design**: Complete game concept and mechanics
- **Art Direction**: Consistent visual style and assets
- **Marketing Strategy**: How to reach your target audience
- **Business Model**: Pricing, DLC, monetization strategy
- **Legal Compliance**: Ratings, regulations, licensing

---

## Estimated Timeline

### Phase 1: Game Development (3-12 months)
- Complete core gameplay mechanics
- Create all game content (levels, art, audio)
- Implement save system and settings
- Polish user experience

### Phase 2: Steam Integration (1-2 months)
- Integrate Steamworks SDK
- Implement Steam features (achievements, etc.)
- Create Steam store assets
- Set up Steam store page

### Phase 3: Testing and Polish (1-3 months)
- Comprehensive QA testing
- Beta testing with users
- Performance optimization
- Bug fixes and polish

### Phase 4: Marketing and Launch (1-2 months)
- Marketing campaign
- Press outreach
- Influencer engagement
- Launch preparation

### Phase 5: Post-Launch Support (Ongoing)
- Community management
- Updates and patches
- DLC development
- Long-term strategy

---

## Resources and Tools

### Development Tools
- **Steam SDK**: Steamworks SDK for Steam integration
- **Analytics**: Steam Analytics, Google Analytics
- **Bug Tracking**: GitHub Issues, Jira, Trello
- **Version Control**: Git (already using)
- **Build Automation**: GitHub Actions, Jenkins

### Marketing Tools
- **Press Kit**: PressKit() for media materials
- **Social Media**: Twitter, Discord, Reddit
- **Video Creation**: OBS Studio for trailers
- **Graphics**: Photoshop, GIMP for store assets
- **Website**: Simple landing page for your game

### Legal and Business
- **Business Formation**: LegalZoom, local attorney
- **Contracts**: Game development contracts
- **Insurance**: Professional liability insurance
- **Accounting**: QuickBooks, FreshBooks
- **Tax Preparation**: Professional tax service

---

## Budget Considerations

### Development Costs
- **Steam Direct Fee**: $100
- **Art Assets**: $500-5,000 (if outsourcing)
- **Audio**: $200-2,000 (music, SFX)
- **Marketing**: $500-10,000
- **Legal/Business**: $500-2,000
- **Tools/Software**: $200-1,000

### Revenue Projections
- **Conservative**: 100-1,000 copies at $10-20
- **Moderate**: 1,000-10,000 copies
- **Optimistic**: 10,000+ copies
- **Steam's Cut**: 30% of gross revenue
- **Break-even**: Calculate based on development costs

---

## Final Checklist

### Before Submitting to Steam
- [ ] Game is complete and thoroughly tested
- [ ] All Steam integration features work
- [ ] Store page is complete with all assets
- [ ] Legal requirements are met
- [ ] Marketing materials are prepared
- [ ] Launch strategy is planned
- [ ] Post-launch support plan is ready

### Launch Day Preparation
- [ ] Final build uploaded and tested
- [ ] Community management plan active
- [ ] Press materials distributed
- [ ] Social media campaign ready
- [ ] Bug reporting system in place
- [ ] Update deployment process tested

---

## Conclusion

Publishing to Steam is a significant undertaking that requires careful planning, thorough testing, and ongoing commitment. Your GameEngine project has a solid technical foundation, but success on Steam depends on creating compelling gameplay, professional presentation, and effective marketing.

Start by completing your game's core content and mechanics, then gradually work through the Steam-specific requirements. Remember that launching on Steam is just the beginning – ongoing support and community engagement are crucial for long-term success.

Good luck with your Steam publishing journey! 🎮🚀

---

*This guide is based on current Steam policies and best practices as of 2024. Always check the latest Steamworks documentation for the most up-to-date requirements.*
