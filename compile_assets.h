// Function for compiling assets and pushing them to GPU inside a
// single VAO (multiple VAOs not supported) for use with a particular
// shader (other shaders not supported), and returning (via
// by-reference parameters) a vector specifying their locations for
// rendering along with the uniform locations of the model, view, and
// projection matrices.

#ifndef LIFE_COMPILE_ASSETS_H
#define LIFE_COMPILE_ASSETS_H

#include <vector>

#include "asset.h"

// Refers to a part of the "mesh atlas".
struct AtlasMesh {
    std::string name;
    int firstElement;
    uint64_t countElements;
};

// Counterpart to a "Model" but contains AtlasMeshes.
struct AtlasModel {
    std::string name;
    std::vector<AtlasMesh> atlasMeshes;
};

// Finds an atlas model by name.
AtlasModel& atlas_model_find(std::vector<AtlasModel>& models, std::string name);

// Pushes all assets to the GPU.
void compile_assets(
    const std::vector<Texture> &textures,
    const std::vector<Mesh> &meshes,
    std::vector<AtlasMesh> &atlasMeshes,
    GLuint &modelUniform,
    GLuint &viewUniform,
    GLuint &projUniform
);

// Maps over defined models and generated atlas meshes to return atlas models.
void map_atlas_models(
    const std::vector<Model> &models,
    const std::vector<AtlasMesh> &atlasMeshes,
    std::vector<AtlasModel> &atlasModels);

// Draws an atlas model.
void draw_model(AtlasModel const& model);

#endif
