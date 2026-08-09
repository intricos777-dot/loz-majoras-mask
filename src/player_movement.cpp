#include "player_movement.h"
#include <algorithm>
#include <cmath>

namespace loz_mm {

namespace {
constexpr float kDeg = 3.14159265f / 180.0f;
constexpr float kGravity = 25.f;
constexpr float kJumpV = 9.5f;
constexpr float kRollDuration = 0.45f;
constexpr float kRollBoost = 1.9f;
}

void PlayerController::face(float yaw_degrees) {
    m_yaw = std::fmod(yaw_degrees, 360.f);
    if (m_yaw > 180.f) m_yaw -= 360.f;
    if (m_yaw < -180.f) m_yaw += 360.f;
}

void PlayerController::jump() {
    if (!m_grounded) return;
    m_vertical = kJumpV;
    m_grounded = false;
}

void PlayerController::roll() {
    if (m_roll_timer > 0.f) return;
    m_roll_timer = kRollDuration;
    m_roll_burst = kRollBoost;
}

void PlayerController::move(float dt, float stick_x, float stick_z, bool running) {
    if (dt <= 0.f) return;

    // dead zone
    float mag = std::sqrt(stick_x * stick_x + stick_z * stick_z);
    if (mag > 1.f) { stick_x /= mag; stick_z /= mag; mag = 1.f; }
    if (mag < 0.05f) { stick_x = 0.f; stick_z = 0.f; mag = 0.f; }

    const float target = (running ? m_run : m_walk) * mag;
    if (m_roll_timer > 0.f) {
        m_roll_timer -= dt;
        if (m_roll_timer <= 0.f) m_roll_burst = 1.f;
    }
    const float target_speed = target * m_roll_burst;
    // exponential blend - smooth takeoff and braking
    const float blend = 1.f - std::exp(-8.f * dt);
    m_speed += (target_speed - m_speed) * blend;
    if (mag < 0.05f && std::fabs(m_speed) < 0.02f) m_speed = 0.f;

    // yaw: face the stick direction
    if (mag > 0.05f) {
        const float goal = std::atan2(stick_x, stick_z) / kDeg;
        float diff = goal - m_yaw;
        while (diff > 180.f) diff -= 360.f;
        while (diff < -180.f) diff += 360.f;
        m_yaw += std::clamp(diff, -14.f * dt, 14.f * dt);
    }

    // integrate facing-forward displacement
    const float s = m_speed;
    const float c = std::cos(m_yaw * kDeg);
    const float sn = std::sin(m_yaw * kDeg);
    m_pos.x += sn * s * dt;
    m_pos.z += c * s * dt;

    // vertical integrate
    if (!m_grounded) m_vertical -= kGravity * dt;
    m_pos.y += m_vertical * dt;
    if (m_pos.y <= 0.f) { m_pos.y = 0.f; m_vertical = 0.f; m_grounded = true; }
}

} // namespace loz_mm