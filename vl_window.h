//
// Created by devkon on 12.12.24.
//
#pragma once
#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vl {

    class VLWindow {
        public:
        VLWindow(int w, int h, std::string name);
        ~VLWindow();

        VLWindow(const VLWindow&) = delete;
        VLWindow& operator=(const VLWindow&) = delete;

        bool shouldClose() {
            return glfwWindowShouldClose(window);
        }

        private:
        void initWindow();

        const int width;
        const int height;

        std::string windowName;
        GLFWwindow* window;
    };
}



