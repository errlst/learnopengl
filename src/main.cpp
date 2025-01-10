#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "camera.h"
#include "shader.h"

#include "model.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

model_t nanosuit_model;
shader_t nanosuit_shader;

camera_t camera;
float camera_front;
float camera_right;

bool mouse_down;

auto camera_window() -> void {
    ImGui::Begin("camera");
    ImGui::Text("pos: (%.2f, %.2f, %.2f)", camera.position_.x, camera.position_.y, camera.position_.z);
    ImGui::Text("front: (%.2f, %.2f, %.2f)", camera.front_.x, camera.front_.y, camera.front_.z);
    ImGui::Text("yaw: %.2f", camera.yaw_);
    ImGui::Text("pitch: %.2f", camera.pitch_);
    ImGui::Text("zoom");
    ImGui::SameLine();
    ImGui::SliderFloat("##zoom", &camera.zoom_, 1, 45);
    ImGui::End();
}

auto gl_init() -> void {
    glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(true);

    nanosuit_shader.init("shader/model.vs", "shader/model.fs");
    nanosuit_model.load("model/nanosuit/nanosuit.obj");
}

auto gl_render() -> void {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.move(camera_front, camera_right, ImGui::GetIO().DeltaTime);

    nanosuit_shader.use();
    nanosuit_shader.set_uniform("projection", glm::perspective(glm::radians(camera.zoom_), (float)1920 / (float)1080, 0.1f, 100.0f));
    nanosuit_shader.set_uniform("view", camera.view_matrix());
    nanosuit_shader.set_uniform("model", glm::scale(glm::translate(glm::mat4(1.0f), {0, 0, 0}), glm::vec3(.2f)));

    nanosuit_shader.set_uniform("view_pos", camera.position_);
    nanosuit_shader.set_uniform("light.position", camera.position_);
    nanosuit_shader.set_uniform("light.direction", camera.front_);
    nanosuit_shader.set_uniform("light.inner_cut", glm::cos(glm::radians(15.f)));
    nanosuit_shader.set_uniform("light.outer_cut", glm::cos(glm::radians(25.f)));
    nanosuit_shader.set_uniform("light.ambient", glm::vec3(0.1f));
    nanosuit_shader.set_uniform("light.diffuse", glm::vec3(0.5f));
    nanosuit_shader.set_uniform("light.specular", glm::vec3(0.2f));
    nanosuit_shader.set_uniform("light.k_constant", 1.f);
    nanosuit_shader.set_uniform("light.k_linear", 0.1f);
    nanosuit_shader.set_uniform("light.k_quadratic", 0.01f);

    nanosuit_model.draw(nanosuit_shader);
}

auto key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) -> void {
    switch (key) {
    case 'w':
    case 'W':
        camera_front = action ? 1 : 0;
        break;
    case 's':
    case 'S':
        camera_front = action ? -1 : 0;
        break;
    case 'a':
    case 'A':
        camera_right = action ? -1 : 0;
        break;
    case 'd':
    case 'D':
        camera_right = action ? 1 : 0;
        break;
    }
}

auto button_callback(GLFWwindow *window, int button, int action, int mods) -> void {
    mouse_down = (action == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse);
}

auto cursor_callback(GLFWwindow *window, double xpos, double ypos) -> void {
    static auto last_xpos = xpos;
    static auto last_ypos = ypos;
    if (mouse_down) {
        camera.rotate(xpos - last_xpos, last_ypos - ypos);
    }
    last_xpos = xpos;
    last_ypos = ypos;
}

int main() {
    glfwInit();
    auto window = glfwCreateWindow(1920, 1080, "opengl", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, button_callback);
    glfwSetCursorPosCallback(window, cursor_callback);

    ImGui::CreateContext();
    ImGui::GetIO().Fonts->Clear();
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./font/MapleMonoNL-Regular.ttf", 28.0f);
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    gl_init();

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gl_render();
        camera_window();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
