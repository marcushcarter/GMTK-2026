#include "application.h"
#include <glad/glad.h>

bool Application::create(const ApplicationCI& p_ci)
{
    window.create(p_ci.title, p_ci.w, p_ci.w);
    
    return true;
}

void Application::destroy()
{
    window.destroy();
}

void Application::run()
{
    while (!window.should_close()) {
        window.poll_events();

        glClearColor(1,0,1,1);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        window.swap_buffers();
    }

    destroy();
}
