#include "scene.h"

bool Scene::initialize()
{
    return true;
}

void Scene::shutdown()
{

}

void Scene::update()
{
    auto now = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration<float>(now - last).count();
    last = now;
    
}
