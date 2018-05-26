// Data structures for representing assets (meshes and textures).

#ifndef LIFE_ASSET_H
#define LIFE_ASSET_H

#include <GL/gl.h>

#include <string>
#include <vector>

struct RGBColor {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

struct Texture {
    std::string name;
    std::vector<std::vector<RGBColor>> pixels;
};

struct MeshVertex {
    GLdouble x;
    GLdouble y;
    GLdouble textureX;
    GLdouble textureY;
};

struct Mesh {
    std::string name;
    std::string texture;
    std::vector<MeshVertex> vertices;
    std::vector<GLuint> elements;
};

struct Model {
    std::string name;
    std::vector<std::string> meshes;
};

#endif
