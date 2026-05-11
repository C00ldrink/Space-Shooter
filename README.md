# 🚀 Space Shooter — The Ultimate Galactic Showdown

> A C++ SFML-powered space shooter with waves of enemies, power-ups, boss fights, and a slick HUD. Buckle up, pilot. 👨‍🚀

---

## 📖 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Class Hierarchy (UML)](#-class-hierarchy-uml)
- [Core Managers](#-core-managers)
- [Game States](#-game-states)
- [Game Modes](#-game-modes)
- [Enemies](#-enemies)
- [Player Abilities](#-player-abilities)
- [Power-Ups](#-power-ups)
- [Controls](#-controls)
- [How to Build](#-how-to-build)

---

## 🌌 Overview

Space Shooter is a vertical scrolling shoot-em-up (shmup) written in **C++** using the **SFML** library. You pilot a spaceship, fight through waves of increasingly dangerous enemies, dodge asteroids, collect power-ups, and face terrifying bosses — all while chasing a high score. 🎯

---

## ✨ Features

- 🌊 **Wave-based progression** — enemies get harder every round
- 👾 **4 enemy types** — Drones, Vipers, Seekers, and 3 boss types
- 🔫 **3 weapon modes** — Standard, Spread, and Piercing
- ⚡ **EMP blasts** — nuke everything on screen
- 🛡️ **Shield system** — absorb hits before taking real damage
- 💨 **Dash mechanic** — blink out of danger with a cooldown
- ☄️ **Asteroids** — environmental hazards that block bullets too
- 🏆 **High score saving** — arcade and survival scores saved to file
- 🎵 **Full audio system** — different sounds for every action
- 🎨 **Animated explosions** — sprite-sheet based frame-by-frame animation

---

## 🗂️ Class Hierarchy (UML)

```
Entity
├── Bullet
├── PowerUp
├── Asteroid
└── ShootableCharacter
    ├── Player
    └── Enemy  (abstract)
        ├── Drone
        ├── Viper
        ├── Seeker
        ├── Cruiser              ← Boss 1
        ├── Turret
        ├── TwinCannon           ← Boss 2 (owns 2 Turrets)
        └── MotherShip           ← Boss 3 (spawns Seekers)
```

### 🔍 What each layer does

| Class | Responsibility |
|---|---|
| `Entity` | Base for everything — position, velocity, sprite, texture loading |
| `Bullet` | Projectile with damage and pierce flag |
| `ShootableCharacter` | Adds health, explosion animation, and a dynamic bullet array |
| `Player` | Input-driven ship with weapons, dash, shield, EMP, and scoring |
| `Enemy` | Abstract base — declares `shoot()`, `move()`, `update()`, `draw()` |
| `Drone` | Straight-line enemy, fires downward on a timer |
| `Viper` | Sine-wave movement, fires downward |
| `Seeker` | Locks on to player position at spawn and rockets toward them |
| `Cruiser` | Boss 1 — side-to-side movement, twin bullets + laser columns |
| `Turret` | Sub-entity of TwinCannon — rotates to aim at player |
| `TwinCannon` | Boss 2 — two turrets that die independently, then body attacks |
| `MotherShip` | Boss 3 — continuously spawns Seekers, fires spread + laser |

---

## ⚙️ Core Managers

### 🎮 `GameObject` — The Game Master
The central class that owns **everything**. It holds the player, all enemies, power-ups, asteroids, stars, the UI, audio, and high scores. It also drives the **game loop** by calling `startGame()` every frame.

Key responsibilities:
- Initializes and caches all textures at startup via `loadTextures()`
- Manages transitions between game states
- Delegates rendering, input, and logic to the right subsystem each frame
- Loads and saves high scores to `highscores.txt`

---

### 💥 `collisionsManager()` — The Referee
This free function handles **all collision detection** in one place. It runs through five phases every frame:

1. **Enemy bullets → Player** — checks every enemy's bullet list against the player's hitbox
2. **Player bullets → Enemies** — checks the player's bullet list against every enemy
3. **Enemy body → Player** — if an enemy rams the player, it's an instant kill
4. **Enemies that reach the bottom** — cleaned up and removed
5. **Power-ups & Asteroids** — collection and bullet-blocking logic

> 💡 Special cases like `TwinCannon`'s turrets and `MotherShip`'s minion Seekers are handled with `dynamic_cast` inside this function.

---

### 🎵 `AudioManager` — The DJ
Manages all game audio using raw arrays of `SoundBuffer*` and `Sound*`. It loads every sound file at construction and exposes clean named methods like:

- `playShoot()` / `playSpreadShoot()` / `playPierceShoot()`
- `playExplosion()` / `playDamage()`
- `playEmp()` — extra loud! 📣
- `playMenuMusic()` / `playGameplayMusic()` / `stopMusic()`
- `playBossSound()` — triggers when a boss wave spawns

---

### 🕹️ `InputManager()` — The Controller
A free function called every frame during gameplay. It handles:

- **WASD** movement with boundary clamping
- **Space** to shoot (with a keyframe cooldown so you can't spam)
- **E** to dash (only if moving, with a 3-second cooldown)
- **N** to fire an EMP blast (destroys all normal enemies, damages bosses)

---

### 🌊 `spawnWave()` — The Wave Factory
Creates a fresh batch of enemies based on the current wave number and game mode. In **Arcade mode**, specific waves (5, 10, 15) spawn bosses instead of regular enemies. In **Survival mode**, enemy counts just keep scaling up forever.

---

### 🖥️ `Screen` & `MainMenuScreen` — The UI Kit
`Screen` is a reusable menu class that renders 3 animated hover buttons and a title. It's used for:
- Main Menu
- Pause Menu
- Game Over Screen
- Win Screen

`MainMenuScreen` extends it to also display the best high scores for each mode below the buttons.

---

## 🎮 Game States

The game is controlled by a simple state machine using `GameState`:

```
Main_Menu  ──► Playing ──► Paused ──► Playing
                 │
                 ├──► Game_Over ──► Retry / Main_Menu
                 │
                 └──► Game_win  ──► Play Again / Main_Menu
```

| State | What's happening |
|---|---|
| `Main_Menu` | Menu displayed, music playing, no active gameplay |
| `Playing` | Full game loop running |
| `Paused` | Game frozen, pause menu drawn on top |
| `Game_Over` | Player died, game over screen shown, high score saved |
| `Game_win` | All 15 waves cleared in Arcade mode 🎉 |

---

## 🏟️ Game Modes

| Mode | Description |
|---|---|
| 🕹️ **Arcade** | 15 waves with 3 fixed boss encounters (waves 5, 10, 15). Has a win condition. |
| ♾️ **Survival** | Infinite waves, enemies scale up forever. No win — just survive as long as possible. |

---

## 👾 Enemies

| Enemy | Movement | Attack | HP | Score |
|---|---|---|---|---|
| 🤖 Drone | Straight down | Fires a bullet every 1.5s | 1 | 10 |
| 🐍 Viper | Sine-wave | Fires a bullet every 1.5s | 1 | 15 |
| 🎯 Seeker | Straight toward player (locked at spawn) | Ram | 1 | 20 |
| 🛸 Cruiser | Side to side | Twin bullets + laser columns | 20 | 50 |
| 🔧 TwinCannon | Stationary | Turrets aim & fire; body attacks after turrets die | 30 | 50 |
| 👑 MotherShip | Stationary | Spread fire + homing lasers + spawns Seekers | 40 | 50 |

---

## 🧑‍✈️ Player Abilities

| Ability | Key | Details |
|---|---|---|
| Move | WASD | Bounded to screen edges |
| Shoot | Space | Rate-limited via keyframe counter |
| Dash | E (while moving) | 150px blink, 3s cooldown, 0.5s invincibility |
| EMP | N | Destroys all weak enemies, deals 15 damage to bosses (max 3 charges) |
| Shield | Power-up | Absorbs 2 hits before taking real damage |

### 🔢 Score Multiplier
Kill enemies quickly back-to-back and your multiplier goes up:
- 1st kill within 3s → **2×**
- 2nd kill within 3s → **4×**
- Take damage → resets to **1×**

---

## ⚡ Power-Ups

Power-ups drop randomly when enemies are destroyed (5% chance for EMP, 15% for others).

| Power-Up | Effect |
|---|---|
| 🔱 Spread | Fires 3 bullets in a fan — great for crowds |
| ☄️ Pierce | Single powerful bullet that passes through 2 enemies |
| 🛡️ Shield | Grants 2 shield hits |
| 💥 EMP | Adds 1 EMP charge (max 3) |

---

## 🕹️ Controls

| Key | Action |
|---|---|
| `W A S D` | Move ship |
| `Space` | Shoot |
| `E` | Dash (in movement direction) |
| `N` | Fire EMP |
| `Esc` | Pause |

---

## 🔧 How to Build

### Requirements
- C++17 or later
- [SFML 2.x](https://www.sfml-dev.org/) installed
- A compiler like `g++` or MSVC

### Linux / macOS
```bash
g++ main.cpp -o SpaceShooter -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -std=c++17
./SpaceShooter
```

### Windows (with SFML linked)
```bash
g++ main.cpp -o SpaceShooter.exe -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -std=c++17
SpaceShooter.exe
```

> 📁 Make sure your `Images/`, `Audios/`, and `assets/fonts/` folders are in the correct location relative to the executable. On Windows they go next to the `.exe`; on other platforms they go in `../SpaceShooter/`.

---

## 🏆 High Scores

Scores are saved automatically to `highscores.txt` in the working directory. The file stores two integers — survival high score and arcade high score — separated by a space. They're loaded on startup and displayed on the main menu. 📄

---

*Made with ❤️, C++, and a concerning amount of `dynamic_cast`.*
