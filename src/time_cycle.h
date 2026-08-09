#pragma once
#include <cstdint>
#include <string>

namespace loz_mm {

// In-game clock for Majora's Mask: one in-game minute per real second,
// three in-game days = 4320 in-game minutes. Nights shift the calendar
// and the Moon; the Song of Time rewinds the clock but keeps progress.
enum class DayPhase { Dawn, Morning, Noon, Afternoon, Dusk, Night };

static constexpr uint32_t kSecondsPerDay = 24u * 60u;      // 1440
static constexpr uint32_t kSecondsForThreeDays = 3u * kSecondsPerDay;
static constexpr uint32_t kMoonFallSecond = 3u * kSecondsPerDay - 60u;  // final in-game minute

class DayCycle {
public:
    DayCycle() = default;

    // advances the clock by `dt` in-game seconds; returns the number of
    // dawn crossovers triggered this tick (0 unless a day just rolled over)
    uint32_t tick(float dt);

    // The Song of Time: time rewinds to the dawn of Day 1, the Moon pulls
    // back; cleared sectors stay remembered for owl-credit.
    void song_of_time();

    void reset();                       // fresh file: Day 1, Dawn
    void set_debug_hour(uint32_t h);    // test hook

    uint32_t day() const { return m_day; }          // 1..3
    uint32_t hours() const { return m_seconds / 60u; }
    uint32_t minutes() const { return m_seconds % 60u; }
    uint32_t whole_seconds() const { return m_seconds; }
    bool moon_falling() const { return m_moon_fall; }
    bool wrapped() const { return m_wrapped; }      // finished the whole cycle
    float night_depth() const;                      // 0f (noon) .. 1f (midnight)

    DayPhase phase() const;

    std::string describe() const;

private:
    uint32_t m_seconds = 0;   // seconds since the dawn of Day 1
    uint32_t m_day = 1;
    bool m_moon_fall = false;
    bool m_wrapped = false;
    uint32_t m_cycles = 0;
};

} // namespace loz_mm