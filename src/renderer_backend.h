#pragma once
#include "renderer/renderer.h"
#include "engine/math_types.h"
#include <cstdint>

namespace loz_mm {
struct QuadVertex {
  float position[3];
  float uv[2];
};

class RendererBackend {
public:
  bool init(void* window_handle);
  void shutdown();
  void draw_hud(uint32_t width, uint32_t height,
                float time_seconds,
                int day,
                int hours,
                int minutes,
                bool moon_falling,
                bool wrapped,
                float pos_x,
                float pos_z,
                float yaw,
                float speed,
                bool grounded);
private:
  te::Renderer* renderer_ = nullptr;
  te::Texture* font_texture_ = nullptr;
  te::Shader* shader_ = nullptr;
  te::RenderPass* pass_ = nullptr;
  bool initialized_ = false;
};
}
