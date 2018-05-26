#include <iostream>
#include <vector>
#include <thread>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "appState.h"
#include "assets.h"
#include "compile_assets.h"
#include "life.h"
#include "vect2.h"

using namespace std;

const glm::mat4 identity_matrix;

void sleepFrame() {
    double startTime = glfwGetTime();
    while (glfwGetTime() - startTime < (16.0 / 1000)) {
        this_thread::sleep_for(chrono::milliseconds(0));
    }
}

int main() {
    // app state
    AppState appState;
    appState.newBoard(50, 50);
    appState.zoomFactor = 8.0;
    appState.windowDimensions = {800, 600};

    glewExperimental = GL_TRUE;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(appState.windowDimensions.x, appState.windowDimensions.y, "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    glewInit();

    std::vector<AtlasMesh> atlas_meshes;
    std::vector<AtlasModel> atlas_models;

    GLuint modelUniform;
    GLuint viewUniform;
    GLuint projUniform;

    compile_assets(
	all_textures(),
	all_meshes(),
	atlas_meshes,
	modelUniform,
	viewUniform,
	projUniform);

    map_atlas_models(all_models(), atlas_meshes, atlas_models);

    auto font_test_square = atlas_model_find(atlas_models, "font_test_square");
    auto square_1_1_gray = atlas_model_find(atlas_models, "square_1_1_gray");
    auto square_1_1_white = atlas_model_find(atlas_models, "square_1_1_white");
    auto square_1_1_red = atlas_model_find(atlas_models, "square_1_1_red");
    auto square_1_1_black = atlas_model_find(atlas_models, "square_1_1_black");

    AtlasModel* font_models[256] = {};
    for (int i = 0; i < mode_line_chars.size(); i++) {
	char c = mode_line_chars[i];
	font_models[c] = &atlas_model_find(atlas_models, std::string("char_") + c);
    }

    // set up glfw callbacks
    glfwSetWindowUserPointer(window, &appState);
#define getAppState(w) (*(AppState *)glfwGetWindowUserPointer(w));

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        AppState &state = getAppState(window);
	state.keyPress(key, action, mods);
    });
    glfwSetCharModsCallback(window, [](GLFWwindow *window, unsigned int codepoint, int mods) {
	AppState &state = getAppState(window);
	state.charPress(codepoint, mods);
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int w, int h) {
	AppState &state = getAppState(window);
        state.windowDimensions.x = w;
        state.windowDimensions.y = h;
    });

    // main loop
    while (!glfwWindowShouldClose(window)) {
        thread fpsThread(sleepFrame);

	glm::mat4 projMatrix = glm::ortho(-(float)appState.windowDimensions.x / 2.0f, (float)appState.windowDimensions.x / 2.0f, -(float)appState.windowDimensions.y / 2.0f, (float)appState.windowDimensions.y / 2.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(projUniform, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glViewport(0, 0, appState.windowDimensions.x, appState.windowDimensions.y);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// current life size
	int x_size = appState.life->x_size();
	int y_size = appState.life->y_size();

	// game config constants
	double cell_size = appState.zoomFactor; // ratio of cell size to border width

	glm::mat4 view_matrix = glm::lookAt(
	    glm::vec3(appState.cameraPos.x * (cell_size + 3.0f), appState.cameraPos.y * (cell_size + 3.0f), 1.0f),
	    glm::vec3(appState.cameraPos.x * (cell_size + 3.0f), appState.cameraPos.y * (cell_size + 3.0f), 0.0f),
	    glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view_matrix));
	// draw live cells
	{
	    for (int x = 0; x < x_size; x++) {
		for (int y = 0; y < y_size; y++) {
		    if (appState.get(x, y)) {
			glm::mat4 model_matrix = glm::scale(
			    glm::translate(
				identity_matrix,
				{x * (cell_size + 3.0) + 2.0, y * (cell_size + 3.0) + 2.0, 0.0}),
			    {cell_size, cell_size, 1.0});
			glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model_matrix));
			draw_model(square_1_1_white);
		    }
		}
	    }
	}

	// draw grid lines
	{
	    glm::mat4 y_scaled_matrix = glm::scale(identity_matrix, {1.0, y_size * (cell_size + 3.0) + 1.0, 1.0});
	    for (int x = 0; x < x_size + 1; x++) {
		glm::mat4 model_matrix = glm::translate(y_scaled_matrix, {(cell_size + 3.0) * x, 0.0, 0.0});
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model_matrix));
		draw_model(square_1_1_gray);
	    }
	    glm::mat4 x_scaled_matrix = glm::scale(identity_matrix, {x_size * (cell_size + 3.0) + 1.0, 1.0, 1.0});
	    for (int y = 0; y < y_size + 1; y++) {
		glm::mat4 model_matrix = glm::translate(x_scaled_matrix, {0.0, (cell_size + 3.0) * y, 0.0});
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model_matrix));
		draw_model(square_1_1_gray);
	    }
	}

	// draw selected square
	{
	    // don't read this code please
	    int x_start = appState.selectedSquare.x;
	    int y_start = appState.selectedSquare.y;

	    int x_size = 0;
	    int y_size = 0;

	    if (appState.selectingRange) {
		x_start = std::min(x_start, appState.selectBegin.x);
		y_start = std::min(y_start, appState.selectBegin.y);

		x_size = std::abs(appState.selectBegin.x - appState.selectedSquare.x);
		y_size = std::abs(appState.selectBegin.y - appState.selectedSquare.y);
	    }

	    glm::mat4 position_matrix = glm::translate(identity_matrix, {x_start * (cell_size + 3.0) + 1.0, y_start * (cell_size + 3.0) + 1.0, 0.0});
	    glm::mat4 left_side_matrix = glm::scale(position_matrix, {1.0, y_size * (cell_size + 3.0) + (cell_size + 2.0), 1.0});
	    glm::mat4 right_side_matrix = glm::translate(left_side_matrix, { x_size * (cell_size + 3.0) + cell_size + 1.0, 0.0, 0.0});
	    glm::mat4 bottom_side_matrix = glm::scale(position_matrix, {x_size * (cell_size + 3.0) + (cell_size + 2.0), 1.0, 1.0});
	    glm::mat4 top_side_matrix = glm::translate(bottom_side_matrix, {0.0, y_size * (cell_size + 3.0) + cell_size + 1.0, 0.0});
	    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(left_side_matrix));
	    draw_model(square_1_1_red);
	    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(right_side_matrix));
	    draw_model(square_1_1_red);
	    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(top_side_matrix));
	    draw_model(square_1_1_red);
	    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(bottom_side_matrix));
	    draw_model(square_1_1_red);
	}

	appState.step();

	// draw mode line
	double mode_line_size = 20.0;

	glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(glm::translate(identity_matrix, {-(float)appState.windowDimensions.x / 2.0f, -(float)appState.windowDimensions.y / 2.0f, 0.0f})));

	if (appState.mode_line.size() > 0) {
	    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(glm::scale(identity_matrix, {appState.windowDimensions.x, mode_line_size, 1.0})));
	    draw_model(square_1_1_black);
	}

	for (int i = 0; i < appState.mode_line.size(); i++) {
	    char c = appState.mode_line[i];
	    if (font_models[c]) {
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::scale(identity_matrix, {mode_line_size, mode_line_size, 1.0}), {i, 0.0, 0.0})));
		draw_model(*font_models[c]);
	    }
	    else {
		cout << "No model for character code " << int(c) << endl;
	    }
	}

        glfwSwapBuffers(window);
        glfwPollEvents();
        fpsThread.join();
    }
}
