#include "game_state.h"
#include <cstdio>

namespace loz_mm {

GameState::GameState() {
    new_game();
}

void GameState::new_game() {
    m_clock.reset();
    m_current_area = AreaId::CT_South;
    m_initialized = true;
    m_play_seconds = 0;
    std::printf("[GameState] New game started — Day 1, Dawn, Clock Town\n");
}

bool GameState::save() {
    std::printf("[GameState] Save: area=%u, time=%u, rupees=%u, hearts=%u\n",
        static_cast<uint32_t>(m_current_area), m_play_seconds,
        m_inventory.rupees, m_inventory.hearts);
    return true;
}

bool GameState::load() {
    std::printf("[GameState] Load: %u play seconds\n", m_play_seconds);
    return true;
}

void GameState::update(float dt) {
    if (!m_initialized) return;
    m_clock.tick(dt);
    m_play_seconds += (uint32_t)dt;
}

std::string GameState::describe() const {
    char buf[512];
    std::snprintf(buf, sizeof(buf),
        "=== Majora's Mask State ===\n"
        "  Area: %s\n"
        "  Clock: %s\n"
        "  Masks owned: %zu/%zu\n"
        "  Songs known: %zu\n"
        "  Items collected: %zu\n"
        "  Dungeons: %zu/4 complete\n"
        "  Stray fairies: %u/20\n"
        "  Quests: %zu complete\n"
        "  %s\n"
        "  Form: %s\n",
        // area name (stub — need world lookup)
        "South Clock Town",
        m_clock.describe().c_str(),
        m_masks.owned_count(), MaskManager::kTotalMasks,
        m_inventory.known_songs().size(),
        m_inventory.total_items(),
        m_dungeons.completed_count(),
        m_dungeons.total_stray_fairies(),
        m_quests.completed_count(),
        m_quests.quest_summary().c_str(),
        form_name(m_masks.current_form()));
    return std::string(buf);
}

float GameState::completion() const {
    float c = 0;
    c += m_masks.owned_count() * 2.0f;
    c += m_inventory.known_songs().size() * 1.5f;
    c += m_dungeons.completed_count() * 10.0f;
    c += m_dungeons.total_stray_fairies() * 0.5f;
    c += m_quests.completed_count() * 3.0f;
    c += m_inventory.count(ItemId::HeartPiece) * 1.0f;
    return c / 100.0f;
}

} // namespace loz_mm
