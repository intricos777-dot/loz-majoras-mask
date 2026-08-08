/* LOZ Majora's Mask - Twilight Elysium entry */
#include "engine_bridge.h"
#include <cstdlib>

int main(int /*argc*/, char** /*argv*/) {
    loz_mm::EngineBridge bridge;
    if (!bridge.init("Zelda Majora's Mask", 1280, 720)) {
        std::fprintf(stderr, "[MM] Failed to initialize engine\n");
        return EXIT_FAILURE;
    }
    int rc = bridge.run();
    bridge.shutdown();
    return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
