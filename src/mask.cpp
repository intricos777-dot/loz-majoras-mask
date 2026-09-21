#include "mask.h"
#include <algorithm>
#include <cstdio>

namespace loz_mm {

MaskManager::MaskManager() {
    // Start with no masks.
    m_owned.fill(false);
    m_equipped = MaskId::COUNT;
    m_current_form = PlayerForm::Human;
}

void MaskManager::unlock(MaskId mask) {
    size_t idx = static_cast<size_t>(mask);
    if (idx >= m_owned.size()) return;
    if (!m_owned[idx]) {
        m_owned[idx] = true;
        std::printf("[Mask] Unlocked: %s\n", mask_name(mask));
    }
}

bool MaskManager::has(MaskId mask) const {
    size_t idx = static_cast<size_t>(mask);
    if (idx >= m_owned.size()) return false;
    return m_owned[idx];
}

bool MaskManager::transform(PlayerForm form) {
    // Check if the player has the corresponding mask.
    MaskId required;
    switch (form) {
    case PlayerForm::Deku: required = MaskId::Deku; break;
    case PlayerForm::Goron: required = MaskId::Goron; break;
    case PlayerForm::Zora: required = MaskId::Zora; break;
    case PlayerForm::FierceDeity: required = MaskId::FierceDeity; break;
    case PlayerForm::Human:
        revert();
        return true;
    default: return false;
    }
    if (!has(required)) {
        std::printf("[Mask] Cannot transform — %s not owned\n", mask_name(required));
        return false;
    }
    m_current_form = form;
    m_equipped = required;
    std::printf("[Mask] Transformed into %s\n", form_name(form));
    return true;
}

void MaskManager::revert() {
    m_current_form = PlayerForm::Human;
    m_equipped = MaskId::COUNT;
}

std::vector<MaskId> MaskManager::owned() const {
    std::vector<MaskId> result;
    for (size_t i = 0; i < m_owned.size(); ++i) {
        if (m_owned[i]) result.push_back(static_cast<MaskId>(i));
    }
    return result;
}

size_t MaskManager::owned_count() const {
    size_t count = 0;
    for (bool b : m_owned) if (b) ++count;
    return count;
}

} // namespace loz_mm
