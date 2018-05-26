#include <tuple>
#include <vector>

#include "SOIL.h"

#include "assets.h"
#include "vect2.h"

// Hard-coded textures
Texture rgbwTexture = {
    "rgbw",
    {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}},
     {{0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}}},
};
Texture redTexture = {
    "red",
    {{{1.0, 0.0, 0.0}}}
};
Texture greenTexture = {
    "green",
    {{{0.0, 1.0, 0.0}}}
};
Texture blueTexture = {
    "blue",
    {{{0.0, 0.0, 1.0}}}
};
Texture whiteTexture = {
    "white",
    {{{1.0, 1.0, 1.0}}}
};
Texture grayTexture = {
    "gray",
    {{{0.5, 0.5, 0.5}}}
};
Texture blackTexture = {
    "black",
    {{{0.0, 0.0, 0.0}}}
};

// Hard-coded meshes
Mesh font_test_square_mesh = {
    "font_test_square",
    "font",
    {
	{200, 200, 1, 0},
	{200, 0, 1, 1},
	{0, 0, 0, 1},
	{0, 200, 0, 0},
    },
    {0, 1, 2, 0, 3, 2},
};

std::vector<MeshVertex> square_1_1_mesh = {
    {1, 1, 0, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 1, 0, 0},
};
std::vector<GLuint> square_1_1_elements = {
    0, 1, 2,
    0, 3, 2,
};

Mesh square_1_1_gray_mesh = {
    "square_1_1_gray",
    "gray",
    square_1_1_mesh,
    square_1_1_elements,
};

Mesh square_1_1_white_mesh = {
    "square_1_1_white",
    "white",
    square_1_1_mesh,
    square_1_1_elements,
};

Mesh square_1_1_red_mesh = {
    "square_1_1_red",
    "red",
    square_1_1_mesh,
    square_1_1_elements,
};

Mesh square_1_1_black_mesh = {
    "square_1_1_black",
    "black",
    square_1_1_mesh,
    square_1_1_elements,
};


// Hard-coded models
Model font_test_square_model = {
    "font_test_square",
    {font_test_square_mesh.name},
};

Model square_1_1_gray_model = {
    "square_1_1_gray",
    {square_1_1_gray_mesh.name},
};

Model square_1_1_white_model = {
    "square_1_1_white",
    {square_1_1_white_mesh.name},
};

Model square_1_1_red_model = {
    "square_1_1_red",
    {square_1_1_red_mesh.name},
};

Model square_1_1_black_model = {
    "square_1_1_black",
    {square_1_1_black_mesh.name},
};



// Char locations inside our texture file
std::vector<std::tuple<char, Vect2I>> charLocations = {
    {' ',	{0,	9}},
    {'!',	{1,	9}},
    {'"',	{2,	9}},
    {'#',	{3,	9}},
    {'$',	{4,	9}},
    {'%',	{5,	9}},
    {'&',	{6,	9}},
    {'\'',	{7,	9}},
    {'(',	{8,	9}},
    {')',	{9,	9}},
    {'*',	{0,	8}},
    {'+',	{1,	8}},
    {',',	{2,	8}},
    {'-',	{3,	8}},
    {'.',	{4,	8}},
    {'/',	{5,	8}},
    {'0',	{6,	8}},
    {'1',	{7,	8}},
    {'2',	{8,	8}},
    {'3',	{9,	8}},
    {'4',	{0,	7}},
    {'5',	{1,	7}},
    {'6',	{2,	7}},
    {'7',	{3,	7}},
    {'8',	{4,	7}},
    {'9',	{5,	7}},
    {':',	{6,	7}},
    {';',	{7,	7}},
    {'<',	{8,	7}},
    {'=',	{9,	7}},
    {'>',	{0,	6}},
    {'?',	{1,	6}},
    {'@',	{2,	6}},
    {'A',	{3,	6}},
    {'B',	{4,	6}},
    {'C',	{5,	6}},
    {'D',	{6,	6}},
    {'E',	{7,	6}},
    {'F',	{8,	6}},
    {'G',	{9,	6}},
    {'H',	{0,	5}},
    {'I',	{1,	5}},
    {'J',	{2,	5}},
    {'K',	{3,	5}},
    {'L',	{4,	5}},
    {'M',	{5,	5}},
    {'N',	{6,	5}},
    {'O',	{7,	5}},
    {'P',	{8,	5}},
    {'Q',	{9,	5}},
    {'R',	{0,	4}},
    {'S',	{1,	4}},
    {'T',	{2,	4}},
    {'U',	{3,	4}},
    {'V',	{4,	4}},
    {'W',	{5,	4}},
    {'X',	{6,	4}},
    {'Y',	{7,	4}},
    {'Z',	{8,	4}},
    {'[',	{9,	4}},
    {'\\',	{0,	3}},
    {']',	{1,	3}},
    {'^',	{2,	3}},
    {'_',	{3,	3}},
    {'`',	{4,	3}},
    {'a',	{5,	3}},
    {'b',	{6,	3}},
    {'c',	{7,	3}},
    {'d',	{8,	3}},
    {'e',	{9,	3}},
    {'f',	{0,	2}},
    {'g',	{1,	2}},
    {'h',	{2,	2}},
    {'i',	{3,	2}},
    {'j',	{4,	2}},
    {'k',	{5,	2}},
    {'l',	{6,	2}},
    {'m',	{7,	2}},
    {'n',	{8,	2}},
    {'o',	{9,	2}},
    {'p',	{0,	1}},
    {'q',	{1,	1}},
    {'r',	{2,	1}},
    {'s',	{3,	1}},
    {'t',	{4,	1}},
    {'u',	{5,	1}},
    {'v',	{6,	1}},
    {'w',	{7,	1}},
    {'x',	{8,	1}},
    {'y',	{9,	1}},
    {'z',	{0,	0}},
    {'{',	{1,	0}},
    {'|',	{2,	0}},
    {'}',	{3,	0}},
    {'~',	{4,	0}},
};



std::vector<Texture> all_textures() {
    // get monospace font texture
    int width, height;
    unsigned char* image = SOIL_load_image("mp1_excalibur-monospace_1.png", &width, &height, 0, SOIL_LOAD_RGB);
    Texture monospaceFont = {
	"font",
    };
    unsigned char* component = image;
    monospaceFont.pixels.resize(height);
    for (int y = 0; y < height; y++) {
	// go ahead and flip image upside-down so that its coordinate system agrees with OpenGL's
	std::vector<RGBColor> &row = monospaceFont.pixels[height - (y + 1)];
	row.resize(width);
	for (int x = 0; x < width; x++) {
	    row[x] = {
		*component++ / 256.0f,
		*component++ / 256.0f,
		*component++ / 256.0f,
	    };
	}
    }
    SOIL_free_image_data(image);

    return {
	rgbwTexture,
	redTexture,
	greenTexture,
	blueTexture,
	whiteTexture,
	grayTexture,
	blackTexture,
	monospaceFont,
    };
}

std::vector<Mesh> all_meshes() {
    std::vector<Mesh> meshes = {
	font_test_square_mesh,
	square_1_1_white_mesh,
	square_1_1_gray_mesh,
	square_1_1_red_mesh,
	square_1_1_black_mesh,
    };


    // Meshes for font characters...

    // dimensions of full font image
    double font_image_width = 860.0;
    double font_image_height = 1400.0;

    // how many characters wide and tall the image is
    double characters_wide = 10.0;
    double characters_high = 10.0;

    // dimensions of each character box (box is larger than character)
    double character_box_width = font_image_width / characters_wide;
    double character_box_height = font_image_height / characters_high;

    // dimensions of each character in pixels
    double character_width = 81.0;
    double character_height = 81.0;

    // offsets into character box
    double character_offset_x = 2.5;
    double character_offset_y = 22.0;

    for (int i = 0; i < charLocations.size(); i++) {
	char c = std::get<0>(charLocations[i]);
	Vect2I location = std::get<1>(charLocations[i]);

	// get character bounding box, in pixels
	double character_x_min_px = character_box_width * location.x + character_offset_x;
	double character_x_max_px = character_x_min_px + character_width;
	double character_y_min_px = character_box_height * location.y + character_offset_y;
	double character_y_max_px = character_y_min_px + character_height;

	// character bounding box in texture coordinates
	double character_x_min = character_x_min_px / font_image_width;
	double character_x_max = character_x_max_px / font_image_width;
	double character_y_min = character_y_min_px / font_image_height;
	double character_y_max = character_y_max_px / font_image_height;

	Mesh char_mesh = {
	    std::string("char_") + c,
	    "font",
	    {
		{1, 1, character_x_max, character_y_max},
		{1, 0, character_x_max, character_y_min},
		{0, 0, character_x_min, character_y_min},
		{0, 1, character_x_min, character_y_max},
	    },
	    {0, 1, 2, 0, 3, 2},
	};
	meshes.push_back(char_mesh);
    }

    return meshes;
}

std::vector<Model> all_models() {
    std::vector<Model> models = {
	square_1_1_gray_model,
	square_1_1_white_model,
	square_1_1_red_model,
	square_1_1_black_model,
	font_test_square_model,
    };

    for (int i = 0; i < charLocations.size(); i++) {
	char c = std::get<0>(charLocations[i]);
	std::string name = std::string("char_") + c;
	Model char_model = {
	    name,
	    {name},
	};
	models.push_back(char_model);
    }

    return models;
}
