#ifndef LIFE_APP_STATE
#define LIFE_APP_STATE

#include <memory>
#include <tuple>
#include <vector>

#include "life.h"
#include "vect2.h"

struct AppState {
    // life state
    std::unique_ptr<Life> life;
    Vect2I selectedSquare;

    // for selecting ranges of squares
    bool selectingRange = false;
    Vect2I selectBegin;

    int pastingRangeIndex = -1;
    std::vector<std::tuple<std::string, std::vector<std::vector<bool>>>> saved_ranges;

    // Turns range select on and off
    void selectRange(bool);

    // for playing forward
    bool playing = false;
    int playFrames = 0;

    // Creates a new board.
    void newBoard(int x, int y);

    // input commands
    int prefix_argument = -1;
    std::string mode_line;

    // camera position
    float zoomFactor;
    Vect2I cameraPos;

    // window shape
    Vect2I windowDimensions;

    // run each frame
    void step();

    // input handlers
    void keyPress(int key, int action, int mods);
    void charPress(unsigned int codepoint, int mods);

    // executes the current mode line
    void run_mode_line();

    // whether a cell should be draw as dead or alive
    bool get(int x, int y);

    static char living_cell_char;
    static char dead_cell_char;
};

extern std::string mode_line_chars;

#endif
