#pragma once

#include "common.h"
#include "GLkit.h"

class GLFWkit {
private:
    //Window
    GLFWwindow* window;
    int window_size[2];

    //Mouse
    bool press_flag;
    Eigen::Vector2f press_position_current;
    Eigen::Vector2f press_position_last;

    //GLrender
    GLkit glkit;

	char* fileName;

public:
    GLFWkit(char *title, int window_w, int window_h,char* m_fileName=NULL) :
        //Window
        window_size{ window_w, window_h },

        //Mouse
        press_flag(false),
        press_position_current{ 0.0f, 0.0f },
        press_position_last{ 0.0f, 0.0f },
		fileName(m_fileName),
        //GLrender
        glkit(window_w, window_h)
    {

        if (!glfwInit()) {
            printf("GLFW was NOT initialized.");
            return;
        }

        window = glfwCreateWindow(window_size[0], window_size[1], title, nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            printf("Window was NOT created.");
            return;
        }

        glfwSwapInterval(0);
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);

        glfwSetWindowSizeCallback(window, changeWindowSize);
        glfwSetKeyCallback(window, createKeyInfo);
        glfwSetCharCallback(window, createCharaInfo);
        glfwSetScrollCallback(window, mouseScrollCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, mouseMoveCallback);
		glfwSwapInterval(0);
		gg::ggInit();
			
    }

    ~GLFWkit() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void setup() {
        glkit.setup();
		glkit.loadObj(fileName);
    }

    inline void render(float *input_xyz, cv::Mat *img = NULL, int texid = 0) {
        glkit.render(input_xyz, img, texid);
    }
	inline void render(float* input_xyz, cv::Mat* img, cv::Mat renderimg) {
		glkit.render(input_xyz, img, renderimg);
	}

    bool update(float *input_xyz, cv::Mat *img = NULL) {
        if (glfwWindowShouldClose(window)) {
            return false;
        }
        glkit.render(input_xyz, img);

        glfwSwapBuffers(window);
        glfwPollEvents();

        return true;
    }

    static void changeWindowSize(GLFWwindow* window, const int width, const int height) {
        GLFWkit* exec = static_cast<GLFWkit*>(glfwGetWindowUserPointer(window));
    }

    static void createKeyInfo(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
        GLFWkit* exec = static_cast<GLFWkit*>(glfwGetWindowUserPointer(window));

        if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            glfwDestroyWindow(exec->window);
            glfwTerminate();
        }
    }

    static void createCharaInfo(GLFWwindow* window, const u_int chara) {
        GLFWkit* exec = static_cast<GLFWkit*>(glfwGetWindowUserPointer(window));
    }

    static void mouseScrollCallback(GLFWwindow* window, const double x_offset, const double y_offset) {
        GLFWkit* exec = static_cast<GLFWkit*>(glfwGetWindowUserPointer(window));
        exec->glkit.setFov(y_offset);
    }

    static void mouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods) {
        double pos_x;
        double pos_y;
        glfwGetCursorPos(window, &pos_x, &pos_y);
        GLFWkit* exec = static_cast<GLFWkit*>(glfwGetWindowUserPointer(window));

        if (button == GLFW_MOUSE_BUTTON_1) {
            switch (action) {
                case GLFW_PRESS: {
                    if (exec->press_flag) {
                        return;
                    }
                    exec->press_flag = true;
                    exec->press_position_current << pos_x, pos_y;
                    exec->glkit.touchStart(exec->press_position_current);
                }
                break;

                case GLFW_RELEASE: {
                    if (!exec->press_flag) {
                        return;
                    }
                    exec->press_flag = false;
                    exec->press_position_current << pos_x, pos_y;
                    exec->glkit.touchEnd(exec->press_position_current);
                }
                break;
            }
        }
    }

    static void mouseMoveCallback(GLFWwindow* window, const double pos_x, const double pos_y) {
        GLFWkit* exec = static_cast<GLFWkit*>(glfwGetWindowUserPointer(window));

        if (!exec->press_flag) {
            return;
        }
        exec->press_position_last = exec->press_position_current;
        exec->press_position_current << pos_x, pos_y;
        exec->glkit.touchMove(exec->press_position_current, exec->press_position_last);
    }

private:
};