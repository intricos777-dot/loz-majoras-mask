#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include "time_cycle.h"
#include "world.h"

namespace loz_mm {

// The four dungeons of Majora's Mask. Each has multiple floors,
// a collection of stray fairies, a map, compass, boss key, and a boss.

enum class DungeonId : uint32_t {
    Woodfall = 0,
    Snowhead,
    GreatBay,
    StoneTower,
    COUNT
};

inline const char* dungeon_name(DungeonId id) {
    switch (id) {
    case DungeonId::Woodfall: return "Woodfall Temple";
    case DungeonId::Snowhead: return "Snowhead Temple";
    case DungeonId::GreatBay: return "Great Bay Temple";
    case DungeonId::StoneTower: return "Stone Tower Temple";
    default: return "???";
    }
}

inline const char* dungeon_boss_name(DungeonId id) {
    switch (id) {
    case DungeonId::Woodfall: return "Odolwa, King of Insects";
    case DungeonId::Snowhead: return "Goht, the Masked Mechanical Beast";
    case DungeonId::GreatBay: return "Gyorg, the Aquatic Beast";
    case DungeonId::StoneTower: return "Twinmold, the Giant Insect";
    default: return "???";
    }
}

// A dungeon room — a single navigable space in a dungeon.
struct RoomDef {
    uint32_t id;
    std::string name;
    std::string description;
    std::vector<uint32_t> connected_rooms;  // indices into the dungeon's room list
    bool has_stray_fairy = false;
    bool has_chest = false;
    bool has_boss_key = false;
    bool has_map = false;
    bool has_compass = false;
    bool is_boss_room = false;
};

class DungeonManager {
public:
    DungeonManager();

    // Generate all four dungeons.
    void generate_dungeons();

    // Dungeon access.
    const std::vector<RoomDef>& rooms(DungeonId id) const;
    std::string name(DungeonId id) const;
    std::string boss_name(DungeonId id) const;

    // Stray fairy count for a dungeon (5 each).
    uint8_t stray_fairy_count(DungeonId id) const;
    uint8_t total_stray_fairies() const;

    // Check if a dungeon is complete (boss defeated).
    bool is_complete(DungeonId id) const;
    void set_complete(DungeonId id);

    // Number of dungeons complete.
    size_t completed_count() const;

    // Total rooms across all dungeons.
    size_t total_rooms() const;

    // Area IDs for each dungeon.
    AreaId boss_room(DungeonId id) const;
    AreaId entry_room(DungeonId id) const;

private:
    std::array<std::vector<RoomDef>, (size_t)DungeonId::COUNT> m_rooms;
    std::array<bool, (size_t)DungeonId::COUNT> m_complete{};
    std::array<uint8_t, (size_t)DungeonId::COUNT> m_stray_fairies{};

    void generate_woodfall();
    void generate_snowhead();
    void generate_great_bay();
    void generate_stone_tower();

    // Helper to build a simple floor (grid of connected rooms).
    void build_floor(std::vector<RoomDef>& rooms, uint32_t floor_idx, uint32_t room_count,
                     uint32_t& next_id, bool top_floor);
};

} // namespace loz_mm
