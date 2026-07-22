#include "application.h"
#include <glad/glad.h>

#ifdef DEV_TOOLS
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#endif

bool Application::create(const ApplicationCI& p_ci)
{
    window.create(p_ci.title, p_ci.w, p_ci.h);
    glfwSetScrollCallback(window.window, input_scroll_callback);
    renderer.initialize(&window);
    scene.initialize();

#ifdef DEV_TOOLS
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
#endif
    
    return true;
}

void Application::destroy()
{
#ifdef DEV_TOOLS
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    scene.shutdown();

    renderer.shutdown();
    window.destroy();
}

void Application::run()
{
    while (!window.should_close()) {
        window.poll_events();
        input.poll(window.window);
        
        renderer.request_resize(window.width, window.height);

        window.set_title(std::to_string(1.0f / scene.dt));

#ifdef DEV_TOOLS
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        _draw_ui();
#endif

        scene.update(input, renderer.width, renderer.height);
        renderer.render(&scene);

#ifdef DEV_TOOLS
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        window.swap_buffers();
    }

    destroy();
}

// #ifdef DEV_TOOLS
void Application::_draw_ui()
{
    // ImGui::ShowDemoWindow();

    // ImGui::Begin("Image Test");
    // ImGui::Text("size = %d x %d", renderer.width, renderer.height);
    // ImGui::Image((ImTextureID)(intptr_t)renderer.out_color,
    // ImVec2(renderer.width/2.0f, renderer.height/2.0f));
    // ImGui::End();

}
// #endif
