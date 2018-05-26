#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

#include "asset.h"
#include "compile_assets.h"
#include "vect2.h"

using std::string;
using std::vector;

// Shader sources
const GLchar *vertexSource = R"glsl(
    #version 150 core
    in vec2 position;
    in vec2 texcoord;
    out vec2 Texcoord;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    void main()
    {
        Texcoord = texcoord;
        gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
    }
)glsl";
const GLchar *fragmentSource = R"glsl(
    #version 150 core
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D tex;
    void main()
    {
        outColor = texture(tex, Texcoord);
    }
)glsl";


size_t textureHeight(const Texture &t) {
    return t.pixels.size();
}

size_t textureWidth(const Texture &t) {
    return t.pixels[0].size();
}

void textureSetDimensions(Texture &t, size_t width, size_t height) {
    t.pixels.resize(height);
    for (int i = 0; i < height; i++) {
        t.pixels[i].resize(width);
    }
}

int textureFind(const string &name, const vector<Texture> &textures) {
    for (size_t i = 0; i < textures.size(); i++) {
        if (name == textures[i].name) {
            return i;
        }
    }
    return -1;
}

// Creates a texture atlas.
void packTextures(
    const vector<Texture> &textures,
    Texture &out,
    vector<Vect2D> &scales,
    vector<Vect2D> &offsets) {
    size_t length = textures.size();
    size_t width = 0;
    size_t height = 0;

    // compute needed atlas size
    for (int i = 0; i < length; i++) {
        width += textureWidth(textures[i]) + 2;
        height = std::max(height, textureHeight(textures[i]));
    }

    // add textures to atlas and create other out data
    scales.resize(length);
    offsets.resize(length);
    textureSetDimensions(out, width, height);
    size_t textureXCoord = 0;
    for (size_t i = 0; i < length; i++) {
        const Texture &texture = textures[i];
        scales[i].x = (double)textureWidth(texture);
        scales[i].y = (double)textureHeight(texture);
        offsets[i].x = textureXCoord + 1.0;
        offsets[i].y = 0;
        for (size_t y = 0; y < textureHeight(texture); y++) {
            for (size_t x = 0; x < textureWidth(texture) + 2; x++) {
                size_t sampleX = std::min(textureWidth(texture), std::max(size_t(1), x)) - 1;
                out.pixels[y][textureXCoord + x] = texture.pixels[y][sampleX];
            }
        }
	if (textureHeight(texture) < height) {
            for (size_t x = 0; x < textureWidth(texture) + 2; x++) {
                size_t sampleX = std::min(textureWidth(texture), std::max(size_t(1), x)) - 1;
                out.pixels[textureHeight(texture)][textureXCoord + x] = texture.pixels[textureHeight(texture) - 1][sampleX];
            }
	}
        textureXCoord += textureWidth(texture) + 2;
    }
}

void packMeshes(
    const vector<Mesh> &meshes,
    const vector<Texture> &textures,
    const vector<Vect2D> &scales,
    const vector<Vect2D> &offsets,
    vector<AtlasMesh> &atlasMeshes,
    vector<MeshVertex> &atlasVertices,
    vector<GLuint> &atlasElements) {
    for (size_t i = 0; i < meshes.size(); i++) {
        const Mesh &mesh = meshes[i];
        // find texture
        int textureIndex = textureFind(mesh.texture, textures);
        if (textureIndex == -1) {
	    std::cout << "Could not find texture: " << mesh.texture << std::endl;
            exit(1);
        }
        // push mesh vertices onto BOA
        int firstVertexIndex = atlasVertices.size();
        for (size_t j = 0; j < mesh.vertices.size(); j++) {
            const MeshVertex &meshVertex = mesh.vertices[j];
            atlasVertices.push_back({
                meshVertex.x,
                meshVertex.y,
                offsets[textureIndex].x + meshVertex.textureX * scales[textureIndex].x,
                offsets[textureIndex].y + meshVertex.textureY * scales[textureIndex].y,
            });
        }
        // push mesh elements onto BOA
        int firstElementIndex = atlasElements.size();
        for (size_t j = 0; j < mesh.elements.size(); j++) {
            atlasElements.push_back(mesh.elements[j] + firstVertexIndex);
        }
        // output atlas mesh
        atlasMeshes.push_back({
            mesh.name,
            firstElementIndex,
            mesh.elements.size(),
        });
    }
}

void compile_assets(
    const vector<Texture> &textures,
    const vector<Mesh> &meshes,
    vector<AtlasMesh> &atlasMeshes,
    GLuint &modelUniform,
    GLuint &viewUniform,
    GLuint &projUniform
) {
    // create atlases,
    Texture atlas;
    vector<Vect2D> scales;
    vector<Vect2D> offsets;
    vector<MeshVertex> atlasVertices;
    vector<GLuint> atlasElements;
    packTextures(textures, atlas, scales, offsets);
    packMeshes(meshes, textures, scales, offsets, atlasMeshes, atlasVertices, atlasElements);

    // create and initialize data to push to gpu
    std::unique_ptr<GLdouble[]> vertices = std::make_unique<GLdouble[]>(atlasVertices.size() * 4);
    std::unique_ptr<GLuint[]> elements = std::make_unique<GLuint[]>(atlasElements.size()); // vector elements should be contiguous but we're copying anyway
    std::unique_ptr<GLfloat[]> pixels = std::make_unique<GLfloat[]>(textureWidth(atlas) * textureHeight(atlas) * 3);
    for (size_t i = 0; i < atlasVertices.size(); i++) {
        const MeshVertex &atlasVertex = atlasVertices[i];
        vertices[4 * i] = atlasVertex.x;
        vertices[4 * i + 1] = atlasVertex.y;
        vertices[4 * i + 2] = atlasVertex.textureX / textureWidth(atlas);
        vertices[4 * i + 3] = atlasVertex.textureY / textureHeight(atlas);
	// std::cout << atlasVertex.x << " " << atlasVertex.y << " " << atlasVertex.textureX << " " << atlasVertex.textureY << std::endl;
    }
    for (size_t i = 0; i < atlasElements.size(); i++) {
        elements[i] = atlasElements[i];
	// std::cout << elements[i] << std::endl;
    }
    for (size_t y = 0; y < textureHeight(atlas); y++) {
	// std::cout << y << std::endl;
        for (size_t x = 0; x < textureWidth(atlas); x++) {
            pixels[3 * (y * textureWidth(atlas) + x)] = atlas.pixels[y][x].r;
            pixels[3 * (y * textureWidth(atlas) + x) + 1] = atlas.pixels[y][x].g;
            pixels[3 * (y * textureWidth(atlas) + x) + 2] = atlas.pixels[y][x].b;
	    // std::cout << atlas.pixels[y][x].r << " " << atlas.pixels[y][x].g << " " << atlas.pixels[y][x].b << std::endl;
        }
    }

    GLint success;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint tex;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, atlasVertices.size() * 4 * sizeof(GLdouble), vertices.get(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, atlasElements.size() * sizeof(GLuint), elements.get(), GL_STATIC_DRAW);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth(atlas), textureHeight(atlas), 0, GL_RGB, GL_FLOAT, pixels.get());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        GLchar *log = new GLchar[maxLength];
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, log);
	std::cout << "error compiling vertex shader" << std::endl << log << std::endl;
        delete[] log;
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        GLchar *log = new GLchar[maxLength];
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, log);
	std::cout << "error compiling fragment shader" << std::endl << log << std::endl;
        delete[] log;
    }

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glBindFragDataLocation(shaderProgram, 1, "outObjectID");
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
        GLchar *log = new GLchar[maxLength];
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, log);
	std::cout << "error linking program" << std::endl << log << std::endl;
        delete[] log;
    }
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_DOUBLE, GL_FALSE, 4 * sizeof(GLdouble), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_DOUBLE, GL_FALSE, 4 * sizeof(GLdouble), (void*)(2 * sizeof(GLdouble)));

    // get uniform locations
    modelUniform = glGetUniformLocation(shaderProgram, "model");
    viewUniform = glGetUniformLocation(shaderProgram, "view");
    projUniform = glGetUniformLocation(shaderProgram, "proj");
}

AtlasModel& atlas_model_find(vector<AtlasModel>& models, std::string name) {
    for (size_t i = 0; i < models.size(); i++) {
	if (models[i].name == name) {
	    return models[i];
	}
    }
    std::cout << "Could not find atlas model " << name << std::endl;
    exit(1);
}

void map_atlas_models(
    const vector<Model> &models,
    const vector<AtlasMesh> &atlasMeshes,
    vector<AtlasModel> &atlasModels
) {
    atlasModels.reserve(atlasMeshes.size());
    for (size_t i = 0; i < models.size(); i++) {
	Model const& model = models[i];
	AtlasModel atlasModel;
	atlasModel.name = model.name;
	for (size_t j = 0; j < model.meshes.size(); j++) {
	    std::string const& mesh_name = model.meshes[j];
	    for (size_t k = 0; k < atlasMeshes.size(); k++) {
		AtlasMesh const& atlasMesh = atlasMeshes[k];
		if (atlasMesh.name == mesh_name) {
		    atlasModel.atlasMeshes.push_back(atlasMesh);
		    goto next_mesh;
		}
	    }
	    std::cout << "Could not find mesh " << mesh_name << std::endl;
	    exit(1);
	next_mesh:;
	}
	atlasModels.push_back(atlasModel);
    }
}

void draw_model(AtlasModel const& model) {
    for (size_t i = 0; i < model.atlasMeshes.size(); i++) {
	AtlasMesh const& mesh = model.atlasMeshes[i];
	glDrawElements(GL_TRIANGLES, mesh.countElements, GL_UNSIGNED_INT, (void *)(mesh.firstElement * sizeof(GLuint)));
    }
}
