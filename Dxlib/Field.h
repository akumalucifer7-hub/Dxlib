#pragma once
#include "DxLib.h"
#include"Items.h"
#include <cmath>
#include <algorithm>
#include"GameObject.h"
class Field : public GameObject
{
public:
	void Init(const GameContext& context) override;
	void Update(const GameContext& context) override;
	void Draw() override;
	bool CheckGround(VECTOR pos,float& groundY);
	VECTOR WallCollision(VECTOR currentPos, VECTOR moveVec);
	bool CheckCeiling(VECTOR pos, float& ceilingY);
	bool IsInsideWall(VECTOR pos);
	bool CheckCameraLine(VECTOR startPos, VECTOR endPos, VECTOR& hitPos);

private:
	int FieldModel1;
	int SkyDome1;
};