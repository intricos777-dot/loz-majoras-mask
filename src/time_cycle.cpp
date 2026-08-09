#include "time_cycle.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace loz_mm {

uint32_t DayCycle::tick(float dt) {
    if (m_wrapped) return 0;
    const uint32_t before = m_seconds / kSecondsPerDay;
    m_seconds += (uint32_t)(dt * 1.0f);   // 1 real second == 1 in-game minute
    if (m_seconds >= kSecondsForThreeDays) m_seconds = kSecondsPerDay * 3u;

    const uint32_t after = m_seconds / kSecondsPerDay;   // 0,1,2,3
    // the 0->1 and 1->2 boundaries are "dawn"; the 2->3 cap is the wrap
    const uint32_t dawns = (after > before && after < 3u) ? (after - before) : 0u;
    m_day = std::min(3u, after + 1u);
    if (m_seconds >= kMoonFallSecond) m_moon_fall = true;
    if (m_seconds >= kSecondsForThreeDays) m_wrapped = true;
    return dawns;
}

void DayCycle::song_of_time() {
    m_seconds = 0;
    m_day = 1;
    m_moon_fall = false;
    ++m_cycles;
}

void DayCycle::reset() {
    m_seconds = 0;
    m_day = 1;
    m_moon_fall = false;
    m_wrapped = false;
    m_cycles = 0;
}

void DayCycle::set_debug_hour(uint32_t h) {
    m_seconds = (h % 24u) * 60u;
}

DayPhase DayCycle::phase() const {
    const uint32_t h = hours() % 24u;
    if (h < 5) return DayPhase::Night;
    if (h < 7) return DayPhase::Dawn;
    if (h < 12) return DayPhase::Morning;
    if (h < 15) return DayPhase::Noon;
    if (h < 18) return DayPhase::Afternoon;
    if (h < 20) return DayPhase::Dusk;
    return DayPhase::Night;
}

float DayCycle::night_depth() const {
    // full dark at midnight, none at noon; sinusoidal across the day
    const float t = (float)(hours() % 24u) / 24.0f;
    const float x = 6.28318f * (t - 0.5f); // noon at t=0.5 -> cos(0)=1 -> dark=0
    const float dark = (1.0f - std::cos(x)) * 0.5f;
    return dark < 0.0f ? 0.0f : (dark > 1.0f ? 1.0f : dark);
}

std::string DayCycle::describe() const {
    const char* ph = "?";
    switch (phase()) {
        case DayPhase::Dawn: ph = "dawn"; break;
        case DayPhase::Morning: ph = "morning"; break;
        case DayPhase::Noon: ph = "noon"; break;
        case DayPhase::Afternoon: ph = "afternoon"; break;
        case DayPhase::Dusk: ph = "dusk"; break;
        case DayPhase::Night: ph = "night"; break;
    }
    char buf[128];
    std::snprintf(buf, sizeof(buf), "day %u @ %02u:%02u (%s)%s",
                  m_day, hours() % 24u, minutes(), ph,
                  m_moon_fall ? " - MOON FALL" : "");
    return buf;
}

} // namespace loz_mm