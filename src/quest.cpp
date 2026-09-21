#include "quest.h"
#include <algorithm>
#include <cstdio>

namespace loz_mm {

QuestLog::QuestLog() {
    // Initialize all quests as locked or inactive.
    for (uint32_t i = 0; i < static_cast<uint32_t>(QuestId::COUNT); ++i) {
        QuestId q = static_cast<QuestId>(i);
        QuestStage stage = QuestStage::Inactive;
        // Main quest is active from the start.
        if (q == QuestId::MainQuest) stage = QuestStage::Active;
        m_quests.push_back({q, stage});
    }
    m_active = QuestId::MainQuest;
}

void QuestLog::start_quest(QuestId quest) {
    ensure_quest(quest);
    for (auto& [q, s] : m_quests) {
        if (q == quest && (s == QuestStage::Inactive || s == QuestStage::Locked)) {
            s = QuestStage::Active;
            std::printf("[Quest] Started: %u\n", static_cast<uint32_t>(quest));
            return;
        }
    }
}

void QuestLog::complete_quest(QuestId quest) {
    for (auto& [q, s] : m_quests) {
        if (q == quest && s == QuestStage::Active) {
            s = QuestStage::Complete;
            std::printf("[Quest] Completed: %u\n", static_cast<uint32_t>(quest));
            return;
        }
    }
}

void QuestLog::fail_quest(QuestId quest) {
    for (auto& [q, s] : m_quests) {
        if (q == quest && s == QuestStage::Active) {
            s = QuestStage::Failed;
            std::printf("[Quest] Failed: %u\n", static_cast<uint32_t>(quest));
            return;
        }
    }
}

QuestStage QuestLog::stage(QuestId quest) const {
    for (const auto& [q, s] : m_quests) {
        if (q == quest) return s;
    }
    return QuestStage::Inactive;
}

void QuestLog::ensure_quest(QuestId quest) {
    // No-op: quests are pre-populated.
    (void)quest;
}

size_t QuestLog::completed_count() const {
    size_t count = 0;
    for (const auto& [q, s] : m_quests) {
        if (s == QuestStage::Complete) ++count;
    }
    return count;
}

std::string QuestLog::quest_summary() const {
    char buf[256];
    std::snprintf(buf, sizeof(buf),
        "Quests: %zu complete | Active: %u | Bosses: %d/4 | Giants: %d/4 | SF: %u/20 | HS: %u/16",
        completed_count(),
        static_cast<uint32_t>(m_active),
        (boss_defeated[0] + boss_defeated[1] + boss_defeated[2] + boss_defeated[3]),
        giants_summoned,
        (stray_fairies[0] + stray_fairies[1] + stray_fairies[2] + stray_fairies[3]),
        heart_pieces);
    return std::string(buf);
}

} // namespace loz_mm
