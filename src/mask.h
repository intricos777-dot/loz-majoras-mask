#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include "time_cycle.h"

namespace loz_mm {

// Mask categories — transformation masks change the body, story masks change the quest.
enum class MaskType : uint32_t {
    Transformation,
    Story,
    Accessory,
};

enum class MaskId : uint32_t {
    // Transformation masks
    Deku = 0,
    Goron,
    Zora,
    FierceDeity,

    // Story / ability masks
    Keaton,
    BunnyHood,
    PostmansHat,
    AllNightMask,
    BlastMask,
    StoneMask,
    GreatFairyMask,
    MaskOfTruth,
    MaskOfScents,
    RomanisMask,
    GarosMask,
    CaptainsMask,
    GibdoMask,
    DonGerosMask,
    KamarosMask,
    CouplesMask,
    KafeisMask,
    BremenMask,
    BremenHat,
    BigBomb,
    GiantMask,
    Garo, // generic Garo mask
    COUNT
};

inline const char* mask_name(MaskId id) {
    switch (id) {
    case MaskId::Deku: return "Deku Mask";
    case MaskId::Goron: return "Goron Mask";
    case MaskId::Zora: return "Zora Mask";
    case MaskId::FierceDeity: return "Fierce Deity's Mask";
    case MaskId::Keaton: return "Keaton Mask";
    case MaskId::BunnyHood: return "Bunny Hood";
    case MaskId::PostmansHat: return "Postman's Hat";
    case MaskId::AllNightMask: return "All-Night Mask";
    case MaskId::BlastMask: return "Blast Mask";
    case MaskId::StoneMask: return "Stone Mask";
    case MaskId::GreatFairyMask: return "Great Fairy's Mask";
    case MaskId::MaskOfTruth: return "Mask of Truth";
    case MaskId::MaskOfScents: return "Mask of Scents";
    case MaskId::RomanisMask: return "Romani's Mask";
    case MaskId::GarosMask: return "Garo's Mask";
    case MaskId::CaptainsMask: return "Captain's Hat";
    case MaskId::GibdoMask: return "Gibdo Mask";
    case MaskId::DonGerosMask: return "Don Gero's Mask";
    case MaskId::KamarosMask: return "Kamaro's Mask";
    case MaskId::CouplesMask: return "Couple's Mask";
    case MaskId::KafeisMask: return "Kafei's Mask";
    case MaskId::BremenMask: return "Bremen Mask";
    case MaskId::BremenHat: return "Bremen Hat";
    case MaskId::BigBomb: return "Big Bomb";
    case MaskId::GiantMask: return "Giant's Mask";
    case MaskId::Garo: return "Garo Mask";
    default: return "Unknown Mask";
    }
}

// Does this mask transform the player?
inline bool is_transformation(MaskId id) {
    return id == MaskId::Deku || id == MaskId::Goron || id == MaskId::Zora || id == MaskId::FierceDeity;
}

// Player form after transformation.
enum class PlayerForm : uint32_t {
    Human = 0,
    Deku,
    Goron,
    Zora,
    FierceDeity,
};

inline const char* form_name(PlayerForm f) {
    switch (f) {
    case PlayerForm::Human: return "Human";
    case PlayerForm::Deku: return "Deku";
    case PlayerForm::Goron: return "Goron";
    case PlayerForm::Zora: return "Zora";
    case PlayerForm::FierceDeity: return "Fierce Deity";
    default: return "Unknown";
    }
}

struct MaskDef {
    MaskId id;
    MaskType type;
    std::string description;
    std::string lore;
};

// Mask system: tracks which masks the player has, and handles transformations.
class MaskManager {
public:
    MaskManager();

    // Unlock a mask (collected or given).
    void unlock(MaskId mask);

    // Check if a mask is owned.
    bool has(MaskId mask) const;

    // Transform into a different form (only if the corresponding mask is owned).
    bool transform(PlayerForm form);

    // Revert to human.
    void revert();

    // Currently equipped mask (which one is on your head right now).
    MaskId equipped_mask() const { return m_equipped; }

    // Current body form.
    PlayerForm current_form() const { return m_current_form; }

    // List all owned masks.
    std::vector<MaskId> owned() const;

    // Total masks (alpha target: 24).
    size_t owned_count() const;
    static constexpr size_t kTotalMasks = 24;

private:
    std::array<bool, (size_t)MaskId::COUNT> m_owned{};
    MaskId m_equipped = MaskId::COUNT;  // none
    PlayerForm m_current_form = PlayerForm::Human;
};

} // namespace loz_mm
