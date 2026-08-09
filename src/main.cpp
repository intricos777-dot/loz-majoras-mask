/* LOZ Majora's Mask - Twilight Elysium entry */
#include "engine_bridge.h"
#include "time_cycle.h"
#include "player_movement.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

namespace {

int run_core() {
    int fails = 0;
    auto check = [&fails](bool ok, const char* what) {
        if (!ok) { std::printf("[FAIL] %s\n", what); ++fails; }
        else std::printf("[ok]   %s\n", what);
    };

    // ---- DayCycle: 3-day loop, dawn crossovers, Moon, Song of Time ----
    loz_mm::DayCycle clock;
    check(clock.day() == 1 && clock.hours() == 0, "starts at dawn of Day 1");

    uint32_t dawns = 0;
    for (int i = 0; i < loz_mm::kSecondsForThreeDays * 2; ++i)
        dawns += clock.tick(1.0f);
    check(dawns == 2, "exactly two dawn crossovers across three days");
    check(clock.wrapped(), "clock wraps after Day 3");
    check(clock.moon_falling(), "moon falls in the final minutes");
    check(clock.phase() == loz_mm::DayPhase::Night, "phase is night at the end");

    clock.song_of_time();
    check(clock.day() == 1 && clock.hours() == 0, "Song of Time rewinds to Dawn of Day 1");
    check(!clock.moon_falling(), "moon retreats after Song of Time");

    clock.set_debug_hour(12);
    check(clock.phase() == loz_mm::DayPhase::Noon, "noon phase at 12:00");

    // ---- PlayerController: movement math ----
    loz_mm::PlayerController hero;
    hero.set_walk_speed(3.0f);
    hero.set_run_speed(6.0f);

    for (int i = 0; i < 60; ++i) hero.move(1.f / 60.f, 0.f, 1.f, true);   // run forward 1s
    const float dist = std::sqrt(hero.pos().x * hero.pos().x + hero.pos().z * hero.pos().z);
    check(dist > 5.0f && dist < 6.2f, "one second of running covers ~6 units");
    check(std::fabs(hero.yaw()) < 1.f, "forward stick keeps yaw near zero");

    for (int i = 0; i < 120; ++i) hero.move(1.f / 60.f, 0.f, 0.f, false);   // idle 2s
    const float after_stop = std::sqrt(hero.pos().x * hero.pos().x + hero.pos().z * hero.pos().z);
    check(after_stop < dist + 0.9f && hero.speed() < 0.2f, "braking: slowdown, small coast");

    hero.set_pos({0.f, 0.f, 0.f});
    hero.jump();
    float apex = 0.f;
    for (int i = 0; i < 90; ++i) { hero.move(1.f / 60.f, 0.f, 0.f, false); apex = std::max(apex, hero.pos().y); }
    check(apex > 1.5f, "jump arcs above ground");
    check(hero.grounded() && hero.pos().y == 0.f, "lands back on the ground");

    std::printf("daycycle: %s\n", clock.describe().c_str());
    return fails == 0 ? 0 : 1;
}

} // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--core") == 0) return run_core();

    loz_mm::EngineBridge bridge;
    if (!bridge.init("Zelda Majora's Mask", 1280, 720)) {
        std::fprintf(stderr, "[MM] Failed to initialize engine\n");
        return EXIT_FAILURE;
    }
    int rc = bridge.run();
    bridge.shutdown();
    return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}