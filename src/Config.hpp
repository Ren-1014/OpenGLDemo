#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>

int width = 1024, height = 760;
const std::string resource_path = "../resource/";
const std::string model_path_prefix = resource_path + "model/";
const std::string shaders_path_prefix = resource_path + "shaders/";
const std::string frag_suffix = "/fragment.glsl";
const std::string vert_suffix = "/vertex.glsl";


#endif