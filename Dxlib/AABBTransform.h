#pragma once
#pragma once
#include "DxLib.h"
#include "Collision.h"
#include"vector"
inline AABB TransformAABB(const AABB& local, const VECTOR& pos, float angleY)
{
    // プレイヤーの回転・平行移動行列を作成
    MATRIX rotMat = MGetRotY(angleY);
    MATRIX transMat = MGetTranslate(pos);
    MATRIX worldMat = MMult(rotMat, transMat);

	// AABBの8つの頂点を計算
	VECTOR localCorners[8] = {
		{local.min.x, local.min.y, local.min.z},
		{local.min.x, local.min.y, local.max.z},
		{local.min.x, local.max.y, local.min.z},
		{local.min.x, local.max.y, local.max.z},
		{local.max.x, local.min.y, local.min.z},
		{local.max.x, local.min.y, local.max.z},
		{local.max.x, local.max.y, local.min.z},
		{local.max.x, local.max.y, local.max.z}
	};
	AABB worldAABB;
    worldAABB.min = VGet(1e9f, 1e9f, 1e9f);
    worldAABB.max = VGet(-1e9f, -1e9f, -1e9f);

    // 8頂点をワールド変換し、最小値・最大値を更新
    for (int i = 0; i < 8; ++i) {
        VECTOR worldCorner = VTransform(localCorners[i], worldMat);
        worldAABB.min.x = (std::min)(worldAABB.min.x, worldCorner.x);
        worldAABB.min.y = (std::min)(worldAABB.min.y, worldCorner.y);
        worldAABB.min.z = (std::min)(worldAABB.min.z, worldCorner.z);

        worldAABB.max.x = (std::max)(worldAABB.max.x, worldCorner.x);
        worldAABB.max.y = (std::max)(worldAABB.max.y, worldCorner.y);
        worldAABB.max.z = (std::max)(worldAABB.max.z, worldCorner.z);
    }

    return worldAABB;
}
