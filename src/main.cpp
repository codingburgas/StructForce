/*
 * Layer usage in main():
 *   Logic layer:        loadSampleContacts (via data layer), initAppState
 *   Presentation layer: applyTheme, renderApp, renderNotification
 *   Data layer:         NOT accessed directly
 */

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <cstdio>

#include "logic.h"
#include "presentation.h"
#include "auth.h"
#include "ui_helpers.h"
#include "logo.h"



static void onGlfwError(int error, const char* description) {
    fprintf(stderr, "[GLFW error %d] %s\n", error, description);
}

int main() {
    glfwSetErrorCallback(onGlfwError);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        return 1;
    }

    const char* glslVersion = "#version 330 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(
        1280, 800, "StructForce — Contact Management System", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return 1;
    }

    glfwSetWindowSizeLimits(window, 960, 640, GLFW_DONT_CARE, GLFW_DONT_CARE);

    // Center window on primary monitor
    {
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        if (mon) {
            const GLFWvidmode* mode = glfwGetVideoMode(mon);
            int wx, wy;
            glfwGetWindowSize(window, &wx, &wy);
            glfwSetWindowPos(window,
                (mode->width  - wx) / 2,
                (mode->height - wy) / 2);
        }
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();