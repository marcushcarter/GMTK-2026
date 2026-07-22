#include "application.h"

bool Application::create(const ApplicationCI& p_ci)
{
    window.create(p_ci.title, p_ci.w, p_ci.w);
    renderer.initialize(&window);
    
    main_scene.initialize();
    active_scene = &main_scene;
    
    return true;
}

void Application::destroy()
{
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

        active_scene->update();
        renderer.render(active_scene);

        window.swap_buffers();
    }

    destroy();
}
