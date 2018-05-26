#ifndef LIFE_LIFE_H
#define LIFE_LIFE_H

#include <fstream>
#include <memory>
#include <string>

class Life {
    std::unique_ptr<bool[]> cells;
    std::unique_ptr<bool[]> cells2;
    int _x_size;
    int _y_size;

public:
    Life(int x, int y);

    // steps game forward N steps
    void step(int n = 1);

    // clear the board
    void clear();

    // get board x and y size
    int x_size();
    int y_size();

    // cell aliveness getter and setter
    bool get(int x, int y);
    void set(int x, int y, bool state);

    // delete copy constructor and assignment operator
    Life(Life&) = delete;
    Life& operator=(Life const& op2) = delete;
};

#endif
