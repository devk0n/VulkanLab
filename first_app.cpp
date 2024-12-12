//
// Created by devkon on 12.12.24.
//

#include "first_app.h"

namespace vl {
    void FirstApp::run() {
        while (vlWindow.shouldClose()) {
            glfwPollEvents();
        }
    }
}
