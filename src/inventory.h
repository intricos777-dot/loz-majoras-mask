#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include "time_cycle.h"
#include "mask.h"

namespace loz_mm {

// Items the player can collect and use.
enum class ItemId : uint32_t {
    // Consumables
    RedPotion = 0,
    GreenPotion,
    BluePotion,
    Milk,
    ChateauRomani,
    LonLonMilk,
    Nuts,
    Sticks,
    MagicBeans,
    Bombs,
    Bombchus,
    PowderKeg,
    Arrows,
    FireArrows,
    IceArrows,
    LightArrows,

    // Equipment
    KokiriSword,
    RazorSword,
    GildedSword,
    MirrorShield,
    Quiver30,
    Quiver40,
    Quiver50,
    BombBag20,
    BombBag30,
    BombBag40,

    // Ocarina & Songs
    OcarinaOfTime,
    SongOfTime,
    SongOfHealing,
    SongOfStorms,
    SongOfSoaring,
    EponaSong,
    SongOfDoubleTime,
    InvertedSongOfTime,
    SongOfSlowTime,
    OathToOrder,
    GoronLullaby,
    NewWaveBossaNova,
    ElegyOfEmptiness,
    SongOfEmpty,
    SongOfAwakening,

    // Key items
    DekuMaskItem,
    GoronMaskItem,
    ZoraMaskItem,
    FierceDeityMaskItem,
    KafeisMask,
    CouplesMask,
    BlastMask,
    StoneMask,
    PostmansHat,
    AllNightMask,
    BunnyHood,
    KeatonMask,
    GaroMask,
    RomanisMask,
    CaptainsHat,
    MaskOfTruth,
    MaskOfScents,
    BremenMask,
    KamarosMask,
    GibdoMask,
    DonGerosMask,
    GreatFairyMask,
    BremenHat,
    BigBomb,
    GiantMask,

    // Dungeon items
    WoodfallKey,
    SnowheadKey,
    GreatBayKey,
    StoneTowerKey,
    BossKey_Woodfall,
    BossKey_Snowhead,
    BossKey_GreatBay,
    BossKey_StoneTower,
    Compass_Woodfall,
    Compass_Snowhead,
    Compass_GreatBay,
    Compass_StoneTower,
    Map_Woodfall,
    Map_Snowhead,
    Map_GreatBay,
    Map_StoneTower,

    // Misc
    HeartContainer,
    HeartPiece,
    StrayFairy,
    SkulltulaSoul,
    GoldSkulltula,
    Note,

    Rupee,
    COUNT
};

inline const char* item_name(ItemId id) {
    switch (id) {
    case ItemId::RedPotion: return "Red Potion";
    case ItemId::GreenPotion: return "Green Potion";
    case ItemId::BluePotion: return "Blue Potion";
    case ItemId::Milk: return "Milk";
    case ItemId::ChateauRomani: return "Chateau Romani";
    case ItemId::LonLonMilk: return "Lon Lon Milk";
    case ItemId::Nuts: return "Deku Nuts";
    case ItemId::Sticks: return "Deku Sticks";
    case ItemId::MagicBeans: return "Magic Beans";
    case ItemId::Bombs: return "Bombs";
    case ItemId::Bombchus: return "Bombchus";
    case ItemId::PowderKeg: return "Powder Keg";
    case ItemId::Arrows: return "Arrows";
    case ItemId::FireArrows: return "Fire Arrows";
    case ItemId::IceArrows: return "Ice Arrows";
    case ItemId::LightArrows: return "Light Arrows";
    case ItemId::KokiriSword: return "Kokiri Sword";
    case ItemId::RazorSword: return "Razor Sword";
    case ItemId::GildedSword: return "Gilded Sword";
    case ItemId::MirrorShield: return "Mirror Shield";
    case ItemId::Quiver30: return "Quiver (30)";
    case ItemId::Quiver40: return "Quiver (40)";
    case ItemId::Quiver50: return "Quiver (50)";
    case ItemId::BombBag20: return "Bomb Bag (20)";
    case ItemId::BombBag30: return "Bomb Bag (30)";
    case ItemId::BombBag40: return "Bomb Bag (40)";
    case ItemId::OcarinaOfTime: return "Ocarina of Time";
    case ItemId::SongOfTime: return "Song of Time";
    case ItemId::SongOfHealing: return "Song of Healing";
    case ItemId::SongOfStorms: return "Song of Storms";
    case ItemId::SongOfSoaring: return "Song of Soaring";
    case ItemId::EponaSong: return "Epona's Song";
    case ItemId::SongOfDoubleTime: return "Song of Double Time";
    case ItemId::InvertedSongOfTime: return "Inverted Song of Time";
    case ItemId::SongOfSlowTime: return "Song of Slow Time";
    case ItemId::OathToOrder: return "Oath to Order";
    case ItemId::GoronLullaby: return "Goron Lullaby";
    case ItemId::NewWaveBossaNova: return "New Wave Bossa Nova";
    case ItemId::ElegyOfEmptiness: return "Elegy of Emptiness";
    case ItemId::Rupee: return "Rupee";
    case ItemId::HeartContainer: return "Heart Container";
    case ItemId::HeartPiece: return "Heart Piece";
    case ItemId::StrayFairy: return "Stray Fairy";
    default: return "Unknown Item";
    }
}

struct ItemSlot {
    ItemId item = ItemId::COUNT;
    uint8_t count = 0;
    uint8_t max_count = 99;
};

struct SongSlot {
    ItemId song = ItemId::COUNT;
    bool learned = false;
};

class Inventory {
public:
    Inventory();

    // Add an item. Returns the quantity actually added.
    uint8_t add_item(ItemId item, uint8_t count = 1);

    // Remove an item. Returns true if enough were removed.
    bool remove_item(ItemId item, uint8_t count = 1);

    // Get count of a consumable item.
    uint8_t count(ItemId item) const;

    // Does the player have this key item?
    bool has_item(ItemId item) const;

    // Equipment slots (consumables for equipment items).
    void set_equipment(ItemId item, uint8_t slot);  // slot 0-3
    ItemId equipment(uint8_t slot) const;

    // Song slots (up to 16 songs).
    bool learn_song(ItemId song);
    bool knows_song(ItemId song) const;
    std::vector<ItemId> known_songs() const;

    // C buttons — three quick-access slots.
    void assign_c_button(uint8_t slot /*0..2*/, ItemId item);
    ItemId c_button(uint8_t slot) const;

    // Currency.
    uint16_t rupees = 0;
    void add_rupees(uint16_t n) { rupees += n; }
    bool remove_rupees(uint16_t n) {
        if (rupees < n) return false;
        rupees -= n;
        return true;
    }

    // Health.
    uint8_t hearts = 3;
    uint8_t max_hearts = 3;
    uint8_t magic = 32;
    uint8_t max_magic = 32;

    // Total items collected.
    size_t total_items() const;

private:
    // Consumable items keyed by ItemId index.
    std::array<uint8_t, (size_t)ItemId::COUNT> m_items{};

    // Equipment — 4 slots (Sword, Shield, Tunic, Boots... or Bombs, Arrows, Beans, Sticks).
    std::array<ItemId, 4> m_equipment{};

    // C buttons.
    std::array<ItemId, 3> m_c_buttons{};

    // Songs learned.
    std::array<bool, 16> m_songs{};
    static size_t song_index(ItemId song);
};

} // namespace loz_mm
