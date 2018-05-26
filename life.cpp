#include <cstring>
#include <sstream>
#include <utility>

#include "life.h"

Life::Life(int x, int y) {
    _x_size = x;
    _y_size = y;
    x += 2;
    y += 2;
    cells = std::make_unique<bool[]>(x * y);
    cells2 = std::make_unique<bool[]>(x * y);
    memset(cells.get(), 0, x * y * sizeof(cells[0]));
    memset(cells2.get(), 0, x * y * sizeof(cells[0]));
}

int Life::x_size() {
    return _x_size;
}

int Life::y_size() {
    return _y_size;
}

bool Life::get(int x, int y) {
    if (x < 0 || x >= _x_size || y < 0 || y >= _y_size) {
	throw "Out of range.";
    }
    return cells[(y + 1) * (_x_size + 2) + (x + 1)];
}

void Life::set(int x, int y, bool state) {
    if (x < 0 || x >= _x_size || y < 0 || y >= _y_size) {
	throw "Out of range.";
    }
    cells[(y + 1) * (_x_size + 2) + (x + 1)] = state;
}

void Life::step(int n) {
    for (int i = 0; i < n; i++) {
	for (int y_idx = 0; y_idx < _y_size; y_idx++) {
	    int y = y_idx + 1;
	    for (int x_idx = 0; x_idx < _x_size; x_idx++) {
		int x = x_idx + 1;
		int living_self = cells[y * (_x_size + 2) + x];
		int living_neighbors = 0;
		living_neighbors += cells[(y - 1) * (_x_size + 2) + (x - 1)];
		living_neighbors += cells[(y - 1) * (_x_size + 2) + x];
		living_neighbors += cells[(y - 1) * (_x_size + 2) + (x + 1)];
		living_neighbors += cells[y * (_x_size + 2) + (x - 1)];
		living_neighbors += cells[y * (_x_size + 2) + (x + 1)];
		living_neighbors += cells[(y + 1) * (_x_size + 2) + (x - 1)];
		living_neighbors += cells[(y + 1) * (_x_size + 2) + x];
		living_neighbors += cells[(y + 1) * (_x_size + 2) + (x + 1)];
		cells2[y * (_x_size + 2) + x] = living_self && living_neighbors == 2 || living_neighbors == 3;
	    }
	}
	std::swap(cells, cells2);
    }
}

void Life::clear() {
    memset(cells.get(), 0, (_x_size + 2) * (_y_size + 2) * sizeof(cells[0]));
}
