#pragma once
#include "DxLib.h"
#include "Collision.h" // AABB構造体が定義されているヘッダー

namespace DebugRenderer {
    // ワイヤーフレームでAABBを描画する
    inline void DrawAABB(const AABB& aabb, unsigned int color) {
        VECTOR c[8] = {
            VGet(aabb.min.x, aabb.min.y, aabb.min.z), // 0: min
            VGet(aabb.max.x, aabb.min.y, aabb.min.z), // 1
            VGet(aabb.min.x, aabb.max.y, aabb.min.z), // 2
            VGet(aabb.max.x, aabb.max.y, aabb.min.z), // 3
            VGet(aabb.min.x, aabb.min.y, aabb.max.z), // 4
            VGet(aabb.max.x, aabb.min.y, aabb.max.z), // 5
            VGet(aabb.min.x, aabb.max.y, aabb.max.z), // 6
            VGet(aabb.max.x, aabb.max.y, aabb.max.z)  // 7: max
        };

        // 底面 (Z-min)
        DrawLine3D(c[0], c[1], color); DrawLine3D(c[1], c[3], color);
        DrawLine3D(c[3], c[2], color); DrawLine3D(c[2], c[0], color);

        // 上面 (Z-max)
        DrawLine3D(c[4], c[5], color); DrawLine3D(c[5], c[7], color);
        DrawLine3D(c[7], c[6], color); DrawLine3D(c[6], c[4], color);

        // 柱 (Y方向)
        DrawLine3D(c[0], c[4], color); DrawLine3D(c[1], c[5], color);
        DrawLine3D(c[2], c[6], color); DrawLine3D(c[3], c[7], color);
    }
}