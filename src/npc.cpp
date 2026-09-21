#include "npc.h"
#include <algorithm>
#include <cstdio>

namespace loz_mm {

// Helper: find the latest schedule entry at or before the given hour.
static const ScheduleEntry* find_entry(const std::vector<ScheduleEntry>& sched, uint32_t hour) {
    const ScheduleEntry* best = nullptr;
    for (const auto& e : sched) {
        if (e.hour <= hour) {
            if (!best || e.hour > best->hour) best = &e;
        }
    }
    return best;
}

// ---- NpcManager ----

NpcManager::NpcManager() {
    generate_default_schedules();
}

LocationId NpcManager::location_at(NpcId npc, uint32_t total_seconds) const {
    const NpcDef* def = this->npc(npc);
    if (!def) return LocationId::Unknown;
    if (def->schedule.empty()) return def->default_location;
    uint32_t hour = (total_seconds / 60u) % 72u;
    const ScheduleEntry* e = find_entry(def->schedule, hour);
    if (e) return e->location;
    return def->default_location;
}

std::string NpcManager::action_at(NpcId npc, uint32_t total_seconds) const {
    const NpcDef* def = this->npc(npc);
    if (!def) return "";
    if (def->schedule.empty()) return "idle";
    uint32_t hour = (total_seconds / 60u) % 72u;
    const ScheduleEntry* e = find_entry(def->schedule, hour);
    if (e) return e->action;
    return "idle";
}

bool NpcManager::can_talk_at(NpcId npc, uint32_t total_seconds) const {
    const NpcDef* def = this->npc(npc);
    if (!def) return false;
    if (def->schedule.empty()) return true;
    uint32_t hour = (total_seconds / 60u) % 72u;
    const ScheduleEntry* e = find_entry(def->schedule, hour);
    if (e) return e->can_talk;
    return true;
}

const NpcDef* NpcManager::npc(NpcId npc) const {
    for (const auto& d : m_npcs) {
        if (d.id == npc) return &d;
    }
    return nullptr;
}

std::vector<const NpcDef*> NpcManager::npcs_at(LocationId loc, uint32_t total_seconds) const {
    std::vector<const NpcDef*> result;
    for (const auto& d : m_npcs) {
        if (location_at(d.id, total_seconds) == loc) {
            result.push_back(&d);
        }
    }
    return result;
}

// ---- Default schedule generation ----

void NpcManager::generate_default_schedules() {
    m_npcs.clear();

    // ---------- Anju (Innkeeper's Daughter) ----------
    // Her schedule is the heart of the game's most beloved quest.
    // Day 1: Receiving guests in lobby (0-14h)
    // Day 1 night: Kitchen prep (14-20h)
    // Day 2: Afternoon room cleaning, waiting for Kafei (6-18h, but she leaves)
    // Day 2 night: Returns to inn, distraught (18-24h)
    // Day 3: Lobby all day, afternoon panic at missing Kafei (0-18h)
    //        Then Carnival prep — until the wedding at the end.
    {
        NpcDef anju;
        anju.name = "Anju";
        anju.id = NpcId::Anju;
        anju.default_location = LocationId::StockpotInnLobby;
        anju.description = "Innkeeper's daughter, waiting for Kafei through every third day";
        anju.schedule = {
            // Day 1 (0-24h)
            {0, LocationId::StockpotInnRoom, "resting in her room"},
            {6, LocationId::StockpotInnKitchen, "preparing breakfast"},
            {8, LocationId::StockpotInnLobby, "greeting guests in the lobby"},
            {12, LocationId::StockpotInnLobby, "managing front desk", true},
            {14, LocationId::StockpotInnKitchen, "afternoon tea prep"},
            {17, LocationId::StockpotInnLobby, "evening reception"},
            {20, LocationId::StockpotInnRoom, "night prep, worrying about Kafei"},
            // Day 2 (24-48h)
            {24, LocationId::StockpotInnRoom, "restless morning"},
            {30, LocationId::StockpotInnLobby, "distracted at the front desk"},
            {32, LocationId::StockpotInnRoom, "hiding tears in her room"},
            {36, LocationId::StockpotInnKitchen, "nervous lunch prep"},
            {40, LocationId::StockpotInnLobby, "afternoon — still no word from Kafei"},
            {44, LocationId::StockpotInnRoom, "weeping quietly"},
            // Day 3 (48-72h)
            {48, LocationId::StockpotInnLobby, "day 3 — holding it together"},
            {54, LocationId::StockpotInnLobby, "afternoon — Kafei should be here by now..."},
            {58, LocationId::StockpotInnRoom, "dreading the Carnival without Kafei"},
            {62, LocationId::StockpotInnLobby, "final hour — the wedding must happen"},
            {68, LocationId::StockpotInnBalcony, "watching the moon, holding hope"},
        };
        m_npcs.push_back(anju);
    }

    // ---------- Kafei ----------
    // The groom. Hiding in Sakon's hideout after being turned young.
    // Player must reunite him with Anju.
    {
        NpcDef kafei;
        kafei.name = "Kafei";
        kafei.id = NpcId::Kafei;
        kafei.default_location = LocationId::LaundryHidden;
        kafei.description = "The groom who vanished — turned young by Skull Kid's curse";
        kafei.schedule = {
            // Hiding in Laundry Pool's secret room most of the time
            {0, LocationId::LaundryHidden, "hiding as a child, brooding"},
            {12, LocationId::LaundryHidden, "stealing his own wedding mask back"},
            {24, LocationId::LaundryHidden, "planning escape from Sakon"},
            {30, LocationId::CuriosityShopBack, "sneaking through Curiosity Shop"},
            {36, LocationId::LaundryHidden, "regrouping, gathering items"},
            {48, LocationId::LaundryHidden, "day 3 — preparing for the wedding"},
            {60, LocationId::StockpotInnRoom, "rushing to Anju"},
            {65, LocationId::StockpotInnBalcony, "the wedding"},
        };
        m_npcs.push_back(kafei);
    }

    // ---------- Guru-Guru (Musician) ----------
    // Member of the Indigo-Go's. Plays in Zora Hall.
    {
        NpcDef guru;
        guru.name = "Guru-Guru";
        guru.id = NpcId::GuruGuru;
        guru.default_location = LocationId::ZoraBandRoom;
        guru.description = "The Indigo-Go's big-hearted bassist, always slightly off-tempo";
        guru.schedule = {
            // Zora Hall — most of the time
            {0, LocationId::ZoraHall, "loitering near the stage"},
            {6, LocationId::ZoraBandRoom, "rehearsing bass lines"},
            {12, LocationId::ZoraBandRoom, "practicing 'Elegy of Emptiness'"},
            {18, LocationId::ZoraHall, "afternoon tea at the cafe"},
            {22, LocationId::ZoraBandRoom, "evening rehearsal"},
            // Day 2
            {24, LocationId::ZoraHall, "morning — where is Lulu?"},
            {30, LocationId::ZoraBandRoom, "frantic practice, the show must go on"},
            {38, LocationId::ZoraHall, "moping about Lulu's eggs"},
            // Day 3
            {48, LocationId::ZoraBandRoom, "final rehearsal before the Carnival"},
            {56, LocationId::ZoraBandRoom, "preparing costumes"},
            {64, LocationId::ZoraBandRoom, "playing for the Carnival of Time"},
        };
        m_npcs.push_back(guru);
    }

    // ---------- Postman ----------
    // Never misses a deadline. Runs his route three times a day.
    {
        NpcDef postman;
        postman.name = "Postman";
        postman.id = NpcId::Postman;
        postman.default_location = LocationId::PostOffice;
        postman.description = "Pat — the postman who has never missed a deadline, not even for the moon";
        postman.schedule = {
            // Day 1 — 3 runs/day (every 8 hours starting at 6am)
            {0, LocationId::PostOffice, "sorting mail at the office"},
            {6, LocationId::SouthClockTown, "morning delivery route"},
            {8, LocationId::EastClockTown, "East Town delivery"},
            {10, LocationId::WestClockTown, "West Town delivery"},
            {12, LocationId::NorthClockTown, "North Town + Mayor's Office delivery"},
            {14, LocationId::PostOffice, "afternoon sorting"},
            {18, LocationId::SouthClockTown, "evening delivery run"},
            {21, LocationId::ClockTowerEntry, "delivering to the Bombers"},
            {23, LocationId::PostOffice, "closing up the office"},
            // Day 2
            {24, LocationId::PostOffice, "day 2 morning sort"},
            {30, LocationId::EastClockTown, "East route"},
            {34, LocationId::WestClockTown, "West route"},
            {38, LocationId::PostOffice, "afternoon at the office"},
            {42, LocationId::NorthClockTown, "evening run"},
            // Day 3 — Final day, increasingly frantic
            {48, LocationId::PostOffice, "final day sorting — the mustache test!"},
            {54, LocationId::SouthClockTown, "morning run, checking the sky"},
            {60, LocationId::PostOffice, "afternoon — deciding whether to flee"},
            {66, LocationId::PostOffice, "final hour — the escape decision"},
            {70, LocationId::ClockTowerEntry, "ringing the escape alarm"},
        };
        m_npcs.push_back(postman);
    }

    // ---------- Mayor Dotar ----------
    // Obsessed with the Carnival, indecisive.
    {
        NpcDef mayor;
        mayor.name = "Mayor Dotar";
        mayor.id = NpcId::MayorDotar;
        mayor.default_location = LocationId::MayorOffice;
        mayor.description = "Clock Town's indecisive mayor, paralyzed between the festival and the crisis";
        mayor.schedule = {
            {0, LocationId::MayorOfficeHall, "sleeping"},
            {8, LocationId::MayorOffice, "reading citizen petitions"},
            {12, LocationId::MayorOffice, "indecisive staff meeting"},
            {16, LocationId::MayorOfficeHall, "family dinner"},
            {20, LocationId::MayorOffice, "late-night policy review"},
            {24, LocationId::MayorOffice, "day 2 — still debating the Carnival"},
            {36, LocationId::MayorOffice, "afternoon — more petitions"},
            {48, LocationId::MayorOffice, "day 3 — the clock is ticking"},
            {60, LocationId::MayorOfficeHall, "evening — ignoring the moon"},
            {68, LocationId::MayorOffice, "final hour — still in his office"},
        };
        m_npcs.push_back(mayor);
    }

    // ---------- Madame Aroma ----------
    // Mayor's wife. Runs the Milk Bar.
    {
        NpcDef aroma;
        aroma.name = "Madame Aroma";
        aroma.id = NpcId::MadameAroma;
        aroma.default_location = LocationId::MilkBar;
        aroma.description = "Mayor's wife, owner of the Milk Bar, worried about her missing son Kafei";
        aroma.schedule = {
            {0, LocationId::MayorOfficeHall, "resting"},
            {7, LocationId::MilkBar, "opening the Milk Bar"},
            {10, LocationId::MilkBar, "serving customers"},
            {14, LocationId::MilkBar, "afternoon rush"},
            {18, LocationId::MilkBar, "evening — Lon Lon Milk special"},
            {22, LocationId::MayorOfficeHall, "returning home, worried about Kafei"},
            {24, LocationId::MilkBar, "day 2 — serving, but distracted"},
            {36, LocationId::MilkBar, "afternoon — word from Kafei?"},
            {48, LocationId::MilkBar, "day 3 — the festival and her son"},
            {60, LocationId::MilkBar, "final evening — waiting for news"},
            {68, LocationId::ClockTowerEntry, "heading to the Carnival"},
        };
        m_npcs.push_back(aroma);
    }

    // ---------- Bomber (the kid) ----------
    {
        NpcDef bomber;
        bomber.name = "Bomber";
        bomber.id = NpcId::Bomber;
        bomber.default_location = LocationId::EastClockTown;
        bomber.description = "The Bombers' leader, running the secret code club";
        bomber.schedule = {
            {0, LocationId::EastClockTown, "night patrol"},
            {8, LocationId::ClockTowerEntry, "watching the Clock Tower"},
            {12, LocationId::EastClockTown, "drilling the secret code"},
            {16, LocationId::EastClockTown, "bomb-related shenanigans"},
            {20, LocationId::EastClockTown, "evening code practice"},
            {24, LocationId::EastClockTown, "day 2 — the code is almost cracked"},
            {40, LocationId::ClockTowerEntry, "staking out the tower"},
            {48, LocationId::EastClockTown, "day 3 — entering the tunnel"},
            {60, LocationId::ClockTowerEntry, "race to the top"},
        };
        m_npcs.push_back(bomber);
    }

    // ---------- Sakon (the thief) ----------
    {
        NpcDef sakon;
        sakon.name = "Sakon";
        sakon.id = NpcId::Sakon;
        sakon.default_location = LocationId::LaundryPool;
        sakon.description = "The cunning thief who stole Kafei's mask and wedding goods";
        sakon.schedule = {
            {0, LocationId::LaundryPool, "lurking by the laundry pool"},
            {10, LocationId::LaundryPool, "waiting for victims"},
            {18, LocationId::LaundryPool, "the hideout is this way"},
            {24, LocationId::LaundryPool, "day 2 — still guarding the secret room"},
            {40, LocationId::LaundryPool, "getting nervous"},
            {48, LocationId::LaundryPool, "day 3 — time is running out"},
            {62, LocationId::LaundryPool, "final hour — will he give up the sun mask?"},
        };
        m_npcs.push_back(sakon);
    }

    // ---------- Cremia (Romani Ranch) ----------
    // Runs the ranch, guards against the aliens.
    {
        NpcDef cremia;
        cremia.name = "Cremia";
        cremia.id = NpcId::Cremia;
        cremia.default_location = LocationId::RomaniRanch;
        cremia.description = "Eldest of the Romani sisters, protector of the ranch and its milk";
        cremia.schedule = {
            {0, LocationId::RomaniRanch, "night watch"},
            {5, LocationId::RomaniRanch, "milking the cows"},
            {8, LocationId::RomaniRanch, "loading milk bottles"},
            {12, LocationId::RomaniRanch, "delivering milk to town"},
            {14, LocationId::MilkBar, "dropping off milk at the bar"},
            {16, LocationId::RomaniRanch, "afternoon prep"},
            {20, LocationId::RomaniRanch, "brewing Chateau Romani"},
            {24, LocationId::RomaniRanch, "day 2 — the aliens are coming"},
            {30, LocationId::RomaniRanch, "reinforcing the barn"},
            {40, LocationId::RomaniRanch, "evening — drinking with Gorman"},
            {48, LocationId::RomaniRanch, "day 3 — the final defense"},
            {56, LocationId::RomaniRanch, "night before the aliens arrive"},
            {64, LocationId::RomaniRanch, "the alien attack begins"},
            {68, LocationId::RomaniRanch, "defending the ranch with Romani"},
        };
        m_npcs.push_back(cremia);
    }

    // ---------- Romani (farm girl) ----------
    {
        NpcDef romani;
        romani.name = "Romani";
        romani.id = NpcId::Romani;
        romani.default_location = LocationId::RomaniRanch;
        romani.description = "The farm girl who saw the moon grow a face and taught the hero to ride";
        romani.schedule = {
            {0, LocationId::RomaniRanch, "sleeping"},
            {6, LocationId::RomaniRanch, "horseback practice"},
            {10, LocationId::RomaniRanch, "teaching Epona's song"},
            {14, LocationId::RomaniRanch, "target practice with the bow"},
            {18, LocationId::RomaniRanch, "evening chores"},
            {24, LocationId::RomaniRanch, "day 2 — preparing for the aliens"},
            {36, LocationId::RomaniRanch, "day 2 night — the attack"},
            {48, LocationId::RomaniRanch, "day 3 — final preparations"},
            {60, LocationId::RomaniRanch, "waiting for the attack"},
            {66, LocationId::RomaniRanch, "fighting off the aliens"},
        };
        m_npcs.push_back(romani);
    }

    // ---------- Lulu (Zora singer) ----------
    {
        NpcDef lulu;
        lulu.name = "Lulu";
        lulu.id = NpcId::Lulu;
        lulu.default_location = LocationId::ZoraLuluRoom;
        lulu.description = "The Zora singer whose voice the tribe stole — her eggs hold her lost eggs";
        lulu.schedule = {
            {0, LocationId::ZoraLuluRoom, "resting, mourning her eggs"},
            {8, LocationId::ZoraHall, "day 1 — first appearance, angry at the world"},
            {14, LocationId::ZoraLuluRoom, "refusing to eat"},
            {20, LocationId::ZoraHall, "evening — avoiding the band"},
            {24, LocationId::ZoraLuluRoom, "day 2 — can she sing again?"},
            {36, LocationId::ZoraLuluRoom, "the eggs are stolen!"},
            {42, LocationId::ZoraHall, "angry, lost"},
            {48, LocationId::ZoraLuluRoom, "day 3 — can she find her voice?"},
            {60, LocationId::ZoraBandRoom, "recovered — rehearsing for the Carnival"},
            {68, LocationId::ZoraBandRoom, "singing for the Carnival"},
        };
        m_npcs.push_back(lulu);
    }

    // ---------- Mikau (Zora guitarist, hero) ----------
    {
        NpcDef mikau;
        mikau.name = "Mikau";
        mikau.id = NpcId::Mikau;
        mikau.default_location = LocationId::GreatBayCoast;
        mikau.description = "The Zora guitarist who tried to save Lulu's eggs — wounded on the shore";
        mikau.schedule = {
            // Mikau is wounded at the coast when you first meet him
            {0, LocationId::GreatBayCoast, "collapsed on the beach, dying"},
            {10, LocationId::GreatBayCoast, "recounting the story to Link"},
            {20, LocationId::GreatBayCoast, "teaching the New Wave Bossa Nova"},
            {24, LocationId::GreatBayCoast, "day 2 — still waiting"},
            {40, LocationId::GreatBayCoast, "fading..."},
            {48, LocationId::GreatBayCoast, "day 3 — his final hours"},
            {60, LocationId::GreatBayCoast, "the New Wave Bossa Nova"},
        };
        m_npcs.push_back(mikau);
    }

    // ---------- Darmani (Goron hero) ----------
    {
        NpcDef darmani;
        darmani.name = "Darmani III";
        darmani.id = NpcId::Darmani;
        darmani.default_location = LocationId::MountainVillage;
        darmani.description = "The Goron hero who died chasing Goht — his spirit asks for a song of healing";
        darmani.schedule = {
            // Darmani is a ghost you meet at his grave
            {0, LocationId::MountainVillage, "his grave site in the blizzard"},
            {12, LocationId::MountainVillage, "spirit appears to the one who plays the Song of Healing"},
            {24, LocationId::MountainVillage, "day 2 — his frozen village"},
            {36, LocationId::MountainVillage, "waiting for the Song of Healing"},
            {48, LocationId::MountainVillage, "day 3 — the blizzard intensifies"},
            {60, LocationId::GoronElderCave, "visiting the Goron elder"},
        };
        m_npcs.push_back(darmani);
    }

    // ----------
    // Happy Mask Salesman
    // ----------
    {
        NpcDef hms;
        hms.name = "Happy Mask Salesman";
        hms.id = NpcId::HappyMaskSalesman;
        hms.default_location = LocationId::ClockTowerEntry;
        hms.description = "He sells masks, and he buys them — his smile is a transaction";
        hms.schedule = {
            {0, LocationId::ClockTowerEntry, "waiting at the Clock Tower, smiling"},
            {10, LocationId::SouthClockTown, "wandering, looking for masks"},
            {20, LocationId::LaundryPool, "checking on something..."},
            {24, LocationId::ClockTowerEntry, "day 2 — 'You've met with a terrible haven't you?'"},
            {36, LocationId::NorthClockTown, "selling masks near the observatory"},
            {48, LocationId::ClockTowerEntry, "day 3 — the carnival approaches"},
            {62, LocationId::ClockTowerEntry, "final hour — the transaction nears"},
        };
        m_npcs.push_back(hms);
    }

    // ----------
    // Tael (fairy captive)
    // ----------
    {
        NpcDef tael;
        tael.name = "Tael";
        tael.id = NpcId::Tael;
        tael.default_location = LocationId::ClockTowerTop;
        tael.description = "The quiet fairy, taken with the Skull Kid into the mask's shadow";
        tael.schedule = {
            {0, LocationId::ClockTowerTop, "trapped with the Skull Kid"},
            {60, LocationId::ClockTowerTop, "the Skull Kid's companion"},
            {70, LocationId::ClockTowerTop, "before the final battle"},
        };
        m_npcs.push_back(tael);
    }

    // ----------
    // Skull Kid
    // ----------
    {
        NpcDef sk;
        sk.name = "Skull Kid";
        sk.id = NpcId::SkullKid;
        sk.default_location = LocationId::ClockTowerTop;
        sk.description = "The lonely imp riding a stolen mask and a stolen moon";
        sk.schedule = {
            {0, LocationId::ClockTowerTop, "taunting Link from the tower"},
            {36, LocationId::ClockTowerTop, "day 2 — the moon grows larger"},
            {60, LocationId::ClockTowerTop, "day 3 — sitting on the clock, laughing"},
            {70, LocationId::ClockTowerTop, "the final confrontation"},
        };
        m_npcs.push_back(sk);
    }

    // ----------
    // Kamaro (dancing NPC)
    // ----------
    {
        NpcDef kamaro;
        kamaro.name = "Kamaro";
        kamaro.id = NpcId::Kamaro;
        kamaro.default_location = LocationId::NorthClockTown;
        kamaro.description = "The dancer who died of a broken spirit — needs the Song of Healing to dance again";
        kamaro.schedule = {
            {0, LocationId::NorthClockTown, "his ghost, dancing in the woods"},
            {10, LocationId::EastClockTown, "morning dance spot"},
            {20, LocationId::NorthClockTown, "afternoon dance"},
            {24, LocationId::NorthClockTown, "day 2 — still dancing"},
            {40, LocationId::EastClockTown, "spreading his dance"},
            {48, LocationId::NorthClockTown, "day 3 — his spirit lingers"},
            {64, LocationId::NorthClockTown, "awaiting the Song of Healing"},
        };
        m_npcs.push_back(kamaro);
    }

    // ----------
    // Pamela & her father (Ikana)
    // ----------
    {
        NpcDef pamela;
        pamela.name = "Pamela";
        pamela.id = NpcId::Pamela;
        pamela.default_location = LocationId::BeneathWell;
        pamela.description = "The girl whose father was turned into a Poe by the well's curse";
        pamela.schedule = {
            {0, LocationId::BeneathWell, "afraid of her father in the well"},
            {12, LocationId::BeneathWell, "checking the well lock"},
            {24, LocationId::BeneathWell, "day 2 — the music box plays"},
            {40, LocationId::IkanaGraveyard, "checking the graveyard"},
            {48, LocationId::BeneathWell, "day 3 — still locked in fear"},
            {60, LocationId::BeneathWell, "the well's secret deepens"},
        };
        m_npcs.push_back(pamela);
    }

    // ----------
    // Gorman (the ranch hand)
    // ----------
    {
        NpcDef gorman;
        gorman.name = "Gorman";
        gorman.id = NpcId::Gorman;
        gorman.default_location = LocationId::RomaniRanch;
        gorman.description = "The ranch hand with a longing gaze, Romani's unspoken admirer";
        gorman.schedule = {
            {0, LocationId::RomaniRanch, "night watch"},
            {8, LocationId::RomaniRanch, "delivering milk to town"},
            {12, LocationId::MilkBar, "picking up Milk"},
            {14, LocationId::RomaniRanch, "afternoon chores"},
            {18, LocationId::RomaniRanch, "evening — talking with Cremia"},
            {24, LocationId::RomaniRanch, "day 2 — helping prepare for aliens"},
            {40, LocationId::RomaniRanch, "drinking with Cremia at dusk"},
            {48, LocationId::RomaniRanch, "day 3 — the night of the attack"},
            {62, LocationId::RomaniRanch, "defending the ranch, quietly in love"},
        };
        m_npcs.push_back(gorman);
    }

    // ----------
    // Goron Elder
    // ----------
    {
        NpcDef goronElder;
        goronElder.name = "Goron Elder";
        goronElder.id = NpcId::GoronElder;
        goronElder.default_location = LocationId::GoronElderCave;
        goronElder.description = "The frozen Goron elder who needs the Lullaby to awaken";
        goronElder.schedule = {
            {0, LocationId::GoronElderCave, "frozen in ice, snoring"},
            {10, LocationId::GoronElderCave, "the Lullaby of Storms must be played"},
            {24, LocationId::GoronElderCave, "day 2 — still frozen"},
            {48, LocationId::GoronElderCave, "day 3 — the blizzard rages"},
            {60, LocationId::GoronElderCave, "awakened? teaching the Goron Lullaby"},
        };
        m_npcs.push_back(goronElder);
    }

    std::printf("[NPC] Generated default schedules for %zu characters\n", m_npcs.size());
}

} // namespace loz_mm
