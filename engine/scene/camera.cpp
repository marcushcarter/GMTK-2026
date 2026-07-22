#include "camera.h"
#include <application/input.h>
#include <cmath>

static float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

static void clamp_to_world(float& x, float& y, float zoom, float vw, float vh)
{
    float half_w = (vw * 0.5f) / zoom;
    float half_h = (vh * 0.5f) / zoom;
    float mx = (Camera::MIN_VISIBLE_TILES < half_w) ? Camera::MIN_VISIBLE_TILES : half_w;
    float my = (Camera::MIN_VISIBLE_TILES < half_h) ? Camera::MIN_VISIBLE_TILES : half_h;
    x = clampf(x, mx - half_w, (float)WORLD_W - mx + half_w);
    y = clampf(y, my - half_h, (float)WORLD_H - my + half_h);
}

void Camera::set_viewport(float w, float h)
{
    viewport_w = (w > 1.0f) ? w : 1.0f;
    viewport_h = (h > 1.0f) ? h : 1.0f;
}

void Camera::screen_to_world(float sx, float sy, float& wx, float& wy) const
{
    wx = (sx - viewport_w * 0.5f) / zoom + pos_x;
    wy = (sy - viewport_h * 0.5f) / zoom + pos_y;
}

void Camera::world_to_screen(float wx, float wy, float& sx, float& sy) const
{
    sx = (wx - pos_x) * zoom + viewport_w * 0.5f;
    sy = (wy - pos_y) * zoom + viewport_h * 0.5f;
}

void Camera::fit_world()
{
    float zx = viewport_w / (float)WORLD_W;
    float zy = viewport_h / (float)WORLD_H;
    target_zoom = clampf(zx < zy ? zx : zy, ZOOM_MIN, ZOOM_MAX);
    target_x = (float)WORLD_W * 0.5f;
    target_y = (float)WORLD_H * 0.5f;
}

void Camera::clip_transform(float out[4]) const
{
    out[0] = 2.0f * zoom / viewport_w;
    out[1] = -2.0f * zoom / viewport_h;
    out[2] = -pos_x * out[0];
    out[3] = -pos_y * out[1];
}

void Camera::visible_tile_rect(int margin, int& x0, int& y0, int& x1, int& y1) const
{
    float tlx, tly, brx, bry;
    screen_to_world(0.0f, 0.0f, tlx, tly);
    screen_to_world(viewport_w, viewport_h, brx, bry);

    x0 = (int)floorf(tlx) - margin;
    y0 = (int)floorf(tly) - margin;
    x1 = (int)ceilf (brx) + margin;
    y1 = (int)ceilf (bry) + margin;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > WORLD_W - 1) x1 = WORLD_W - 1;
    if (y1 > WORLD_H - 1) y1 = WORLD_H - 1;
}

void Camera::update(const Input& in, float dt)
{
    if (in.scroll != 0.0f) {
        float bx = (in.mouse_x - viewport_w * 0.5f) / target_zoom + target_x;
        float by = (in.mouse_y - viewport_h * 0.5f) / target_zoom + target_y;
        target_zoom = clampf(target_zoom * powf(ZOOM_STEP, in.scroll), ZOOM_MIN, ZOOM_MAX);
        float ax = (in.mouse_x - viewport_w * 0.5f) / target_zoom + target_x;
        float ay = (in.mouse_y - viewport_h * 0.5f) / target_zoom + target_y;
        target_x += bx - ax;
        target_y += by - ay;
    }

    if (in.keys[GLFW_KEY_EQUAL]) target_zoom = clampf(target_zoom * powf(ZOOM_STEP, 8.0f * dt), ZOOM_MIN, ZOOM_MAX);
    if (in.keys[GLFW_KEY_MINUS]) target_zoom = clampf(target_zoom * powf(ZOOM_STEP, -8.0f * dt), ZOOM_MIN, ZOOM_MAX);
    if (in.keys_pressed[GLFW_KEY_F]) fit_world();
    if (in.keys_pressed[GLFW_KEY_H]) {
        target_x = (float)WORLD_W * 0.5f;
        target_y = (float)WORLD_H * 0.5f;
    }

    float dx = 0.0f, dy = 0.0f;
    if (in.keys[GLFW_KEY_A] || in.keys[GLFW_KEY_LEFT]) dx -= 1.0f;
    if (in.keys[GLFW_KEY_D] || in.keys[GLFW_KEY_RIGHT]) dx += 1.0f;
    if (in.keys[GLFW_KEY_W] || in.keys[GLFW_KEY_UP]) dy -= 1.0f;
    if (in.keys[GLFW_KEY_S] || in.keys[GLFW_KEY_DOWN]) dy += 1.0f;

    if (dx != 0.0f || dy != 0.0f) {
        float len = sqrtf(dx * dx + dy * dy);
        dx /= len;
        dy /= len;
        float speed = PAN_PIXELS_PER_SEC / target_zoom;
        if (in.keys[GLFW_KEY_LEFT_SHIFT] || in.keys[GLFW_KEY_RIGHT_SHIFT]) speed *= 3.0f;
        target_x += dx * speed * dt;
        target_y += dy * speed * dt;
    }

    float t = 1.0f - expf(-SMOOTH_RATE * dt);
    zoom  += (target_zoom - zoom)  * t;
    pos_x += (target_x - pos_x) * t;
    pos_y += (target_y - pos_y) * t;

    bool grab_start = in.mouse_pressed[GLFW_MOUSE_BUTTON_MIDDLE] || (in.keys[GLFW_KEY_SPACE] && in.mouse_pressed[GLFW_MOUSE_BUTTON_LEFT]);

    if (grab_start) {
        grabbing = true;
        screen_to_world(in.mouse_x, in.mouse_y, grab_world_x, grab_world_y);
    }
    if (!in.mouse_down[GLFW_MOUSE_BUTTON_MIDDLE] && !in.mouse_down[GLFW_MOUSE_BUTTON_LEFT])
        grabbing = false;

    if (grabbing) {
        float cur_x, cur_y;
        screen_to_world(in.mouse_x, in.mouse_y, cur_x, cur_y);
        pos_x += grab_world_x - cur_x;
        pos_y += grab_world_y - cur_y;
        target_x = pos_x;
        target_y = pos_y;
    }

    clamp_to_world(target_x, target_y, target_zoom, viewport_w, viewport_h);
    clamp_to_world(pos_x, pos_y, zoom, viewport_w, viewport_h);
    if (grabbing) { target_x = pos_x; target_y = pos_y; }
}
