#pragma once

#include <cmath>
#include <vector>
#include <Windows.h>

#include "core/engine/types/Types.hpp"
#include "config/Current.hpp"

namespace LuviAim
{
    inline int HotKeyList[] = { VK_LBUTTON, VK_LMENU, VK_RBUTTON, VK_XBUTTON1, VK_XBUTTON2, VK_CAPITAL, VK_LSHIFT, VK_LCONTROL };

    inline void ApplyAimSettings()
    {
        if (cfg::aimbot::hotkey >= 0 && cfg::aimbot::hotkey < 8)
        {
            // The UI exposes the selected hotkey index; the actual hook can use this later.
        }
    }

    inline bool IsHotKeyDown()
    {
        if (cfg::aimbot::hotkey >= 0 && cfg::aimbot::hotkey < 8)
            return (GetAsyncKeyState(HotKeyList[cfg::aimbot::hotkey]) & 0x8000) != 0;
        return false;
    }

    inline void AimBot(const Vec3& localPos, const Vec3& targetPos, Vec3& viewAngle, float& fov)
    {
        if (!cfg::aimbot::enabled)
            return;

        Vec3 delta = targetPos - localPos;
        float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (distance <= 0.0f)
            return;

        float yaw = std::atan2(delta.y, delta.x) * 57.2957795f - viewAngle.y;
        float pitch = -std::atan(delta.z / distance) * 57.2957795f - viewAngle.x;
        float norm = std::sqrt(yaw * yaw + pitch * pitch);

        if (norm > cfg::aimbot::fov)
            return;

        float smooth = cfg::aimbot::smooth;
        float easedYaw = yaw * (1.0f - smooth) + viewAngle.y;
        float easedPitch = pitch * (1.0f - smooth) + viewAngle.x;

        viewAngle.y = easedYaw;
        viewAngle.x = easedPitch;
    }
}
