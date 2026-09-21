#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include "time_cycle.h"
#include "npc.h"
#include "world.h"
#include "mask.h"
#include "inventory.h"
#include "quest.h"
#include "dungeon.h"

namespace loz_mm {

// Top-level game state: combines all systems into a coherent save state.
class GameState {
public:
    GameState();

    // Initialize a fresh game.
    void new_game();

    // Save/load to file (stub — serializes the state).
    bool save();
    bool load();

    // Getters for systems.
    DayCycle& clock() { return m_clock; }
    NpcManager& npcs() { return m_npcs; }
    WorldDef& world() { return m_world; }
    MaskManager& masks() { return m_masks; }
    Inventory& inventory() { return m_inventory; }
    QuestLog& quests() { return m_quests; }
    DungeonManager& dungeons() { return m_dungeons; }

    // Player current location (tracked separately from controller).
    AreaId current_area() const { return m_current_area; }
    void set_current_area(AreaId a) { m_current_area = a; }

    // Advance the world state by dt.
    void update(float dt);

    // Full description for debug output.
    std::string describe() const;

    // Total completion percentage.
    float completion() const;

private:
    DayCycle m_clock;
    NpcManager m_npcs;
    WorldDef m_world;
    MaskManager m_masks;
    Inventory m_inventory;
    QuestLog m_quests;
    DungeonManager m_dungeons;

    AreaId m_current_area = AreaId::CT_South;

    bool m_initialized = false;
    uint32_t m_play_seconds = 0;  // real time in seconds
};

} // namespace loz_mm
