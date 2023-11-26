#ifndef UTILS_H
#define UTILS_H

#include "OBJ_Loader.hpp"
#include <GL/freeglut.h>
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <memory>
#include <vector>
#include <array>
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

const std::string model_path_prefix = resource_path + "model/";

#if 0
struct Texture{
    int w = 1, h = 1, c = 3; // width , height, channels
    unsigned int tex_id;
    Texture(string filename =""){
	cout << filename << endl;
        if(filename == ""){
            unsigned char *data = new unsigned char(0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            delete data;
            return;
        }
        filename = model_path_prefix + filename;
	cout << filename << endl;
        glGenTextures(1, & tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 加载并生成纹理
    cout << filename <<endl;
        unsigned char *data = stbi_load(filename.c_str(), &w, &h, &c, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            cout << "Failed to load texture" << endl;
        }
        stbi_image_free(data);
    }
};
#endif

#if 1
class Texture {
private:
    int w = 1, h = 1, c = 3;

public:
    unsigned int tex_id;

    Texture(std::string filename) {
        if (filename == "") {
            unsigned char *data = new unsigned char(0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            delete data;
            return;
        }
        
        filename = model_path_prefix + filename;
        glGenTextures(1, & tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 加载并生成纹理
        std::cout << filename << std::endl;
        unsigned char *data = stbi_load(filename.c_str(), &w, &h, &c, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
};
#endif



struct Material{
    Shader shader;
    std::vector<Texture> texes;
};


struct Mesh {
    enum attributes {pos, uv, normal};
    vector<glm::vec4> vertices;
    vector<glm::vec2> uvs;
    vector<glm::vec3> normals;
    vector<array<int,3>> triangles;
    shared_ptr<Material> mat;
    unsigned int vao, vbo, ebo;
    glm::mat4 Mm = glm::mat4(1.0f);

    void load_obj(string obj_fn) {
        objl::Loader loader;
        obj_fn = model_path_prefix + obj_fn;
        if(!loader.LoadFile(obj_fn)){
            cout << "load obj fail" <<endl;
            exit(-1);
        }
        // for simplicity, assume only one mesh in .obj file
        objl::Mesh curMesh = loader.LoadedMeshes[0];
        cout << curMesh.Vertices.size() <<endl;
        cout << curMesh.Indices.size() <<endl;
        for (int j = 0; j < curMesh.Vertices.size(); j++) {
            auto v = curMesh.Vertices[j];
            auto pos = v.Position;
            vertices.push_back({pos.X, pos.Y, pos.Z, 1});
            auto tex_coord = v.TextureCoordinate;
            uvs.push_back({tex_coord.X, tex_coord.Y});
            auto norm = v.Normal;
            normals.push_back({norm.X, norm.Y, norm.Z});
        }
        for (int j = 0; j < curMesh.Indices.size(); j += 3) {
            triangles.push_back({(int) curMesh.Indices[j],(int)  curMesh.Indices[j + 1],(int)  curMesh.Indices[j + 2]});
        }
    }

    void mesh2gl() {
        glGenVertexArrays(1, &(vao));
        glGenBuffers(1, &(vbo));
        glGenBuffers(1, &(ebo));
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        int ver_size = sizeof(glm::vec4) * vertices.size();
        int uv_size = sizeof(glm::vec2) * uvs.size();
        int norm_size = sizeof(glm::vec3) * normals.size();
        glBufferData(GL_ARRAY_BUFFER, ver_size + uv_size + norm_size, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, ver_size, &vertices[0]);
        glBufferSubData(GL_ARRAY_BUFFER, ver_size, uv_size, &uvs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, ver_size + uv_size, norm_size, &normals[0]);
        glVertexAttribPointer(Mesh::pos, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
        glVertexAttribPointer(Mesh::uv, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const GLvoid*)(ver_size));
        glVertexAttribPointer(Mesh::normal, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const GLvoid*)(ver_size + uv_size));
        glEnableVertexAttribArray(Mesh::pos);
        glEnableVertexAttribArray(Mesh::uv);
        glEnableVertexAttribArray(Mesh::normal);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * triangles.size() * 3, &triangles[0], GL_STATIC_DRAW);
        cout << "triangles[0]" << &triangles[0] << endl;
        glBindVertexArray(0);
    }
};
# endif