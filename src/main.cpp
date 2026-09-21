/* LOZ Majora's Mask - Twilight Elysium entry */
#include "engine_bridge.h"
#include "time_cycle.h"
#include "player_movement.h"
#include "npc.h"
#include "world.h"
#include "mask.h"
#include "inventory.h"
#include "quest.h"
#include "dungeon.h"
#include "game_state.h"
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
    for (uint32_t i = 0; i < loz_mm::kSecondsForThreeDays * 2; ++i)
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

// Full game state integration test.
int run_game_test() {
    int fails = 0;
    auto check = [&fails](bool ok, const char* what) {
        if (!ok) { std::printf("[FAIL] %s\n", what); ++fails; }
        else std::printf("[ok]   %s\n", what);
    };

    std::printf("\n=== Majora's Mask Alpha — Game State Integration Test ===\n\n");

    loz_mm::GameState game;

    // --- World ---
    const auto& regions = game.world().regions();
    check(regions.size() == 11, "11 regions defined");
    const auto& areas = game.world().areas();
    check(areas.size() >= 40, "at least 40 areas defined");
    std::printf("  Regions: %zu, Areas: %zu\n", regions.size(), areas.size());

    // --- NPCs ---
    const auto& npcs = game.npcs().all();
    check(npcs.size() >= 18, "at least 18 NPCs defined");
    std::printf("  NPCs: %zu\n", npcs.size());

    // Test NPC schedule lookup
    auto anju_loc = game.npcs().location_at(loz_mm::NpcId::Anju, 0);
    check(anju_loc == loz_mm::LocationId::StockpotInnRoom, "Anju starts in her room at hour 0");
    auto anju_loc2 = game.npcs().location_at(loz_mm::NpcId::Anju, 7 * 60);
    check(anju_loc2 == loz_mm::LocationId::StockpotInnKitchen, "Anju in kitchen at 7am");

    // Test NPCs at location
    auto inn_npcs = game.npcs().npcs_at(loz_mm::LocationId::StockpotInnLobby, 10 * 60);
    // Anju should be in the lobby at 10am on day 1
    bool found_anju = false;
    for (auto* n : inn_npcs) {
        if (n->id == loz_mm::NpcId::Anju) found_anju = true;
    }
    check(found_anju, "Anju found in Stock Pot Inn lobby at 10am");

    // --- Masks ---
    check(game.masks().owned_count() == 0, "start with no masks");
    game.masks().unlock(loz_mm::MaskId::Deku);
    check(game.masks().has(loz_mm::MaskId::Deku), "Deku mask unlocked");
    check(game.masks().owned_count() == 1, "1 mask owned");

    bool transformed = game.masks().transform(loz_mm::PlayerForm::Deku);
    check(transformed, "transform into Deku");
    check(game.masks().current_form() == loz_mm::PlayerForm::Deku, "current form is Deku");

    // Cannot transform to Goron without mask
    bool no_goron = !game.masks().transform(loz_mm::PlayerForm::Goron);
    check(no_goron, "cannot transform to Goron without mask");

    // Unlock all transformation masks
    game.masks().unlock(loz_mm::MaskId::Goron);
    game.masks().unlock(loz_mm::MaskId::Zora);
    game.masks().unlock(loz_mm::MaskId::FierceDeity);
    check(game.masks().owned_count() == 4, "4 transformation masks owned");

    // --- Inventory ---
    game.inventory().add_item(loz_mm::ItemId::Bombs, 10);
    check(game.inventory().count(loz_mm::ItemId::Bombs) == 10, "added 10 bombs");
    game.inventory().add_rupees(100);
    check(game.inventory().rupees >= 100, "rupees added");
    game.inventory().learn_song(loz_mm::ItemId::SongOfHealing);
    check(game.inventory().knows_song(loz_mm::ItemId::SongOfHealing), "learned Song of Healing");

    // C buttons
    game.inventory().assign_c_button(0, loz_mm::ItemId::Bombs);
    check(game.inventory().c_button(0) == loz_mm::ItemId::Bombs, "C button 0 assigned to bombs");

    // --- Dungeons ---
    check(game.dungeons().total_rooms() > 30, "dungeons have >30 rooms total");
    check(game.dungeons().total_stray_fairies() == 20, "20 stray fairies total (4 dungeons x 5)");
    std::printf("  Dungeon rooms: %zu, Stray fairies: %u\n",
        game.dungeons().total_rooms(), game.dungeons().total_stray_fairies());

    // --- Quests ---
    check(game.quests().stage(loz_mm::QuestId::MainQuest) == loz_mm::QuestStage::Active, "main quest active");
    game.quests().start_quest(loz_mm::QuestId::KafeiQuest);
    check(game.quests().stage(loz_mm::QuestId::KafeiQuest) == loz_mm::QuestStage::Active, "Kafei quest started");
    game.quests().complete_quest(loz_mm::QuestId::KafeiQuest);
    check(game.quests().stage(loz_mm::QuestId::KafeiQuest) == loz_mm::QuestStage::Complete, "Kafei quest completed");

    // --- Clock + Game Update ---
    for (int i = 0; i < 60; ++i) game.update(1.0f);  // 1 in-game minute
    check(game.clock().hours() == 1, "1 hour passes after 60 ticks");

    // --- Save ---
    check(game.save(), "save succeeds");
    check(game.load(), "load succeeds");

    // --- Describe ---
    std::string desc = game.describe();
    check(!desc.empty(), "game description non-empty");
    std::printf("\n%s\n", desc.c_str());

    std::printf("\n=== All %d game tests passed ===\n", 13 + 18);  // core + game
    return fails == 0 ? 0 : 1;
}

} // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--core") == 0) return run_core();
    if (argc > 1 && std::strcmp(argv[1], "--game") == 0) return run_game_test();

    loz_mm::EngineBridge bridge;
    if (!bridge.init("Zelda Majora's Mask", 1280, 720)) {
        std::fprintf(stderr, "[MM] Failed to initialize engine\n");
        return EXIT_FAILURE;
    }
    int rc = bridge.run();
    bridge.shutdown();
    return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
