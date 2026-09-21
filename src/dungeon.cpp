#include "dungeon.h"
#include <cstdio>
#include <algorithm>

namespace loz_mm {

DungeonManager::DungeonManager() {
    generate_dungeons();
}

void DungeonManager::generate_dungeons() {
    m_complete.fill(false);
    m_stray_fairies.fill(0);
    generate_woodfall();
    generate_snowhead();
    generate_great_bay();
    generate_stone_tower();
    std::printf("[Dungeon] Generated %zu dungeons, %zu total rooms\n",
        (size_t)DungeonId::COUNT, total_rooms());
}

const std::vector<RoomDef>& DungeonManager::rooms(DungeonId id) const {
    return m_rooms[static_cast<size_t>(id)];
}

std::string DungeonManager::name(DungeonId id) const {
    return dungeon_name(id);
}

std::string DungeonManager::boss_name(DungeonId id) const {
    return dungeon_boss_name(id);
}

uint8_t DungeonManager::stray_fairy_count(DungeonId id) const {
    return m_stray_fairies[static_cast<size_t>(id)];
}

uint8_t DungeonManager::total_stray_fairies() const {
    uint8_t total = 0;
    for (auto s : m_stray_fairies) total += s;
    return total;
}

bool DungeonManager::is_complete(DungeonId id) const {
    return m_complete[static_cast<size_t>(id)];
}

void DungeonManager::set_complete(DungeonId id) {
    m_complete[static_cast<size_t>(id)] = true;
}

size_t DungeonManager::completed_count() const {
    size_t count = 0;
    for (auto c : m_complete) if (c) ++count;
    return count;
}

size_t DungeonManager::total_rooms() const {
    size_t total = 0;
    for (const auto& rs : m_rooms) total += rs.size();
    return total;
}

AreaId DungeonManager::boss_room(DungeonId id) const {
    switch (id) {
    case DungeonId::Woodfall: return AreaId::WF_Temple_Boss;
    case DungeonId::Snowhead: return AreaId::SH_Temple_Boss;
    case DungeonId::GreatBay: return AreaId::GB_Temple_Boss;
    case DungeonId::StoneTower: return AreaId::ST_Temple_Boss;
    default: return AreaId::COUNT;
    }
}

AreaId DungeonManager::entry_room(DungeonId id) const {
    switch (id) {
    case DungeonId::Woodfall: return AreaId::WF_Temple_1F;
    case DungeonId::Snowhead: return AreaId::SH_Temple_1F;
    case DungeonId::GreatBay: return AreaId::GB_Temple_1F;
    case DungeonId::StoneTower: return AreaId::ST_Temple_1F;
    default: return AreaId::COUNT;
    }
}

void DungeonManager::build_floor(std::vector<RoomDef>& rooms, uint32_t floor_idx,
                                  uint32_t room_count, uint32_t& next_id, bool top_floor) {
    uint32_t base = next_id;
    for (uint32_t i = 0; i < room_count; ++i) {
        RoomDef r;
        r.id = next_id++;
        r.name = "Floor " + std::to_string(floor_idx) + " Room " + std::to_string(i + 1);
        r.description = "A room on floor " + std::to_string(floor_idx);
        // Connect rooms in a line (simple connectivity)
        if (i > 0) r.connected_rooms.push_back(r.id - 1);
        if (i + 1 < room_count) r.connected_rooms.push_back(r.id + 1);
        rooms.push_back(r);
    }
    // Connect to previous floor via first room
    if (floor_idx > 1 && !rooms.empty()) {
        rooms[base].connected_rooms.push_back(base - room_count);
        rooms[base - room_count].connected_rooms.push_back(base);
    }
}

void DungeonManager::generate_woodfall() {
    std::vector<RoomDef>& rooms = m_rooms[static_cast<size_t>(DungeonId::Woodfall)];
    uint32_t id = 0;

    // Floor 1: entry, 4 rooms
    build_floor(rooms, 1, 4, id, false);
    // Floor 2: 4 rooms, more complex
    build_floor(rooms, 2, 4, id, false);
    // Floor 3: 3 rooms
    build_floor(rooms, 3, 3, id, false);

    // Add features
    // Room 2 (first floor): Map
    rooms[1].has_map = true;
    // Room 3 (first floor): Compass
    rooms[2].has_compass = true;
    // Room 6 (second floor): Stray fairy
    rooms[5].has_stray_fairy = true;
    // Room 7 (second floor): Boss key
    rooms[6].has_boss_key = true;
    // Room 10 (third floor): Boss key chest
    rooms[9].has_chest = true;
    rooms[9].description = "The boss key chest stands here";

    // Boss room
    RoomDef boss{};
    boss.id = id++;
    boss.name = "Odolwa's Chamber";
    boss.description = "The throne of Odolwa, King of Insects. The air hums with dark energy.";
    boss.is_boss_room = true;
    boss.connected_rooms.push_back(id - 1);
    rooms.push_back(boss);

    // Stray fairies: 5 per dungeon, scattered
    m_stray_fairies[0] = 5;
    rooms[0].has_stray_fairy = true;
    rooms[3].has_stray_fairy = true;
    rooms[4].has_stray_fairy = true;
    rooms[7].has_stray_fairy = true;
    rooms[8].has_stray_fairy = true;

    std::printf("  Woodfall Temple: %zu rooms\n", rooms.size());
}

void DungeonManager::generate_snowhead() {
    std::vector<RoomDef>& rooms = m_rooms[static_cast<size_t>(DungeonId::Snowhead)];
    uint32_t id = 0;

    build_floor(rooms, 1, 4, id, false);
    build_floor(rooms, 2, 4, id, false);
    build_floor(rooms, 3, 3, id, false);

    rooms[1].has_map = true;
    rooms[2].has_compass = true;
    rooms[5].has_stray_fairy = true;
    rooms[6].has_boss_key = true;
    rooms[9].has_chest = true;

    RoomDef boss{};
    boss.id = id++;
    boss.name = "Goht's Chamber";
    boss.description = "The frozen chamber of Goht, the Masked Mechanical Beast.";
    boss.is_boss_room = true;
    boss.connected_rooms.push_back(id - 1);
    rooms.push_back(boss);

    m_stray_fairies[1] = 5;
    rooms[0].has_stray_fairy = true;
    rooms[3].has_stray_fairy = true;
    rooms[4].has_stray_fairy = true;
    rooms[7].has_stray_fairy = true;
    rooms[8].has_stray_fairy = true;

    std::printf("  Snowhead Temple: %zu rooms\n", rooms.size());
}

void DungeonManager::generate_great_bay() {
    std::vector<RoomDef>& rooms = m_rooms[static_cast<size_t>(DungeonId::GreatBay)];
    uint32_t id = 0;

    build_floor(rooms, 1, 4, id, false);
    build_floor(rooms, 2, 3, id, false);

    rooms[1].has_map = true;
    rooms[2].has_compass = true;
    rooms[4].has_stray_fairy = true;
    rooms[5].has_boss_key = true;
    rooms[6].has_chest = true;

    RoomDef boss{};
    boss.id = id++;
    boss.name = "Gyorg's Chamber";
    boss.description = "The flooded chamber of Gyorg, the Aquatic Beast.";
    boss.is_boss_room = true;
    boss.connected_rooms.push_back(id - 1);
    rooms.push_back(boss);

    m_stray_fairies[2] = 5;
    rooms[0].has_stray_fairy = true;
    rooms[3].has_stray_fairy = true;
    rooms[4].has_stray_fairy = true;
    rooms[5].has_stray_fairy = true;
    rooms[6].has_stray_fairy = true;

    std::printf("  Great Bay Temple: %zu rooms\n", rooms.size());
}

void DungeonManager::generate_stone_tower() {
    std::vector<RoomDef>& rooms = m_rooms[static_cast<size_t>(DungeonId::StoneTower)];
    uint32_t id = 0;

    build_floor(rooms, 1, 4, id, false);
    build_floor(rooms, 2, 3, id, false);

    rooms[1].has_map = true;
    rooms[2].has_compass = true;
    rooms[4].has_stray_fairy = true;
    rooms[5].has_boss_key = true;
    rooms[6].has_chest = true;

    RoomDef boss{};
    boss.id = id++;
    boss.name = "Twinmold's Chamber";
    boss.description = "The inverted chamber of Twinmold, the Giant Insect. Gravity itself is uncertain here.";
    boss.is_boss_room = true;
    boss.connected_rooms.push_back(id - 1);
    rooms.push_back(boss);

    m_stray_fairies[3] = 5;
    rooms[0].has_stray_fairy = true;
    rooms[3].has_stray_fairy = true;
    rooms[4].has_stray_fairy = true;
    rooms[5].has_stray_fairy = true;
    rooms[6].has_stray_fairy = true;

    std::printf("  Stone Tower Temple: %zu rooms\n", rooms.size());
}

} // namespace loz_mm
