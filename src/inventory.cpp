#include "inventory.h"
#include <algorithm>
#include <cstdio>

namespace loz_mm {

Inventory::Inventory() {
    // Start with Ocarina and some basics.
    add_item(ItemId::OcarinaOfTime, 1);
    add_item(ItemId::KokiriSword, 1);
    add_item(ItemId::Rupee, 200);
    hearts = 3;
    max_hearts = 3;
    magic = 32;
    max_magic = 32;

    // Start with Deku Mask (first thing you get)
    // add_item(ItemId::DekuMaskItem, 1);  // given in game

    // Equipment defaults
    m_equipment.fill(ItemId::COUNT);
    m_c_buttons.fill(ItemId::COUNT);
    m_items.fill(0);
    m_songs.fill(false);
}

uint8_t Inventory::add_item(ItemId item, uint8_t count) {
    size_t idx = static_cast<size_t>(item);
    if (idx >= m_items.size()) return 0;
    // Handle rupees specially.
    if (item == ItemId::Rupee) {
        add_rupees(count);
        return count;
    }
    // Key items don't stack.
    if (item >= ItemId::DekuMaskItem && item < ItemId::Note) {
        if (m_items[idx] == 0) {
            m_items[idx] = 1;
            std::printf("[Inventory] Got %s\n", item_name(item));
            return 1;
        }
        return 0;
    }
    // Normal consumables stack up to 99.
    uint8_t added = std::min(count, (uint8_t)(99 - m_items[idx]));
    m_items[idx] += added;
    return added;
}

bool Inventory::remove_item(ItemId item, uint8_t count) {
    if (item == ItemId::Rupee) return remove_rupees(count);
    size_t idx = static_cast<size_t>(item);
    if (idx >= m_items.size()) return false;
    if (m_items[idx] < count) return false;
    m_items[idx] -= count;
    return true;
}

uint8_t Inventory::count(ItemId item) const {
    if (item == ItemId::Rupee) return rupees;
    size_t idx = static_cast<size_t>(item);
    if (idx >= m_items.size()) return 0;
    return m_items[idx];
}

bool Inventory::has_item(ItemId item) const {
    if (item == ItemId::Rupee) return rupees > 0;
    size_t idx = static_cast<size_t>(item);
    if (idx >= m_items.size()) return false;
    return m_items[idx] > 0;
}

void Inventory::set_equipment(ItemId item, uint8_t slot) {
    if (slot < 4) m_equipment[slot] = item;
}

ItemId Inventory::equipment(uint8_t slot) const {
    if (slot < 4) return m_equipment[slot];
    return ItemId::COUNT;
}

size_t Inventory::song_index(ItemId song) {
    switch (song) {
    case ItemId::SongOfTime: return 0;
    case ItemId::SongOfHealing: return 1;
    case ItemId::SongOfStorms: return 2;
    case ItemId::SongOfSoaring: return 3;
    case ItemId::EponaSong: return 4;
    case ItemId::SongOfDoubleTime: return 5;
    case ItemId::InvertedSongOfTime: return 6;
    case ItemId::SongOfSlowTime: return 7;
    case ItemId::OathToOrder: return 8;
    case ItemId::GoronLullaby: return 9;
    case ItemId::NewWaveBossaNova: return 10;
    case ItemId::ElegyOfEmptiness: return 11;
    case ItemId::SongOfAwakening: return 12;
    default: return 15; // sentinel/invalid
    }
}

bool Inventory::learn_song(ItemId song) {
    size_t idx = song_index(song);
    if (idx >= 16) return false;
    if (!m_songs[idx]) {
        m_songs[idx] = true;
        std::printf("[Song] Learned: %s\n", item_name(song));
        return true;
    }
    return false;
}

bool Inventory::knows_song(ItemId song) const {
    size_t idx = song_index(song);
    if (idx >= 16) return false;
    return m_songs[idx];
}

std::vector<ItemId> Inventory::known_songs() const {
    std::vector<ItemId> result;
    static const ItemId song_list[] = {
        ItemId::SongOfTime,
        ItemId::SongOfHealing,
        ItemId::SongOfStorms,
        ItemId::SongOfSoaring,
        ItemId::EponaSong,
        ItemId::SongOfDoubleTime,
        ItemId::InvertedSongOfTime,
        ItemId::SongOfSlowTime,
        ItemId::OathToOrder,
        ItemId::GoronLullaby,
        ItemId::NewWaveBossaNova,
        ItemId::ElegyOfEmptiness,
        ItemId::SongOfAwakening,
    };
    for (size_t i = 0; i < 13; ++i) {
        if (m_songs[i]) result.push_back(song_list[i]);
    }
    return result;
}

void Inventory::assign_c_button(uint8_t slot, ItemId item) {
    if (slot < 3) m_c_buttons[slot] = item;
}

ItemId Inventory::c_button(uint8_t slot) const {
    if (slot < 3) return m_c_buttons[slot];
    return ItemId::COUNT;
}

size_t Inventory::total_items() const {
    size_t total = 0;
    for (auto c : m_items) if (c > 0) ++total;
    total += known_songs().size();
    // Count mask items
    for (size_t i = static_cast<size_t>(ItemId::DekuMaskItem);
         i <= static_cast<size_t>(ItemId::GiantMask); ++i) {
        if (m_items[i] > 0) ++total;
    }
    return total;
}

} // namespace loz_mm
