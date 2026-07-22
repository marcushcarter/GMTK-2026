#include "scene.h"
#include <application/input.h>

bool Scene::initialize() {
    world.generate(1337); // eventually random
    return true;
}

void Scene::shutdown()
{

}

void Scene::update(const Input& in, uint32_t viewport_w, uint32_t viewport_h)
{
    auto now = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration<float>(now - last).count();
    last = now;
    if (dt > 0.1f) dt = 0.1f;

    camera.set_viewport(glm::vec2((float)viewport_w, (float)viewport_h));
    camera.update(in, dt);
}
