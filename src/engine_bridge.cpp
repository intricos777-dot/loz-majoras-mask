/* LOZ Majora's Mask - Twilight Elysium engine bridge */
#include "engine_bridge.h"
#include <cstdio>

namespace loz_mm {
  bool EngineBridge::init(const char* title, int width, int height) {
    te::EngineConfig cfg;
    cfg.window_title = title ? title : "Zelda Majora's Mask";
    cfg.window_width = (uint32_t)width;
    cfg.window_height = (uint32_t)height;
    cfg.base_width = 640;
    cfg.base_height = 360;
    cfg.scale_mode = 5; // Auto
    if (!te::Engine::instance().initialize(cfg)) return false;
    std::printf("[MM] Engine initialized\n");
    return true;
  }
  void EngineBridge::frame() {
    // Frame stub: renderer integration point
  }
  void EngineBridge::run() {
    te::Engine::instance().run();
  }
  void EngineBridge::shutdown() {
    te::Engine::instance().shutdown();
  }
}
