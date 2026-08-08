#include "engine_bridge.h"
int main() {
  loz_mm::EngineBridge bridge;
  if (!bridge.init("Zelda Majora's Mask", 1280, 720)) return 1;
  bridge.run();
  return 0;
}
