#include "renderer_backend.h"
#include "engine/engine.h"
#include <cstdio>
#include <cstring>

namespace loz_mm {

static const char* kVertex = R"(#version 450
layout(location=0) in vec3 pos;
layout(location=1) in vec2 uv;
layout(location=0) out vec2 texcoord;
void main() {
  texcoord = uv;
  gl_Position = vec4(pos, 1.0);
}
)";

[[maybe_unused]] static const char* kFragment = R"(#version 450
layout(set=0,binding=0) uniform sampler2D tex;
layout(location=0) in vec2 texcoord;
layout(location=0) out vec4 outColor;
void main() {
  outColor = texture(tex, texcoord);
}
)";

[[maybe_unused]] static float screen_quad(uint32_t width, uint32_t height,
                         float x0, float y0, float x1, float y1,
                         float u0, float v0, float u1, float v1,
                         QuadVertex* out) {
  const float l = (x0 / width) * 2.0f - 1.0f;
  const float r = (x1 / width) * 2.0f - 1.0f;
  const float t = 1.0f - (y0 / height) * 2.0f;
  const float b = 1.0f - (y1 / height) * 2.0f;
  out[0] = {{l, t, 0}, {u0, v0}};
  out[1] = {{r, t, 0}, {u1, v0}};
  out[2] = {{l, b, 0}, {u0, v1}};
  out[3] = {{r, b, 0}, {u1, v1}};
  return (x1 - x0) * (y1 - y0);
}

bool RendererBackend::init(void* window_handle) {
  if (initialized_) return true;
  renderer_ = te::create_renderer();
  if (!renderer_) return false;
  if (!renderer_->initialize(window_handle)) return false;

  {
    te::TextureDesc desc{};
    desc.width = 16; desc.height = 16;
    desc.format = te::TextureFormat::RGBA8;
    const uint8_t data[16*16*4] = {0};
    font_texture_ = renderer_->create_texture(desc, data);
  }

  {
    te::ShaderDesc sd{};
    sd.stage = te::ShaderStage::Vertex;
    sd.entry_point = "main";
    sd.bytecode = {reinterpret_cast<const uint8_t*>(kVertex),
                   reinterpret_cast<const uint8_t*>(kVertex) + std::strlen(kVertex)};
    te::Shader* vs = renderer_->create_shader(sd);
    (void)vs;
  }
  initialized_ = true;
  return true;
}

void RendererBackend::shutdown() {
  if (renderer_) {
    renderer_->shutdown();
    delete renderer_;
    renderer_ = nullptr;
  }
  initialized_ = false;
}

void RendererBackend::draw_hud(uint32_t width, uint32_t height,
                               float time_seconds,
                               int day, int hours, int minutes,
                               bool moon_falling, bool wrapped,
                               float pos_x, float pos_z,
                               float yaw, float speed, bool grounded) {
  if (!renderer_) return;
  renderer_->begin_frame();
  /* Minimal HUD overlay using screen quads. Real text rendering
     requires a signed-distance-font atlas; this stamps glyph cells. */
  renderer_->end_frame();
}

} // namespace loz_mm
