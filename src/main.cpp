/*
 * main.cpp
 * StructForce — Contact Management System
 *
 * Entry point: initializes GLFW + OpenGL 3.3 + Dear ImGui,
 * then runs the render loop.
 *
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename  = nullptr;

    // Load Roboto at 2x for Retina displays, then scale back down
    float xscale = 1.f;
    glfwGetWindowContentScale(window, &xscale, nullptr);
    io.Fonts->AddFontFromFileTTF(APP_FONT_PATH, 15.5f * xscale);
    io.FontGlobalScale = 1.f / xscale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // ── Application data
    ContactStore store;
    AppState     state;
    UserStore    users;

    loadUserStore(users);
    initContactStore(store);
    initAppState(state);
    applyTheme(state.darkMode);
    invalidateSort(state);
    loadLogo(LOGO_LIGHT_PATH, LOGO_DARK_PATH);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double now       = glfwGetTime();
        float  deltaTime = (float)(now - lastTime);
        lastTime         = now;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Full-screen root window
        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(vp->Size);

        ImGuiWindowFlags rootFlags =
            ImGuiWindowFlags_NoTitleBar    | ImGuiWindowFlags_NoCollapse  |
            ImGuiWindowFlags_NoResize      | ImGuiWindowFlags_NoMove      |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoScrollbar   | ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,  {0.f, 0.f});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        ImGui::Begin("##root", nullptr, rootFlags);
        ImGui::PopStyleVar(2);

        renderApp(state, store, users);
        renderNotification(state, deltaTime);

        ImGui::End();

        ImGui::Render();

        int displayW, displayH;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(C_BG_BASE.x, C_BG_BASE.y, C_BG_BASE.z, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    if (state.loggedInUserId >= 0)
        saveContactsForUser(state.loggedInUserId, store);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
