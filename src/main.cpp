#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OBJ_Loader.hpp"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include "Shader.hpp"
#include <memory>
#include "Utils.hpp"

using namespace std;

int width = 1024, height = 760;

namespace camera {
    float fov = 60.0f, aspect = (float)width/ (float)height;
    glm::vec3 cameraPos = glm::vec3(0.0f,0.0,-8.f);
    glm::vec3 cameraFront= glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 cameraUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 cameraRight(){ return glm::cross(cameraFront,cameraUp); }
    glm::mat4 Mv; // model view matrix
    glm::mat4 Mp; // perspective matrix
    void update_matrix(){
        glViewport(0, 0,width, height);
        aspect = (float)width/ (float)height;
        Mv= glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        Mp= glm::perspective(glm::radians(fov/2), aspect, 0.01f, 100.0f);
    }
    void update_shader_camera(Shader& shader){
        update_matrix();
        shader.setMat4f("Mv",Mv);
        shader.setMat4f("Mp",Mp);
        shader.setVec3f("camera_pos", cameraPos);
    }
};

namespace light{
    glm::vec3 light_pos(-3.f,3.f,-3.f);
    glm::vec3 light_ambient(0.1f, 0.1f, 0.1f);
    glm::vec3 light_color(0.8f, 0.8f, 0.8f);
    void update_shader_light(Shader& shader){
        shader.setVec3f("light_pos",light_pos);
        shader.setVec3f("light_ambient",light_ambient);
        shader.setVec3f("light_color",light_color);
    }
};

namespace glut_event_control{
    int pre_x, pre_y, button;
	void mouse_click(int button_, int state, int x, int y){
        button =  button_;
        if(button == GLUT_LEFT_BUTTON){
            if(state == GLUT_DOWN){
                pre_x = x, pre_y = y, button = GLUT_LEFT_BUTTON;
            }
        }
    }
    glm::vec3 up(0,1,0);
    glm::vec3 up_normal(0,1,0);
    glm::vec3 up_reverse(0,-1,0);    
    const float rot_speed = 0.8f;
    const float PI = 3.1415926535;
    float yaw = 0.0, tilt = 0.0;
    void mouse_press_move(int x, int y){
        if(button != GLUT_LEFT_BUTTON) return; 
        int dx = x - pre_x, dy = y - pre_y;
        tilt += (float)dy / (float)height * rot_speed;
        yaw += (float)dx / (float)width * rot_speed;
        // Ensures that tilt stays within a certain range
        while( tilt < (PI * -1) ) tilt += (2 * PI);
        while( tilt > PI ) tilt -= (2 * PI);
        if( (tilt > (0.5*PI)) || (tilt < (-0.5*PI))) up = up_reverse;
        else up = up_normal;
        camera::cameraFront = glm::normalize( glm::vec3(sinf(yaw) * cos(tilt), sin(tilt), cosf(yaw) * cos(tilt)) );
        glm::vec3 right = glm::cross(camera::cameraFront, up);
        camera::cameraUp = glm::cross( right, camera::cameraFront);
        pre_x = x, pre_y = y;
        glutPostRedisplay();
    }
    void key_press(unsigned char key, int x, int y){ 
        float s = 0.1f;
        switch (key) {
        case 'S' : 
            camera::cameraPos += (s * camera::cameraUp);
            break;
        case 'W' :
            camera::cameraPos -= (s * camera::cameraUp);
            break;
        case 'A' :
            camera::cameraPos += (s * camera::cameraRight());
            break;
        case 'D' :
            camera::cameraPos -= (s * camera::cameraRight());
            break;
        case 'Q' :
            exit(0);
            break;
        }
        glutPostRedisplay();
    } 
    void scroll_callback(int wheel, int direction, int x, int y) {
        if (camera::fov >= 1.0f && camera::fov <= 60.0f)
            camera::fov -= (float)direction;
        if (camera::fov <= 1.0f)
            camera::fov = 1.0f;
        if (camera::fov >= 60.0f)
            camera::fov = 60.0f;
        glutPostRedisplay();
    }
    void reshape(int w, int h){ width = w, height = h; }
    // void idle(){ glutSwapBuffers(); }
    void idle(){ glutPostRedisplay(); }
}

Mesh build_spot(){
        shared_ptr<Material> spot_mat = make_shared<Material>();
        spot_mat->shader = Shader("blinn_phong");
        spot_mat->texes.emplace_back("spot/spot_texture.png");
        Mesh spot_mesh;
        spot_mesh.load_obj("spot/spot_triangulated_good.obj");
        spot_mesh.mat = spot_mat;
        return spot_mesh;
}

namespace render{
    vector<Mesh> meshes;
    void render_config(){
        glEnable(GL_DEPTH_TEST);
        meshes.push_back(build_spot());
        for(Mesh& mesh : meshes){
            mesh.mesh2gl();
        }
    }
    void render(){
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for(Mesh& mesh : meshes){
            Shader& shader = mesh.mat->shader;
            shader.use();
            light::update_shader_light(shader);
            camera::update_shader_camera(shader);
            shader.setMat4f("Mm",mesh.Mm);
            const vector<Texture>& texes = mesh.mat->texes;
            for(int i = 0; i < texes.size(); i++){
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, texes[i].tex_id);
                shader.setInt("tex" + to_string(i), i);
            }
            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, (GLsizei)mesh.triangles.size() * 3, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        glFlush();
        glutSwapBuffers();
    }
};

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Demo");
    GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "glewInit Error:" << glewGetErrorString << endl;
        exit(-1);
	}
	glutReshapeFunc(glut_event_control::reshape);
	//glutIdleFunc(glut_event_control::idle);
	glutMouseFunc(glut_event_control::mouse_click);
	glutMotionFunc(glut_event_control::mouse_press_move);
    glutMouseWheelFunc(glut_event_control::scroll_callback);
	glutKeyboardFunc(glut_event_control::key_press);
    render::render_config();
	glutDisplayFunc(render::render);
    glutMainLoop();
	return 0;
}

