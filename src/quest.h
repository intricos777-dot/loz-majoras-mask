#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "time_cycle.h"

namespace loz_mm {

// Quest tracking system — follows MM's Bingo-style quest log.
// Quests have objectives, stages, and completion status.

enum class QuestId : uint32_t {
    MainQuest,
    KafeiQuest,
    AnjuAndKafei,
    RomaniRanch,
    BombersCode,
    PostmanEscape,
    PostmanFreedom,
    SwampArrows,
    Mikau,
    ZoraEggs,
    Lulu,
    GoronElder,
    Darmani,
    SnowheadClear,
    PoeCollector,
    PamelaFather,
    StoneTower,
    HeartPieces,
    StrayFairies,
    Skulltulas,
    MaskCollection,
    GiantSummoning,
    COUNT
};

enum class QuestStage : uint32_t {
    Inactive,       // not yet started
    Active,         // in progress
    Complete,       // done
    Failed,         // failed (can be reset with Song of Time)
    Locked,         // not yet unlocked
};

struct QuestObjective {
    std::string description;
    bool completed = false;
};

class QuestLog {
public:
    QuestLog();

    // Start a quest.
    void start_quest(QuestId quest);

    // Complete the current quest (and advance).
    void complete_quest(QuestId quest);

    // Fail a quest.
    void fail_quest(QuestId quest);

    // Check status.
    QuestStage stage(QuestId quest) const;

    // Set the current active quest for the HUD.
    void set_active(QuestId quest) { m_active = quest; }
    QuestId active() const { return m_active; }

    // Main quest progress tracking.
    uint8_t boss_defeated[4] = {0, 0, 0, 0};  // Woodfall, Snowhead, GreatBay, StoneTower
    uint8_t giants_summoned = 0;

    // Number of stray fairies collected per temple (5 each, 20 total).
    uint8_t stray_fairies[4] = {0, 0, 0, 0};

    // Skulltula houses (10 tokens needed per house, 4 houses = 40).
    uint16_t gold_skulltulas = 0;

    // Heart pieces collected.
    uint16_t heart_pieces = 0;

    // Masks traded / collected.
    uint8_t masks_traded = 0;

    // Total completed quests.
    size_t completed_count() const;

    // Quest description for the HUD.
    std::string quest_summary() const;

    const std::vector<std::pair<QuestId, QuestStage>>& all() const { return m_quests; }

private:
    QuestId m_active = QuestId::MainQuest;
    std::vector<std::pair<QuestId, QuestStage>> m_quests;

    void ensure_quest(QuestId quest);
};

} // namespace loz_mm
