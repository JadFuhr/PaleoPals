# PaleoPals - Project Overview

## Executive Summary
PaleoPals is a C++ game developed using the **SFML 3.0 library** that simulates a paleontology experience. Players control paleontologist characters to dig for fossils across a procedural underground map, collect dinosaur remains, and display their findings in a museum.

---

## Project Architecture

### Technology Stack
- **Language:** C++ (Visual Studio 2022 project)
- **Graphics/Rendering:** SFML (Simple and Fast Multimedia Library) 3.0
  - sfml-graphics: 2D rendering
  - sfml-audio: Sound effects and music
  - sfml-window: Window management and events
  - sfml-system: Core utilities
  - sfml-network: (included but purpose unclear)
- **Configuration Format:** JSON (using nlohmann/json library)
- **Game Loop:** Custom implementation with event processing, updates, and rendering

### Main Entry Point
[main.cpp](main.cpp) initializes the SFML library dependencies via pragma comments and creates a single `Game` instance that runs the main game loop.

---

## Core Modules & Relationships

### 1. **Game Manager** ([Game.h](Game.h) / [Game.cpp](Game.cpp))
**Role:** Central controller managing all game states and systems
**Key Responsibilities:**
- Manages game state transitions (MainMenu → Gameplay → Museum → Paused, etc.)
- Owns the main SFML render window (1800×900 pixels)
- Maintains a camera view for scrolling/panning across the game world
- Processes events and keyboard input
- Updates all game systems each frame (delta-time based)
- Calls render() to draw all active elements

**Key Dependencies:**
- Map (terrain grid system)
- Menu (main menu UI)
- PauseMenu (pause screen)
- Museum (fossil display building)
- Paleontologist (one or more character instances)
- TraderMenu (trading interface)

**Game States Enum:**
```
MainMenu → Gameplay ↔ Museum/Trader/Paused → Settings → Exit
```

---

### 2. **Map System** ([Map.h](Map.h) / [Map.cpp](Map.cpp))
**Role:** Procedural terrain generation and tile management
**Key Features:**
- Loads terrain configuration from JSON (map.json)
- Generates grid of tiles with 4 layered depth system:
  1. **Topsoil** (hardness: 1) - surface
  2. **Sediment** (hardness: 2)
  3. **Rock** (hardness: 5)
  4. **Bedrock** (hardness: 10) - deepest/hardest

- **Tile Management:**
  - Each tile has a sprite, layer hardness value, and grid position
  - Supports tile removal (mining)
  - Tracks tile hover state for UI feedback

- **Building Integration:**
  - Museum sprite placement (position: 1600, 338)
  - Trader sprite placement (position: 25, 361)

- **Fossil Integration:**
  - Loads all dinosaur data from JSON
  - Spawns fossil pieces at specific grid locations
  - Manages fossil discovery state

**Dependencies:**
- Museum, Trader, Fossil classes for building/object rendering
- JSON configuration parsing

---

### 3. **Character System** ([Paleontologist.h](Paleontologist.h) / [Paleontologist.cpp](Paleontologist.cpp))

**Role:** Player-controlled or AI-controlled digger characters

**Properties:**
- Position on map
- Speed (adjustable)
- Target tile for pathfinding
- Mining progress (0.0 - 1.0)
- Current behavior state

**Rendering:**
- Sprite-based animation (4 frames, 192×192 pixels each)
- Frame updates every 150ms
- Progress bar visualization during mining

**AI/Behavior System:**
Uses a state machine (defined in [Behaviours.h](Behaviours.h)):
- **Idle:** Waiting for input/target
- **Wandering:** Moving randomly across map
- **SearchingForFossil:** Seeking known fossil locations
- **MovingToTarget:** Using A* pathfinding to reach a tile
- **Mining:** Active dig operation on tile (hardness determines duration)

**Navigation:**
- A* pathfinding algorithm for intelligent path planning
- Movement constrained to walkable tiles
- Updates position based on delta-time

**Dependencies:**
- Map (for pathfinding and collision detection)
- Behaviours (AI state machine)

---

### 4. **Fossil System** ([Fossil.h](Fossil.h) / [Fossil.cpp](Fossil.cpp))

**Role:** Manages all paleontological specimen data and discovery

**Data Structures:**
- **FossilPiece:** Individual bone/fragment
  - Texture and sprite for rendering
  - Fossil ID (e.g., "Tyrannosaurus_Skull")
  - Dinosaur species name
  - Category (Carnivore/Herbivore/Pterosaur)
  - Grid position (row, col) - location underground
  - Discovery state flag

- **DinosaurData:** Dinosaur species definition
  - Scientific name
  - Category classification
  - Background image (for museum display)
  - List of pieces that compose the skeleton

**FossilManager:**
- Loads all 7+ dinosaur species from JSON
- Spawns fossil pieces at map generation
- Tracks discovery progress per species
- Calculates collection completion percentage

**Supported Species:**
- Carnivores: T-Rex (4 pieces), Allosaurus (4 pieces), Spinosaurus
- Herbivores: Triceratops, Ankylosaurus, Dreadnoughtus, Therizinosaurus
- Pterosaurs: Pteranodon, Quetzalcoatlus

**Dependencies:**
- Map (for sprite layering and position data)
- Textures loaded from ASSETS/IMAGES/Fossils/

---

### 5. **Museum Building** ([Museum.h](Museum.h) / [Museum.cpp](Museum.cpp))

**Role:** Game building where players view collected fossils

**Functionality:**
- Loads from JSON configuration
- Sprite-based building rendering (160×113 pixels)
- Detects mouse hover for interaction feedback
- Positioned at world coordinate (1600, 338)

**Workflow:**
- When museum is interacted with, Game state switches to `GameState::Museum`
- Displays collected/reconstructed dinosaur skeletons

**Dependencies:**
- JSON configuration for position and texture

---

### 6. **Trader Building** ([Trader.h](Trader.h) / [Trader.cpp](Trader.cpp))

**Role:** NPC building for trading resources/items

**Functionality:**
- Loads from JSON configuration
- Sprite-based building rendering (161×91 pixels)
- Point-in-sprite collision detection
- Positioned at world coordinate (25, 361)

**Workflow:**
- When trader is interacted with, opens TraderMenu
- Game state switches to `GameState::Trader`

**Dependencies:**
- TraderMenu for UI display
- JSON configuration

---

### 7. **User Interface Systems**

#### Menu System ([Menu.h](Menu.h) / [Menu.cpp](Menu.cpp))
- Main menu screen with start and quit buttons
- Texture-based button rendering
- Click detection and state transition handling

#### Pause Menu ([Paused.h](Paused.h) / [Paused.cpp](Paused.cpp))
- In-game pause functionality
- Accessible via keyboard shortcut during gameplay
- Resume/Quit options

#### Trader Menu ([TraderMenu.h](TraderMenu.h) / [TraderMenu.cpp](TraderMenu.cpp))
- Trading interface when interacting with Trader building
- Likely manages item exchanges and inventory

---

## Configuration System

### map.json Structure
Centralized JSON file defining all game content:

```json
{
  "map": {
    "layers": [...]          // Terrain definitions (4 layers)
  },
  "museum": {...},           // Museum building sprite/position
  "trader": {...},           // Trader building sprite/position
  "dinosaurs": [...]         // All species and fossil piece definitions
}
```

**Key Point:** All content data (assets, positions, mechanics) is externalized to JSON, making the game data-driven and easy to modify without recompilation.

---

## Asset Structure

```
ASSETS/
├── IMAGES/
│   ├── Terrain/           (Topsoil.png, Sediment.png, Bedrock.png)
│   ├── Sprites/
│   │   ├── Buildings/     (Museum.png, Trader.png)
│   │   └── Characters/    (Paleontologist animation sprites)
│   ├── Fossils/
│   │   ├── Carnivore/     (T-Rex, Allosaurus, Spinosaurus pieces)
│   │   ├── Herbivore/     (Trike, Anki, Dread, Theri pieces)
│   │   └── Pterosaur/     (Pteranodon, Quetzalcoatlus pieces)
│   └── Screens/           (Menu backgrounds, UI elements)
├── AUDIO/                 (Music/SFX - empty in current build)
├── FONTS/                 (Text rendering - empty in current build)
└── CONFIG/
    └── map.json          (Master configuration file)
```

---

## Game Flow

### 1. **Initialization**
- main() → Game() constructor
- Game loads map.json configuration
- Map generates terrain grid with 4 layers
- Fossil pieces spawn at random grid positions
- Menu initializes with UI assets

### 2. **Main Menu State**
- Display start and quit buttons
- Wait for user click
- Click start → transitions to Gameplay

### 3. **Gameplay State**
- Paleontologist characters update positions/mining
- Player controls camera panning (arrow keys or mouse)
- Click on tiles → paleontologist mines that location
- Each tile takes time based on hardness value
- Discovered fossils added to inventory
- Click buildings → state transitions (Museum/Trader)
- Pause available anytime → Paused state

### 4. **Museum State**
- View complete/incomplete dinosaur reconstructions
- See collected specimens
- Can examine fossil pieces and category
- Return to gameplay

### 5. **Trader State**
- Exchange resources/items with NPC
- Potentially unlock new dig sites or tools
- Return to gameplay

---

## Key Design Patterns

### 1. **State Machine**
- Game uses enum-based state switching
- Each state controls which systems are active
- Decouples different game modes

### 2. **Component-Based Architecture**
- Entities (Paleontologist, Museum, Trader, Map) are independent classes
- Game class composes and orchestrates them
- Easy to add/remove features

### 3. **Data-Driven Design**
- Game content lives in JSON
- Minimal hardcoding of values
- Easy to balance/modify without code changes

### 4. **AI Behavior Tree**
- Paleontologist uses state enum with clear transitions
- A* pathfinding for intelligent navigation
- Modular behavior system for future expansion

### 5. **Frustum Culling**
- Camera view bounds tracked for performance
- Only renders sprites within visible area
- Scales to large procedural maps

---

## Development Progress

### Completed Features
✓ Core game loop (events, update, render)
✓ Terrain generation (4-layer system)
✓ Camera system with panning
✓ Fossil spawning and discovery
✓ Paleontologist character with animation
✓ A* pathfinding algorithm
✓ Mining mechanic with progress tracking
✓ Museum building (UI state)
✓ Trader building (UI state)
✓ JSON configuration loading
✓ Main menu and pause system

### In Development / Stub Classes
- **Researcher.h:** Currently empty (planned for co-op/multiplayer?)
- **TraderMenu:** Exists but functionality may be minimal
- **Audio System:** Assets folder exists but not implemented

---

## Technical Notes

### Platform
- Windows-specific (Visual Studio project)
- Debug and Release builds configured separately with SFML library linking

### Code Organization
- Clear separation of concerns via header/implementation files
- Using `#pragma once` for include guards
- Smart pointers (`std::unique_ptr`) for paleontologist vector
- Forward declarations to reduce coupling

### Performance Considerations
- Frustum culling implemented for camera
- Delta-time based movement (frame-rate independent)
- Sprite-based rendering (efficient for 2D)
- JSON parsed once at startup

---

## Summary for Exam/Presentation

**PaleoPals** is a single-player 2D excavation game where you control paleontologists digging through stratified terrain to find fossil pieces. The game demonstrates:
- **Software Architecture:** State machines, composition, data-driven design
- **Game Programming:** 2D rendering, character animation, AI pathfinding, UI systems
- **Asset Management:** Sprite sheets, textures, JSON configuration
- **Game Mechanics:** Mining (time-based on difficulty), fossil collection, building interactions

The project is well-structured for a game development assignment, using industry-standard patterns and libraries.
