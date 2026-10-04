# PaleoPals - Project Overview

## Executive Summary
PaleoPals is a C++ mining simulation game developed using **SFML 3.0** featuring player-controlled excavation with fossil collection and upgrade systems. Players dig through stratified terrain layers to uncover fossils, earn money, and build an underground collection. The game features physics-based movement and a pickup-upgrade economy system. 

**Known Issue:** Autonomous AI worker systems (behavior trees and A* pathfinding) are currently non-functional. While the hiring UI is present and functional, hired Paleontologists do not autonomously mine as intended; they remain idle after being hired. All fossil collection must be performed manually by the player.

---

## Project Architecture

### Technology Stack
- **Language:** C++ (Visual Studio 2022 project)
- **Graphics/Rendering:** SFML (Simple and Fast Multimedia Library) 3.0
  - sfml-graphics: 2D sprite and shape rendering
  - sfml-window: Window management and event handling
  - sfml-system: Core utilities and timing
- **Configuration Format:** JSON (using nlohmann/json library)
- **Game Loop:** Fixed 60 FPS timestep with event-driven state machine
- **Resolution:** 1800×900 pixels

### Main Entry Point
[main.cpp](main.cpp) initializes SFML and creates a `Game` instance that runs the main game loop with fixed 60 FPS timestep.

---

## Core Modules & Relationships

### 1. **Game Manager** ([Game.h](Game.h) / [Game.cpp](Game.cpp))
**Role:** Central state machine managing all game modes and systems

**Game States Enum:**
- `MainMenu` - Start screen with Start and Quit buttons
- `Gameplay` - Main excavation loop (player mining, AI workers, upgrades)
- `Paused` - Pause menu (Resume/Settings/Quit)
- `Settings` - (Currently stub with no functionality)
- `Exit` - Graceful shutdown

**Key Responsibilities:**
- Manages state transitions and event processing
- Owns the SFML render window (1800×900 pixels)
- Maintains camera view with clamping to map bounds
- Processes player input (WASD, Left-click, ESC, M, F3)
- Updates all game systems with fixed 60 FPS timestep
- Calls render() to draw state-specific elements

**Key Dependencies:**
- Map (terrain and collision system)
- Player (player-controlled excavator with pickaxe)
- Menu (main menu UI)
- Paused (pause screen)
- std::vector of Paleontologist instances (hired AI workers)
- Museum & Trader buildings (interactive locations)
- MuseumInterior (fossil collection gallery)

---

### 2. **Map System** ([Map.h](Map.h) / [Map.cpp](Map.cpp))
**Role:** Procedural terrain grid and tile management

**Key Features:**
- **Grid System:** 75 columns × 100 rows of 24-pixel tiles
- **4-Layer Terrain Depth:**
  1. **Topsoil** (hardness: 8) - surface layer, fastest to mine
  2. **Sediment** (hardness: 35) - mid-layer
  3. **Rock** (hardness: 250) - deep layer
  4. **Bedrock** (hardness: 600) - impenetrable deepest layer

- **Tile Properties:**
  - Health pool (HP) based on hardness value
  - Sprite texture with visual crack progression as health decreases
  - Grid position (row, column)
  - Layer type identifier

- **Tile Destruction Mechanics:**
  - When tile is damaged below 0 HP, it's destroyed and removed from map
  - 45% chance to spawn a collectible (fossil piece or treasure)
  - Destroyed tiles create traversable empty spaces

- **Building Integration:**
  - Museum positioned at world coordinates (1600, 338) - 160×113 px
  - Trader positioned at world coordinates (25, 361) - 161×91 px
  - Buildings load from JSON configuration

**Collectible Types (12 total):**
- 6 fossil piece variants (different dinosaur types and pieces)
- Amber (bonus collectible)
- Trash variants

**Dependencies:**
- Fossil class data for metadata
- Textures loaded from ASSETS/IMAGES/Terrain/
- JSON configuration from map.json

---

### 3. **Player Character** ([Player.h](Player.h) / [Player.cpp](Player.cpp))
**Role:** Player-controlled excavator with upgradeable pickaxe and movement

**Properties:**
- **Position & Movement:**
  - WASD controls for left/right movement
  - W or Space to jump
  - Physics-based jumping with gravity (1200 units/sec²)
  - Jump force of -400 units/sec
  - Grounding detection for jump availability

- **Pickaxe System:**
  - Left-click to swing pickaxe at tiles
  - Circular collision detection (6 pixel radius at pickaxe tip)
  - Damage applied per swing (base damage + upgrades)
  - Cooldown between swings (prevents spam)
  - Visual pickaxe animation with frame direction

- **Animation:**
  - Sprite-based walk cycle (4 frames, 192×192 pixels each, scaled to 0.2x)
  - Frame cycling at ~100ms intervals
  - Direction-based sprite orientation

- **Inventory System:**
  - Tracks collected fossil pieces per dinosaur
  - Amber collectible counter
  - Trash collectible counter
  - Total money earned display (top-left HUD)
  - Visual pickup notifications

- **Pickup System:**
  - Automatic collection of nearby items
  - Pickup radius (default 24.0 units, upgradeable)
  - Items add to inventory and register as discovered

- **Upgradeable Stats (via Trader):**
  1. **Pickaxe Radius** - increases area of effect (cost: 100 + level×50 gold)
  2. **Tile Damage** - increases damage per swing (cost: 200 + level×75 gold)
  3. **Pickup Radius** - auto-collect from farther away (cost: 150 + level×60 gold)
  4. **Jump Height** - increases jump force (cost: 175 + level×70 gold)

**Input Controls:**
- `A` - Move left
- `D` - Move right
- `W` / `Space` - Jump
- `Left Mouse` - Swing pickaxe
- `ESC` - Pause
- `M` - Return to main menu
- `F3` - Toggle debug overlay (grid visualization, tile info)

**Dependencies:**
- Map (for collision and tile damage)
- Fossil data for inventory tracking

---

### 4. **Paleontologist (AI Worker)** ([Paleontologist.h](Paleontologist.h) / [Paleontologist.cpp](Paleontologist.cpp))

**Role:**  CURRENTLY NON-FUNCTIONAL - Intended to be autonomous hired worker that mines tiles and collects fossils

**Status:** Behavior tree and A* pathfinding systems not implemented/broken

**Intended Properties (not currently working):**
- **Position & Movement:**
  - Was intended: Autonomous tile-based movement (not continuous like player)
  - Was intended: Speed 60 units/second (configurable)
  - **A* pathfinding not working** - Navigation broken

- **AI Behavior System: NON-FUNCTIONAL**
  
  Originally designed to use state machine with 5 behavior states:
  1. **Idle Behavior:** Random pause (1-3 seconds), then transition to another state
  2. **Wandering Behavior:** Seeks random walkable tiles, explores the map
  3. **Searching For Fossil:** Was to use A* pathfinding to locate fossils - **NOT WORKING**
  4. **Mining Behavior:** Continuously damages tiles using pickaxe attack - **NOT WORKING**
  5. **Returning to Surface:** Pathfind back to top - **NOT WORKING**
  
  **Current Issue:** Behavior tree transitions do not execute properly; Paleontologists remain idle and do not autonomously perform mining or pathfinding tasks.

- **Mining Mechanics:** NOT WORKING - Paleontologists do not mine
- **Performance Optimization:** Code exists but unused due to non-functional behavior system

**Hiring System:**
- Players can hire multiple paleontologist instances via Trader (UI functional)
- Each instance is independently managed by unique_ptr
- **Paleontologists do NOT generate income or autonomously work** - they spawn but remain stationary/inactive

**Dependencies:**
- Map (for pathfinding and navigation)
- Behaviors.h (behavior state classes)
- Pathfinding.h (A* algorithm)

---

### 5. **Fossil & Collection System** ([Fossil.h](Fossil.h) / [Fossil.cpp](Fossil.cpp))

**Role:** Manages collectible fossil pieces and dinosaur metadata

**Data Structures:**

- **Collectible (Fossil Piece):**
  - Texture and sprite for rendering
  - Fossil ID (e.g., "Spinosaurus_Skull")
  - Dinosaur species name
  - Category (Carnivore/Herbivore/Pterosaur)
  - Grid position (spawned when tile destroyed)
  - Monetary value (fossils worth 40 gold each)
  - Visual pickup indicator when collected

- **Dinosaur Data (12 species):**
  - Scientific name and common name
  - Category classification
  - Background image texture (for museum display)
  - 4 individual fossil pieces that compose the skeleton
  - Display scale (large dinos like Dreadnoughtus scale down; small ones up)

**Supported Dinosaurs:**
- **Carnivores:** Tyrannosaurus Rex (4 pieces), Allosaurus, Spinosaurus
- **Herbivores:** Triceratops, Ankylosaurus, Dreadnoughtus, Therizinosaurus
- **Pterosaurs:** Pteranodon, Quetzalcoatlus

**Collection Tracking:**
- Inventory tracks which pieces collected per dinosaur (0-4 pieces per species)
- Collection completion percentage calculated
- Museum interior displays all collected pieces with species metadata

**Dependencies:**
- Textures from ASSETS/IMAGES/Fossils/
- JSON configuration from map.json for dinosaur definitions

---

### 6. **Museum Building & Gallery** ([Museum.h](Museum.h) / [Museum.cpp](Museum.cpp) / [MuseumInterior.h](MuseumInterior.h) / [MuseumInterior.cpp](MuseumInterior.cpp))

**Museum Building (exterior):**
- Fixed location at world coordinate (1600, 338)
- Sprite-based rendering (160×113 pixels)
- Click detection for interaction
- When clicked, switches to MuseumInterior display

**MuseumInterior (gallery view):**
- Displays all collected dinosaur specimens
- Shows 4 fossil pieces per dinosaur with images
- Background texture + optional "skin" overlay texture variation per dinosaur
- Navigation controls:
  - Left/Right arrow buttons to browse between dinosaurs
  - Back button to return to gameplay
- Scale adjustments for display (large dinos compress, small ones expand)
- Tracks collected vs. uncollected pieces
- Educational display of paleontological specimens

**Workflow:**
1. Player clicks Museum building in gameplay
2. MuseumInterior overlays on screen
3. Player browses collected fossils
4. Back button returns to gameplay

**Dependencies:**
- Fossil class for metadata and piece data
- Textures from ASSETS/IMAGES/Fossils/

---

### 7. **Trader Building & Upgrade Menu** ([Trader.h](Trader.h) / [Trader.cpp](Trader.cpp) / [TraderMenu.h](TraderMenu.h) / [TraderMenu.cpp](TraderMenu.cpp))

**Trader Building (exterior):**
- Fixed location at world coordinate (25, 361)
- Sprite-based rendering (161×91 pixels)
- Click detection for interaction
- When clicked, opens TraderMenu overlay

**TraderMenu (interactive UI):**
Overlay menu with two tabs:

**Tab 1: Hiring**
- **Hire Paleontologist Button:** 
  - Costs 500 gold per hire
  - Spawns new Paleontologist instance with unique pathfinding
  - Button disabled if insufficient funds
  - Creates autonomous income stream
- **Hire Researcher Button:**
  - Currently stub/empty (displays "Researcher hiring not yet implemented")
  - Placeholder for future feature

**Tab 2: Upgrades**
Four purchasable upgrade buttons with progressive costs:
1. **Pickaxe Radius Upgrade** - Increases pickaxe damage area
   - Cost: 100 + (current_level × 50) gold
2. **Tile Damage Upgrade** - Increases damage per pickaxe swing
   - Cost: 200 + (current_level × 75) gold
3. **Pickup Radius Upgrade** - Increases auto-collection distance
   - Cost: 150 + (current_level × 60) gold
4. **Jump Height Upgrade** - Increases jump force
   - Cost: 175 + (current_level × 70) gold

**Menu Features:**
- Drawn as overlay on top of gameplay (semi-transparent background)
- Close button returns to gameplay
- Dynamic button enabling/disabling based on player gold
- Current upgrade level display per button
- Real-time cost calculation

**Workflow:**
1. Player clicks Trader building in gameplay
2. TraderMenu overlays on screen (game pauses rendering but not logic)
3. Player spends gold to hire workers or upgrade abilities
4. Close button returns to gameplay

**Dependencies:**
- Player class (for money and upgrade tracking)
- Paleontologist class (for hiring)
- Game class (for state switching)

---

### 8. **User Interface Systems**

#### Menu System ([Menu.h](Menu.h) / [Menu.cpp](Menu.cpp))
**Main Menu Screen:**
- Start button (transitions to Gameplay state)
- Quit button (transitions to Exit state)
- Texture-based button rendering with mouse hover detection
- Click detection and state transition handling

#### Pause Menu ([Paused.h](Paused.h) / [Paused.cpp](Paused.cpp))
**Pause Screen:**
- Resume button (returns to Gameplay state)
- Settings button (transitions to Settings - currently stub)
- Quit button (returns to MainMenu state)
- Accessible via ESC key during gameplay
- Pauses game state and rendering

---

## Game Mechanics

### Mining & Tile Destruction
- Player left-clicks to swing pickaxe at tiles
- Pickaxe has circular collision detection (6px radius)
- Each swing applies damage to tile
- Tile destruction threshold based on hardness:
  - Topsoil: 8 HP
  - Sediment: 35 HP
  - Rock: 250 HP
  - Bedrock: 600 HP (effectively indestructible in normal gameplay)
- Visual crack progression shows tile damage as it accumulates
- When destroyed, 45% chance to spawn collectible

### Fossil Spawning & Collection
- Destroyed tiles spawn collectibles based on probability
- Fossils assigned random type from 12 collectible pool
- Collectibles auto-collect when player walks within pickup radius
- Money awarded immediately upon collection:
  - Fossil pieces: 40 gold each
  - Amber: variable value
  - Trash: 10 gold
- Inventory displays total gold earned and items collected

### AI Worker Economy  BROKEN
- Hire Paleontologists for 500 gold each (UI works, but functionality broken)
- **AI workers do NOT autonomously mine** - behavior tree not functional
- **A* pathfinding broken** - workers cannot navigate
- **Workers do NOT collect fossils** - spawned but remain idle
- **Passive income system non-functional** - player must earn all money manually
- Hiring interface exists but is cosmetic - workers don't perform intended tasks

### Upgrade System
- Progressive upgrade levels increase cost with each level
- Player selects which stat to upgrade at Trader
- Upgrades persist across gameplay session
- **Note:** Upgrades were designed to affect both player and AI workers, but only work for player:
  - Pickaxe radius increases player area of effect (AI upgrades non-functional)
  - Tile damage increases player damage (AI upgrades non-functional)
  - Pickup radius only affects player collection distance
  - Jump height only affects player movement

### Physics & Movement
- Gravity-based jumping (1200 units/sec² downward)
- Jump impulse of -400 units/sec (upward)
- Grounding detection prevents aerial jumping
- WASD movement with smooth acceleration/deceleration
- Collision detection prevents walking through terrain

### Camera System
- Follows player character with bounds clamping
- Zoomed-out view (0.5x scale) showing large map area
- Camera constrained within map boundaries
- Allows full map traversal

### Debug Mode
- F3 toggle activates debug overlay
- Shows tile grid visualization
- Displays hover tile information (coordinates, hardness, HP)
- Helps with development and balancing

---

## Configuration System

### map.json Structure
Centralized JSON configuration containing:

```json
{
  "map": {
    "layers": [...]          // 4 terrain layer definitions with hardness/HP values
  },
  "museum": {...},           // Museum building sprite/position data
  "trader": {...},           // Trader building sprite/position data
  "dinosaurs": [...]         // 12 dinosaur species with 4-piece fossil data each
}
```

**Data-Driven Approach:**
- All game content externalized to JSON
- No hardcoded values for assets, positions, or mechanics
- Easy to modify difficulty and balance without recompilation
- Supports future expansion of collectibles and dinosaur species

---

## Asset Structure

```
ASSETS/
├── IMAGES/
│   ├── Terrain/                    (4 layer textures)
│   ├── Sprites/
│   │   ├── Buildings/              (Museum, Trader sprites)
│   │   └── Characters/             (Player, Paleontologist animations)
│   ├── Fossils/                    (12+ collectible piece sprites)
│   │   ├── Carnivore/              (T-Rex, Allosaurus, Spinosaurus)
│   │   ├── Herbivore/              (Trike, Anki, Dread, Theri)
│   │   └── Pterosaur/              (Pteranodon, Quetzalcoatlus)
│   ├── Screens/                    (Menu backgrounds, UI elements)
│   └── Collectibles_Sheet.png      (All pickup items sprite sheet)
├── AUDIO/                          (Audio assets - not currently implemented)
├── FONTS/                          (Font assets - not currently implemented)
└── CONFIG/
    └── map.json                    (Master game configuration)
```

---

## Game Flow

### 1. **Initialization**
- main() creates Game instance
- Game loads map.json configuration
- Map generates 75×100 tile grid with 4 procedural terrain layers
- Paleontologist and Menu systems initialize with UI assets
- Main menu displays with Start and Quit buttons

### 2. **Main Menu State**
- Display start and quit buttons
- Wait for user click
- Click Start → transitions to Gameplay
- Click Quit → transitions to Exit (graceful shutdown)

### 3. **Gameplay State** (Core Loop)
- **Player Actions:**
  - WASD to move and explore
  - Left-click tiles to swing pickaxe and mine
  - Click Museum building to view fossil collection
  - Click Trader building to hire workers or upgrade abilities
  - ESC to pause game

- **Autonomous Systems:**
  - **❌Hired Paleontologists DO NOT work autonomously** - they spawn but remain inactive (behavior tree/pathfinding broken)
  - Map generates collectibles when tiles are destroyed (player must collect)
  - Camera follows player with view culling

- **Progression:**
  - Collect fossils and earn money
  -  Hire AI workers (interface available but they don't function)
  - Upgrade pickaxe and movement abilities
  - Grow collection of dinosaur specimens
  - **Note:** Player must do all mining manually; hired Paleontologists do not work

### 4. **Museum State** (Fossil Gallery)
- View all collected dinosaur specimens
- Browse 4-piece skeletons per species
- See collection completion percentage
- Educational display of paleontological discoveries
- Back button returns to gameplay

### 5. **Paused State**
- Pause overlay displays over gameplay
- Resume - returns to gameplay
- Settings - transitions to settings screen (currently stub)
- Quit - returns to main menu

### 6. **Exit State**
- Graceful shutdown of game loop
- Window closes cleanly

---

## Key Design Patterns

### 1. **State Machine**
- Game uses enum-based state management
- Each state (MainMenu, Gameplay, Paused, Exit) controls active rendering and input
- Clear state transitions without overlapping logic
- Easy to add new game modes

### 2. **Component-Based Architecture**
- Independent classes (Player, Paleontologist, Map, Museum, Trader) with clear responsibilities
- Game class composes and orchestrates systems
- Minimal coupling between components
- Easy to add/remove features without affecting others

### 3. **Data-Driven Design**
- Game content lives in JSON (not hardcoded)
- Config file defines terrain, buildings, collectibles, costs
- Easy to balance and modify without recompilation
- Supports future content expansion

### 4. **Behavior Tree (Modified State Machine)**  NON-FUNCTIONAL
- **Attempted design:** Paleontologists use 5 predefined behavior states with transitions
- **Status:** State transitions not executing; Paleontologists remain idle
- **Issue:** Behavior tree logic does not properly update character state
- **Impact:** Autonomous decision-making completely broken

### 5. **Pathfinding & AI**  NON-FUNCTIONAL
- **Attempted:** A* algorithm for intelligent AI navigation
- **Status:** Pathfinding algorithm not working; workers cannot navigate map
- **Issue:** Path calculation returns invalid or no valid paths
- **Impact:** Workers cannot move or seek fossils

### 6. **Frustum Culling**
- Camera view bounds tracked for performance
- Only visible paleontologists rendered
- Prevents framerate drop with many AI workers on large maps

### 7. **Memory Management**
- std::unique_ptr for dynamic paleontologist instances
- Clean destructors prevent memory leaks
- Smart pointer pattern for safe resource management

---

## Development Status

###   Fully Implemented & Playable Features
- Complete mining mechanics with tile damage and destruction
- Player-controlled excavation with pickaxe physics
- Physics system (gravity, jumping, collision detection)
- 4-layer procedural terrain generation
- Fossil spawning and collection system
- Inventory tracking and money economy
- Upgrade system with 4 purchasable abilities
- Museum interior fossil gallery display
- Trader building with upgrade and hiring menus
- Main menu and pause menu
- Camera system with view clamping
- Animation system (sprite frame cycling)
- Debug mode with grid visualization
- JSON configuration loading

###  Partially Implemented / Stub Features
- **Researcher Class:** Defined but empty; hiring button shows "not yet implemented"
- **Settings Menu:** Exists as game state but no UI or functionality
- **Audio System:** Assets folder prepared but no audio playback implemented
- **Trader Hiring UI:** Interface present, hiring buttons work, but hired workers don't function

###  Not Implemented / Broken Features
- Save/Load system
- Win condition or end-game goals
- Sound effects and background music
- Visual effects (particles, screen shake, etc.)
- Multiplayer or networking
- Tutorial or help system
- **Paleontologist AI with behavior tree** - BROKEN, workers remain idle
- **A* pathfinding for worker navigation** - BROKEN, pathfinding not functional
- **Autonomous worker income system** - BROKEN, workers do not perform tasks
---

## Technical Details

### Platform & Build
- **OS:** Windows-specific (Visual Studio 2019+ project)
- **Language:** C++17 or later
- **Build System:** Visual Studio (.vcxproj)
- **Debug Configuration:** Configured with SFML library linking

### Performance Metrics
- **Target FPS:** 60 (fixed timestep)
- **Resolution:** 1800×900 pixels
- **Map Size:** 75×100 tiles (1800×2400 world units at 24px per tile)
- **Max Characters:** 1 player + practical limit of 50-100 paleontologists (no enforced upper limit)

### Code Quality
- Clear separation of concerns via dedicated header/implementation files
- Forward declarations to reduce coupling
- Smart pointers (std::unique_ptr) for memory safety
- Consistent naming conventions and code organization
- ~5000+ lines of code across 15+ source files

### Key Implementation Details
- **Pickaxe Collision:** Circular collision detection (6px radius) at calculated tip position
- **AI Navigation:**  **BROKEN** - Pathfinding algorithm implementation incomplete; workers do not navigate or move
- **Tile HP Display:** Visual progress bar above paleontologist during mining (code present but unused since mining is broken)
- **Dinosaur Display Scaling:** Adjusts scale per species for museum view (large/small variety)
- **Fossil Spawning:** Weighted random selection from 12 collectible types
- **Money System:** Immediate gold reward on collectible pickup with HUD display (player-only, passive worker income broken)

---

## Game Summary

**PaleoPals** is a playable single-player mining simulator. Players dig through procedurally generated stratified terrain to uncover fossils, earn money and upgrade abilities. The game demonstrates solid software engineering practices including state machines, component-based architecture and data-driven content design. 

**Known Limitation:** The autonomous worker system (Paleontologist AI with behavior trees and A* pathfinding) is currently non-functional. Workers can be hired but remain idle and do not perform mining tasks. All fossil collection must be done manually by the player.

**Core Loop (Player-Only):** Dig → Collect → Earn → Upgrade → Repeat

The game is fully playable from a single-player excavation perspective, with meaningful progression through pickaxe upgrades and fossil collection. Extended gameplay is possible for dedicated players.
