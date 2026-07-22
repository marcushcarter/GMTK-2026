#include "camera.h"
#include <application/input.h>
#include <cmath>

static void clamp_to_world(glm::vec2& p, float zoom, glm::vec2 viewport)
{
    glm::vec2 half = (viewport * 0.5f) / zoom;
    glm::vec2 m = glm::min(glm::vec2(Camera::MIN_VISIBLE_TILES), half);
    glm::vec2 world((float)WORLD_W, (float)WORLD_H);
    p = glm::clamp(p, m - half, world - m + half);
}

void Camera::set_viewport(glm::vec2 v) { viewport = glm::max(v, glm::vec2(1.0f)); }

glm::vec2 Camera::screen_to_world(glm::vec2 s) const { return (s - viewport * 0.5f) / zoom + pos; }

glm::vec2 Camera::world_to_screen(glm::vec2 w) const { return (w - pos) * zoom + viewport * 0.5f; }

void Camera::fit_world()
{
    glm::vec2 z = viewport / glm::vec2((float)WORLD_W, (float)WORLD_H);
    target_zoom = glm::clamp(glm::min(z.x, z.y), ZOOM_MIN, ZOOM_MAX);
    target_pos  = glm::vec2((float)WORLD_W, (float)WORLD_H) * 0.5f;
}

glm::vec4 Camera::clip_transform() const
{
    glm::vec2 scale(2.0f * zoom / viewport.x, -2.0f * zoom / viewport.y);
    return glm::vec4(scale, -pos * scale);
}

glm::ivec4 Camera::visible_tile_rect(int margin) const
{
    glm::vec2 tl = screen_to_world(glm::vec2(0.0f));
    glm::vec2 br = screen_to_world(viewport);
    glm::ivec2 lo = glm::ivec2(glm::floor(tl)) - margin;
    glm::ivec2 hi = glm::ivec2(glm::ceil (br)) + margin;
    lo = glm::max(lo, glm::ivec2(0));
    hi = glm::min(hi, glm::ivec2(WORLD_W - 1, WORLD_H - 1));
    return glm::ivec4(lo, hi);
}

void Camera::update(const Input& in, float dt)
{
    glm::vec2 mouse(in.mouse_x, in.mouse_y);
    
    if (in.scroll != 0.0f) {
        glm::vec2 before = (mouse - viewport * 0.5f) / target_zoom + target_pos;
        target_zoom = glm::clamp(target_zoom * powf(ZOOM_STEP, in.scroll), ZOOM_MIN, ZOOM_MAX);
        glm::vec2 after  = (mouse - viewport * 0.5f) / target_zoom + target_pos;
        target_pos += before - after;
    }

    if (in.keys[GLFW_KEY_EQUAL]) target_zoom = glm::clamp(target_zoom * powf(ZOOM_STEP,  8.0f * dt), ZOOM_MIN, ZOOM_MAX);
    if (in.keys[GLFW_KEY_MINUS]) target_zoom = glm::clamp(target_zoom * powf(ZOOM_STEP, -8.0f * dt), ZOOM_MIN, ZOOM_MAX);

    if (in.keys_pressed[GLFW_KEY_F]) fit_world();
    if (in.keys_pressed[GLFW_KEY_H]) target_pos = glm::vec2((float)WORLD_W, (float)WORLD_H) * 0.5f;

    glm::vec2 d(0.0f);
    if (in.keys[GLFW_KEY_A] || in.keys[GLFW_KEY_LEFT])  d.x -= 1.0f;
    if (in.keys[GLFW_KEY_D] || in.keys[GLFW_KEY_RIGHT]) d.x += 1.0f;
    if (in.keys[GLFW_KEY_W] || in.keys[GLFW_KEY_UP])    d.y -= 1.0f;
    if (in.keys[GLFW_KEY_S] || in.keys[GLFW_KEY_DOWN])  d.y += 1.0f;

    if (d.x != 0.0f || d.y != 0.0f) {
        d = glm::normalize(d);
        float speed = PAN_PIXELS_PER_SEC / target_zoom;
        if (in.keys[GLFW_KEY_LEFT_SHIFT] || in.keys[GLFW_KEY_RIGHT_SHIFT]) speed *= 3.0f;
        target_pos += d * speed * dt;
    }

    float t = 1.0f - expf(-SMOOTH_RATE * dt);
    zoom = glm::mix(zoom, target_zoom, t);
    pos = glm::mix(pos, target_pos, t);

    bool grab_start = in.mouse_pressed[GLFW_MOUSE_BUTTON_MIDDLE] || (in.keys[GLFW_KEY_SPACE] && in.mouse_pressed[GLFW_MOUSE_BUTTON_LEFT]);

    if (grab_start) {
        grabbing = true;
        grab_world = screen_to_world(mouse);
    }
    if (!in.mouse_down[GLFW_MOUSE_BUTTON_MIDDLE] && !in.mouse_down[GLFW_MOUSE_BUTTON_LEFT])
        grabbing = false;

    if (grabbing) {
        pos += grab_world - screen_to_world(mouse);
        target_pos = pos;
    }

    clamp_to_world(target_pos, target_zoom, viewport);
    clamp_to_world(pos, zoom, viewport);
    if (grabbing) target_pos = pos;
}
