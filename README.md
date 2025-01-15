# Pokemon OpenGL Project

A 3D Pokemon world implementation using modern OpenGL, featuring dynamic animations, weather effects, and real-time lighting.

## 🚀 Features

- **Dynamic Pokemon Animations**
  - Circular flight patterns (Yveltal)
  - Figure-8 flight patterns (Zapdos)
  - Interactive behaviors (jumping, spinning)

- **Advanced Graphics**
  - Real-time shadow mapping
  - Dynamic lighting system
  - Atmospheric fog effects
  - Particle-based weather system

- **Interactive Controls**
  - WASD movement
  - Mouse-look camera
  - Zoom functionality
  - Cinematic presentation mode

- **Weather System**
  - Dynamic rain particles
  - Wind direction and strength
  - Weather-based sound effects

## 🎮 Controls

- **Movement**: WASD keys
- **Camera**: Mouse movement
- **Zoom**: Mouse scroll wheel
- **Toggle Rain**: R key
- **Wind Control**: 
  - Enable/Disable: U key
  - Direction: 1/2 keys
  - Strength: 3/4 keys
- **View Shadow Map**: M key
- **Pokemon Interactions**:
  - Jump: Q key
  - Spin: E key

## 🔧 Building the Project

### Prerequisites
- CMake 3.28 or higher
- OpenGL 4.1+
- GLFW
- OpenAL
- libsndfile

### Build Steps

1. Clone the repository:
git clone https://github.com/BogdanSavianu/pokemon_opengl_game.git
2. You will need to create the "objects" directory and put the following objects inside: https://mega.nz/fm/yPgl2IjB
3. Create build directory in the root of the project
4. Run `cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release` in the root folder
5. Run `make`
6. Run `./Pokemon`

## 🎨 Graphics Pipeline

### Shader System
The project uses modern GLSL shaders for:
- Phong lighting model
- Shadow mapping
- Fog effects
- Normal mapping
- Particle effects

### Shadow Mapping
Implements a two-pass rendering system:
1. Depth map generation from light's perspective
2. Shadow calculation using depth information
3. Soft shadow edges using PCF filtering

### Weather Effects
The rain system features:
- GPU-accelerated particle system
- Wind-affected particle trajectories
- Distance-based visibility
- Alpha blending for realistic appearance

## 🎵 Audio System

- 3D spatial audio positioning
- Dynamic sound effects
- Distance-based volume adjustment
- Weather-related ambient sounds

## ⚡ Performance Optimizations

- Frustum culling for off-screen objects
- Efficient shadow map resolution management
- Batch rendering for particle systems
- Resource pooling and smart memory management
