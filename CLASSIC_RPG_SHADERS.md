# 🎮 Classic RPG Shader Collection

## 🌟 **10 New Classic RPG Shaders Inspired by Final Fantasy 1-6**

### 🎯 **Complete Shader Arsenal**
Your game engine now includes **17 total shaders**:
- **7 Basic Effects**: none, tint, wave, wave_test, pixelate, glow, rainbow
- **10 Classic RPG Effects**: Inspired by legendary RPGs like Final Fantasy 1-6

---

## ⚔️ **Combat & Damage Effects**

### 1. **Damage Flash** (`damage_flash.frag`)
**Inspired by**: FF1-6 hit effects
**Effect**: Classic white/red flash when taking damage
**Parameters**:
- `flashIntensity`: 0.0-1.0 (auto-animated fast pulse)
- `flashColor`: RGB color (default: white)
**Usage**: Perfect for hit reactions, critical hits, healing effects

### 2. **Berserk/Rage** (`berserk_rage.frag`)
**Inspired by**: FF6 Berserk status, FF4 Yang's Focus
**Effect**: Red aura with screen shake simulation and fiery edges
**Parameters**:
- `rageLevel`: 0.0-1.0 (intensity of rage)
- `time`: For pulsing and shake effects
**Usage**: Berserk status, rage mode, power-up states

---

## 🧪 **Status Effect Shaders**

### 3. **Poison Effect** (`poison_effect.frag`)
**Inspired by**: FF series poison status
**Effect**: Green pulsing tint with transparency variation
**Parameters**:
- `poisonColor`: RGB (default: green)
- `time`: For pulsing animation
**Usage**: Poison, disease, corruption effects

### 4. **Stone/Petrify** (`stone_petrify.frag`)
**Inspired by**: FF series petrification
**Effect**: Gradual stone transformation with sparkle effects
**Parameters**:
- `petrifyLevel`: 0.0-1.0 (transformation progress)
- `time`: For sparkle animation
**Usage**: Petrification, stone curse, statue transformation

### 5. **Freeze/Ice** (`freeze_ice.frag`)
**Inspired by**: FF series ice spells and freeze status
**Effect**: Ice blue tint with crystal patterns and frost sparkles
**Parameters**:
- `freezeLevel`: 0.0-1.0 (how frozen)
- `time`: For crystal animation
**Usage**: Freeze status, ice spells, cold environments

### 6. **Invisibility** (`invisibility.frag`)
**Inspired by**: FF series invisibility spells
**Effect**: Shimmering transparency with distortion
**Parameters**:
- `invisLevel`: 0.0-1.0 (transparency level)
- `time`: For shimmer effect
**Usage**: Invisibility spells, stealth mode, ghost effects

### 7. **Darkness/Blind** (`darkness_blind.frag`)
**Inspired by**: FF series blind/darkness status
**Effect**: Dark purple tint with swirling shadows and vignette
**Parameters**:
- `darknessLevel`: 0.0-1.0 (darkness intensity)
- `time`: For swirling animation
**Usage**: Blind status, shadow magic, dark environments

---

## ✨ **Magic & Enhancement Effects**

### 8. **Magic Charge** (`magic_charge.frag`)
**Inspired by**: FF4-6 spell casting animations
**Effect**: Pulsing magical aura that builds up over time
**Parameters**:
- `chargeLevel`: 0.0-1.0 (spell charge progress)
- `magicColor`: RGB (default: blue)
- `time`: For pulsing energy
**Usage**: Spell casting, magic charging, mana buildup

### 9. **Holy/Protect** (`holy_protect.frag`)
**Inspired by**: FF series Protect/Shell/Holy spells
**Effect**: Golden barrier with light rays and gentle pulsing
**Parameters**:
- `protectionLevel`: 0.0-1.0 (protection strength)
- `time`: For barrier animation
**Usage**: Protection spells, holy magic, divine blessing

### 10. **Haste/Speed** (`haste_speed.frag`)
**Inspired by**: FF series Haste spell
**Effect**: Motion blur with yellow energy trails
**Parameters**:
- `speedLevel`: 0.0-1.0 (speed boost intensity)
- `time`: For trail animation
**Usage**: Haste status, speed boosts, quick movement

---

## 🎮 **How to Use in Your Game**

### **In Shader Demo**
1. Launch game: `make run`
2. Select "Shader Demo" from menu
3. Press **Space** to cycle through all **17 shaders**
4. Press **R** to reload shaders (great for development)

### **In Your Code**
```cpp
// Load a specific RPG shader
ShaderManager& shaderManager = m_game->getShaderManager();
shaderManager.loadFragmentShader("poison_effect", "assets/shaders/poison_effect.frag");

// Apply to a sprite with parameters
sf::Shader* shader = shaderManager.getShader("poison_effect");
if (shader) {
    shader->setUniform("time", gameTime);
    shader->setUniform("poisonColor", sf::Glsl::Vec3(0.2f, 0.8f, 0.2f));
    window.draw(sprite, shader);
}
```

### **Parameter Animation Examples**
```cpp
// Damage flash on hit
float flashIntensity = hitTimer < 0.5f ? 1.0f : 0.0f;
shader->setUniform("flashIntensity", flashIntensity);

// Gradual petrification
float petrifyProgress = petrifyTimer / maxPetrifyTime;
shader->setUniform("petrifyLevel", petrifyProgress);

// Pulsing magic charge
float chargeLevel = sin(time * 2.0) * 0.5 + 0.5;
shader->setUniform("chargeLevel", chargeLevel);
```

---

## 🎨 **Visual Style Guide**

### **Color Schemes**
- **🔴 Damage/Rage**: Red, orange, white flashes
- **🟢 Poison/Nature**: Green, sickly yellow-green
- **🔵 Magic/Ice**: Blue, cyan, white sparkles
- **🟡 Holy/Speed**: Gold, yellow, bright white
- **🟣 Dark/Curse**: Purple, black, dark gray
- **⚪ Stone/Neutral**: Gray, brown, muted tones

### **Animation Speeds**
- **Fast (6-8 Hz)**: Damage flash, berserk rage
- **Medium (2-4 Hz)**: Magic charge, poison pulse
- **Slow (0.5-1.5 Hz)**: Petrification, protection aura

### **Intensity Levels**
- **Subtle (0.2-0.4)**: Background status effects
- **Moderate (0.5-0.7)**: Active spell effects
- **Dramatic (0.8-1.0)**: Critical moments, boss abilities

---

## 🎯 **Perfect For**

### **RPG Systems**
- ✅ **Status Effects**: Visual feedback for all major RPG status ailments
- ✅ **Spell Casting**: Magic charge-up and release effects
- ✅ **Combat Feedback**: Hit reactions and damage visualization
- ✅ **Character States**: Buffs, debuffs, transformations

### **Game Genres**
- ✅ **Classic JRPGs**: Turn-based combat with status effects
- ✅ **Action RPGs**: Real-time combat with visual feedback
- ✅ **Tactical RPGs**: Grid-based combat with spell effects
- ✅ **Roguelikes**: Dungeon crawling with magical effects

### **Development Benefits**
- ✅ **Instant Visual Feedback**: Players immediately understand status changes
- ✅ **Professional Polish**: AAA-quality shader effects
- ✅ **Easy Integration**: Simple uniform parameters
- ✅ **Performance Optimized**: Efficient fragment shaders

---

## 🚀 **Ready to Use!**

Your game engine now has a complete arsenal of **classic RPG shader effects** ready for immediate use! Each shader is:

- **🎮 Game-Ready**: Optimized for real-time gameplay
- **🎨 Visually Striking**: Inspired by beloved classics
- **⚙️ Easily Customizable**: Simple parameters for any situation
- **📚 Well Documented**: Clear usage examples and guidelines

Transform your RPG with these legendary visual effects! ✨🎮
