# no-preserve-root

> A nursery simulation game built with C++17, demonstrating advanced object-oriented design patterns through an interactive terminal-based interface.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Design Patterns](#design-patterns)
- [Architecture](#architecture)
- [Installation](#installation)
- [Building the Project](#building-the-project)
- [Running the Game](#running-the-game)
- [Gameplay Guide](#gameplay-guide)
- [Testing](#testing)
- [Project Structure](#project-structure)
- [Documentation](#documentation)
- [Contributing](#contributing)

---

## Overview

A terminal-based nursery simulation game where you manage a plant nursery business. Players cultivate plants through various growth stages, serve customers, manage staff, and balance business metrics like money and reputation. The project serves as both an engaging game and a comprehensive demonstration of software design patterns in C++.

### Core Gameplay Loop

1. **Planting**: Select and plant seeds in user created plots
2. **Cultivation**: Monitor plant growth through states (Seedling → Growing → Mature)
3. **Care**: Gardeners water plants, fertilize withering plants and more to manage the plants life cycle
4. **Business**: Cashiers serve customers with purchase requests and plant recommendations
5. **Management**: Hire staff (Cashiers and Gardeners) to automate operations
6. **Persistence**: Save and load game progress using the Memento pattern

---

## Features

### Gameplay Features

- **17 Unique Plant Types**: Aloe, Bamboo, Basil, Cactus, Daisy, Fern, Ivy, Lavender, Marigold, Mint, Orchid, Petunia, Rose, SnakePlant, Succulent, Sunflower, Tulip
- **Dynamic Customer System**: Randomly generated customer requests (purchases and recommendations)
- **Staff Management**: Hire Cashiers (serve customers) and Gardeners (tend plants)
- **Seasonal Mechanics**: Plants have seasonal preferences affecting customer requests
- **Reputation System**: Build reputation by fulfilling customer requests successfully
- **Economic Simulation**: Manage money through sales, staff costs, and fertilizer expenses
- **Plant Decorators**: Gift wrap, pots, and ribbons increase sale value
- **State-Based Plant Lifecycle**: Seedling → Growing → Mature, Withering → Withered
- **Sales**: Mature plants are moved to storage for sale, where they are preserved indefinitely

### Technical Features

- **Save/Load System**: Persistent game state using JSON serialization
- **Observer Pattern**: Automated plant care through supervisor monitoring
- **Command Queue**: Asynchronous command execution with logging
- **Iterator Patterns**: Flexible inventory traversal (Pre-order, Level-order, Filtered)
- **Factory Pattern**: Dynamic plant creation with type registration
- **Builder Pattern**: Customer request specification construction
- **Chain of Responsibility**: Staff request handling and delegation
- **Terminal UI**: Full-featured TUI using cpp-terminal library
- **Memory Safety**: Smart pointers (shared_ptr, weak_ptr, unique_ptr) throughout
- **Comprehensive Testing**: 198+ unit tests using doctest framework

---

## Design Patterns

This project extensively implements the Gang of Four design patterns:

### Creational Patterns

| Pattern | Implementation | Purpose |
|---------|----------------|---------|
| **Factory** | `PlantFactory` hierarchy | Creates 17 different plant types dynamically |
| **Builder** | `PlantSpecificationBuilder` | Constructs customer requests with optional decorators |
| **Prototype** | `clone()` and `blueprintClone()` | Deep copying for save/load and group duplication (group duplication not currently implemented in the UI) |

### Structural Patterns

| Pattern | Implementation | Purpose |
|---------|----------------|---------|
| **Composite** | `InventoryComponent` hierarchy | Unified interface for plants and groups |
| **Decorator** | `PlantDecorator` (`GiftWrapDecorator`, `PotDecorator`, `RibbonDecorator`) | Adds features to plants without modifying base classes |

### Behavioral Patterns

| Pattern | Implementation | Purpose |
|---------|----------------|---------|
| **Observer** | `NurserySupervisor` observes `Plant` subjects | Automatic plant care command creation |
| **Command** | `Command`  (Water, Fertilize, Customer, Remove, Storage) | Encapsulates requests as objects for queuing and logging |
| **State** | `PlantState` hierarchy (Seedling, Growing, Mature, Withering, Withered) | Changes plant behavior based on current state |
| **Chain of Responsibility** | `Staff` chain | Request delegation through staff hierarchy |
| **Iterator** | `CompositeIterator`, `TraversalStrategy` | Multiple traversal algorithms for inventory |
| **Memento** | `Memento` with `Nursery` originator | Captures and restores complete game state |
| **Strategy** | `TraversalStrategy` (PreOrder, LevelOrder, Filtered) | Interchangeable traversal algorithms |

---

## Architecture

### High-Level Component Diagram

```
┌──────────────────────────────────────────────────────┐
│                        Nursery                       │
│  ┌────────────┐  ┌─────────────┐  ┌──────────────┐   │
│  │ Inventory  │  │ Staff Chain │  │ Command Queue│   │
│  └────────────┘  └─────────────┘  └──────────────┘   │
│  ┌────────────┐  ┌─────────────┐  ┌──────────────┐   │
│  │ Supervisor │  │ SaveSystem  │  │ Factories    │   │
│  └────────────┘  └─────────────┘  └──────────────┘   │
└──────────────────────────────────────────────────────┘
           │                   │                  │
    ┌──────▼──────┐    ┌──────▼──────┐   ┌──────▼──────┐
    │  Components │    │  Commands   │   │   States    │
    │  (Composite)│    │  (Command)  │   │   (State)   │
    └─────────────┘    └─────────────┘   └─────────────┘
```

### Key Subsystems

#### 1. **Inventory System** (Composite Pattern)
- **Root**: `Inventory` holds top-level groups and storage
- **Groups**: Organize plants into plots (owning) or views (non-owning)
- **Plants**: Individual plant components with state and properties
- **Auto-Move**: Adding to owning group automatically removes from previous owner

#### 2. **Command System** (Command + Chain of Responsibility)
- **Command Queue**: FIFO queue with smart processing (non-blocking)
- **Staff Chain**: Finds capable staff member with busy state management
- **Logging**: `LoggingCommand` wrapper tracks pending/completed status
- **Types**: Water, Fertilize, Remove, Customer fulfillment, Add to Storage

#### 3. **Observer System** (Observer Pattern)
- **Subject**: `Plant` notifies observers at the beginning of each day
- **Observer**: `NurserySupervisor` creates care commands automatically
- **Triggers**: Low water → Water command, Withering → Fertilize, Withered → Remove, Mature → Storage

#### 4. **State System** (State Pattern)
- **States**: Seedling, Growing, Mature, Withering, Withered
- **Transitions**: Time-based and care-based (watering, fertilizing)
- **Behavior**: Each state handles daily activities and state changes differently

#### 5. **Save/Load System** (Memento Pattern)
- **Format**: JSON serialization using nlohmann/json
- **Scope**: Complete game state (inventory, staff, commands, business metrics)
- **Validation**: File validation and error handling
- **Cloning**: Uses `clone()` for snapshot clones preserving IDs

---

## Installation

### Prerequisites

- **C++ Compiler**: g++ with C++17 support (g++ 7.0+ recommended)
- **Make**: CMake
- **Git**: For cloning the repository (with submodules)
- **Terminal**: ANSI-compatible terminal for UI rendering

### Clone Repository

```bash
git clone --recursive https://github.com/davidvrjn/no-preserve-root.git
cd no-preserve-root
```

> **Note**: The `--recursive` flag is important as it initializes the cpp-terminal submodule.

If you've already cloned without `--recursive`:

```bash
git submodule update --init --recursive
```

---

## Building the Project

### Quick Build

```bash
make
```

Can use
```bash
make -j8
```
for faster compilation


This compiles the project and creates the executable in `bin/main`.

### Build Targets

| Target | Command | Description |
|--------|---------|-------------|
| **Default** | `make` or `make all` | Compiles the project |
| **Run** | `make run` or `make r` | Compiles and runs the game |
| **Tests** | `make test` | Compiles and runs all unit tests |
| **Clean** | `make clean` or `make c` | Removes all build artifacts |
| **Format** | `make format` | Formats code using clang-format |
| **Lint** | `make clang-tidy` | Runs static analysis |

---

## Running the Game

### Start the Game

```bash
make run
# or
./bin/main
```

### Main Menu Navigation

Use **arrow keys** to navigate menus and **Enter** to select:

```
╔═══════════════════════════════════════╗
║          NURSERY SIMULATION           ║
╠═══════════════════════════════════════╣
║  ► New Game                           ║
║    Load Game                          ║
║    Quit                               ║
╚═══════════════════════════════════════╝
```

---

## Gameplay Guide

### 1. Starting a New Game

- Select **New Game** from the main menu
- You start with:
  - **R1000** money
  - **50/100** reputation
  - **1 Cashier** + **1 Gardener**
  - Empty inventory with Storage group

### 2. The Day Cycle

Each day consists of **5 steps** with breaks between:

```
Day Start 
→ Step 1 → Break 
→ Step 2 → Break 
→ Step 3 → Break 
→ Step 4 → Break 
→ Step 5 → Day End
```

**At each break you can:**
- Plant seeds in plots
- View inventory (all plants)
- View cultivating plants (Seedling/Growing only)

**At Day End you can additionally:**
- Save game progress
- Hire additional staff (R80/staff/day)

### 3. Planting Seeds

1. Select **Plant Seeds** from the menu
2. Choose a plant type from the catalogue
3. Enter a plot name (creates new plot if it doesn't exist)
4. Plant is added in **Seedling** state

### 4. Plant Growth Stages

```
Seedling (X days) → Growing (Y days) → Mature → [if neglected in Seedling or Growing] → Withering → Withered
                                         ↓
                                    [auto-moved to Storage when Mature]
```

- **Water Level**: Decreases daily, triggers watering at <50%
- **Age**: Increases daily, affects state transitions
- **Health**: Dictates withered and withering state

### 5. Customer Requests

Customers spawn randomly based on reputation:

#### Purchase Requests (65%)
- Customer wants a specific plant type
- May request decorators (Gift Wrap, Pot, Ribbon)
- **Success**: Gain money
- **Failure**: Lose reputation (-5)

#### Recommendation Requests (35%)
- Customer provides water requirement + season
- System samples 50% of inventory for matches
- **Success**: Gain reputation (+5)
- **Failure**: Lose reputation (-5)

#### Cashier are busy
- Customer leaves because they cannot be assisted
- Lose reputation (-5)

### 6. Staff Management

#### Cashier
- **Handles**: Customer requests
- **Cost**: R80/day
- **Status**: Busy after serving one customer per step

#### Gardener
- **Handles**: Water, Fertilize, Remove withered, Add to Storage commands
- **Cost**: R80/day
- **Status**: Busy after caring for one plant per step

### 7. Commands & Automation

The **Observer** pattern automatically creates commands:

| Trigger | Command | Effect |
|---------|---------| --------|
| Water < 50% | Water | Restores water to 100% |
| State = Withering | Fertilize | Sets health to 20, prevents Withered, -R30 |
| State = Withered | Remove | Removes dead plant from plot |
| State = Mature | Add to Storage | Moves mature plant to Storage group |

### 8. Business Metrics

#### Money
- **Income**: Selling plants to customers
- **Expenses**: Staff wages (R80/staff/day), Fertilizer (R30/use), Planting Seeds

#### Reputation (0-100)
- **Increases**: Fulfilling customer recommendation (+5)
- **Decreases**: Customer leaves without purchase (-3/customer)
- **Effect**: Higher reputation = more customers spawn

### 9. Saving & Loading

- **Save**: Available at Day End, stores complete game state
- **Load**: From main menu, select from available save files
- **Format**: JSON files in `saves/` directory
- **Validation**: Automatic validation before loading

---

## Testing

### Run All Tests

```bash
make test
```

### Test Framework

- **Framework**: [doctest](https://github.com/doctest/doctest) (single-header)
- **Test Count**: 198 test cases, 1133+ assertions
- **Coverage**: Unit tests for all major systems

### Test Categories

```
tests/
├── test_builder.cpp              # Builder pattern (PlantSpecification)
├── test_command.cpp              # Command pattern (Water, Fertilize, etc.)
├── test_decorator.cpp            # Decorator pattern (GiftWrap, Pot, Ribbon)
├── test_factory.cpp              # Factory pattern (all 17 plant types)
├── test_group_operations.cpp     # Composite pattern (Groups, ownership)
├── test_inventory.cpp            # Inventory operations
├── test_iterator.cpp             # Iterator patterns (traversals)
├── test_memento_savesystem.cpp   # Memento & SaveSystem
├── test_nursery.cpp              # Nursery facade & simulation
├── test_observer_attachment.cpp  # Observer pattern
├── test_plant.cpp                # Plant component
├── test_staff.cpp                # Chain of Responsibility
└── test_state.cpp                # State pattern (plant states)
└── etc...
```
---

## Basic Project Structure

```
no-preserve-root/
├── include/                    # All header files (.h)
│   ├── Actors/                 # Staff (Chain of Responsibility)
│   │   ├── Cashier.h
│   │   ├── Customer.h
│   │   ├── Gardener.h
│   │   └── Staff.h
│   ├── Components/             # Composite hierarchy
│   │   ├── Cactus.h
│   │   ├── Group.h
│   │   ├── InventoryComponent.h
│   │   ├── Plant.h
│   │   ├── PlantAttributes.h
│   │   └── Rose.h
│   ├── Core/                   # Core systems
│   │   ├── Client.h
│   │   ├── Inventory.h
│   │   ├── Nursery.h           # Main facade
│   │   └── SaveSystem.h
│   ├── Patterns/               # Design pattern implementations
│   │   ├── Builder/            # Builder pattern
│   │   ├── Command/            # Command pattern
│   │   ├── Decorator/          # Decorator pattern
│   │   ├── Factory/            # Factory pattern (17 plant factories)
│   │   ├── Iterator/           # Iterator & Strategy patterns
│   │   ├── Memento/            # Memento pattern
│   │   ├── Observer/           # Observer pattern
│   │   └── State/              # State pattern
│   └── Utils/                  # Utility classes
│       └── CommandLog.h
├── src/                        # All implementation files (.cpp)
│   ├── main.cpp                # Entry point & UI
│   ├── Actors/
│   ├── Components/
│   ├── Core/
│   └── Patterns/
├── tests/                      # Unit tests (doctest)
│   └── test_*.cpp
├── docs/                       # Documentation
│   └── HEADER_GUIDE.md         # Implementation contracts
├── cpp-terminal/               # Terminal UI library (submodule)
├── Report/                     # Project reports
├── saves/                      # Save game files (JSON)
├── makefile                    # Build system
├── .clang-format               # Code formatting rules
├── .clang-tidy                 # Static analysis config
└── README.md                   # This file
```

### Directory Conventions

- **Headers**: All `.h` files in `include/`
- **Source**: All `.cpp` files in `src/`
- **Tests**: All test files in `tests/`
- **Main**: Entry point in `src/main.cpp`

---

## Documentation

### Code Documentation

- **Doxygen-style comments** throughout codebase
- **Inline comments**: Explain complex algorithms and design decisions

### External Libraries

- **cpp-terminal**: Terminal manipulation library (submodule)
  - Repository: https://github.com/jupyter-xeus/cpp-terminal
  - Used for: Cross-platform terminal UI rendering and input handling

- **nlohmann/json**: JSON library (single header)
  - Repository: https://github.com/nlohmann/json
  - Used for: Save/load serialization

- **doctest**: Testing framework (single header)
  - Repository: https://github.com/doctest/doctest
  - Used for: Unit testing

---

## Authors

- **David van Rooijen** (Lead Developer) - [@davidvrjn](https://github.com/davidvrjn)
- **Stephan Kritzinger** - [@Stephan-Kritzinger](https://github.com/Stephan-Kritzinger)
- **Dandré Nel** - [@DandreNel7](https://github.com/DandreNel7)
- **Michael Koch** - [@MichaelKoch23](https://github.com/MichaelKoch23)
- **Sebastian Pienaar** - [@Sebastian-Pienaar](https://github.com/Sebastian-Pienaar)
- **Zipho Maduna** - [@ZiphoZ](https://github.com/ZiphoZ)
- **Caitanya Narain Singh** - [@u24603199](https://github.com/u24603199)
---

<div align="center">

</div>
