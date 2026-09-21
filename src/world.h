#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include "time_cycle.h"
#include "npc.h"

namespace loz_mm {

// Region IDs — the major areas of Termina the player can visit.
enum class RegionId : uint32_t {
    ClockTown = 0,
    SouthernSwamp,
    Woodfall,
    MountainVillage,
    Snowhead,
    GreatBayCoast,
    ZoraHall,
    IkanaCanyon,
    RomaniRanch,
    TerminaField,
    Moon,
    COUNT
};

// Each Region contains one or more Areas (sub-regions/rooms).
// Clock Town is the hub and is divided into 4 directional quadrants
// plus several interiors (Inn, Mayor's Office, Milk Bar, Shops).
enum class AreaId : uint32_t {
    // Clock Town areas
    CT_East = 0,
    CT_West,
    CT_North,
    CT_South,
    CT_CenterPlaza,
    CT_Inn,
    CT_MayorOffice,
    CT_MilkBar,
    CT_TradingPost,
    CT_BombShop,
    CT_CuriosityShop,
    CT_HoneyDarling,
    CT_TreasureChest,
    CT_SwordMaster,
    CT_LaundryPool,
    CT_PostOffice,
    CT_AstralObservatory,

    // Southern Swamp / Woodfall
    SW_Main,
    SW_RoadToSwamp,
    SW_DekuPalace,
    SW_Woodfall,
    WF_Main,
    WF_Temple_1F,
    WF_Temple_2F,
    WF_Temple_3F,
    WF_Temple_Boss,

    // Mountain Village / Snowhead
    MV_Main,
    MV_GoronElder,
    MV_Snowhead,
    SH_Main,
    SH_Temple_1F,
    SH_Temple_2F,
    SH_Temple_3F,
    SH_Temple_Boss,

    // Great Bay / Zora Hall
    GB_Coast,
    GB_PiratesFortress,
    GB_ZoraHall,
    GB_ZoraCape,
    GB_Temple_1F,
    GB_Temple_2F,
    GB_Temple_Boss,
    GB_Labo,

    // Ikana
    IK_Graveyard,
    IK_Canyon,
    IK_Cave,
    IK_Well,
    IK_AncientCastle,
    ST_Main,
    ST_Temple_1F,
    ST_Temple_2F,
    ST_Temple_Boss,

    // Romani Ranch
    RR_Main,
    RR_Barn,
    RR_Cucco,

    // Termina Field
    TF_Main,
    TF_LonLonRanch,
    TF_MilkRoad,
    TF_RanchHouse,

    // Moon
    MOON_Field,
    MOON_Majora,

    COUNT
};

// A region is a collection of connected areas.
struct AreaDef {
    AreaId id;
    std::string name;
    std::string description;
    RegionId region;
    std::vector<LocationId> spawn_locations;  // where NPCs can spawn
    bool indoor = false;           // indoor areas have no time-of-day lighting
    bool has_music = true;
};

struct RegionDef {
    RegionId id;
    std::string name;
    std::string description;
    std::vector<AreaId> areas;
    bool unlocked = true;
};

// World definition: all regions and areas, with their connections.
class WorldDef {
public:
    WorldDef();

    const std::vector<RegionDef>& regions() const { return m_regions; }
    const std::vector<AreaDef>& areas() const { return m_areas; }

    const AreaDef* area(AreaId id) const;
    const RegionDef* region(RegionId id) const;
    std::vector<const AreaDef*> areas_in(RegionId region) const;

private:
    std::vector<RegionDef> m_regions;
    std::vector<AreaDef> m_areas;
};

} // namespace loz_mm
