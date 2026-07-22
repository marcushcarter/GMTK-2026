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
    renderer.initialize(&window);
    
    main_scene.initialize();
    active_scene = &main_scene;

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

    main_scene.shutdown();

    renderer.shutdown();
    window.destroy();
}

void Application::run()
{
    while (!window.should_close()) {
        window.poll_events();
        renderer.request_resize(window.width, window.height);

        window.set_title(std::to_string(1.0f / active_scene->dt));

#ifdef DEV_TOOLS
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        _draw_ui();
#endif

        active_scene->update();
        renderer.render(active_scene);

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
    ImGui::ShowDemoWindow();

    ImGui::Begin("Image Test");
    ImGui::Text("size = %d x %d", renderer.width, renderer.height);
    ImGui::Image((ImTextureID)(intptr_t)renderer.out_color,
    ImVec2(renderer.width/10.0f, renderer.height/10.0f));
    ImGui::End();

}
// #endif
