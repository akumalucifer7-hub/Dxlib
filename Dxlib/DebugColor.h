#pragma once
#include "DxLib.h"

namespace DebugColor {
    const unsigned int Hurtbox = GetColor(0, 255, 0);     // 被弾判定: 緑
    const unsigned int Hitbox = GetColor(255, 0, 0);     // 攻撃判定: 赤
    const unsigned int ActiveHit = GetColor(255, 255, 0);   // 命中に成功した瞬間: 黄
}