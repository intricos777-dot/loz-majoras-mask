#include "world.h"
#include <algorithm>
#include <cstdio>

namespace loz_mm {

WorldDef::WorldDef() {
    // ---- Regions ----
    m_regions = {
        {RegionId::ClockTown, "Clock Town", "The carnival city at Termina's center", {}, true},
        {RegionId::SouthernSwamp, "Southern Swamp", "The poisoned Deku swamp and the fallen Woodfall", {}, true},
        {RegionId::Woodfall, "Woodfall", "The temple of the swamp, crowned with poison", {}, true},
        {RegionId::MountainVillage, "Mountain Village", "The frozen village beneath the blizzard", {}, true},
        {RegionId::Snowhead, "Snowhead", "The mountain of Gorons, cursed by the grieving Goht", {}, true},
        {RegionId::GreatBayCoast, "Great Bay", "The Zora sea, the pirates, and the bay's wild heart", {}, true},
        {RegionId::ZoraHall, "Zora Hall", "The music-filled hall of the Zora tribe", {}, true},
        {RegionId::IkanaCanyon, "Ikana Canyon", "The kingdom of the dead — Stone Tower, the Castle, the Well", {}, true},
        {RegionId::RomaniRanch, "Romani Ranch", "The ranch outside town, where Romani guards against the sky", {}, true},
        {RegionId::TerminaField, "Termina Field", "The open field connecting all regions", {}, true},
        {RegionId::Moon, "The Moon", "The falling moon at the end of all things", {}, true},
    };

    // ---- Areas ----

    // Clock Town areas
    m_areas.push_back({AreaId::CT_East, "East Clock Town",
        "The east side — Stock Pot Inn, Milk Bar, Town Observatory",
        RegionId::ClockTown, {LocationId::EastClockTown, LocationId::StockpotInnLobby, LocationId::StockpotInnKitchen, LocationId::StockpotInnRoom, LocationId::StockpotInnBalcony, LocationId::MilkBar, LocationId::TownObservatory}, false, true});
    m_areas.push_back({AreaId::CT_West, "West Clock Town",
        "The west side — Trading Post, Bomb Shop, Curiosity Shop, Post Office",
        RegionId::ClockTown, {LocationId::WestClockTown, LocationId::TradingPost, LocationId::BombShop, LocationId::BombShopBack, LocationId::CuriosityShop, LocationId::CuriosityShopBack, LocationId::PostOffice}, false, true});
    m_areas.push_back({AreaId::CT_North, "North Clock Town",
        "The north side — the park, the Mayor's Office, the exit to the Observatory",
        RegionId::ClockTown, {LocationId::NorthClockTown, LocationId::MayorOffice, LocationId::MayorOfficeHall}, false, true});
    m_areas.push_back({AreaId::CT_South, "South Clock Town",
        "The south side — Clock Tower entrance, the square, the exit to Termina Field",
        RegionId::ClockTown, {LocationId::SouthClockTown, LocationId::ClockTowerEntry, LocationId::ClockTowerTop}, false, true});
    m_areas.push_back({AreaId::CT_CenterPlaza, "Clock Town Plaza",
        "The central plaza — the Clock Tower, the fountain, the meeting place",
        RegionId::ClockTown, {LocationId::SouthClockTown, LocationId::ClockTowerEntry}, false, true});
    m_areas.push_back({AreaId::CT_Inn, "Stock Pot Inn",
        "The inn where Anju waits for Kafei — lobby, kitchen, and upstairs room",
        RegionId::ClockTown, {LocationId::StockpotInnLobby, LocationId::StockpotInnKitchen, LocationId::StockpotInnRoom, LocationId::StockpotInnBalcony}, true, true});
    m_areas.push_back({AreaId::CT_MayorOffice, "Mayor's Office",
        "Mayor Dotar's office and residence — petitions pile high",
        RegionId::ClockTown, {LocationId::MayorOffice, LocationId::MayorOfficeHall}, true, true});
    m_areas.push_back({AreaId::CT_MilkBar, "Milk Bar",
        "Madame Aroma's Milk Bar — Lon Lon Milk and Zora Echo available",
        RegionId::ClockTown, {LocationId::MilkBar}, true, true});
    m_areas.push_back({AreaId::CT_TradingPost, "Trading Post",
        "Clock Town's general goods shop — potions, arrows, and news",
        RegionId::ClockTown, {LocationId::TradingPost}, true, true});
    m_areas.push_back({AreaId::CT_BombShop, "Bomb Shop",
        "The bomb shop — bombs, bombchus, and powder kegs",
        RegionId::ClockTown, {LocationId::BombShop, LocationId::BombShopBack}, true, true});
    m_areas.push_back({AreaId::CT_CuriosityShop, "Curiosity Shop",
        "The curiosity shop — Keaton goods and the Kafei connection",
        RegionId::ClockTown, {LocationId::CuriosityShop, LocationId::CuriosityShopBack}, true, true});
    m_areas.push_back({AreaId::CT_HoneyDarling, "Honey & Darling's Shop",
        "The daily game shop — each day brings a new challenge",
        RegionId::ClockTown, {LocationId::HoneyAndDarlingShop}, true, true});
    m_areas.push_back({AreaId::CT_TreasureChest, "Treasure Chest Mini-Game",
        "The chest game — can you open them all?",
        RegionId::ClockTown, {LocationId::TreasureChestGame}, true, true});
    m_areas.push_back({AreaId::CT_SwordMaster, "Sword Master's House",
        "The sword master — he teaches the secret techniques",
        RegionId::ClockTown, {LocationId::SwordMasterHouse}, true, true});
    m_areas.push_back({AreaId::CT_LaundryPool, "Laundry Pool",
        "The laundry pool — home of the Postman and a hidden room",
        RegionId::ClockTown, {LocationId::LaundryPool, LocationId::LaundryHidden}, false, true});
    m_areas.push_back({AreaId::CT_PostOffice, "Post Office",
        "The post office — the Postman never misses a deadline",
        RegionId::ClockTown, {LocationId::PostOffice}, true, true});
    m_areas.push_back({AreaId::CT_AstralObservatory, "Astral Observatory",
        "The observatory outside town — the Bombers watch the sky here",
        RegionId::ClockTown, {LocationId::AstralObservatory}, false, true});

    // Southern Swamp / Woodfall
    m_areas.push_back({AreaId::SW_Main, "Southern Swamp",
        "The poisoned swamp — Deku shrines, the Swamp Tourist Center, and the monkey's plight",
        RegionId::SouthernSwamp, {LocationId::SouthernSwamp}, false, true});
    m_areas.push_back({AreaId::SW_RoadToSwamp, "Road to Southern Swamp",
        "The path from Termina Field to the swamp — a Deku Scrub guards the entrance",
        RegionId::SouthernSwamp, {LocationId::SouthernSwamp}, false, true});
    m_areas.push_back({AreaId::SW_DekuPalace, "Deku Palace",
        "The royal palace of the Deku — the Princess awaits the hero",
        RegionId::SouthernSwamp, {LocationId::DekuPalace}, false, true});
    m_areas.push_back({AreaId::SW_Woodfall, "Woodfall",
        "The Woodfall region — the temple rises from the poisoned waters",
        RegionId::Woodfall, {LocationId::Woodfall}, false, true});
    m_areas.push_back({AreaId::WF_Main, "Woodfall Temple Entrance",
        "The approach to the temple — lily pads and Deku flowers",
        RegionId::Woodfall, {LocationId::WoodfallTemple}, false, true});
    m_areas.push_back({AreaId::WF_Temple_1F, "Woodfall Temple (1F)",
        "First floor — the Deku flowers, the elevator, the first fairies",
        RegionId::Woodfall, {LocationId::WoodfallTemple}, true, true});
    m_areas.push_back({AreaId::WF_Temple_2F, "Woodfall Temple (2F)",
        "Second floor — the lily pad maze, the flame pillars",
        RegionId::Woodfall, {LocationId::WoodfallTemple}, true, true});
    m_areas.push_back({AreaId::WF_Temple_3F, "Woodfall Temple (3F)",
        "Third floor — the odolwa antechamber",
        RegionId::Woodfall, {LocationId::WoodfallTemple}, true, true});
    m_areas.push_back({AreaId::WF_Temple_Boss, "Woodfall Temple (Odolwa)",
        "The boss chamber — Odolwa, King of Insects awaits",
        RegionId::Woodfall, {LocationId::WoodfallTemple}, true, true});

    // Mountain Village / Snowhead
    m_areas.push_back({AreaId::MV_Main, "Mountain Village",
        "The village beneath the blizzard — Gorons and the path to Snowhead",
        RegionId::MountainVillage, {LocationId::MountainVillage}, false, true});
    m_areas.push_back({AreaId::MV_GoronElder, "Goron Elder's Cave",
        "The cave where the Goron Elder is frozen — the Lullaby is needed",
        RegionId::MountainVillage, {LocationId::GoronElderCave}, true, true});
    m_areas.push_back({AreaId::MV_Snowhead, "Snowhead Pass",
        "The path from the village to Snowhead — deep snow and frozen foes",
        RegionId::Snowhead, {LocationId::Snowhead}, false, true});
    m_areas.push_back({AreaId::SH_Main, "Snowhead Region",
        "The Snowhead region — frozen lakes and the approach to the temple",
        RegionId::Snowhead, {LocationId::Snowhead}, false, true});
    m_areas.push_back({AreaId::SH_Temple_1F, "Snowhead Temple (1F)",
        "First floor — the ice walls, the Goron pillars, the first fairies",
        RegionId::Snowhead, {LocationId::SnowheadTemple}, true, true});
    m_areas.push_back({AreaId::SH_Temple_2F, "Snowhead Temple (2F)",
        "Second floor — the rotating room, the frozen lava",
        RegionId::Snowhead, {LocationId::SnowheadTemple}, true, true});
    m_areas.push_back({AreaId::SH_Temple_3F, "Snowhead Temple (3F)",
        "Third floor — the bridge of ice, Goht's chamber above",
        RegionId::Snowhead, {LocationId::SnowheadTemple}, true, true});
    m_areas.push_back({AreaId::SH_Temple_Boss, "Snowhead Temple (Goht)",
        "The boss chamber — Goht, the Masked Mechanical Beast awaits",
        RegionId::Snowhead, {LocationId::SnowheadTemple}, true, true});

    // Great Bay / Zora Hall
    m_areas.push_back({AreaId::GB_Coast, "Great Bay Coast",
        "The Zora coast — Mikau lies wounded here, and the pirates' fortress looms",
        RegionId::GreatBayCoast, {LocationId::GreatBayCoast}, false, true});
    m_areas.push_back({AreaId::GB_PiratesFortress, "Pirate's Fortress",
        "The pirate fortress — Gerudo pirates guard their treasure",
        RegionId::GreatBayCoast, {LocationId::PiratesFortress}, false, true});
    m_areas.push_back({AreaId::GB_ZoraHall, "Zora Hall",
        "The Zora concert hall — Lulu, Guru-Guru, and the Indigo-Go's",
        RegionId::ZoraHall, {LocationId::ZoraHall, LocationId::ZoraLuluRoom, LocationId::ZoraBandRoom}, true, true});
    m_areas.push_back({AreaId::GB_ZoraCape, "Zora Cape",
        "The cape beyond the hall — a waterfall, a breach, and the deep water",
        RegionId::GreatBayCoast, {LocationId::GreatBayCoast}, false, true});
    m_areas.push_back({AreaId::GB_Temple_1F, "Great Bay Temple (1F)",
        "First floor — the water wheel, the first green pipes",
        RegionId::ZoraHall, {LocationId::GreatBayTemple}, true, true});
    m_areas.push_back({AreaId::GB_Temple_2F, "Great Bay Temple (2F)",
        "Second floor — the water direction puzzles, the boss antechamber",
        RegionId::ZoraHall, {LocationId::GreatBayTemple}, true, true});
    m_areas.push_back({AreaId::GB_Temple_Boss, "Great Bay Temple (Gyorg)",
        "The boss chamber — Gyorg, the Aquatic Beast awaits",
        RegionId::ZoraHall, {LocationId::GreatBayTemple}, true, true});
    m_areas.push_back({AreaId::GB_Labo, "Zora Lab",
        "The seaside lab — the scientist studies the creatures of the bay",
        RegionId::GreatBayCoast, {LocationId::GreatBayCoast}, true, true});

    // Ikana
    m_areas.push_back({AreaId::IK_Graveyard, "Ikana Graveyard",
        "The graveyard of Ikana — the spirit of Captain Keeta guards it",
        RegionId::IkanaCanyon, {LocationId::IkanaGraveyard}, false, true});
    m_areas.push_back({AreaId::IK_Canyon, "Ikana Canyon",
        "The canyon of the dead — the Poe Collector's lair and the river of souls",
        RegionId::IkanaCanyon, {LocationId::IkanaCave, LocationId::BeneathWell}, false, true});
    m_areas.push_back({AreaId::IK_Cave, "Ikana Cave",
        "The cave beneath the canyon — a secret passage and strange machinery",
        RegionId::IkanaCanyon, {LocationId::IkanaCave}, true, true});
    m_areas.push_back({AreaId::IK_Well, "Beneath the Well",
        "The well beneath the canyon — a Poe curse and Pamela's father",
        RegionId::IkanaCanyon, {LocationId::BeneathWell}, true, true});
    m_areas.push_back({AreaId::IK_AncientCastle, "Ancient Castle of Ikana",
        "The ruined castle — Wizrobe and the rising tower",
        RegionId::IkanaCanyon, {LocationId::AncientCastleIkana}, false, true});
    m_areas.push_back({AreaId::ST_Main, "Stone Tower",
        "The Stone Tower — reaching past the sky, inverted in its temple",
        RegionId::IkanaCanyon, {LocationId::StoneTower}, false, true});
    m_areas.push_back({AreaId::ST_Temple_1F, "Stone Tower Temple (1F)",
        "First floor — the light arrows, the Elegy of Emptiness",
        RegionId::IkanaCanyon, {LocationId::StoneTowerTemple}, true, true});
    m_areas.push_back({AreaId::ST_Temple_2F, "Stone Tower Temple (2F)",
        "Second floor — the inverted tower, the death armos",
        RegionId::IkanaCanyon, {LocationId::StoneTowerTemple}, true, true});
    m_areas.push_back({AreaId::ST_Temple_Boss, "Stone Tower Temple (Twinmold)",
        "The boss chamber — Twinmold, the Giant Insect awaits",
        RegionId::IkanaCanyon, {LocationId::StoneTowerTemple}, true, true});

    // Romani Ranch
    m_areas.push_back({AreaId::RR_Main, "Romani Ranch",
        "The ranch outside Clock Town — Cremia, Romani, and the cows",
        RegionId::RomaniRanch, {LocationId::RomaniRanch}, false, true});
    m_areas.push_back({AreaId::RR_Barn, "Ranch Barn",
        "The barn — milk storage, cuccos, and a hidden fairy",
        RegionId::RomaniRanch, {LocationId::RomaniRanch}, true, true});
    m_areas.push_back({AreaId::RR_Cucco, "Cucco Pen",
        "The cucco pen — chickens and Romani's training",
        RegionId::RomaniRanch, {LocationId::RomaniRanch}, false, true});

    // Termina Field
    m_areas.push_back({AreaId::TF_Main, "Termina Field",
        "The open field connecting all regions — four exits, one heart",
        RegionId::TerminaField, {LocationId::LostWoods}, false, true});
    m_areas.push_back({AreaId::TF_LonLonRanch, "Lon Lon Ranch",
        "The ranch in the field — no Malon here, just memories of Hyrule",
        RegionId::TerminaField, {LocationId::RomaniRanch}, false, true});
    m_areas.push_back({AreaId::TF_MilkRoad, "Milk Road",
        "The road between Clock Town and Romani Ranch",
        RegionId::TerminaField, {LocationId::MilkRoad}, false, true});
    m_areas.push_back({AreaId::TF_RanchHouse, "Ranch House",
        "The house on the ranch — empty now, but not for long",
        RegionId::TerminaField, {LocationId::RomaniRanch}, true, true});

    // Moon
    m_areas.push_back({AreaId::MOON_Field, "The Moon's Surface",
        "The moon — a field of children playing beneath a grinning face",
        RegionId::Moon, {LocationId::ClockTowerTop}, false, true});
    m_areas.push_back({AreaId::MOON_Majora, "Majora's Chamber",
        "The final chamber — Majora's Mask awaits the one who ended all the curses",
        RegionId::Moon, {LocationId::ClockTowerTop}, true, true});

    // Assign areas to regions
    for (auto& area : m_areas) {
        auto it = std::find_if(m_regions.begin(), m_regions.end(),
            [&area](const RegionDef& r) { return r.id == area.region; });
        if (it != m_regions.end()) {
            it->areas.push_back(area.id);
        }
    }

    std::printf("[World] Generated %zu areas across %zu regions\n", m_areas.size(), m_regions.size());
}

const AreaDef* WorldDef::area(AreaId id) const {
    for (const auto& a : m_areas) {
        if (a.id == id) return &a;
    }
    return nullptr;
}

const RegionDef* WorldDef::region(RegionId id) const {
    for (const auto& r : m_regions) {
        if (r.id == id) return &r;
    }
    return nullptr;
}

std::vector<const AreaDef*> WorldDef::areas_in(RegionId region) const {
    std::vector<const AreaDef*> result;
    for (const auto& a : m_areas) {
        if (a.region == region) result.push_back(&a);
    }
    return result;
}

} // namespace loz_mm
