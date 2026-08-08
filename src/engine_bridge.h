#pragma once
#include <cstdint>
#include "../../twilight-elysium/src/engine/engine.h"

namespace loz_mm {
  class EngineBridge {
  public:
    bool init(const char* title, int width, int height);
    void frame();
    void run();
    void shutdown();
  };
}
