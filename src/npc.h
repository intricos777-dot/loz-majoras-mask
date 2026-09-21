#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include "time_cycle.h"

namespace loz_mm {

// NPCs move through the world on schedules driven by the DayCycle clock.
// Each NPC has a sequence of (hour, location, action) entries that define
// where they are and what they're doing at any given time.

enum class NpcId : uint32_t {
    Anju = 0,
    Kafei,
    GuruGuru,
    Postman,
    MayorDotar,
    MadameAroma,
    Bomber,
    Sakon,
    OldLady,
    Tingle,
    Cremia,
    Romani,
    Lulu,
    Mikau,
    Darmani,
    Pamela,
    Gorman,
    HappyMaskSalesman,
    Tael,
    SkullKid,
    KafeiHidden,   // Kafei in Sakon's hideout
    CursedMan,     // Cursed Swamp Scrub
    GoronElder,
    ZoraElder,
    Kamaro,
    Keaton,
    PostmanTent,
    BombLady,
    FourGiants,
    COUNT
};

enum class LocationId : uint32_t {
    // Clock Town interiors
    StockpotInnLobby,
    StockpotInnKitchen,
    StockpotInnRoom,
    StockpotInnBalcony,
    MayorOffice,
    MayorOfficeHall,
    MilkBar,
    TradingPost,
    BombShop,
    BombShopBack,
    CuriosityShop,
    CuriosityShopBack,
    SwordMasterHouse,
    HoneyAndDarlingShop,
    TreasureChestGame,
    PostOffice,
    LaundryPool,
    LaundryHidden,
    // Clock Town areas
    EastClockTown,
    WestClockTown,
    NorthClockTown,
    SouthClockTown,
    ClockTowerTop,
    ClockTowerEntry,
    // Outside
    TownObservatory,
    AstralObservatory,
    LostWoods,
    SouthernSwamp,
    Woodfall,
    WoodfallTemple,
    MountainVillage,
    Snowhead,
    SnowheadTemple,
    GreatBayCoast,
    ZoraHall,
    ZoraLuluRoom,
    ZoraBandRoom,
    GreatBayTemple,
    PiratesFortress,
    IkanaGraveyard,
    IkanaCave,
    StoneTower,
    StoneTowerTemple,
    BeneathWell,
    AncientCastleIkana,
    RomaniRanch,
    MilkRoad,
    GoronElderCave,
    JabuJabu,
    DekuPalace,
    Unknown,
    COUNT
};

inline const char* to_string(LocationId loc) {
    switch (loc) {
    case LocationId::StockpotInnLobby: return "Stock Pot Inn (Lobby)";
    case LocationId::StockpotInnKitchen: return "Stock Pot Inn (Kitchen)";
    case LocationId::StockpotInnRoom: return "Stock Pot Inn (Anju's Room)";
    case LocationId::StockpotInnBalcony: return "Stock Pot Inn (Balcony)";
    case LocationId::MayorOffice: return "Mayor's Office";
    case LocationId::MayorOfficeHall: return "Mayor's Residence";
    case LocationId::MilkBar: return "Milk Bar";
    case LocationId::TradingPost: return "Trading Post";
    case LocationId::BombShop: return "Bomb Shop";
    case LocationId::BombShopBack: return "Bomb Shop (Back)";
    case LocationId::CuriosityShop: return "Curiosity Shop";
    case LocationId::CuriosityShopBack: return "Curiosity Shop (Back Room)";
    case LocationId::SwordMasterHouse: return "Sword Master's House";
    case LocationId::HoneyAndDarlingShop: return "Honey & Darling's Shop";
    case LocationId::TreasureChestGame: return "Treasure Chest Mini-Game";
    case LocationId::PostOffice: return "Post Office";
    case LocationId::LaundryPool: return "Laundry Pool";
    case LocationId::LaundryHidden: return "Laundry Pool (Hidden Room)";
    case LocationId::EastClockTown: return "East Clock Town";
    case LocationId::WestClockTown: return "West Clock Town";
    case LocationId::NorthClockTown: return "North Clock Town";
    case LocationId::SouthClockTown: return "South Clock Town";
    case LocationId::ClockTowerTop: return "Top of Clock Tower";
    case LocationId::ClockTowerEntry: return "Clock Tower Entrance";
    case LocationId::TownObservatory: return "Town Observatory";
    case LocationId::AstralObservatory: return "Astral Observatory";
    case LocationId::LostWoods: return "Lost Woods";
    case LocationId::SouthernSwamp: return "Southern Swamp";
    case LocationId::Woodfall: return "Woodfall";
    case LocationId::WoodfallTemple: return "Woodfall Temple";
    case LocationId::MountainVillage: return "Mountain Village";
    case LocationId::Snowhead: return "Snowhead";
    case LocationId::SnowheadTemple: return "Snowhead Temple";
    case LocationId::GreatBayCoast: return "Great Bay Coast";
    case LocationId::ZoraHall: return "Zora Hall";
    case LocationId::ZoraLuluRoom: return "Lulu's Room";
    case LocationId::ZoraBandRoom: return "Zora Band Room";
    case LocationId::GreatBayTemple: return "Great Bay Temple";
    case LocationId::PiratesFortress: return "Pirate's Fortress";
    case LocationId::IkanaGraveyard: return "Ikana Graveyard";
    case LocationId::IkanaCave: return "Ikana Cave";
    case LocationId::StoneTower: return "Stone Tower";
    case LocationId::StoneTowerTemple: return "Stone Tower Temple";
    case LocationId::BeneathWell: return "Beneath the Well";
    case LocationId::AncientCastleIkana: return "Ancient Castle of Ikana";
    case LocationId::RomaniRanch: return "Romani Ranch";
    case LocationId::MilkRoad: return "Milk Road";
    case LocationId::GoronElderCave: return "Goron Elder's Cave";
    case LocationId::JabuJabu: return "Inside Jabu-Jabu";
    case LocationId::DekuPalace: return "Deku Palace";
    case LocationId::Unknown: return "???";
    default: return "???";
    }
}

struct ScheduleEntry {
    uint32_t hour;          // 0..71 (3-day cycle)
    LocationId location;
    std::string action;
    bool can_talk = true;
};

struct NpcDef {
    std::string name;
    NpcId id;
    LocationId default_location;
    std::string description;
    std::vector<ScheduleEntry> schedule;
    bool active = true;
};

// NPC manager: holds all NPCs and resolves their location at any time.
class NpcManager {
public:
    NpcManager();

    // Generate full schedules for all major NPCs.
    void generate_default_schedules();

    // Resolve where an NPC is at a given time.
    LocationId location_at(NpcId npc, uint32_t total_seconds) const;
    std::string action_at(NpcId npc, uint32_t total_seconds) const;
    bool can_talk_at(NpcId npc, uint32_t total_seconds) const;
    const NpcDef* npc(NpcId npc) const;

    // Find NPCs at a given location (for rendering/dialogue)
    std::vector<const NpcDef*> npcs_at(LocationId loc, uint32_t total_seconds) const;

    const std::vector<NpcDef>& all() const { return m_npcs; }

private:
    std::vector<NpcDef> m_npcs;
};

} // namespace loz_mm
