/**
 * @file main.cpp
 * @brief Nursery Management Game
 *
 * Main entry point for the nursery simulation game with terminal-based UI.
 * Uses cpp-terminal library for cross-platform terminal rendering and input handling.
 *
 */

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../include/Actors/Cashier.h"
#include "../include/Actors/Gardener.h"
#include "../include/Actors/Staff.h"
#include "../include/Components/Group.h"
#include "../include/Components/InventoryComponent.h"
#include "../include/Components/Plant.h"
#include "../include/Core/Inventory.h"
#include "../include/Core/InventoryView.h"
#include "../include/Core/Nursery.h"
#include "../include/Core/SaveSystem.h"
#include "../include/Patterns/Factory/PlantFactory.h"
#include "../include/Patterns/Iterator/FilteredTraversal.h"
#include "../include/Patterns/Iterator/Iterator.h"
#include "../include/Patterns/Iterator/PreOrderTraversal.h"
#include "../include/Patterns/Memento/Memento.h"
#include "../include/Patterns/State/Growing.h"
#include "../include/Patterns/State/Seedling.h"
#include "cpp-terminal/color.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

// Inventory UI persistent state (shared between render and input handlers)
static std::size_t g_inv_selectedGroup = 0;
static bool g_inv_inGroupView = false;
static std::size_t g_inv_selectedPlant = 0;
// Track previous in-group state so we can detect transitions and reset cursor on entry
static bool g_inv_prev_inGroupView = false;

// Plant Seeds UI persistent state
enum class PlantSeedsView {
    PLOT_LIST,      // Selecting plot or creating new plot
    POSITION_GRID,  // Selecting position 1-9 in selected plot
    PLANT_TYPE      // Selecting plant type to plant
};
static PlantSeedsView g_ps_view = PlantSeedsView::PLOT_LIST;
static std::size_t g_ps_selectedPlot = 0;
static std::size_t g_ps_selectedPosition = 0;  // 0-8 for positions 1-9
static std::size_t g_ps_selectedPlantType = 0;

// ============================================================================
// SCREEN ENUMERATION
// ============================================================================

/**
 * @enum Screen
 * @brief All possible screens in the UI
 */
enum class Screen {
    MAIN_MENU,           // New Game, Load Game, Exit
    GAME_DASHBOARD,      // Main game screen with actions menu
    INVENTORY_VIEW,      // Browse plants by groups
    CULTIVATING_PLANTS,  // View plants in Seedling/Growing state
    PLANT_SEEDS,         // Select and plant seeds
    HIRE_STAFF,          // Hire Gardener/Cashier (DAY_END only)
    SAVE_GAME,           // Save game dialog
    LOAD_GAME,           // Load game dialog
    NO_SAVES             // No save files found dialog
};

// ============================================================================
// FORWARD DECLARATIONS - Rendering Functions
// ============================================================================

void renderMainMenu(std::size_t selectedOption);
void renderGameDashboard(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption);
void renderInventoryView(const std::shared_ptr<Nursery>& nursery);
void renderCultivatingPlantsView(const std::shared_ptr<Nursery>& nursery);
void renderPlantSeedsMenu(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption);
void renderHireStaffMenu(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption);
void renderSaveGameScreen(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption,
                          const std::string& editBuffer);
void renderLoadGameScreen(std::size_t selectedOption);

// Helper to check for save files and new no-saves screen
bool hasSaveFiles();
void renderNoSavesScreen(std::size_t selectedOption);
Screen handleNoSavesInput(Term::Event& event, std::size_t& selectedOption);

// Input handlers for save/load
Screen handleSaveGameInput(Term::Event& event, std::size_t& selectedOption,
                           std::shared_ptr<Nursery>& nursery, std::string& editBuffer);
Screen handleLoadGameInput(Term::Event& event, std::size_t& selectedOption,
                           std::shared_ptr<Nursery>& nursery, std::string& editBuffer);

// Small text input helper (returns true if accepted; out set to value)
bool textInputPrompt(const std::string& title, const std::string& prompt,
                     const std::string& initial, std::string& out);

// ============================================================================
// FORWARD DECLARATIONS - Input Handlers
// ============================================================================

Screen handleMainMenuInput(Term::Event& event, std::size_t& selectedOption, bool& running,
                           std::shared_ptr<Nursery>& nursery);
Screen handleGameDashboardInput(Term::Event& event, std::size_t& selectedOption,
                                std::shared_ptr<Nursery>& nursery);
// Inventory input handler now needs access to the nursery so it can query inventory
Screen handleInventoryInput(Term::Event& event, std::shared_ptr<Nursery>& nursery);
Screen handleCultivatingPlantsInput(Term::Event& event, std::shared_ptr<Nursery>& nursery);
Screen handlePlantSeedsInput(Term::Event& event, std::size_t& selectedOption,
                             std::shared_ptr<Nursery>& nursery);
Screen handleHireStaffInput(Term::Event& event, std::size_t& selectedOption,
                            std::shared_ptr<Nursery>& nursery);
// Confirmation prompt
bool confirmPrompt(const std::string& title, const std::string& question);

// ============================================================================
// RENDERING FUNCTIONS (Stubs - to be implemented)
// ============================================================================

/**
 * @brief Renders the main menu screen
 * @param selectedOption Currently selected menu option (0-2)
 */
void renderMainMenu(std::size_t selectedOption) {
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);

    // Title banner
    Term::cout << Term::color_fg(Term::Color::Name::Green) << Term::style(Term::Style::Bold);
    Term::cout << "\n";
    Term::cout << "  ╔══════════════════════════════════════════════════════════╗\n";
    Term::cout << "  ║                                                          ║\n";
    Term::cout << "  ║                NURSERY MANAGEMENT GAME                   ║\n";
    Term::cout << "  ║                                                          ║\n";
    Term::cout << "  ╚══════════════════════════════════════════════════════════╝\n";
    Term::cout << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default);
    Term::cout << "\n\n";

    // Menu options with better color contrast
    const std::vector<std::string> menuOptions = {"Load Game", "New Game", "Exit"};

    for (std::size_t i = 0; i < menuOptions.size(); ++i) {
        Term::cout << "    ";

        if (i == selectedOption) {
            // Highlight selected option
            Term::cout << Term::color_fg(Term::Color::Name::Black)
                       << Term::color_bg(Term::Color::Name::White)
                       << Term::style(Term::Style::Bold);
            Term::cout << " > " << menuOptions[i] << " < ";
            Term::cout << Term::style(Term::Style::Reset)
                       << Term::color_fg(Term::Color::Name::Default)
                       << Term::color_bg(Term::Color::Name::Default);
        } else {
            Term::cout << "   " << menuOptions[i] << "   ";
        }

        Term::cout << "\n";
    }

    // Instructions
    Term::cout << "\n\n";
    Term::cout << Term::color_fg(Term::Color::Name::Gray);
    Term::cout << "  ─────────────────────────────────────────────────────────\n";
    Term::cout << "  Controls: ↑/↓ to navigate, Enter to select, Q to quit\n";
    Term::cout << "  ─────────────────────────────────────────────────────────\n";
    Term::cout << Term::color_fg(Term::Color::Name::Default);

    Term::cout << std::flush;
}

/**
 * @brief Renders the game dashboard (main game screen)
 * @param nursery The game state
 * @param selectedOption Currently selected action
 */
void renderGameDashboard(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption) {
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);

    // Header
    Term::cout << Term::color_fg(Term::Color::Name::Cyan) << Term::style(Term::Style::Bold);
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << "                     NURSERY DASHBOARD                    \n";
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default);
    Term::cout << "\n";

    // Game state display
    Term::cout << "  Day: " << Term::color_fg(Term::Color::Name::Yellow) << nursery->getCurrentDay()
               << Term::color_fg(Term::Color::Name::Default) << "\n";

    Term::cout << "  Season: " << Term::color_fg(Term::Color::Name::Green);
    switch (nursery->getCurrentSeason()) {
        case Season::SPRING:
            Term::cout << "Spring";
            break;
        case Season::SUMMER:
            Term::cout << "Summer";
            break;
        case Season::FALL:
            Term::cout << "Fall";
            break;
        case Season::WINTER:
            Term::cout << "Winter";
            break;
    }
    Term::cout << Term::color_fg(Term::Color::Name::Default) << "\n";

    Term::cout << "  Money: " << Term::color_fg(Term::Color::Name::Green) << "R"
               << nursery->getMoney() << Term::color_fg(Term::Color::Name::Default) << "\n";

    // Color-code reputation
    int reputation = nursery->getReputation();
    Term::Color::Name repColor = Term::Color::Name::Yellow;
    if (reputation >= 75)
        repColor = Term::Color::Name::Green;
    else if (reputation <= 25)
        repColor = Term::Color::Name::Red;

    Term::cout << "  Reputation: " << Term::color_fg(repColor) << reputation << "/100"
               << Term::color_fg(Term::Color::Name::Default) << "\n";

    Term::cout << "  Step: " << nursery->getCurrentStep() << "/5\n";

    // Show current phase
    Term::cout << "  Phase: ";
    GamePhase phase = nursery->getCurrentPhase();
    switch (phase) {
        case GamePhase::IDLE:
            Term::cout << Term::color_fg(Term::Color::Name::Gray) << "IDLE";
            break;
        case GamePhase::DAY_START:
            Term::cout << Term::color_fg(Term::Color::Name::Cyan) << "DAY START";
            break;
        case GamePhase::STEP_BREAK:
            Term::cout << Term::color_fg(Term::Color::Name::Yellow) << "STEP BREAK";
            break;
        case GamePhase::DAY_END:
            Term::cout << Term::color_fg(Term::Color::Name::Green) << "DAY END";
            break;
    }
    Term::cout << Term::color_fg(Term::Color::Name::Default) << "\n\n";

    // Actions menu (phase-aware)
    Term::cout << "  " << Term::color_fg(Term::Color::Name::Cyan)
               << "Actions:" << Term::color_fg(Term::Color::Name::Default) << "\n";
    Term::cout << "  ───────────────────────────────────────────────────────\n";

    // Build action menu based on current phase (reuse phase variable from above)
    std::vector<std::string> actions;

    if (phase == GamePhase::IDLE) {
        actions.push_back("Start New Day");
        actions.push_back("View Inventory");
        actions.push_back("View Cultivating Plants");
        actions.push_back("Return to Menu");
    } else if (phase == GamePhase::DAY_START || phase == GamePhase::STEP_BREAK) {
        actions.push_back("Advance Step");
        actions.push_back("Plant Seeds");
        actions.push_back("View Inventory");
        actions.push_back("View Cultivating Plants");
        actions.push_back("Return to Menu");
    } else if (phase == GamePhase::DAY_END) {
        actions.push_back("Start New Day");
        actions.push_back("Plant Seeds");
        actions.push_back("View Inventory");
        actions.push_back("View Cultivating Plants");
        if (nursery->canHire()) actions.push_back("Manage Staff");
        if (nursery->canSave()) actions.push_back("Save Game");
        actions.push_back("Return to Menu");
    }

    // Display actions with selection highlight
    for (std::size_t i = 0; i < actions.size(); ++i) {
        Term::cout << "    ";

        if (i == selectedOption) {
            Term::cout << Term::color_fg(Term::Color::Name::Black)
                       << Term::color_bg(Term::Color::Name::White)
                       << Term::style(Term::Style::Bold);
            Term::cout << " > " << actions[i] << " ";
            Term::cout << Term::style(Term::Style::Reset)
                       << Term::color_fg(Term::Color::Name::Default)
                       << Term::color_bg(Term::Color::Name::Default);
        } else {
            Term::cout << "   " << actions[i];
        }

        Term::cout << "\n";
    }

    Term::cout << "\n";
    Term::cout << Term::color_fg(Term::Color::Name::Gray);
    Term::cout << "  ───────────────────────────────────────────────────────\n";

    // Show step info only during STEP_BREAK or DAY_END
    if (phase == GamePhase::STEP_BREAK || phase == GamePhase::DAY_END) {
        // Show customer spawn info
        int customersSpawned = nursery->getCustomersSpawnedThisStep();
        if (customersSpawned > 0) {
            Term::cout << "  Customers spawned this step: ";
            Term::cout << Term::color_fg(Term::Color::Name::Cyan) << customersSpawned
                       << Term::color_fg(Term::Color::Name::Gray) << "\n";
        } else {
            Term::cout << "  No customers spawned this step\n";
        }

        // Show command activity for this step
        Term::cout << "  Commands completed this step: ";
        auto completed = nursery->getCompletedCommandsThisStep();
        if (completed.empty()) {
            Term::cout << "(none)\n";
        } else {
            Term::cout << "\n";
            for (const auto& c : completed) {
                Term::cout << "    - " << c << "\n";
            }
        }

        Term::cout << "  Commands remaining in queue: ";
        auto remaining = nursery->getRemainingCommandsAtStepEnd();
        if (remaining.empty()) {
            Term::cout << "(none)\n";
        } else {
            Term::cout << "\n";
            for (const auto& r : remaining) {
                Term::cout << "    - " << r << "\n";
            }
        }

        Term::cout << "  Customers left this step: ";
        Term::cout << Term::color_fg(Term::Color::Name::Red) << nursery->getCustomersLeftThisStep()
                   << Term::color_fg(Term::Color::Name::Default) << "\n";
        Term::cout << "  ───────────────────────────────────────────────────────\n";
    }

    Term::cout << "  ESC: Return to menu | Q: Quit\n";
    Term::cout << "  ───────────────────────────────────────────────────────\n";
    Term::cout << Term::color_fg(Term::Color::Name::Default);

    Term::cout << std::flush;
}

/**
 * @brief Renders the inventory view screen
 * @param nursery The game state
 *
 * TODO: Implement inventory browser:
 * - List groups (Storage, Growing, Mature, Withering)
 * - Show plants in each group
 * - Display plant details
 */
void renderInventoryView(const std::shared_ptr<Nursery>& nursery) {
    using namespace UI;

    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
    Term::cout << "=== INVENTORY ===\n\n";

    if (!nursery) {
        Term::cout << "No game in progress. Press ESC to return.\n";
        Term::cout << std::flush;
        return;
    }

    // Use shared UI state so selection persists between render and input
    auto selectedGroup = &g_inv_selectedGroup;
    auto inGroupView = &g_inv_inGroupView;
    auto selectedPlant = &g_inv_selectedPlant;

    UI::InventoryView view(nursery->getInventory());
    auto groups = view.listGroupNames();

    Term::cout << "Groups:\n";
    // We'll show an extra "Return to Dashboard" option at the end
    std::size_t maxIndex = groups.size();  // last selectable index is this (return)
    if (groups.empty()) {
        Term::cout << "  (no groups found)\n";
    } else {
        for (std::size_t i = 0; i < groups.size(); ++i) {
            Term::cout << "  ";
            if (i == *selectedGroup && !*inGroupView) {
                Term::cout << Term::color_fg(Term::Color::Name::Black)
                           << Term::color_bg(Term::Color::Name::White)
                           << Term::style(Term::Style::Bold);
                Term::cout << " > " << groups[i] << " ";
                Term::cout << Term::style(Term::Style::Reset)
                           << Term::color_fg(Term::Color::Name::Default)
                           << Term::color_bg(Term::Color::Name::Default);
            } else {
                Term::cout << "   " << groups[i];
            }
            Term::cout << "\n";
        }
    }

    // Render the explicit return item
    Term::cout << "  ";
    if (maxIndex == *selectedGroup && !*inGroupView) {
        Term::cout << Term::color_fg(Term::Color::Name::Black)
                   << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                   << " > Return to Dashboard " << Term::style(Term::Style::Reset)
                   << Term::color_fg(Term::Color::Name::Default)
                   << Term::color_bg(Term::Color::Name::Default) << "\n";
    } else {
        Term::cout << "   Return to Dashboard\n";
    }
    Term::cout << "\n";

    // If we've just entered group view, reset the plant cursor to the top
    if (*inGroupView && !g_inv_prev_inGroupView) {
        *selectedPlant = 0;
        g_inv_prev_inGroupView = true;
    }

    if (*inGroupView && !groups.empty()) {
        std::string gname = groups[*selectedGroup];
        Term::cout << Term::style(Term::Style::Bold) << "Plants in " << gname << ":\n"
                   << Term::style(Term::Style::Reset);
        auto plants = view.listPlantsInGroup(gname);
        if (plants.empty()) {
            Term::cout << "  (no plants in this group)\n";
        } else {
            for (std::size_t i = 0; i < plants.size(); ++i) {
                Term::cout << "  ";
                if (i == *selectedPlant) {
                    Term::cout << Term::color_fg(Term::Color::Name::Black)
                               << Term::color_bg(Term::Color::Name::White)
                               << Term::style(Term::Style::Bold);
                    Term::cout << " > ";
                } else {
                    Term::cout << "   ";
                }
                // Display plant info - if name and type are the same, just show type
                if (!gname.compare("Storage")) {
                    if (plants[i].name == plants[i].type) {
                        Term::cout << plants[i].type;
                    } else {
                        Term::cout << plants[i].name << " (" << plants[i].type << ")";
                    }
                    if (i == *selectedPlant) {
                        Term::cout << Term::style(Term::Style::Reset)
                                   << Term::color_fg(Term::Color::Name::Default)
                                   << Term::color_bg(Term::Color::Name::Default);
                    }
                } else {
                    if (plants[i].name == plants[i].type) {
                        Term::cout << plants[i].type << " - " << plants[i].state;
                    } else {
                        Term::cout << plants[i].name << " (" << plants[i].type << ")" << " - "
                                   << plants[i].state;
                    }
                    if (i == *selectedPlant) {
                        Term::cout << Term::style(Term::Style::Reset)
                                   << Term::color_fg(Term::Color::Name::Default)
                                   << Term::color_bg(Term::Color::Name::Default);
                    }
                }
                Term::cout << "\n";
            }
        }
        Term::cout << "\n";
    }

    Term::cout
        << "Controls: ↑/↓ to navigate, Enter to open group/select plant, ESC or Q to go back\n";
    Term::cout << std::flush;
}

/**
 * @brief Renders the cultivating plants view
 * @param nursery The game state
 *
 * Shows all plants in Seedling or Growing state that are in plots (not Storage).
 * Uses FilteredTraversal to iterate through all plants in plots.
 */
void renderCultivatingPlantsView(const std::shared_ptr<Nursery>& nursery) {
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);

    // Header
    Term::cout << Term::color_fg(Term::Color::Name::Cyan) << Term::style(Term::Style::Bold);
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << "                CULTIVATING PLANTS                       \n";
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default);
    Term::cout << "\n";

    if (!nursery) {
        Term::cout << "No game in progress. Press ESC to return.\n";
        Term::cout << std::flush;
        return;
    }

    auto inventory = nursery->getInventory();
    if (!inventory) {
        Term::cout << "No inventory found. Press ESC to return.\n";
        Term::cout << std::flush;
        return;
    }

    // Get all groups (plots) except Storage
    auto allGroups = inventory->getAllGroups();
    std::vector<std::shared_ptr<Group>> plots;

    for (auto& group : allGroups) {
        if (group && group->getName() != "Storage" && group->getName() != "InventoryRoot" &&
            group->owns()) {
            plots.push_back(group);
        }
    }

    // Sort plots alphabetically
    std::sort(plots.begin(), plots.end(),
              [](const auto& a, const auto& b) { return a->getName() < b->getName(); });

    // Collect all cultivating plants (Seedling or Growing state) from all plots
    struct PlantInfo {
        std::string plotName;
        std::string plantName;
        std::string plantType;
        std::string state;
        int age;
        int waterLevel;
    };
    std::vector<PlantInfo> cultivatingPlants;

    for (auto& plot : plots) {
        // Create a filtered iterator for this plot
        auto filter = [](const std::shared_ptr<InventoryComponent>& comp) {
            auto plant = std::dynamic_pointer_cast<Plant>(comp);
            if (!plant) return false;

            auto state = plant->getState();
            if (!state) return false;

            // Check if in Seedling or Growing state
            return (dynamic_cast<Seedling*>(state) != nullptr ||
                    dynamic_cast<Growing*>(state) != nullptr);
        };

        auto filteredStrategy =
            std::make_unique<FilteredTraversal>(std::make_unique<PreOrderTraversal>(), filter);

        auto iterator = plot->createIterator(std::move(filteredStrategy));

        while (iterator->hasNext()) {
            auto comp = iterator->next();
            auto plant = std::dynamic_pointer_cast<Plant>(comp);
            if (plant) {
                PlantInfo info;
                info.plotName = plot->getName();
                info.plantName = plant->getName();
                info.plantType = plant->typeName();
                info.age = plant->getAge();
                info.waterLevel = plant->getWaterLevel();

                auto state = plant->getState();
                if (dynamic_cast<Seedling*>(state)) {
                    info.state = "Seedling";
                } else if (dynamic_cast<Growing*>(state)) {
                    info.state = "Growing";
                } else {
                    info.state = "Unknown";
                }

                cultivatingPlants.push_back(info);
            }
        }
    }

    // Display results
    if (cultivatingPlants.empty()) {
        Term::cout << "  " << Term::color_fg(Term::Color::Name::Yellow)
                   << "No plants are currently being cultivated."
                   << Term::color_fg(Term::Color::Name::Default) << "\n";
        Term::cout << "  Plant some seeds to get started!\n\n";
    } else {
        Term::cout << "  Found " << Term::color_fg(Term::Color::Name::Green)
                   << cultivatingPlants.size() << Term::color_fg(Term::Color::Name::Default)
                   << " cultivating plant(s):\n\n";

        // Group by plot for organized display
        std::string currentPlot = "";
        for (const auto& info : cultivatingPlants) {
            if (info.plotName != currentPlot) {
                if (!currentPlot.empty()) Term::cout << "\n";
                currentPlot = info.plotName;
                Term::cout << "  " << Term::style(Term::Style::Bold)
                           << Term::color_fg(Term::Color::Name::Cyan) << currentPlot << ":"
                           << Term::style(Term::Style::Reset)
                           << Term::color_fg(Term::Color::Name::Default) << "\n";
            }

            Term::cout << "    • ";

            // Display plant info
            if (info.plantName == info.plantType) {
                Term::cout << info.plantType;
            } else {
                Term::cout << info.plantName << " (" << info.plantType << ")";
            }

            Term::cout << " - ";

            // Color-code state
            if (info.state == "Seedling") {
                Term::cout << Term::color_fg(Term::Color::Name::Yellow) << info.state;
            } else if (info.state == "Growing") {
                Term::cout << Term::color_fg(Term::Color::Name::Green) << info.state;
            } else {
                Term::cout << info.state;
            }
            Term::cout << Term::color_fg(Term::Color::Name::Default);

            Term::cout << " | Age: " << info.age << " days";
            Term::cout << " | Water: ";

            // Color-code water level
            if (info.waterLevel < 30) {
                Term::cout << Term::color_fg(Term::Color::Name::Red);
            } else if (info.waterLevel < 60) {
                Term::cout << Term::color_fg(Term::Color::Name::Yellow);
            } else {
                Term::cout << Term::color_fg(Term::Color::Name::Green);
            }
            Term::cout << info.waterLevel << "%" << Term::color_fg(Term::Color::Name::Default);

            Term::cout << "\n";
        }
        Term::cout << "\n";
    }

    Term::cout << Term::color_fg(Term::Color::Name::Gray);
    Term::cout << "  ───────────────────────────────────────────────────────\n";
    Term::cout << "  ESC or Q: Return to dashboard\n";
    Term::cout << "  ───────────────────────────────────────────────────────\n";
    Term::cout << Term::color_fg(Term::Color::Name::Default);

    Term::cout << std::flush;
}

/**
 * @brief Renders the plant seeds menu
 * @param nursery The game state
 * @param selectedOption Currently selected option in current view
 *
 * Three-stage planting flow:
 * 1. PLOT_LIST: Select plot or create new plot
 * 2. POSITION_GRID: Select position 1-9 in plot
 * 3. PLANT_TYPE: Select plant type to plant at position
 */
void renderPlantSeedsMenu(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption) {
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);

    // Header
    Term::cout << Term::color_fg(Term::Color::Name::Cyan) << Term::style(Term::Style::Bold);
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << "                     PLANT SEEDS                         \n";
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default);
    Term::cout << "\n";

    Term::cout << "  Money: " << Term::color_fg(Term::Color::Name::Green) << "R"
               << nursery->getMoney() << Term::color_fg(Term::Color::Name::Default) << "\n\n";

    if (g_ps_view == PlantSeedsView::PLOT_LIST) {
        // Stage 1: Plot selection
        Term::cout << "  " << Term::color_fg(Term::Color::Name::Cyan)
                   << "Select a Plot:" << Term::color_fg(Term::Color::Name::Default) << "\n";
        Term::cout << "  ───────────────────────────────────────────────────────\n";

        // Get all groups, filter for plots (non-Storage owning groups)
        auto inventory = nursery->getInventory();
        std::vector<std::shared_ptr<Group>> plots;

        if (inventory) {
            auto allGroups = inventory->getAllGroups();
            for (auto& group : allGroups) {
                if (group && group->getName() != "Storage" && group->getName() != "InventoryRoot" &&
                    group->owns()) {
                    plots.push_back(group);
                }
            }
        }
        // Sort plots alphabetically by name for consistent ordering
        std::sort(plots.begin(), plots.end(),
                  [](const auto& a, const auto& b) { return a->getName() < b->getName(); });

        // Display plots + create new + return options
        for (std::size_t i = 0; i < plots.size(); ++i) {
            auto members = plots[i]->members();
            int plantCount = 0;
            for (auto& m : members) {
                if (dynamic_cast<Plant*>(m.get())) plantCount++;
            }

            Term::cout << "    ";
            if (i == selectedOption) {
                Term::cout << Term::color_fg(Term::Color::Name::Black)
                           << Term::color_bg(Term::Color::Name::White)
                           << Term::style(Term::Style::Bold) << " > " << plots[i]->getName() << " ("
                           << plantCount << "/9 plants) " << Term::style(Term::Style::Reset)
                           << Term::color_fg(Term::Color::Name::Default)
                           << Term::color_bg(Term::Color::Name::Default);
            } else {
                Term::cout << "   " << plots[i]->getName() << " (" << plantCount << "/9 plants)";
            }
            Term::cout << "\n";
        }

        // Create New Plot option
        Term::cout << "    ";
        if (selectedOption == plots.size()) {
            Term::cout << Term::color_fg(Term::Color::Name::Black)
                       << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                       << " > Create New Plot " << Term::style(Term::Style::Reset)
                       << Term::color_fg(Term::Color::Name::Default)
                       << Term::color_bg(Term::Color::Name::Default);
        } else {
            Term::cout << "   Create New Plot";
        }
        Term::cout << "\n";

        // Return option
        Term::cout << "    ";
        if (selectedOption == plots.size() + 1) {
            Term::cout << Term::color_fg(Term::Color::Name::Black)
                       << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                       << " > Return to Dashboard " << Term::style(Term::Style::Reset)
                       << Term::color_fg(Term::Color::Name::Default)
                       << Term::color_bg(Term::Color::Name::Default);
        } else {
            Term::cout << "   Return to Dashboard";
        }
        Term::cout << "\n\n";

        Term::cout << Term::color_fg(Term::Color::Name::Gray);
        Term::cout << "  ↑/↓: Navigate | Enter: Select | ESC: Return\n";
        Term::cout << Term::color_fg(Term::Color::Name::Default);
    } else if (g_ps_view == PlantSeedsView::POSITION_GRID) {
        // Stage 2: Position selection (1-9 grid)
        auto inventory = nursery->getInventory();
        std::vector<std::shared_ptr<Group>> plots;

        if (inventory) {
            auto allGroups = inventory->getAllGroups();
            for (auto& group : allGroups) {
                if (group && group->getName() != "Storage" && group->getName() != "InventoryRoot" &&
                    group->owns()) {
                    plots.push_back(group);
                }
            }
        }
        // Keep plots sorted alphabetically to match the plot list
        std::sort(plots.begin(), plots.end(),
                  [](const auto& a, const auto& b) { return a->getName() < b->getName(); });

        if (g_ps_selectedPlot < plots.size()) {
            auto selectedPlot = plots[g_ps_selectedPlot];
            auto members = selectedPlot->members();

            // Build occupancy map (position -> plant name)
            std::map<int, std::string> occupied;
            int pos = 0;
            for (auto& m : members) {
                auto plant = std::dynamic_pointer_cast<Plant>(m);
                if (plant && pos < 9) {
                    // Assume plants have position metadata or use sequential assignment
                    // For now, just mark as occupied sequentially
                    occupied[pos] = plant->getName();
                    pos++;
                }
            }

            Term::cout << "  Plot: " << Term::color_fg(Term::Color::Name::Cyan)
                       << selectedPlot->getName() << Term::color_fg(Term::Color::Name::Default)
                       << "\n";
            Term::cout << "  ───────────────────────────────────────────────────────\n";
            Term::cout << "  Select position (1-9):\n\n";

            // Display 3x3 grid
            for (int row = 0; row < 3; ++row) {
                Term::cout << "    ";
                for (int col = 0; col < 3; ++col) {
                    int pos = row * 3 + col;
                    bool isOccupied = occupied.find(pos) != occupied.end();
                    bool isSelected = (pos == static_cast<int>(selectedOption));

                    if (isSelected) {
                        Term::cout << Term::color_fg(Term::Color::Name::Black)
                                   << Term::color_bg(Term::Color::Name::White)
                                   << Term::style(Term::Style::Bold);
                    }

                    Term::cout << "[ ";
                    if (isOccupied) {
                        Term::cout << "X";  // Occupied
                    } else {
                        Term::cout << (pos + 1);  // Position number
                    }
                    Term::cout << " ]";

                    if (isSelected) {
                        Term::cout << Term::style(Term::Style::Reset)
                                   << Term::color_fg(Term::Color::Name::Default)
                                   << Term::color_bg(Term::Color::Name::Default);
                    }
                    Term::cout << " ";
                }
                Term::cout << "\n";
            }

            Term::cout << "\n";

            if (selectedOption < 9) {
                bool isOccupied = occupied.find(selectedOption) != occupied.end();
                if (isOccupied) {
                    Term::cout << "  Position " << (selectedOption + 1) << ": "
                               << Term::color_fg(Term::Color::Name::Yellow) << "OCCUPIED"
                               << Term::color_fg(Term::Color::Name::Default) << " ("
                               << occupied[selectedOption] << ")\n";
                } else {
                    Term::cout << "  Position " << (selectedOption + 1) << ": "
                               << Term::color_fg(Term::Color::Name::Green) << "AVAILABLE"
                               << Term::color_fg(Term::Color::Name::Default) << "\n";
                }
            }

            Term::cout << "\n";
            Term::cout << Term::color_fg(Term::Color::Name::Gray);
            Term::cout
                << "  ↑/↓/←/→: Navigate | Enter: Select empty position | ESC or Q: Back to plots\n";
            Term::cout << Term::color_fg(Term::Color::Name::Default);
        }
    } else if (g_ps_view == PlantSeedsView::PLANT_TYPE) {
        // Stage 3: Plant type selection
        Term::cout << "  " << Term::color_fg(Term::Color::Name::Cyan)
                   << "Select Plant Type:" << Term::color_fg(Term::Color::Name::Default) << "\n";
        Term::cout << "  ───────────────────────────────────────────────────────\n";

        // Get available plant types from factories
        std::vector<std::pair<std::string, double>> plantTypes;

        // Get actual factories and costs from nursery
        const auto& factories = nursery->getPlantFactories();
        for (const auto& pair : factories) {
            plantTypes.push_back({pair.first, pair.second->getSeedCost()});
        }

        // Sort alphabetically for consistency
        std::sort(plantTypes.begin(), plantTypes.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });

        // Display plant types
        std::size_t maxIndex = plantTypes.size();  // Last option is "Cancel"

        for (std::size_t i = 0; i < plantTypes.size(); ++i) {
            bool canAfford = nursery->getMoney() >= plantTypes[i].second;

            Term::cout << "    ";
            if (i == selectedOption) {
                Term::cout << Term::color_fg(Term::Color::Name::Black)
                           << Term::color_bg(Term::Color::Name::White)
                           << Term::style(Term::Style::Bold) << " > " << plantTypes[i].first
                           << " (R" << static_cast<int>(plantTypes[i].second) << ") ";

                if (!canAfford) {
                    Term::cout << "[Can't afford]";
                }

                Term::cout << " " << Term::style(Term::Style::Reset)
                           << Term::color_fg(Term::Color::Name::Default)
                           << Term::color_bg(Term::Color::Name::Default);
            } else {
                Term::cout << "   " << plantTypes[i].first << " (R"
                           << static_cast<int>(plantTypes[i].second) << ")";

                if (!canAfford) {
                    Term::cout << " " << Term::color_fg(Term::Color::Name::Gray) << "[Can't afford]"
                               << Term::color_fg(Term::Color::Name::Default);
                }
            }
            Term::cout << "\n";
        }

        // Cancel option
        Term::cout << "    ";
        if (selectedOption == maxIndex) {
            Term::cout << Term::color_fg(Term::Color::Name::Black)
                       << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                       << " > Cancel " << Term::style(Term::Style::Reset)
                       << Term::color_fg(Term::Color::Name::Default)
                       << Term::color_bg(Term::Color::Name::Default);
        } else {
            Term::cout << "   Cancel";
        }
        Term::cout << "\n\n";

        Term::cout << Term::color_fg(Term::Color::Name::Gray);
        Term::cout << "  ↑/↓: Navigate | Enter: Plant seed | ESC: Back to grid\n";
        Term::cout << Term::color_fg(Term::Color::Name::Default);
    }

    Term::cout << std::flush;
}

/**
 * @brief Renders the hire staff menu
 * @param nursery The game state
 * @param selectedOption Currently selected staff type
 *
 * Shows available staff types with current counts and daily costs.
 * Each staff member costs R80 per day.
 */
void renderHireStaffMenu(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption) {
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);

    // Header
    Term::cout << Term::color_fg(Term::Color::Name::Cyan) << Term::style(Term::Style::Bold);
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << "                    MANAGE STAFF                         \n";
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default);
    Term::cout << "\n";

    // Count current staff
    int cashierCount = 0;
    int gardenerCount = 0;
    auto head = nursery->getStaffChainHead();
    while (head) {
        if (dynamic_cast<Cashier*>(head.get()))
            cashierCount++;
        else if (dynamic_cast<Gardener*>(head.get()))
            gardenerCount++;
        head = head->getSuccessor();
    }
    int totalStaff = cashierCount + gardenerCount;
    const int costperStaff = 80;
    int dailyCost = totalStaff * costperStaff;

    // Display current state
    Term::cout << "  Money: " << Term::color_fg(Term::Color::Name::Green) << "R"
               << nursery->getMoney() << Term::color_fg(Term::Color::Name::Default) << "\n\n";

    Term::cout << "  Current Staff:\n";
    Term::cout << "  ───────────────────────────────────────────────────────\n";
    Term::cout << "    Cashiers:  " << cashierCount << "\n";
    Term::cout << "    Gardeners: " << gardenerCount << "\n";
    Term::cout << "    Total:     " << totalStaff << "\n";
    Term::cout << "    Daily Cost: " << Term::color_fg(Term::Color::Name::Yellow) << "R"
               << dailyCost << Term::color_fg(Term::Color::Name::Default) << "\n\n";

    // Hiring menu
    Term::cout << "  " << Term::color_fg(Term::Color::Name::Cyan)
               << "Manage Staff:" << Term::color_fg(Term::Color::Name::Default) << "\n";
    Term::cout << "  ───────────────────────────────────────────────────────\n";

    std::vector<std::string> options = {"Hire Cashier", "Hire Gardener", "Fire Cashier",
                                        "Fire Gardener", "Return to Dashboard"};

    for (std::size_t i = 0; i < options.size(); ++i) {
        Term::cout << "    ";

        if (i == selectedOption) {
            Term::cout << Term::color_fg(Term::Color::Name::Black)
                       << Term::color_bg(Term::Color::Name::White)
                       << Term::style(Term::Style::Bold);
            Term::cout << " > " << options[i];

            // Show cost for hire options
            if (i < 2) {
                Term::cout << " (R" << costperStaff << "/day)";
            }
            // Show availability for fire options
            else if (i == 2 && cashierCount == 0) {
                Term::cout << " (none to fire)";
            } else if (i == 3 && gardenerCount == 0) {
                Term::cout << " (none to fire)";
            }

            Term::cout << " ";
            Term::cout << Term::style(Term::Style::Reset)
                       << Term::color_fg(Term::Color::Name::Default)
                       << Term::color_bg(Term::Color::Name::Default);
        } else {
            Term::cout << "   " << options[i];
            if (i < 2) {
                Term::cout << " (R" << costperStaff << "/day)";
            } else if (i == 2 && cashierCount == 0) {
                Term::cout << Term::color_fg(Term::Color::Name::Gray) << " (none to fire)"
                           << Term::color_fg(Term::Color::Name::Default);
            } else if (i == 3 && gardenerCount == 0) {
                Term::cout << Term::color_fg(Term::Color::Name::Gray) << " (none to fire)"
                           << Term::color_fg(Term::Color::Name::Default);
            }
        }

        Term::cout << "\n";
    }

    Term::cout << "\n";
    Term::cout << Term::color_fg(Term::Color::Name::Gray);
    Term::cout << "  ───────────────────────────────────────────────────────\n";
    Term::cout << "  Note: Staff members cost R80 per day each.\n";
    Term::cout << "  ESC: Return to dashboard\n";
    Term::cout << "  ───────────────────────────────────────────────────────\n";
    Term::cout << Term::color_fg(Term::Color::Name::Default);

    Term::cout << std::flush;
}

/**
 * @brief Renders the save game screen (file browser + new-save buffer)
 */
void renderSaveGameScreen(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption,
                          const std::string& editBuffer) {
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
    Term::cout << Term::style(Term::Style::Bold)
               << "=== SAVE GAME ===" << Term::style(Term::Style::Reset) << "\n\n";

    // Gather save files from ./saves
    const std::string saveDir = "saves";
    std::vector<std::string> files;
    DIR* dir = opendir(saveDir.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn(entry->d_name);
            if (fn.size() >= 5 && fn.substr(fn.size() - 5) == ".json") {
                std::string path = saveDir + "/" + fn;
                struct stat st;
                if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                    files.push_back(fn);
                }
            }
        }
        closedir(dir);
    }

    Term::cout << "Money: R" << nursery->getMoney() << "  Day: " << nursery->getCurrentDay()
               << "\n\n";

    Term::cout << "  0) New Save..." << (selectedOption == 0 ? " <-" : "") << "\n";
    for (std::size_t i = 0; i < files.size(); ++i) {
        Term::cout << "  " << (i + 1) << ") " << files[i];
        if (selectedOption == (std::size_t)(i + 1)) Term::cout << "  <-";
        Term::cout << "\n";
    }

    if (selectedOption == 0) {
        Term::cout << "\nFilename: " << editBuffer << "\n";
    }

    Term::cout << "\nNavigate with ↑/↓, Enter to save, ESC to cancel" << std::flush;
}

/**
 * @brief Renders the load game screen (file browser)
 */
void renderLoadGameScreen(std::size_t selectedOption) {
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
    Term::cout << Term::style(Term::Style::Bold)
               << "=== LOAD GAME ===" << Term::style(Term::Style::Reset) << "\n\n";

    const std::string saveDir = "saves";
    std::vector<std::string> files;
    DIR* dir = opendir(saveDir.c_str());
    if (!dir) {
        // Save directory missing — inform user; main loop will switch to NO_SAVES
        Term::cout << "No save directory found ('./saves'). Press any key to return..."
                   << std::flush;
        return;
    } else {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn(entry->d_name);
            if (fn.size() >= 5 && fn.substr(fn.size() - 5) == ".json") {
                std::string path = saveDir + "/" + fn;
                struct stat st;
                if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                    files.push_back(fn);
                }
            }
        }
        closedir(dir);
    }

    if (files.empty()) {
        Term::cout << "No save files found in './saves/'. Press any key to return..." << std::flush;
        return;
    }

    for (std::size_t i = 0; i < files.size(); ++i) {
        Term::cout << "  " << (i + 1) << ") ";
        if (selectedOption == i) {
            Term::cout << Term::color_fg(Term::Color::Name::Black)
                       << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                       << files[i] << Term::style(Term::Style::Reset)
                       << Term::color_fg(Term::Color::Name::Default)
                       << Term::color_bg(Term::Color::Name::Default);
        } else {
            Term::cout << files[i];
        }
        Term::cout << "\n";
    }

    // Add an explicit "Return to Main Menu" option as the last selectable item
    std::size_t returnIndex = files.size();
    Term::cout << "\n  ";
    if (selectedOption == returnIndex) {
        Term::cout << Term::color_fg(Term::Color::Name::Black)
                   << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                   << " > Return to Main Menu " << Term::style(Term::Style::Reset)
                   << Term::color_fg(Term::Color::Name::Default)
                   << Term::color_bg(Term::Color::Name::Default) << "\n";
    } else {
        Term::cout << "   Return to Main Menu\n";
    }

    Term::cout << "\nNavigate with ↑/↓, Enter to load/select, ESC or Q to cancel" << std::flush;
}

// Check whether there are any save files in ./saves
bool hasSaveFiles() {
    const std::string saveDir = "saves";
    DIR* dir = opendir(saveDir.c_str());
    if (!dir) return false;
    bool found = false;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fn(entry->d_name);
        if (fn.size() >= 5 && fn.substr(fn.size() - 5) == ".json") {
            std::string path = saveDir + "/" + fn;
            struct stat st;
            if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                found = true;
                break;
            }
        }
    }
    closedir(dir);
    return found;
}

// Render the 'no saves found' screen with a single selectable option
void renderNoSavesScreen(std::size_t selectedOption) {
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
    Term::cout << Term::style(Term::Style::Bold)
               << "=== LOAD GAME ===" << Term::style(Term::Style::Reset) << "\n\n";
    Term::cout << "No save files found in './saves/'.\n\n";

    // Single option: Return to Main Menu
    Term::cout << "  ";
    if (selectedOption == 0) {
        Term::cout << Term::color_fg(Term::Color::Name::Black)
                   << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                   << " > Return to Main Menu " << Term::style(Term::Style::Reset)
                   << Term::color_fg(Term::Color::Name::Default)
                   << Term::color_bg(Term::Color::Name::Default) << "\n";
    } else {
        Term::cout << "   Return to Main Menu \n";
    }

    Term::cout << "\nUse ↑/↓ to navigate (only one option), Enter to select.\n" << std::flush;
}

// Input handler for the no-saves screen: navigate and return to MAIN_MENU on Enter or Esc
Screen handleNoSavesInput(Term::Event& event, std::size_t& selectedOption) {
    if (event.type() != Term::Event::Type::Key) return Screen::NO_SAVES;
    Term::Key key(event);
    if (key == Term::Key::ArrowUp || key == Term::Key::ArrowDown) {
        // only one option; keep it at 0
        selectedOption = 0;
        return Screen::NO_SAVES;
    }
    if (key == Term::Key::Enter || key == Term::Key::Esc || key == Term::Key::q) {
        return Screen::MAIN_MENU;
    }
    return Screen::NO_SAVES;
}

// -----------------------------
// Small text input prompt
// -----------------------------
bool textInputPrompt(const std::string& title, const std::string& prompt,
                     const std::string& initial, std::string& out) {
    std::string buffer = initial;
    const int boxW = 64;
    const int startRow = 8;
    const int startCol = 6;

    while (true) {
        Term::cout << Term::cursor_move(startRow, startCol);
        Term::cout << " ";
        for (int i = 0; i < boxW; ++i) Term::cout << "─";
        Term::cout << " \n";
        Term::cout << Term::cursor_move(startRow + 1, startCol) << "│ "
                   << Term::style(Term::Style::Bold) << title << Term::style(Term::Style::Reset);
        for (int i = 0; i < boxW - 2 - (int)title.size(); ++i) Term::cout << ' ';
        Term::cout << " │\n";
        Term::cout << Term::cursor_move(startRow + 2, startCol) << "│ ";
        std::string p = prompt;
        if ((int)p.size() > boxW - 4) p = p.substr(0, boxW - 7) + "...";
        Term::cout << p;
        for (int i = 0; i < boxW - 2 - (int)p.size(); ++i) Term::cout << ' ';
        Term::cout << " │\n";

        Term::cout << Term::cursor_move(startRow + 3, startCol) << "│ ";
        // show buffer truncated to box
        std::string show = buffer;
        if ((int)show.size() > boxW - 6) show = show.substr(show.size() - (boxW - 6));
        Term::cout << show;
        for (int i = 0; i < boxW - 2 - (int)show.size(); ++i) Term::cout << ' ';
        Term::cout << " │\n";

        Term::cout << Term::cursor_move(startRow + 4, startCol) << "│ ";
        Term::cout << "Press Enter to accept, Esc to cancel. Backspace supported.";
        for (int i = 0; i < boxW - 2 - 46; ++i) Term::cout << ' ';
        Term::cout << " │\n";

        Term::cout << Term::cursor_move(startRow + 5, startCol) << " ";
        for (int i = 0; i < boxW; ++i) Term::cout << "─";
        Term::cout << " \n";
        Term::cout << Term::style(Term::Style::Reset) << std::flush;

        Term::Event ev = Term::read_event();
        if (ev.type() == Term::Event::Type::Key) {
            Term::Key k(ev);
            if (k == Term::Key::Enter) {
                out = buffer;
                return true;
            }
            if (k == Term::Key::Esc) {
                return false;
            }
            if (k == Term::Key::Backspace) {
                if (!buffer.empty()) buffer.pop_back();
            } else {
                // append printable keys via Term::Key
                Term::Key kk(ev);
                if (kk.isprint()) {
                    char c = static_cast<char>(static_cast<int>(kk));
                    buffer.push_back(c);
                }
            }
        }
    }
    return false;
}

// ============================================================================
// CONFIRMATION PROMPT
// ============================================================================

bool confirmPrompt(const std::string& title, const std::string& question) {
    // Improved modal overlay: centered bordered box with Yes/No buttons and keyboard navigation
    const int boxWidth = 64;
    // Simple static placement: center horizontally at column 4 (approx), vertical offset 6
    // (Avoid querying terminal size for portability; fixed placement should be fine for typical
    // terminals.)
    const int startRow = 6;
    const int startCol = 6;

    // title + spacer + question + spacer + buttons + padding
    // Default to No selected (safer for destructive actions)
    bool selectionIsYes = false;  // false -> No selected, true -> Yes selected

    while (true) {
        // Draw box border
        Term::cout << Term::cursor_move(startRow, startCol);
        Term::cout << Term::color_bg(Term::Color::Name::Black)
                   << Term::color_fg(Term::Color::Name::White);

        // Top border
        Term::cout << " ";
        for (int i = 0; i < boxWidth; ++i) Term::cout << "─";  // box-drawing
        Term::cout << " \n";

        // Title line
        Term::cout << Term::cursor_move(startRow + 1, startCol);
        Term::cout << "│ ";
        Term::cout << Term::style(Term::Style::Bold) << title << Term::style(Term::Style::Reset);
        // fill rest of line
        int titleLen = static_cast<int>(title.size()) + 1;
        for (int i = 0; i < boxWidth - titleLen; ++i) Term::cout << ' ';
        Term::cout << " │\n";  // │

        // Empty spacer
        Term::cout << Term::cursor_move(startRow + 2, startCol);
        Term::cout << "│ ";
        for (int i = 0; i < boxWidth - 1; ++i) Term::cout << ' ';
        Term::cout << " │\n";

        // Question line (wrap naive: single line truncated)
        Term::cout << Term::cursor_move(startRow + 3, startCol);
        Term::cout << "│ ";
        std::string q = question;
        if ((int)q.size() > boxWidth - 3) q = q.substr(0, boxWidth - 6) + "...";
        Term::cout << q;
        for (int i = 0; i < boxWidth - 1 - (int)q.size(); ++i) Term::cout << ' ';
        Term::cout << " │\n";

        // Empty spacer
        Term::cout << Term::cursor_move(startRow + 4, startCol);
        Term::cout << "\u2502 ";
        for (int i = 0; i < boxWidth - 1; ++i) Term::cout << ' ';
        Term::cout << " \u2502\n";

        // Buttons line (No on left, Yes on right)
        Term::cout << Term::cursor_move(startRow + 5, startCol);
        Term::cout << "│ ";
        // Compute positions for buttons (No then Yes)
        std::string noLabel = "[ No ]";
        std::string yesLabel = "[ Yes ]";
        int paddingLeft = 4;
        for (int i = 0; i < paddingLeft; ++i) Term::cout << ' ';

        // No button (left)
        if (!selectionIsYes) {
            Term::cout << Term::color_bg(Term::Color::Name::White)
                       << Term::color_fg(Term::Color::Name::Black) << noLabel
                       << Term::color_bg(Term::Color::Name::Default)
                       << Term::color_fg(Term::Color::Name::Default);
        } else {
            Term::cout << noLabel;
        }

        // Spacer between buttons
        for (int i = 0; i < 6; ++i) Term::cout << ' ';

        // Yes button (right)
        if (selectionIsYes) {
            Term::cout << Term::color_bg(Term::Color::Name::White)
                       << Term::color_fg(Term::Color::Name::Black) << yesLabel
                       << Term::color_bg(Term::Color::Name::Default)
                       << Term::color_fg(Term::Color::Name::Default);
        } else {
            Term::cout << yesLabel;
        }

        // Fill rest
        int used = paddingLeft + (int)yesLabel.size() + 6 + (int)noLabel.size();
        for (int i = 0; i < boxWidth - 1 - used; ++i) Term::cout << ' ';
        Term::cout << " \u2502\n";

        // Bottom border
        Term::cout << Term::cursor_move(startRow + 6, startCol);
        Term::cout << " ";
        for (int i = 0; i < boxWidth; ++i) Term::cout << "─";
        Term::cout << " \n";

        Term::cout << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default)
                   << Term::color_bg(Term::Color::Name::Default) << std::flush;

        // Read input and handle navigation
        Term::Event ev = Term::read_event();
        if (ev.type() == Term::Event::Type::Key) {
            Term::Key k(ev);
            if (k == Term::Key::ArrowLeft || k == Term::Key::ArrowUp) {
                // Move selection left -> No
                selectionIsYes = false;
            } else if (k == Term::Key::ArrowRight || k == Term::Key::ArrowDown) {
                // Move selection right -> Yes
                selectionIsYes = true;
            } else if (k == Term::Key::Enter) {
                return selectionIsYes;
            } else if (k == Term::Key::y || k == Term::Key::Y) {
                return true;
            } else if (k == Term::Key::n || k == Term::Key::N) {
                return false;
            } else if (k == Term::Key::Esc || k == Term::Key::q) {
                return false;
            }
        }
        // Otherwise, loop and redraw (reflect selection changes)
    }
    // unreachable
    return false;
}

// ============================================================================
// INPUT HANDLERS (Stubs - to be implemented)
// ============================================================================

/**
 * @brief Handles input on main menu screen
 * @param event The input event
 * @param selectedOption Currently selected menu option
 * @param running Game running flag
 * @param nursery Game state (will be created on "New Game")
 * @return Next screen to display
 */
Screen handleMainMenuInput(Term::Event& event, std::size_t& selectedOption, bool& running,
                           std::shared_ptr<Nursery>& nursery) {
    const std::size_t menuItemCount = 3;  // Load Game, New Game, Exit

    if (event.type() == Term::Event::Type::Key) {
        Term::Key key(event);

        // Navigation
        if (key == Term::Key::ArrowUp) {
            if (selectedOption > 0)
                selectedOption--;
            else
                selectedOption = menuItemCount - 1;  // Wrap to bottom
        } else if (key == Term::Key::ArrowDown) {
            selectedOption = (selectedOption + 1) % menuItemCount;  // Wrap to top
        }
        // Selection
        else if (key == Term::Key::Enter) {
            switch (selectedOption) {
                case 0:  // Load Game
                    // If save files exist, go to LOAD_GAME; otherwise show NO_SAVES screen
                    if (hasSaveFiles())
                        return Screen::LOAD_GAME;
                    else
                        return Screen::NO_SAVES;

                case 1:  // New Game
                {
                    bool ok = confirmPrompt("Start New Game", "Start a new game?");
                    if (!ok) break;  // cancel
                    // Create a properly setup nursery (register factories, attach supervisor)
                    nursery = Nursery::createAndSetup();
                    return Screen::GAME_DASHBOARD;
                }

                case 2:  // Exit
                {
                    bool ok = confirmPrompt("Exit Game", "Are you sure you want to exit?");
                    if (ok) running = false;
                    break;
                }
            }
        }
        // Quick quit
        else if (key == Term::Key::q || key == Term::Key::Esc) {
            bool ok = confirmPrompt("Exit Game", "Are you sure you want to exit?");
            if (ok) running = false;
        }
    }

    return Screen::MAIN_MENU;
}

/**
 * @brief Handles input on game dashboard
 * @param event The input event
 * @param selectedOption Currently selected action
 * @param nursery Game state
 * @return Next screen to display
 */
Screen handleGameDashboardInput(Term::Event& event, std::size_t& selectedOption,
                                std::shared_ptr<Nursery>& nursery) {
    if (event.type() != Term::Event::Type::Key) return Screen::GAME_DASHBOARD;

    Term::Key key(event);

    // Build action list based on phase (must match renderGameDashboard)
    std::vector<std::string> actions;
    GamePhase phase = nursery->getCurrentPhase();

    if (phase == GamePhase::IDLE) {
        actions.push_back("Start New Day");
        actions.push_back("View Inventory");
        actions.push_back("View Cultivating Plants");
        actions.push_back("Return to Menu");
    } else if (phase == GamePhase::DAY_START || phase == GamePhase::STEP_BREAK) {
        actions.push_back("Advance Step");
        actions.push_back("Plant Seeds");
        actions.push_back("View Inventory");
        actions.push_back("View Cultivating Plants");
        actions.push_back("Return to Menu");
    } else if (phase == GamePhase::DAY_END) {
        actions.push_back("Start New Day");
        actions.push_back("Plant Seeds");
        actions.push_back("View Inventory");
        actions.push_back("View Cultivating Plants");
        if (nursery->canHire()) actions.push_back("Hire Staff");
        if (nursery->canSave()) actions.push_back("Save Game");
        actions.push_back("Return to Menu");
    }

    // Clamp selection to valid range (in case menu size changed)
    if (selectedOption >= actions.size()) selectedOption = 0;

    // Navigation
    if (key == Term::Key::ArrowUp) {
        if (selectedOption > 0)
            selectedOption--;
        else
            selectedOption = actions.size() - 1;
    } else if (key == Term::Key::ArrowDown) {
        selectedOption = (selectedOption + 1) % actions.size();
    }
    // Selection
    else if (key == Term::Key::Enter) {
        std::string selectedAction = actions[selectedOption];

        if (selectedAction == "Start New Day") {
            nursery->startNewDay();
            selectedOption = 0;  // Reset to first action
        } else if (selectedAction == "Advance Step") {
            nursery->advanceStep();
            selectedOption = 0;  // Reset to first action
        } else if (selectedAction == "Plant Seeds") {
            selectedOption = 0;
            return Screen::PLANT_SEEDS;
        } else if (selectedAction == "View Inventory") {
            return Screen::INVENTORY_VIEW;
        } else if (selectedAction == "View Cultivating Plants") {
            return Screen::CULTIVATING_PLANTS;
        } else if (selectedAction == "Hire Staff") {
            selectedOption = 0;
            return Screen::HIRE_STAFF;
        } else if (selectedAction == "Save Game") {
            selectedOption = 0;
            return Screen::SAVE_GAME;
        } else if (selectedAction == "Return to Menu") {
            // Confirm before returning to main menu via action menu
            {
                bool ok = confirmPrompt("Return to Menu",
                                        "Return to the main menu? Unsaved progress will be lost.");
                if (ok) {
                    // User expects nursery to be deallocated when returning to main menu
                    nursery.reset();
                    return Screen::MAIN_MENU;
                }
            }
        }
    }
    // Quick shortcuts
    else if (key == Term::Key::q || key == Term::Key::Esc) {
        // Confirm before returning to main menu
        bool ok = confirmPrompt("Return to Menu",
                                "Return to the main menu? Unsaved progress will be lost.");
        if (ok) {
            nursery.reset();
            return Screen::MAIN_MENU;
        }
    }

    return Screen::GAME_DASHBOARD;
}

/**
 * @brief Handles input on inventory view
 * @param event The input event
 * @return Next screen to display
 *
 * TODO: Implement inventory navigation
 */
Screen handleInventoryInput(Term::Event& event, std::shared_ptr<Nursery>& nursery) {
    using namespace UI;

    // Use shared UI state
    auto selectedGroup = &g_inv_selectedGroup;
    auto inGroupView = &g_inv_inGroupView;
    auto selectedPlant = &g_inv_selectedPlant;

    if (!nursery) return Screen::GAME_DASHBOARD;

    InventoryView view(nursery->getInventory());
    auto groups = view.listGroupNames();

    if (event.type() == Term::Event::Type::Key) {
        Term::Key key(event);

        if (!*inGroupView) {
            // navigation now includes an extra 'Return to Dashboard' index at groups.size()
            std::size_t maxIndex = groups.size();
            if (key == Term::Key::ArrowUp) {
                if (*selectedGroup > 0)
                    (*selectedGroup)--;
                else
                    *selectedGroup = maxIndex;
                return Screen::INVENTORY_VIEW;
            }
            if (key == Term::Key::ArrowDown) {
                *selectedGroup = (*selectedGroup + 1) % (maxIndex + 1);
                return Screen::INVENTORY_VIEW;
            }
            if (key == Term::Key::Enter) {
                // If return selected, go back to dashboard
                if (*selectedGroup == maxIndex) return Screen::GAME_DASHBOARD;
                if (!groups.empty()) {
                    *inGroupView = true;
                    *selectedPlant = 0;
                }
                return Screen::INVENTORY_VIEW;
            }
            if (key == Term::Key::Esc || key == Term::Key::q) {
                // Return to dashboard
                return Screen::GAME_DASHBOARD;
            }
        } else {
            // Navigating plants in a group
            std::string gname = groups.empty() ? std::string() : groups[*selectedGroup];
            auto plants = view.listPlantsInGroup(gname);

            if (key == Term::Key::ArrowUp) {
                if (*selectedPlant > 0)
                    (*selectedPlant)--;
                else
                    *selectedPlant = plants.empty() ? 0 : plants.size() - 1;
                return Screen::INVENTORY_VIEW;
            }
            if (key == Term::Key::ArrowDown) {
                if (!plants.empty()) *selectedPlant = (*selectedPlant + 1) % plants.size();
                return Screen::INVENTORY_VIEW;
            }
            if (key == Term::Key::Enter) {
                // For now Enter does nothing (could open plant detail later)
                return Screen::INVENTORY_VIEW;
            }
            if (key == Term::Key::Esc || key == Term::Key::q) {
                // Back to group list
                *inGroupView = false;
                return Screen::INVENTORY_VIEW;
            }
            // 'q' is intentionally not handled here to avoid accidental quit from inventory view
        }
    }

    return Screen::INVENTORY_VIEW;
}

/**
 * @brief Handles input on cultivating plants view
 * @param event The input event
 * @param nursery Game state
 * @return Next screen to display
 *
 * Simple read-only view with ESC/Q to return to dashboard
 */
Screen handleCultivatingPlantsInput(Term::Event& event, std::shared_ptr<Nursery>& nursery) {
    if (event.type() == Term::Event::Type::Key) {
        auto key = Term::Key(event);

        // ESC or Q to return to dashboard
        if (key == Term::Key::Esc || key == Term::Key::q || key == Term::Key::Q) {
            return Screen::GAME_DASHBOARD;
        }
    }

    return Screen::CULTIVATING_PLANTS;
}

/**
 * @brief Handles input on plant seeds menu
 * @param event The input event
 * @param selectedOption Currently selected option in current view
 * @param nursery Game state
 * @return Next screen to display
 *
 * Three-stage planting flow with navigation and plant creation
 */
Screen handlePlantSeedsInput(Term::Event& event, std::size_t& selectedOption,
                             std::shared_ptr<Nursery>& nursery) {
    if (event.type() == Term::Event::Type::Key) {
        if (g_ps_view == PlantSeedsView::PLOT_LIST) {
            // Stage 1: Plot selection

            // Normal plot list navigation
            auto inventory = nursery->getInventory();
            std::vector<std::shared_ptr<Group>> plots;

            if (inventory) {
                auto allGroups = inventory->getAllGroups();
                for (auto& group : allGroups) {
                    if (group && group->getName() != "Storage" &&
                        group->getName() != "InventoryRoot" && group->owns()) {
                        plots.push_back(group);
                    }
                }
            }
            // Ensure plot order matches the listing (alphabetical)
            std::sort(plots.begin(), plots.end(),
                      [](const auto& a, const auto& b) { return a->getName() < b->getName(); });

            int maxOptions = plots.size() + 2;  // plots + create + return

            if (event == Term::Key::ArrowUp) {
                if (selectedOption > 0)
                    selectedOption--;
                else
                    selectedOption = maxOptions - 1;
            } else if (event == Term::Key::ArrowDown) {
                selectedOption = (selectedOption + 1) % maxOptions;
            } else if (event == Term::Key::Enter) {
                if (selectedOption < plots.size()) {
                    // Selected a plot - go to position grid
                    g_ps_selectedPlot = selectedOption;
                    g_ps_view = PlantSeedsView::POSITION_GRID;
                    selectedOption = 0;  // Reset selection for grid
                } else if (selectedOption == plots.size()) {
                    // Create new plot - use textInputPrompt
                    std::string plotName;
                    if (textInputPrompt("Create New Plot", "Enter plot name:", "", plotName)) {
                        if (!plotName.empty() && nursery && nursery->getInventory()) {
                            if (inventory->findGroupByName(plotName) == nullptr) {
                                auto newPlot = std::make_shared<Group>(plotName, true);
                                nursery->getInventory()->add(newPlot);
                            } else {
                                // Name already exists - show error
                                confirmPrompt("Error",
                                              "A plot with that name already exists.");
                            }
                        }
                    }
                } else {
                    // Return to dashboard
                    g_ps_view = PlantSeedsView::PLOT_LIST;
                    selectedOption = 0;
                    return Screen::GAME_DASHBOARD;
                }
            } else if (event == Term::Key::Esc || event == Term::Key::q) {
                g_ps_view = PlantSeedsView::PLOT_LIST;
                selectedOption = 0;
                return Screen::GAME_DASHBOARD;
            }
        } else if (g_ps_view == PlantSeedsView::POSITION_GRID) {
            // Stage 2: Position grid navigation (3x3)

            if (event == Term::Key::ArrowUp) {
                if (selectedOption >= 3) selectedOption -= 3;
            } else if (event == Term::Key::ArrowDown) {
                if (selectedOption < 6) selectedOption += 3;
            } else if (event == Term::Key::ArrowLeft) {
                if (selectedOption % 3 != 0) selectedOption--;
            } else if (event == Term::Key::ArrowRight) {
                if (selectedOption % 3 != 2) selectedOption++;
            } else if (event == Term::Key::Enter) {
                // Check if position is available
                auto inventory = nursery->getInventory();
                std::vector<std::shared_ptr<Group>> plots;

                if (inventory) {
                    auto allGroups = inventory->getAllGroups();
                    for (auto& group : allGroups) {
                        if (group && group->getName() != "Storage" &&
                            group->getName() != "InventoryRoot" && group->owns()) {
                            plots.push_back(group);
                        }
                    }
                }
                // Keep plots sorted alphabetically to match the listing
                std::sort(plots.begin(), plots.end(),
                          [](const auto& a, const auto& b) { return a->getName() < b->getName(); });

                if (g_ps_selectedPlot < plots.size()) {
                    auto selectedPlot = plots[g_ps_selectedPlot];
                    auto members = selectedPlot->members();

                    // Check if position is occupied
                    std::map<int, bool> occupied;
                    int pos = 0;
                    for (auto& m : members) {
                        auto plant = std::dynamic_pointer_cast<Plant>(m);
                        if (plant && pos < 9) {
                            occupied[pos] = true;
                            pos++;
                        }
                    }

                    bool isOccupied = occupied.find(selectedOption) != occupied.end();

                    if (!isOccupied && selectedOption < 9 && members.size() < 9) {
                        // Position is available - go to plant type selection
                        g_ps_selectedPosition = selectedOption;
                        g_ps_view = PlantSeedsView::PLANT_TYPE;
                        selectedOption = 0;  // Reset for plant type list
                    }
                }
            } else if (event == Term::Key::Esc || event == Term::Key::q) {
                // Back to plot list
                g_ps_view = PlantSeedsView::PLOT_LIST;
                selectedOption = 0;
            }
        } else if (g_ps_view == PlantSeedsView::PLANT_TYPE) {
            // Stage 3: Plant type selection

            // Build plant type list from factories
            std::vector<std::pair<std::string, double>> plantTypes;
            const auto& factories = nursery->getPlantFactories();
            for (const auto& pair : factories) {
                plantTypes.push_back({pair.first, pair.second->getSeedCost()});
            }
            std::sort(plantTypes.begin(), plantTypes.end(),
                      [](const auto& a, const auto& b) { return a.first < b.first; });

            int maxOptions = plantTypes.size() + 1;  // types + cancel

            if (event == Term::Key::ArrowUp) {
                if (selectedOption > 0)
                    selectedOption--;
                else
                    selectedOption = maxOptions - 1;
            } else if (event == Term::Key::ArrowDown) {
                selectedOption = (selectedOption + 1) % maxOptions;
            } else if (event == Term::Key::Enter) {
                if (selectedOption < plantTypes.size()) {
                    // Plant the seed
                    std::string plantType = plantTypes[selectedOption].first;
                    double seedCost = plantTypes[selectedOption].second;

                    if (nursery->getMoney() >= seedCost) {
                        // Get the factory
                        const auto& factoryMap = nursery->getPlantFactories();
                        auto factoryIt = factoryMap.find(plantType);

                        if (factoryIt != factoryMap.end()) {
                            // Deduct money
                            nursery->adjustMoney(-seedCost);

                            // Create plant using factory
                            auto newPlant = factoryIt->second->createPlant();

                            // Get the selected plot
                            auto inventory = nursery->getInventory();
                            std::vector<std::shared_ptr<Group>> plots;

                            if (inventory) {
                                auto allGroups = inventory->getAllGroups();
                                for (auto& group : allGroups) {
                                    if (group && group->getName() != "Storage" &&
                                        group->getName() != "InventoryRoot" && group->owns()) {
                                        plots.push_back(group);
                                    }
                                }
                            }
                            // Ensure plot order matches the listing (alphabetical)
                            std::sort(plots.begin(), plots.end(), [](const auto& a, const auto& b) {
                                return a->getName() < b->getName();
                            });

                            if (g_ps_selectedPlot < plots.size()) {
                                auto selectedPlot = plots[g_ps_selectedPlot];

                                // Add to plot (ownership transfer, observer attachment automatic)
                                selectedPlot->add(newPlant);
                            }
                        }

                        // After planting, remain on the position grid for the same plot
                        g_ps_view = PlantSeedsView::POSITION_GRID;
                        selectedOption = g_ps_selectedPosition;
                    }
                } else {
                    // Cancel - back to grid
                    g_ps_view = PlantSeedsView::POSITION_GRID;
                    selectedOption = g_ps_selectedPosition;
                }
            } else if ((event == Term::Key::Esc) || event == Term::Key::q) {
                // Back to position grid
                g_ps_view = PlantSeedsView::POSITION_GRID;
                selectedOption = g_ps_selectedPosition;
            }
        }
    }

    return Screen::PLANT_SEEDS;
}

/**
 * @brief Handles input on hire staff menu
 * @param event The input event
 * @param selectedOption Currently selected staff type
 * @param nursery Game state
 * @return Next screen to display
 *
 * Allows hiring and firing Cashier or Gardener
 */
Screen handleHireStaffInput(Term::Event& event, std::size_t& selectedOption,
                            std::shared_ptr<Nursery>& nursery) {
    const int NUM_OPTIONS = 5;  // Hire Cashier, Hire Gardener, Fire Cashier, Fire Gardener, Return

    if (event.type() == Term::Event::Type::Key) {
        // Arrow navigation
        if (event == Term::Key::ArrowUp) {
            if (selectedOption > 0) {
                selectedOption--;
            } else {
                selectedOption = NUM_OPTIONS - 1;
            }
        } else if (event == Term::Key::ArrowDown) {
            selectedOption = (selectedOption + 1) % NUM_OPTIONS;
        }
        // Enter to confirm
        else if (event == Term::Key::Enter) {
            // Option 0: Hire Cashier
            if (selectedOption == 0) {
                // Create new cashier
                auto newCashier = std::make_shared<Cashier>();

                // Add to end of chain
                auto head = nursery->getStaffChainHead();
                if (!head) {
                    nursery->setStaffChainHead(newCashier);
                } else {
                    auto current = head;
                    while (current->getSuccessor()) {
                        current = current->getSuccessor();
                    }
                    current->setSuccessor(newCashier);
                }
            }
            // Option 1: Hire Gardener
            else if (selectedOption == 1) {
                // Create new gardener
                auto newGardener = std::make_shared<Gardener>();

                // Add to end of chain
                auto head = nursery->getStaffChainHead();
                if (!head) {
                    nursery->setStaffChainHead(newGardener);
                } else {
                    auto current = head;
                    while (current->getSuccessor()) {
                        current = current->getSuccessor();
                    }
                    current->setSuccessor(newGardener);
                }
            }
            // Option 2: Fire Cashier
            else if (selectedOption == 2) {
                auto head = nursery->getStaffChainHead();
                std::shared_ptr<Staff> prev = nullptr;

                // Find first cashier in chain
                while (head) {
                    if (dynamic_cast<Cashier*>(head.get())) {
                        // Found a cashier to fire
                        if (prev) {
                            // Remove from middle/end of chain
                            prev->setSuccessor(head->getSuccessor());
                        } else {
                            // Remove from head of chain
                            nursery->setStaffChainHead(head->getSuccessor());
                        }
                        break;
                    }
                    prev = head;
                    head = head->getSuccessor();
                }
            }
            // Option 3: Fire Gardener
            else if (selectedOption == 3) {
                auto head = nursery->getStaffChainHead();
                std::shared_ptr<Staff> prev = nullptr;

                // Find first gardener in chain
                while (head) {
                    if (dynamic_cast<Gardener*>(head.get())) {
                        // Found a gardener to fire
                        if (prev) {
                            // Remove from middle/end of chain
                            prev->setSuccessor(head->getSuccessor());
                        } else {
                            // Remove from head of chain
                            nursery->setStaffChainHead(head->getSuccessor());
                        }
                        break;
                    }
                    prev = head;
                    head = head->getSuccessor();
                }
            }
            // Option 4: Return to dashboard
            else if (selectedOption == 4) {
                selectedOption = 0;  // Reset for next time
                return Screen::GAME_DASHBOARD;
            }
        }
        // ESC or Q to return
        else if (event == Term::Key::Esc || event == Term::Key::q) {
            selectedOption = 0;  // Reset for next time
            return Screen::GAME_DASHBOARD;
        }
    }

    return Screen::HIRE_STAFF;
}

// -----------------------------
// Save / Load input handlers
// -----------------------------

Screen handleSaveGameInput(Term::Event& event, std::size_t& selectedOption,
                           std::shared_ptr<Nursery>& nursery, std::string& editBuffer) {
    const std::string saveDir = "saves";
    std::vector<std::string> files;
    DIR* dir = opendir(saveDir.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn(entry->d_name);
            if (fn.size() >= 5 && fn.substr(fn.size() - 5) == ".json") {
                std::string path = saveDir + "/" + fn;
                struct stat st;
                if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                    files.push_back(fn);
                }
            }
        }
        closedir(dir);
    }

    if (event.type() == Term::Event::Type::Key) {
        Term::Key key(event);
        // navigation
        std::size_t maxIndex = files.size();  // number of existing files
        // selectedOption ranges from 0 (New Save) to maxIndex (last file index + 1)
        if (key == Term::Key::ArrowUp) {
            if (selectedOption > 0)
                selectedOption--;
            else
                selectedOption = maxIndex;
            return Screen::SAVE_GAME;
        }
        if (key == Term::Key::ArrowDown) {
            selectedOption = (selectedOption + 1) % (maxIndex + 1);
            return Screen::SAVE_GAME;
        }

        if (key == Term::Key::Enter) {
            // New save
            if (selectedOption == 0) {
                std::string out;
                bool ok =
                    textInputPrompt("Save As", "Enter filename (no extension):", editBuffer, out);
                if (!ok) return Screen::SAVE_GAME;
                editBuffer = out;
                // ensure folder exists
                struct stat st;
                if (stat(saveDir.c_str(), &st) == -1) {
                    std::filesystem::create_directories(saveDir);
                }
                std::string name = editBuffer;
                if (name.size() < 5 || name.substr(name.size() - 5) != ".json") name += ".json";
                std::string path = saveDir + "/" + name;
                if (SaveSystem::fileExists(path)) {
                    bool overwrite = confirmPrompt("Overwrite", "File exists. Overwrite?");
                    if (!overwrite) return Screen::SAVE_GAME;
                }
                SaveSystem saver;
                saver.save(nursery, path);

                // After save, reload the saved state to ensure consistency
                // This puts the user in IDLE phase (no day in progress)
                auto m = saver.load(path);
                if (m) {
                    nursery = std::make_shared<Nursery>();
                    nursery->restoreFromMemento(m.get());
                    nursery->postRestoreInit();
                }
                return Screen::GAME_DASHBOARD;
            } else {
                std::size_t idx = selectedOption - 1;
                if (idx < files.size()) {
                    std::string path = saveDir + "/" + files[idx];
                    bool overwrite = confirmPrompt(
                        "Overwrite", std::string("Overwrite \"") + files[idx] + "\"?");
                    if (!overwrite) return Screen::SAVE_GAME;
                    SaveSystem saver;
                    saver.save(nursery, path);

                    // After save, reload the saved state to ensure consistency
                    // This puts the user in IDLE phase (no day in progress)
                    auto m = saver.load(path);
                    if (m) {
                        nursery = std::make_shared<Nursery>();
                        nursery->restoreFromMemento(m.get());
                        nursery->postRestoreInit();
                    }
                    return Screen::GAME_DASHBOARD;
                }
            }
        }

        if (key == Term::Key::Esc || key == Term::Key::q) {
            return Screen::GAME_DASHBOARD;
        }
    }

    return Screen::SAVE_GAME;
}

Screen handleLoadGameInput(Term::Event& event, std::size_t& selectedOption,
                           std::shared_ptr<Nursery>& nursery, std::string& editBuffer) {
    // (no-op)
    const std::string saveDir = "saves";
    std::vector<std::string> files;
    DIR* dir = opendir(saveDir.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn(entry->d_name);
            if (fn.size() >= 5 && fn.substr(fn.size() - 5) == ".json") {
                std::string path = saveDir + "/" + fn;
                struct stat st;
                if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                    files.push_back(fn);
                }
            }
        }
        closedir(dir);
    }

    // Provide one extra selectable option: 'Return to Main Menu' at index files.size()
    std::size_t maxIndex = files.size();  // last valid index is maxIndex (return option)

    // Ensure selection is within bounds (0 .. maxIndex)
    if (selectedOption > maxIndex) selectedOption = 0;

    if (event.type() == Term::Event::Type::Key) {
        Term::Key key(event);
        if (key == Term::Key::ArrowUp) {
            if (selectedOption > 0)
                selectedOption--;
            else
                selectedOption = maxIndex;
            return Screen::LOAD_GAME;
        }
        if (key == Term::Key::ArrowDown) {
            selectedOption = (selectedOption + 1) % (maxIndex + 1);
            return Screen::LOAD_GAME;
        }
        if (key == Term::Key::Enter) {
            // If the user selected the final 'Return to Main Menu' option
            if (selectedOption == maxIndex) {
                return Screen::MAIN_MENU;
            }

            // Otherwise attempt to load the selected file
            if (selectedOption < files.size()) {
                std::string path = saveDir + "/" + files[selectedOption];
                SaveSystem saver;
                auto m = saver.load(path);
                if (!m) {
                    confirmPrompt("Load Failed", "Failed to load file. Press OK to continue.");
                    return Screen::LOAD_GAME;
                }
                if (!nursery) {
                    nursery = std::make_shared<Nursery>();
                }
                nursery->restoreFromMemento(m.get());
                // Reinitialize runtime-only systems (commandLog, factories, observer attachments)
                nursery->postRestoreInit();
                return Screen::GAME_DASHBOARD;
            }
        }
        if (key == Term::Key::Esc || key == Term::Key::q) {
            return Screen::MAIN_MENU;
        }
    }

    return Screen::LOAD_GAME;
}

// ============================================================================
// MAIN GAME LOOP
// ============================================================================

int main() {
    try {
        // Initialize terminal
        Term::terminal.setOptions(Term::Option::Raw, Term::Option::NoSignalKeys,
                                  Term::Option::ClearScreen, Term::Option::NoCursor);

        if (!Term::is_stdin_a_tty()) {
            throw Term::Exception("Terminal is not attached to a TTY. Cannot capture input.");
        }

        // Game state
        std::shared_ptr<Nursery> nursery = nullptr;
        Screen currentScreen = Screen::MAIN_MENU;
        // Track previous screen to detect screen changes
        Screen previousScreen = currentScreen;
        bool running = true;
        std::size_t selectedOption = 0;
        std::string editBuffer;

        // Main game loop
        while (running) {
            // Render current screen
            switch (currentScreen) {
                case Screen::MAIN_MENU:
                    renderMainMenu(selectedOption);
                    break;

                case Screen::GAME_DASHBOARD:
                    if (nursery) renderGameDashboard(nursery, selectedOption);
                    break;

                case Screen::INVENTORY_VIEW:
                    if (nursery) renderInventoryView(nursery);
                    break;

                case Screen::CULTIVATING_PLANTS:
                    if (nursery) renderCultivatingPlantsView(nursery);
                    break;

                case Screen::PLANT_SEEDS:
                    if (nursery) renderPlantSeedsMenu(nursery, selectedOption);
                    break;

                case Screen::HIRE_STAFF:
                    if (nursery) renderHireStaffMenu(nursery, selectedOption);
                    break;

                case Screen::SAVE_GAME:
                    if (nursery) renderSaveGameScreen(nursery, selectedOption, editBuffer);
                    break;

                case Screen::LOAD_GAME:
                    renderLoadGameScreen(selectedOption);
                    break;
                case Screen::NO_SAVES:
                    renderNoSavesScreen(selectedOption);
                    break;
            }

            // Handle input
            Term::Event event = Term::read_event();

            switch (currentScreen) {
                case Screen::MAIN_MENU:
                    currentScreen = handleMainMenuInput(event, selectedOption, running, nursery);
                    break;

                case Screen::GAME_DASHBOARD:
                    if (nursery)
                        currentScreen = handleGameDashboardInput(event, selectedOption, nursery);
                    else
                        currentScreen = Screen::MAIN_MENU;  // no nursery -> go back to main menu
                    break;

                case Screen::INVENTORY_VIEW:
                    currentScreen = handleInventoryInput(event, nursery);
                    break;

                case Screen::CULTIVATING_PLANTS:
                    if (nursery)
                        currentScreen = handleCultivatingPlantsInput(event, nursery);
                    else
                        currentScreen = Screen::MAIN_MENU;
                    break;

                case Screen::PLANT_SEEDS:
                    if (nursery)
                        currentScreen = handlePlantSeedsInput(event, selectedOption, nursery);
                    else
                        currentScreen = Screen::MAIN_MENU;
                    break;

                case Screen::SAVE_GAME:
                    if (nursery)
                        currentScreen =
                            handleSaveGameInput(event, selectedOption, nursery, editBuffer);
                    else
                        currentScreen = Screen::MAIN_MENU;
                    break;

                case Screen::LOAD_GAME:
                    currentScreen = handleLoadGameInput(event, selectedOption, nursery, editBuffer);
                    break;
                case Screen::NO_SAVES:
                    currentScreen = handleNoSavesInput(event, selectedOption);
                    break;

                case Screen::HIRE_STAFF:
                    if (nursery)
                        currentScreen = handleHireStaffInput(event, selectedOption, nursery);
                    else
                        currentScreen = Screen::MAIN_MENU;
                    break;

                default:
                    currentScreen = Screen::MAIN_MENU;
                    break;
            }

            // If we just switched screens, reset the selection so the top item is selected
            if (currentScreen != previousScreen) {
                selectedOption = 0;
                // Reset inventory UI state whenever screens change so the cursor is at top
                g_inv_selectedGroup = 0;
                g_inv_inGroupView = false;
                g_inv_selectedPlant = 0;
                g_inv_prev_inGroupView = false;
                // Reset plant seeds UI state
                g_ps_view = PlantSeedsView::PLOT_LIST;
                g_ps_selectedPlot = 0;
                g_ps_selectedPosition = 0;
                g_ps_selectedPlantType = 0;
            }
            // Update previousScreen for next iteration
            previousScreen = currentScreen;
        }

        // Cleanup
        Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
        Term::cout << "Thanks for playing!\n" << std::flush;
    } catch (const Term::Exception& e) {
        Term::cerr << "Terminal error: " << e.what() << std::endl;
        return 2;
    } catch (const std::exception& e) {
        Term::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        Term::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
