//////////////////////////////////pain log//////////////////////////////////////
/*

there are important things in here i promise

begun 23h20

04h03: its so over

⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣷⣄⣀⣀⠀⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿
⣿⡟⠉⠛⠛⠻⣿⣿⣿⡿⠋⠀⣴⣿⣿⠟⠻⠿⠿⣿⣿⣿
⣿⣿⣶⠂⠀⣠⣿⣿⡏⠀⠠⠾⣿⣿⣿⣦⣤⠀⠀⣸⣿⣿ - 0h, 2025/10/31 
⣿⣿⠁⠀⠚⠻⣿⣿⣷⣤⣤⣀⣠⣿⣿⠟⠁⠠⠾⣿⣿⣿
⣿⣿⣷⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣤⣤⣤⣾⣿⣿



ALWAYS INCLUDE A METHOD TO RETURN TO PREVIOUS SCREEN 
DO NOT ALLOW "press any key" AS IT DOES NOT LIKE IT
see load game screen with and without a save file

plants are gonna reach a watering equilibrium where they will need 
to be constantly watered. the values need to change
such that they are watered a further amount beyond their trigger
if it reaches < 50
it will be watered to around 50
lose water again to be < 50
cycle repeats

THE PLANTS ARENT CALLING NOTIFY??????????????
i cant man
also nurserysupervisor needs to be completed to continue properly
the github issue does not cover the full extent of what needs to be done


cpp terminal has memory leaks apparently :D
idk if we can fix that or just explain it somewhere
it has to do with the threads i think


save and load works probably ¯\_(ツ)_/¯
^ future me, it doesnt


even more getters for data that i failed to realise would be needed


need to stringify commands
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
i dont have the mental capacity to think of all the cases for that
but i'll try some

yea nah this is too much for me, please allocate this to people
idk how to allocate it properly tho. this branch is a mess

commands get destoryed when completed :D
wrapper classes for commands to log them maybe?
hmmmmmm


i really did not think this through well enough


i didnt save staff??????????????????????????????????????????????
seiop[fhje[ogihw[eighwe[ghsdj'g;jsd'ghj423-4235u23b5p23th-g8wyh1rgb ['1]]]]]
im gonna go have a beer and a ciggie 



unit tests failing because of command logging wrapper :(

=============================================================================
=============================================================================
=============================================================================
***********THIS PART IS ACTUALLY IMPORTANT********************
                                       L A R G E    T E X T    SO   YOU    SEE IT
plots will be a 3x3 grid
UI for planting into a plot should be like

[][][]
[][][]
[][][]

with already planted spots marked

also idk how we are actually creating plots
they should be able to be named

probably a seperate render/screen or something idk
im not really sure how we will be showing everything
you can use some creative liberty here
=============================================================================
=============================================================================
=============================================================================

since we are going with the whole "magic storage of infinite plant life"
our plots are also magically able to specifically climatize for the plants inside them
and with different climates at each area in the plot
so we can have roses in the same plot as cacti and there are no issues
yay


plants are doing their daily updates and that
but customers arent spawning? and commands arent being created???
ah observers arent linked and arent being linked anywhere

*/
////////////////////////////////////////////////////////////////////////////////
/**
 * @file main.cpp
 * @brief Nursery Management Game - Terminal UI Skeleton
 * 
 * Main entry point for the nursery simulation game with terminal-based UI.
 * Uses cpp-terminal library for cross-platform terminal rendering and input handling.
 * 
 * CURRENT STATUS: Skeleton structure with stub functions
 * TODO: Implement all rendering and input handling functions
 */

#include <memory>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

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

#include "../include/Core/Nursery.h"
#include "../include/Core/SaveSystem.h"
#include "../include/Patterns/Memento/Memento.h"

// ============================================================================
// SCREEN ENUMERATION
// ============================================================================

/**
 * @enum Screen
 * @brief All possible screens in the UI
 */
enum class Screen {
    MAIN_MENU,        // New Game, Load Game, Exit
    GAME_DASHBOARD,   // Main game screen with actions menu
    INVENTORY_VIEW,   // Browse plants by groups
    PLANT_SEEDS,      // Select and plant seeds
    HIRE_STAFF,       // Hire Gardener/Cashier (DAY_END only)
    SAVE_GAME,        // Save game dialog
    LOAD_GAME,        // Load game dialog
    NO_SAVES          // No save files found dialog
};

// ============================================================================
// FORWARD DECLARATIONS - Rendering Functions
// ============================================================================

void renderMainMenu(std::size_t selectedOption);
void renderGameDashboard(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption);
void renderInventoryView(const std::shared_ptr<Nursery>& nursery);
void renderPlantSeedsMenu(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption);
void renderHireStaffMenu(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption);
void renderSaveGameScreen(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption, const std::string& editBuffer);
void renderLoadGameScreen(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption);

// Helper to check for save files and new no-saves screen
bool hasSaveFiles();
void renderNoSavesScreen(std::size_t selectedOption);
Screen handleNoSavesInput(Term::Event& event, std::size_t& selectedOption);

// Input handlers for save/load
Screen handleSaveGameInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery, std::string& editBuffer);
Screen handleLoadGameInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery, std::string& editBuffer);

// Small text input helper (returns true if accepted; out set to value)
bool textInputPrompt(const std::string& title, const std::string& prompt, const std::string& initial, std::string &out);

// ============================================================================
// FORWARD DECLARATIONS - Input Handlers
// ============================================================================

Screen handleMainMenuInput(Term::Event& event, std::size_t& selectedOption, bool& running, std::shared_ptr<Nursery>& nursery);
Screen handleGameDashboardInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery);
Screen handleInventoryInput(Term::Event& event);
Screen handlePlantSeedsInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery);
Screen handleHireStaffInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery);
// Confirmation prompt
bool confirmPrompt(const std::string& title, const std::string& question);

// ============================================================================
// RENDERING FUNCTIONS (Stubs - to be implemented)
// ============================================================================

/**
 * @brief Renders the main menu screen
 * @param selectedOption Currently selected menu option (0-2)
 */
void renderMainMenu(std::size_t selectedOption)
{
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
    
    // Menu options
    const std::vector<std::string> menuOptions = {
        "Load Game",
        "New Game",
        "Exit"
    };
    
    for (std::size_t i = 0; i < menuOptions.size(); ++i)
    {
        Term::cout << "    ";
        
        if (i == selectedOption)
        {
            // Highlight selected option
            Term::cout << Term::color_fg(Term::Color::Name::Black) 
                      << Term::color_bg(Term::Color::Name::White)
                      << Term::style(Term::Style::Bold);
            Term::cout << " > " << menuOptions[i] << " < ";
            Term::cout << Term::style(Term::Style::Reset) 
                      << Term::color_fg(Term::Color::Name::Default)
                      << Term::color_bg(Term::Color::Name::Default);
        }
        else
        {
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
void renderGameDashboard(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption)
{
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
    
    // Header
    Term::cout << Term::color_fg(Term::Color::Name::Cyan) << Term::style(Term::Style::Bold);
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << "                    NURSERY DASHBOARD                    \n";
    Term::cout << "  ═══════════════════════════════════════════════════════\n";
    Term::cout << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default);
    Term::cout << "\n";
    
    // Game state display
    Term::cout << "  Day: " << Term::color_fg(Term::Color::Name::Yellow) 
              << nursery->getCurrentDay() << Term::color_fg(Term::Color::Name::Default) << "\n";
    
    Term::cout << "  Money: " << Term::color_fg(Term::Color::Name::Green) 
              << "R" << nursery->getMoney() << Term::color_fg(Term::Color::Name::Default) << "\n";
    
    // Color-code reputation
    int reputation = nursery->getReputation();
    Term::Color::Name repColor = Term::Color::Name::Yellow;
    if(reputation >= 75) repColor = Term::Color::Name::Green;
    else if(reputation <= 25) repColor = Term::Color::Name::Red;
    
    Term::cout << "  Reputation: " << Term::color_fg(repColor) 
              << reputation << "/100" << Term::color_fg(Term::Color::Name::Default) << "\n";
    
    Term::cout << "  Step: " << nursery->getCurrentStep() << "/5\n";
    
    // Show current phase
    Term::cout << "  Phase: ";
    GamePhase phase = nursery->getCurrentPhase();
    switch(phase)
    {
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
    Term::cout << "  " << Term::color_fg(Term::Color::Name::Cyan) << "Actions:" 
              << Term::color_fg(Term::Color::Name::Default) << "\n";
    Term::cout << "  ───────────────────────────────────────────────────────\n";
    
    // Build action menu based on current phase (reuse phase variable from above)
    std::vector<std::string> actions;
    
    if(phase == GamePhase::IDLE)
    {
        actions.push_back("Start New Day");
        actions.push_back("View Inventory");
        actions.push_back("Return to Menu");
    }
    else if(phase == GamePhase::DAY_START || phase == GamePhase::STEP_BREAK)
    {
        actions.push_back("Advance Step");
        actions.push_back("Plant Seeds");
        actions.push_back("View Inventory");
        actions.push_back("Return to Menu");
    }
    else if(phase == GamePhase::DAY_END)
    {
        actions.push_back("Start New Day");
        actions.push_back("Plant Seeds");
        actions.push_back("View Inventory");
        if(nursery->canHire())
            actions.push_back("Hire Staff");
        if(nursery->canSave())
            actions.push_back("Save Game");
        actions.push_back("Return to Menu");
    }
    
    // Display actions with selection highlight
    for (std::size_t i = 0; i < actions.size(); ++i)
    {
        Term::cout << "    ";
        
        if (i == selectedOption)
        {
            Term::cout << Term::color_fg(Term::Color::Name::Black) 
                      << Term::color_bg(Term::Color::Name::White)
                      << Term::style(Term::Style::Bold);
            Term::cout << " > " << actions[i] << " ";
            Term::cout << Term::style(Term::Style::Reset) 
                      << Term::color_fg(Term::Color::Name::Default)
                      << Term::color_bg(Term::Color::Name::Default);
        }
        else
        {
            Term::cout << "   " << actions[i];
        }
        
        Term::cout << "\n";
    }
    
    Term::cout << "\n";
    Term::cout << Term::color_fg(Term::Color::Name::Gray);
    Term::cout << "  ───────────────────────────────────────────────────────\n";
    // Show command activity for this step
    Term::cout << "  Commands completed this step: ";
    auto completed = nursery->getCompletedCommandsThisStep();
    if (completed.empty()) {
        Term::cout << "(none)\n";
    } else {
        Term::cout << "\n";
        for (const auto &c : completed) {
            Term::cout << "    - " << c << "\n";
        }
    }

    Term::cout << "  Commands remaining in queue: ";
    auto remaining = nursery->getRemainingCommandsAtStepEnd();
    if (remaining.empty()) {
        Term::cout << "(none)\n";
    } else {
        Term::cout << "\n";
        for (const auto &r : remaining) {
            Term::cout << "    - " << r << "\n";
        }
    }

    Term::cout << "  Customers left this step: ";
    Term::cout << Term::color_fg(Term::Color::Name::Red) << nursery->getCustomersLeftThisStep() << Term::color_fg(Term::Color::Name::Default) << "\n";

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
void renderInventoryView(const std::shared_ptr<Nursery>& nursery)
{
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
    Term::cout << "=== INVENTORY ===\n\n";
    Term::cout << "[Inventory View - Stub]\n";
    Term::cout << "\nPress any key to return...\n";
    Term::cout << std::flush;
}

/**
 * @brief Renders the plant seeds menu
 * @param nursery The game state
 * @param selectedOption Currently selected plant type (0-16)
 * 
 * TODO: Implement seed planting menu:
 * - List all 17 plant types with seed costs
 * - Show which are affordable
 * - Allow planting (deduct money, add to inventory)
 */
void renderPlantSeedsMenu(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption)
{
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
    Term::cout << "=== PLANT SEEDS ===\n\n";
    Term::cout << "Money: R" << nursery->getMoney() << "\n\n";
    Term::cout << "[Plant Seeds Menu - Stub]\n";
    Term::cout << "Selected: " << selectedOption << "\n";
    Term::cout << "\nPress ESC to return\n";
    Term::cout << std::flush;
}

/**
 * @brief Renders the hire staff menu
 * @param nursery The game state
 * @param selectedOption Currently selected staff type
 * 
 * TODO: Implement staff hiring:
 * - Show available staff types (Gardener, Cashier)
 * - Display costs
 * - Allow hiring (add to chain, deduct cost)
 * - Only available when canHire() == true
 */
void renderHireStaffMenu(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption)
{
    Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
    Term::cout << "=== HIRE STAFF ===\n\n";
    Term::cout << "Money: R" << nursery->getMoney() << "\n\n";
    Term::cout << "[Hire Staff Menu - Stub]\n";
    Term::cout << "Selected: " << selectedOption << "\n";
    Term::cout << "\nPress ESC to return\n";
    Term::cout << std::flush;
}

/**
 * @brief Renders the save game screen (file browser + new-save buffer)
 */
void renderSaveGameScreen(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption, const std::string& editBuffer)
{
    Term::cout << Term::clear_screen() << Term::cursor_move(1,1);
    Term::cout << Term::style(Term::Style::Bold) << "=== SAVE GAME ===" << Term::style(Term::Style::Reset) << "\n\n";

    // Gather save files from ./saves
    const std::string saveDir = "saves";
    std::vector<std::string> files;
    DIR* dir = opendir(saveDir.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn(entry->d_name);
            if (fn.size() >= 5 && fn.substr(fn.size()-5) == ".json") {
                std::string path = saveDir + "/" + fn;
                struct stat st;
                if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                    files.push_back(fn);
                }
            }
        }
        closedir(dir);
    }

    Term::cout << "Money: R" << nursery->getMoney() << "  Day: " << nursery->getCurrentDay() << "\n\n";

    Term::cout << "  0) New Save..." << (selectedOption==0?" <-":"") << "\n";
    for (std::size_t i = 0; i < files.size(); ++i) {
        Term::cout << "  " << (i+1) << ") " << files[i];
        if (selectedOption == (std::size_t)(i+1)) Term::cout << "  <-";
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
void renderLoadGameScreen(const std::shared_ptr<Nursery>& nursery, std::size_t selectedOption)
{
    Term::cout << Term::clear_screen() << Term::cursor_move(1,1);
    Term::cout << Term::style(Term::Style::Bold) << "=== LOAD GAME ===" << Term::style(Term::Style::Reset) << "\n\n";

    const std::string saveDir = "saves";
    std::vector<std::string> files;
    DIR* dir = opendir(saveDir.c_str());
    if (!dir) {
        // Save directory missing — inform user; main loop will switch to NO_SAVES
        Term::cout << "No save directory found ('./saves'). Press any key to return..." << std::flush;
        return;
    }
    else {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn(entry->d_name);
            if (fn.size() >= 5 && fn.substr(fn.size()-5) == ".json") {
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
        Term::cout << "  " << (i+1) << ") ";
        if (selectedOption == i) {
            Term::cout << Term::color_fg(Term::Color::Name::Black) << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                      << files[i]
                      << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default) << Term::color_bg(Term::Color::Name::Default);
        } else {
            Term::cout << files[i];
        }
        Term::cout << "\n";
    }

    // Add an explicit "Return to Main Menu" option as the last selectable item
    std::size_t returnIndex = files.size();
    Term::cout << "\n  ";
    if (selectedOption == returnIndex) {
        Term::cout << Term::color_fg(Term::Color::Name::Black) << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                  << " > Return to Main Menu " << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default) << Term::color_bg(Term::Color::Name::Default) << "\n";
    } else {
        Term::cout << "   Return to Main Menu\n";
    }

    Term::cout << "\nNavigate with ↑/↓, Enter to load/select, ESC to cancel" << std::flush;
}

// Check whether there are any save files in ./saves
bool hasSaveFiles()
{
    const std::string saveDir = "saves";
    DIR* dir = opendir(saveDir.c_str());
    if (!dir) return false;
    bool found = false;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fn(entry->d_name);
        if (fn.size() >= 5 && fn.substr(fn.size()-5) == ".json") {
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
void renderNoSavesScreen(std::size_t selectedOption)
{
    Term::cout << Term::clear_screen() << Term::cursor_move(1,1);
    Term::cout << Term::style(Term::Style::Bold) << "=== LOAD GAME ===" << Term::style(Term::Style::Reset) << "\n\n";
    Term::cout << "No save files found in './saves/'.\n\n";

    // Single option: Return to Main Menu
    Term::cout << "  ";
    if (selectedOption == 0) {
        Term::cout << Term::color_fg(Term::Color::Name::Black) << Term::color_bg(Term::Color::Name::White) << Term::style(Term::Style::Bold)
                  << " > Return to Main Menu " << Term::style(Term::Style::Reset) << Term::color_fg(Term::Color::Name::Default) << Term::color_bg(Term::Color::Name::Default) << "\n";
    } else {
        Term::cout << "   Return to Main Menu \n";
    }

    Term::cout << "\nUse ↑/↓ to navigate (only one option), Enter to select.\n" << std::flush;
}

// Input handler for the no-saves screen: navigate and return to MAIN_MENU on Enter or Esc
Screen handleNoSavesInput(Term::Event& event, std::size_t& selectedOption)
{
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
bool textInputPrompt(const std::string& title, const std::string& prompt, const std::string& initial, std::string &out)
{
    std::string buffer = initial;
    const int boxW = 64;
    const int startRow = 8;
    const int startCol = 6;

    while (true) {
        Term::cout << Term::cursor_move(startRow, startCol);
        Term::cout << " "; for (int i = 0; i < boxW; ++i) Term::cout << "─"; Term::cout << " \n";
        Term::cout << Term::cursor_move(startRow + 1, startCol) << "│ " << Term::style(Term::Style::Bold) << title << Term::style(Term::Style::Reset);
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

        Term::cout << Term::cursor_move(startRow + 5, startCol) << " "; for (int i = 0; i < boxW; ++i) Term::cout << "─"; Term::cout << " \n";
        Term::cout << Term::style(Term::Style::Reset) << std::flush;

        Term::Event ev = Term::read_event();
        if (ev.type() == Term::Event::Type::Key) {
            Term::Key k(ev);
            if (k == Term::Key::Enter) {
                out = buffer;
                return true;
            }
            if (k == Term::Key::Esc || k == Term::Key::q) {
                return false;
            }
            if (k == Term::Key::Backspace) {
                if (!buffer.empty()) buffer.pop_back();
            }
            else {
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

bool confirmPrompt(const std::string& title, const std::string& question)
{
    // Improved modal overlay: centered bordered box with Yes/No buttons and keyboard navigation
    const int boxWidth = 64;
    // Simple static placement: center horizontally at column 4 (approx), vertical offset 6
    // (Avoid querying terminal size for portability; fixed placement should be fine for typical terminals.)
    const int startRow = 6;
    const int startCol = 6;

    // title + spacer + question + spacer + buttons + padding
    // Default to No selected (safer for destructive actions)
    bool selectionIsYes = false; // false -> No selected, true -> Yes selected

    while (true) {
        // Draw box border
        Term::cout << Term::cursor_move(startRow, startCol);
        Term::cout << Term::color_bg(Term::Color::Name::Black) << Term::color_fg(Term::Color::Name::White);

    // Top border
    Term::cout << " ";
    for (int i = 0; i < boxWidth; ++i) Term::cout << "─"; // box-drawing
    Term::cout << " \n";

        // Title line
        Term::cout << Term::cursor_move(startRow + 1, startCol);
    Term::cout << "│ ";
        Term::cout << Term::style(Term::Style::Bold) << title << Term::style(Term::Style::Reset);
        // fill rest of line
        int titleLen = static_cast<int>(title.size()) + 1;
        for (int i = 0; i < boxWidth - titleLen; ++i) Term::cout << ' ';
    Term::cout << " │\n"; // │

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
            Term::cout << Term::color_bg(Term::Color::Name::White) << Term::color_fg(Term::Color::Name::Black) << noLabel
                      << Term::color_bg(Term::Color::Name::Default) << Term::color_fg(Term::Color::Name::Default);
        } else {
            Term::cout << noLabel;
        }

        // Spacer between buttons
        for (int i = 0; i < 6; ++i) Term::cout << ' ';

        // Yes button (right)
        if (selectionIsYes) {
            Term::cout << Term::color_bg(Term::Color::Name::White) << Term::color_fg(Term::Color::Name::Black) << yesLabel
                      << Term::color_bg(Term::Color::Name::Default) << Term::color_fg(Term::Color::Name::Default);
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
            }
            else if (k == Term::Key::ArrowRight || k == Term::Key::ArrowDown) {
                // Move selection right -> Yes
                selectionIsYes = true;
            }
            else if (k == Term::Key::Enter) {
                return selectionIsYes;
            }
            else if (k == Term::Key::y || k == Term::Key::Y) {
                return true;
            }
            else if (k == Term::Key::n || k == Term::Key::N) {
                return false;
            }
            else if (k == Term::Key::Esc || k == Term::Key::q) {
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
Screen handleMainMenuInput(Term::Event& event, std::size_t& selectedOption, bool& running, std::shared_ptr<Nursery>& nursery)
{
    const std::size_t menuItemCount = 3;  // Load Game, New Game, Exit
    
    if(event.type() == Term::Event::Type::Key)
    {
        Term::Key key(event);
        
        // Navigation
        if(key == Term::Key::ArrowUp)
        {
            if(selectedOption > 0)
                selectedOption--;
            else
                selectedOption = menuItemCount - 1;  // Wrap to bottom
        }
        else if(key == Term::Key::ArrowDown)
        {
            selectedOption = (selectedOption + 1) % menuItemCount;  // Wrap to top
        }
        // Selection
        else if(key == Term::Key::Enter)
        {
            switch(selectedOption)
            {
                case 0:  // Load Game
                    // If save files exist, go to LOAD_GAME; otherwise show NO_SAVES screen
                    if (hasSaveFiles()) return Screen::LOAD_GAME;
                    else return Screen::NO_SAVES;
                    
                case 1:  // New Game
                {
                    bool ok = confirmPrompt("Start New Game", "Start a new game?");
                    if (!ok) break; // cancel
                    nursery = std::make_shared<Nursery>();
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
        else if(key == Term::Key::q || key == Term::Key::Esc)
        {
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
Screen handleGameDashboardInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery)
{
    if(event.type() != Term::Event::Type::Key)
        return Screen::GAME_DASHBOARD;
    
    Term::Key key(event);
    
    // Build action list based on phase (must match renderGameDashboard)
    std::vector<std::string> actions;
    GamePhase phase = nursery->getCurrentPhase();
    
    if(phase == GamePhase::IDLE)
    {
        actions.push_back("Start New Day");
        actions.push_back("View Inventory");
        actions.push_back("Return to Menu");
    }
    else if(phase == GamePhase::DAY_START || phase == GamePhase::STEP_BREAK)
    {
        actions.push_back("Advance Step");
        actions.push_back("Plant Seeds");
        actions.push_back("View Inventory");
        actions.push_back("Return to Menu");
    }
    else if(phase == GamePhase::DAY_END)
    {
        actions.push_back("Start New Day");
        actions.push_back("Plant Seeds");
        actions.push_back("View Inventory");
        if(nursery->canHire())
            actions.push_back("Hire Staff");
        if(nursery->canSave())
            actions.push_back("Save Game");
        actions.push_back("Return to Menu");
    }
    
    // Clamp selection to valid range (in case menu size changed)
    if(selectedOption >= actions.size())
        selectedOption = 0;
    
    // Navigation
    if(key == Term::Key::ArrowUp)
    {
        if(selectedOption > 0)
            selectedOption--;
        else
            selectedOption = actions.size() - 1;
    }
    else if(key == Term::Key::ArrowDown)
    {
        selectedOption = (selectedOption + 1) % actions.size();
    }
    // Selection
    else if(key == Term::Key::Enter)
    {
        std::string selectedAction = actions[selectedOption];
        
        if(selectedAction == "Start New Day")
        {
            nursery->startNewDay();
            selectedOption = 0;  // Reset to first action
        }
        else if(selectedAction == "Advance Step")
        {
            nursery->advanceStep();
            selectedOption = 0;  // Reset to first action
        }
        else if(selectedAction == "Plant Seeds")
        {
            selectedOption = 0;
            return Screen::PLANT_SEEDS;
        }
        else if(selectedAction == "View Inventory")
        {
            return Screen::INVENTORY_VIEW;
        }
        else if(selectedAction == "Hire Staff")
        {
            selectedOption = 0;
            return Screen::HIRE_STAFF;
        }
        else if(selectedAction == "Save Game")
        {
            selectedOption = 0;
            return Screen::SAVE_GAME;
        }
        else if(selectedAction == "Return to Menu")
        {
            // Confirm before returning to main menu via action menu
            {
                bool ok = confirmPrompt("Return to Menu", "Return to the main menu? Unsaved progress will be lost.");
                if (ok) {
                    // User expects nursery to be deallocated when returning to main menu
                    nursery.reset();
                    return Screen::MAIN_MENU;
                }
            }
        }
    }
    // Quick shortcuts
    else if(key == Term::Key::q || key == Term::Key::Esc)
    {
        // Confirm before returning to main menu
        bool ok = confirmPrompt("Return to Menu", "Return to the main menu? Unsaved progress will be lost.");
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
Screen handleInventoryInput(Term::Event& event)
{
    if(event.type() == Term::Event::Type::Key)
    {
        return Screen::GAME_DASHBOARD;
    }
    
    return Screen::INVENTORY_VIEW;
}

/**
 * @brief Handles input on plant seeds menu
 * @param event The input event
 * @param selectedOption Currently selected plant type
 * @param nursery Game state
 * @return Next screen to display
 * 
 * TODO: Implement seed planting logic
 */
Screen handlePlantSeedsInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery)
{
    if(event.type() == Term::Event::Type::Key)
    {
        Term::Key key(event);
        if(key == Term::Key::q || key == Term::Key::Esc)
        {
            return Screen::GAME_DASHBOARD;
        }
        // TODO: Handle plant selection and planting
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
 * TODO: Implement staff hiring logic
 */
Screen handleHireStaffInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery)
{
    if(event.type() == Term::Event::Type::Key)
    {
        Term::Key key(event);
        if(key == Term::Key::q || key == Term::Key::Esc)
        {
            return Screen::GAME_DASHBOARD;
        }
        // TODO: Handle staff hiring
    }
    
    return Screen::HIRE_STAFF;
}

// -----------------------------
// Save / Load input handlers
// -----------------------------

Screen handleSaveGameInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery, std::string& editBuffer)
{
    const std::string saveDir = "saves";
    std::vector<std::string> files;
    DIR* dir = opendir(saveDir.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn(entry->d_name);
            if (fn.size() >= 5 && fn.substr(fn.size()-5) == ".json") {
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
        std::size_t maxIndex = files.size(); // number of existing files
        // selectedOption ranges from 0 (New Save) to maxIndex (last file index + 1)
        if (key == Term::Key::ArrowUp) {
            if (selectedOption > 0) selectedOption--; else selectedOption = maxIndex;
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
                bool ok = textInputPrompt("Save As", "Enter filename (no extension):", editBuffer, out);
                if (!ok) return Screen::SAVE_GAME;
                editBuffer = out;
                // ensure folder exists
                struct stat st;
                if (stat(saveDir.c_str(), &st) == -1) {
                    mkdir(saveDir.c_str(), 0755);
                }
                std::string name = editBuffer;
                if (name.size() < 5 || name.substr(name.size()-5) != ".json") name += ".json";
                std::string path = saveDir + "/" + name;
                if (SaveSystem::fileExists(path)) {
                    bool overwrite = confirmPrompt("Overwrite", "File exists. Overwrite?");
                    if (!overwrite) return Screen::SAVE_GAME;
                }
                SaveSystem saver;
                saver.save(nursery, path);
                return Screen::GAME_DASHBOARD;
            }
            else {
                std::size_t idx = selectedOption - 1;
                if (idx < files.size()) {
                    std::string path = saveDir + "/" + files[idx];
                    bool overwrite = confirmPrompt("Overwrite", std::string("Overwrite \"")+files[idx]+"\"?");
                    if (!overwrite) return Screen::SAVE_GAME;
                    SaveSystem saver;
                    saver.save(nursery, path);
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

Screen handleLoadGameInput(Term::Event& event, std::size_t& selectedOption, std::shared_ptr<Nursery>& nursery, std::string& editBuffer)
{
    // (no-op)
    const std::string saveDir = "saves";
    std::vector<std::string> files;
    DIR* dir = opendir(saveDir.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn(entry->d_name);
            if (fn.size() >= 5 && fn.substr(fn.size()-5) == ".json") {
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
    std::size_t maxIndex = files.size(); // last valid index is maxIndex (return option)

    // Ensure selection is within bounds (0 .. maxIndex)
    if (selectedOption > maxIndex) selectedOption = 0;

    if (event.type() == Term::Event::Type::Key) {
        Term::Key key(event);
        if (key == Term::Key::ArrowUp) {
            if (selectedOption > 0) selectedOption--; else selectedOption = maxIndex;
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

int main()
{
    try
    {
        // Initialize terminal
        Term::terminal.setOptions(Term::Option::Raw, 
                                  Term::Option::NoSignalKeys, 
                                  Term::Option::ClearScreen, 
                                  Term::Option::NoCursor);
        
        if(!Term::is_stdin_a_tty())
        {
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
        while(running)
        {
            // Render current screen
            switch(currentScreen)
            {
                case Screen::MAIN_MENU:
                    renderMainMenu(selectedOption);
                    break;
                    
                case Screen::GAME_DASHBOARD:
                    if(nursery) renderGameDashboard(nursery, selectedOption);
                    break;
                    
                case Screen::INVENTORY_VIEW:
                    if(nursery) renderInventoryView(nursery);
                    break;
                    
                case Screen::PLANT_SEEDS:
                    if(nursery) renderPlantSeedsMenu(nursery, selectedOption);
                    break;
                    
                case Screen::HIRE_STAFF:
                    if(nursery) renderHireStaffMenu(nursery, selectedOption);
                    break;
                    
                case Screen::SAVE_GAME:
                    if (nursery) renderSaveGameScreen(nursery, selectedOption, editBuffer);
                    break;

                case Screen::LOAD_GAME:
                    renderLoadGameScreen(nursery, selectedOption);
                    break;
                case Screen::NO_SAVES:
                    renderNoSavesScreen(selectedOption);
                    break;
            }
            

            // Handle input
            Term::Event event = Term::read_event();
            
            switch(currentScreen)
            {
                case Screen::MAIN_MENU:
                    currentScreen = handleMainMenuInput(event, selectedOption, running, nursery);
                    break;
                    
                case Screen::GAME_DASHBOARD:
                    if (nursery) currentScreen = handleGameDashboardInput(event, selectedOption, nursery);
                    else currentScreen = Screen::MAIN_MENU; // no nursery -> go back to main menu
                    break;
                    
                case Screen::INVENTORY_VIEW:
                    currentScreen = handleInventoryInput(event);
                    break;
                    
                case Screen::PLANT_SEEDS:
                    if (nursery) currentScreen = handlePlantSeedsInput(event, selectedOption, nursery);
                    else currentScreen = Screen::MAIN_MENU;
                    break;
                    
                case Screen::SAVE_GAME:
                    if (nursery) currentScreen = handleSaveGameInput(event, selectedOption, nursery, editBuffer);
                    else currentScreen = Screen::MAIN_MENU;
                    break;

                case Screen::LOAD_GAME:
                    currentScreen = handleLoadGameInput(event, selectedOption, nursery, editBuffer);
                    break;
                case Screen::NO_SAVES:
                    currentScreen = handleNoSavesInput(event, selectedOption);
                    break;
                    
                case Screen::HIRE_STAFF:
                    if (nursery) currentScreen = handleHireStaffInput(event, selectedOption, nursery);
                    else currentScreen = Screen::MAIN_MENU;
                    break;
                    
                default:
                    currentScreen = Screen::MAIN_MENU;
                    break;
            }

            // If we just switched screens, reset the selection so the top item is selected
            if (currentScreen != previousScreen) {
                selectedOption = 0;
            }
            // Update previousScreen for next iteration
            previousScreen = currentScreen;
        }
        
        // Cleanup
        Term::cout << Term::clear_screen() << Term::cursor_move(1, 1);
        Term::cout << "Thanks for playing!\n" << std::flush;
    }
    catch(const Term::Exception& e)
    {
        Term::cerr << "Terminal error: " << e.what() << std::endl;
        return 2;
    }
    catch(const std::exception& e)
    {
        Term::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch(...)
    {
        Term::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
    
    return 0;
}
