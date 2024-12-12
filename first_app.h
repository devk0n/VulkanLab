//
// Created by devkon on 12.12.24.
//
#pragma once

#include "vl_window.h"

namespace vl {

    class FirstApp {
        public:
        static const int WIDTH = 1280;
        static const int HEIGHT = 960;

        void run();

        private:
        VLWindow vlWindow{WIDTH, HEIGHT, "Hello Vulkan!"};

    };
}