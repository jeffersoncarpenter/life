#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>

#include <GLFW/glfw3.h>

#include "appState.h"

std::string mode_line_chars = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890[]!@#$%^&*(){}'\",.<>/=\\+;:-_";

void inc_prefix_argument(int& prefix_argument, int n) {
    if (prefix_argument == -1) {
	prefix_argument = n;
    }
    else {
	prefix_argument *= 10;
	prefix_argument += n;
    }
}
void pop_prefix_argument(int& prefix_argument) {
    prefix_argument /= 10;
    if (prefix_argument == 0) {
	prefix_argument = -1;
    }
}
int get_prefix_argument(int& prefix_argument) {
    int result = prefix_argument;
    prefix_argument = -1;
    if (result == -1) {
	result = 1;
    }
    return result;
}
bool prefix_argument_is_set(int& prefix_argument) {
    return prefix_argument != -1;
}

void AppState::newBoard(int x, int y) {
    life = std::make_unique<Life>(x, y);
    cameraPos = {x/2, y/2};
    selectedSquare = {x/2, y/2};
}

void AppState::selectRange(bool x) {
    if (!x) {
	selectingRange = false;
    }
    else if (!selectingRange) {
	selectingRange = true;
	selectBegin = selectedSquare;
    }
}

void AppState::keyPress(int key, int action, int mods) {
    if (action == GLFW_RELEASE) {
	return;
    }
    if (mode_line.size() > 0) {
	if (key == GLFW_KEY_ESCAPE) {
	    mode_line = "";
	    get_prefix_argument(prefix_argument);
	    return;
	}
	if (key == GLFW_KEY_BACKSPACE) {
	    if (mode_line[0] == ':') {
		mode_line = mode_line.substr(0, mode_line.size() - 1);
	    }
	    else {
		pop_prefix_argument(prefix_argument);
	    }
	}
	if (key == GLFW_KEY_ENTER) {
	    if (mode_line[0] == ':') {
		run_mode_line();
		return;
	    }
	}
    }
    if (key == GLFW_KEY_ESCAPE) {
	selectRange(false);
	pastingRangeIndex = -1;
    }
    if (key == GLFW_KEY_SPACE && mode_line.substr(0, 1) != ":") {
	get_prefix_argument(prefix_argument);
	if (pastingRangeIndex != -1) {
	    std::vector<std::vector<bool>> const& range = std::get<1>(saved_ranges[pastingRangeIndex]);
	    for (int y = selectBegin.y; y <= selectedSquare.y; y++) {
		std::vector<bool> const& row = range[y - selectBegin.y];
		for (int x = selectBegin.x; x <= selectedSquare.x; x++) {
		    life->set(x, y, row[x - selectBegin.x]);
		}
	    }
	}
	else if (!selectingRange) {
	    life->set(selectedSquare.x, selectedSquare.y, !life->get(selectedSquare.x, selectedSquare.y));
	}
	return;
    }
    if (key == GLFW_KEY_HOME) {
	cameraPos = selectedSquare;
    }
    if (key == GLFW_KEY_LEFT) {
	int ntimes = get_prefix_argument(prefix_argument);
	if (mods & GLFW_MOD_CONTROL) {
	    cameraPos.x = std::max(0, cameraPos.x - ntimes);
	}
	else {
	    if (mods & GLFW_MOD_SHIFT) {
		selectRange(true);
	    }
	    if (selectingRange && !(mods & GLFW_MOD_SHIFT)) {
		ntimes = std::min(ntimes, std::min(selectedSquare.x, selectBegin.x));
		selectedSquare.x -= ntimes;
		selectBegin.x -= ntimes;
	    }
	    else if (pastingRangeIndex == -1) {
		selectedSquare.x = std::max(0, selectedSquare.x - ntimes);
	    }
	}
	return;
    }
    if (key == GLFW_KEY_RIGHT) {
	int ntimes = get_prefix_argument(prefix_argument);
	if (mods & GLFW_MOD_CONTROL) {
	    cameraPos.x = std::min(life->x_size() - 1, cameraPos.x + ntimes);
	}
	else {
	    if (mods & GLFW_MOD_SHIFT) {
		selectRange(true);
	    }
	    if (selectingRange && !(mods & GLFW_MOD_SHIFT)) {
		ntimes = std::min(ntimes, life->x_size() - 1 - std::max(selectedSquare.x, selectBegin.x));
		selectedSquare.x += ntimes;
		selectBegin.x += ntimes;
	    }
	    else if (pastingRangeIndex == -1) {
		selectedSquare.x = std::min(life->x_size() - 1, selectedSquare.x + ntimes);
	    }
	}
	return;
    }
    if (key == GLFW_KEY_DOWN) {
	int ntimes = get_prefix_argument(prefix_argument);
	if (mods & GLFW_MOD_CONTROL) {
	    cameraPos.y = std::max(0, cameraPos.y - ntimes);
	}
	else {
	    if (mods & GLFW_MOD_SHIFT) {
		selectRange(true);
	    }
	    if (selectingRange && !(mods & GLFW_MOD_SHIFT)) {
		ntimes = std::min(ntimes, std::min(selectedSquare.y, selectBegin.y));
		selectedSquare.y -= ntimes;
		selectBegin.y -= ntimes;
	    }
	    else if (pastingRangeIndex == -1) {
		selectedSquare.y = std::max(0, selectedSquare.y - ntimes);
	    }
	}
	return;
    }
    if (key == GLFW_KEY_UP) {
	int ntimes = get_prefix_argument(prefix_argument);
	if (mods & GLFW_MOD_CONTROL) {
	    cameraPos.y = std::min(life->y_size() - 1, cameraPos.y + ntimes);
	}
	else {
	    if (mods & GLFW_MOD_SHIFT) {
		selectRange(true);
	    }
	    if (selectingRange && !(mods & GLFW_MOD_SHIFT)) {
		ntimes = std::min(ntimes, life->y_size() - 1 - std::max(selectedSquare.y, selectBegin.y));
		selectedSquare.y += ntimes;
		selectBegin.y += ntimes;
	    }
	    else if (pastingRangeIndex == -1) {
		selectedSquare.y = std::min(life->y_size() - 1, selectedSquare.y + ntimes);
	    }
	}
	return;
    }
    if (key == GLFW_KEY_ENTER) {
	int ntimes = get_prefix_argument(prefix_argument);
	auto now = std::chrono::high_resolution_clock::now();
	life->step(ntimes);
	std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - now;
	std::cout << duration.count() << " seconds" <<std::endl;
	return;
    }
    if (key == GLFW_KEY_PAGE_DOWN) {
	int ntimes = get_prefix_argument(prefix_argument);
	for (int i = 0; i < ntimes; i++) {
	    zoomFactor /= 1.1;
	}
	return;
    }
    if (key == GLFW_KEY_PAGE_UP) {
	int ntimes = get_prefix_argument(prefix_argument);
	for (int i = 0; i < ntimes; i++) {
	    zoomFactor *= 1.1;
	}
	return;
    }
    if (mode_line.substr(0, 1) != ":") {
	if (key == GLFW_KEY_0) {
	    inc_prefix_argument(prefix_argument, 0);
	    return;
	}
	if (key == GLFW_KEY_1) {
	    inc_prefix_argument(prefix_argument, 1);
	    return;
	}
	if (key == GLFW_KEY_2) {
	    inc_prefix_argument(prefix_argument, 2);
	    return;
	}
	if (key == GLFW_KEY_3) {
	    inc_prefix_argument(prefix_argument, 3);
	    return;
	}
	if (key == GLFW_KEY_4) {
	    inc_prefix_argument(prefix_argument, 4);
	    return;
	}
	if (key == GLFW_KEY_5) {
	    inc_prefix_argument(prefix_argument, 5);
	    return;
	}
	if (key == GLFW_KEY_6) {
	    inc_prefix_argument(prefix_argument, 6);
	    return;
	}
	if (key == GLFW_KEY_7) {
	    inc_prefix_argument(prefix_argument, 7);
	    return;
	}
	if (key == GLFW_KEY_8) {
	    inc_prefix_argument(prefix_argument, 8);
	    return;
	}
	if (key == GLFW_KEY_9) {
	    inc_prefix_argument(prefix_argument, 9);
	    return;
	}
    }
}

void AppState::charPress(unsigned int codepoint, int mods) {
    if (mode_line.size() == 0 || mode_line.substr(0, 1) != ":") {
	if (codepoint == ':') {
	    mode_line = ":";
	    get_prefix_argument(prefix_argument);
	}
    }
    else if (mode_line.substr(0, 1) == ":") {
	if (mode_line_chars.find(codepoint) != std::string::npos) {
	    mode_line += codepoint;
	}
    }
}

void AppState::run_mode_line() {
    // command is ":[letter]" at the beginning of the mode line
    auto command = mode_line.substr(0, 2);

    // arg is everything following the command excluding spaces
    auto arg = mode_line.substr(2);
    while (arg.substr(0, 1) == " ") {
	arg = arg.substr(1);
    }

    if (command == ":q") {
	exit(0); //ahhahahah OS can clean this up
    }
    else if (command == ":c") {
	if (selectingRange) {
	    if (pastingRangeIndex == -1) {
		int min_x = std::min(selectBegin.x, selectedSquare.x);
		int max_x = std::max(selectBegin.x, selectedSquare.x);
		int min_y = std::min(selectBegin.y, selectedSquare.y);
		int max_y = std::max(selectBegin.y, selectedSquare.y);

		for (int y = min_y; y <= max_y; y++) {
		    for (int x = min_x; x <= max_x; x++) {
			life->set(x, y, false);
		    }
		}
	    }
	}
	else {
	    life->clear();
	}
    }
    else if (command == ":s" || command == ":w") {
	std::fstream out(arg, std::fstream::out | std::fstream::trunc);
	int x_size = life->x_size();
	int y_size = life->y_size();
	out << x_size << " " << y_size << std::endl;
	for (int y = y_size - 1; y >= 0; y--) {
	    for (int x = 0; x < x_size; x++) {
		out << (life->get(x, y) ? living_cell_char : dead_cell_char);
	    }
	    out << std::endl;
	}
	out.close();
    }
    else if (command == ":l" || command == ":o") {
	selectingRange = false;
	pastingRangeIndex = -1;
	std::fstream in(arg, std::fstream::in);
	std::string x_str;
	std::string y_str;
	in >> x_str;
	if (in.fail()) {
	    goto end;
	}
	in >> y_str;
	if (in.fail()) {
	    goto end;
	}
	size_t idx;
	int x_size = stoi(x_str, &idx);
	if (idx != x_str.size()) {
	    goto end;
	}
	int y_size = stoi(y_str, &idx);
	if (idx != y_str.size()) {
	    goto end;
	}
	newBoard(x_size, y_size);
	std::string line;
	for (int y = y_size - 1; y >= 0; y--) {
	    in >> line;
	    if (in.fail() || line.size() != x_size) {
		goto end;
	    }
	    for (int x = 0; x < x_size; x++) {
		char c = line[x];
		if (c == living_cell_char) {
		    life->set(x, y, true);
		}
		else if (c == dead_cell_char) {
		    life->set(x, y, false);
		}
		else {
		    goto end;
		}
	    }
	}
    }
    else if (command == ":n") {
	size_t space_pos = arg.find(' ');
	if (space_pos == std::string::npos) {
	    goto end;
	}
	std::string x_str = arg.substr(0, space_pos);
	std::string y_str = arg.substr(space_pos + 1);
	size_t idx;
	int x = stoi(x_str, &idx);
	if (idx != x_str.size()) {
	    goto end;
	}
	int y = stoi(y_str, &idx);
	if (idx != y_str.size()) {
	    goto end;
	}
	newBoard(x, y);
    }
    else if (command == ":y") {
	if (arg == "") {
	    goto end;
	}
	if (selectingRange && pastingRangeIndex == -1) {
	    // copy range from life board
	    int min_x = std::min(selectedSquare.x, selectBegin.x);
	    int max_x = std::max(selectedSquare.x, selectBegin.x);

	    int min_y = std::min(selectedSquare.y, selectBegin.y);
	    int max_y = std::max(selectedSquare.y, selectBegin.y);

	    std::vector<std::vector<bool>> range;

	    range.resize(max_y - min_y + 1);
	    for (int y = min_y; y <= max_y; y++) {
		std::vector<bool>& row = range[y - min_y];
		row.resize(max_x - min_x + 1);
		for (int x = min_x; x <= max_x; x++) {
		    row[x - min_x] = life->get(x, y);
		}
	    }

	    // update saved range if it exists, else add it
	    bool exists = false;
	    for (int i = 0; i < saved_ranges.size(); i++) {
		if (arg == std::get<0>(saved_ranges[i])) {
		    std::get<1>(saved_ranges[i]) = range;
		    exists = true;
		    pastingRangeIndex = i;
		}
	    }
	    if (!exists) {
		pastingRangeIndex = saved_ranges.size();
		saved_ranges.push_back({arg, range});
	    }
	    selectBegin.x = min_x;
	    selectBegin.y = min_y;
	    selectedSquare.x = max_x;
	    selectedSquare.y = max_y;
	}
	else {
	    for (int i = 0; i < saved_ranges.size(); i++) {
		if (arg == std::get<0>(saved_ranges[i])) {
		    int min_selected_x = selectedSquare.x;
		    int min_selected_y = selectedSquare.y;
		    int range_size_x = std::get<1>(saved_ranges[i])[0].size();
		    int range_size_y = std::get<1>(saved_ranges[i]).size();
		    if (selectingRange) {
			min_selected_x = std::min(min_selected_x, selectBegin.x);
			min_selected_y = std::min(min_selected_y, selectBegin.y);
		    }
		    min_selected_x = std::min(min_selected_x, life->x_size() - range_size_x);
		    min_selected_y = std::min(min_selected_y, life->y_size() - range_size_y);
		    selectBegin.x = min_selected_x;
		    selectBegin.y = min_selected_y;
		    selectedSquare.x = min_selected_x + range_size_x - 1;
		    selectedSquare.y = min_selected_y + range_size_y - 1;

		    selectingRange = true;
		    pastingRangeIndex = i;
		    goto end;
		}
	    }
	    selectingRange = false;
	    pastingRangeIndex = -1;
	}
    }
    else if (command == ":p") {
	playing = !playing;
    }
  end:
    mode_line = "";
}

void AppState::step() {
    if (prefix_argument_is_set(prefix_argument)) {
	mode_line = std::to_string(prefix_argument);
    }
    else if (mode_line.substr(0, 1) != ":") {
	mode_line = "";
    }

    if (playing) {
	playFrames++;
	if (playFrames == 10) {
	    playFrames = 0;
	    life->step();
	}
    }
}

bool AppState::get(int x, int y) {
    if (pastingRangeIndex != -1) {
	if (selectBegin.x <= x && x <= selectedSquare.x &&
	    selectBegin.y <= y && y <= selectedSquare.y) {
	    std::vector<std::vector<bool>> const& selectedRange = std::get<1>(saved_ranges[pastingRangeIndex]);
	    return selectedRange[y - selectBegin.y][x - selectBegin.x];
	}
    }
    return life->get(x, y);
}

char AppState::living_cell_char = '*';
char AppState::dead_cell_char = '.';
