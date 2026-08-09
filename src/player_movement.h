#pragma once
#include <cstdint>
#include <engine/math_types.h>

namespace loz_mm {

// Movement core for the Hero of Time. Analog input in world space drives a
// yaw-steered walk/run loop; speed blend is exponential to avoid snapping.
// All physics are deterministic and testable headless.
class PlayerController {
public:
    PlayerController() = default;

    // input: (-1..1) x/z stick; run is a held modifier
    void move(float dt, float stick_x, float stick_z, bool running);

    void jump();
    void roll();          // sets a short burst multiplier
    void face(float yaw_degrees);   // snap-turn (lock-on)

    te::Vec3 pos() const { return m_pos; }
    float yaw() const { return m_yaw; }                 // degrees, -180..180
    float speed() const { return m_speed; }             // current units/s
    bool grounded() const { return m_grounded; }
    bool rolling() const { return m_roll_timer > 0.0f; }

    void set_pos(const te::Vec3& p) { m_pos = p; }
    void set_walk_speed(float u) { m_walk = u; }
    void set_run_speed(float u) { m_run = u; }

private:
    te::Vec3 m_pos{0.f, 0.f, 0.f};
    float m_yaw = 0.f;
    float m_speed = 0.f;
    float m_walk = 3.0f;
    float m_run = 6.0f;
    float m_vertical = 0.f;
    bool m_grounded = true;
    float m_roll_timer = 0.f;
    float m_roll_burst = 1.0f;
};

} // namespace loz_mm