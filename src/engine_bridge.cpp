#include "engine_bridge.h"
#include "time_cycle.h"
#include "player_movement.h"
#include <cstdio>
#include <cmath>

namespace loz_mm {

static DayCycle* g_clock = nullptr;

bool EngineBridge::init(const char* title, int width, int height) {
  te::EngineConfig cfg;
  cfg.window_title = title ? title : "Zelda Majora's Mask";
  cfg.window_width = (uint32_t)width;
  cfg.window_height = (uint32_t)height;
  cfg.base_width = 640;
  cfg.base_height = 360;
  cfg.scale_mode = 5;
  if (!te::Engine::instance().initialize(cfg)) return false;

  if (!g_clock) {
    static DayCycle clock;
    g_clock = &clock;
  }

  te::Engine::instance().attach_tick([](float dt, uint64_t /*frame*/) {
    if (g_clock) g_clock->tick(dt);
  });

  std::printf("[MM] Engine initialized\n");
  return true;
}

int EngineBridge::run() {
  DayCycle clock;
  PlayerController hero;
  hero.set_walk_speed(3.0f);
  hero.set_run_speed(6.0f);

  const int frames = 600;
  for (int i = 0; i < frames; ++i) {
    hero.move(1.0f / 60.0f, 0.0f, 1.0f, true);
    clock.tick(1.0f / 60.0f);
  }
  std::printf("[MM] Final pos(%.2f, %.2f) yaw=%.2f speed=%.2f grounded=%d\n",
              hero.pos().x, hero.pos().z, hero.yaw(), hero.speed(), hero.grounded());
  std::printf("[MM] Final clock day=%d %02d:%02d wrapped=%d moon=%d phase=%d\n",
              clock.day(), clock.hours(), clock.minutes(), clock.wrapped(),
              clock.moon_falling(), (int)clock.phase());
  return 0;
}

void EngineBridge::shutdown() {
  g_clock = nullptr;
  te::Engine::instance().shutdown();
}

} // namespace loz_mm
