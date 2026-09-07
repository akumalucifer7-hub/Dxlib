#include "Field.h"

void Field::Init(const GameContext& context)
{
	// フィールドの初期化
	modelHandle = MV1LoadModel("Assets/3Dmodel/Field/Ruin/02_Ruin_Main.pmx");
	MV1SetupCollInfo(FieldModel1, -1, 8, 8, 8);
	SkyDome1= MV1LoadModel("Assets/3Dmodel/Field/Ruin/01_Ruin_Sky.pmx");
}

void Field::Update(const GameContext& context)
{
	MV1SetPosition(modelHandle, pos);
	MV1SetPosition(SkyDome1, pos);

}
void Field::Draw()
{
	MV1DrawModel(SkyDome1);
	MV1DrawModel(modelHandle);

}
bool Field::CheckGround(VECTOR pos,float& groundY)
{
	VECTOR top = VGet(pos.x, pos.y + 15.0f, pos.z);
	VECTOR bottom = VGet(pos.x, pos.y - 5.0f, pos.z);
	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(modelHandle, -1, top, bottom);
	if (hitPoly.HitFlag)
	{
		groundY = hitPoly.HitPosition.y;
		return true;
	}
	return false;	
}



VECTOR Field::WallCollision(VECTOR currentPos, VECTOR moveVec)
{
	float playerRadius = 3.0f;
	float checkOffsetY = 10.0f; 

	// 1.仮の移動先の座標を計算
	VECTOR nextPos = VAdd(currentPos, moveVec);
	VECTOR checkPos = VGet(nextPos.x, nextPos.y + checkOffsetY, nextPos.z);

	// 2.移動先の座標で球による当たり判定
	MV1_COLL_RESULT_POLY_DIM HitData = MV1CollCheck_Sphere(modelHandle, -1, checkPos, playerRadius);

	// 3.壁と衝突していた場合の「壁ずり」処理
	if (HitData.HitNum > 0)
	{
		for (int i = 0; i < HitData.HitNum; i++)
		{
			VECTOR normal = HitData.Dim[i].Normal;

			normal.y = 0.0f;

			if (VSquareSize(normal) > 0.0001f)
			{
				normal = VNorm(normal);
				float dot = VDot(moveVec, normal);

				if (dot < 0.0f)
				{
					VECTOR slideVec = VSub(moveVec, VScale(normal, dot));
					moveVec = slideVec;
					nextPos = VAdd(currentPos, moveVec);
				}
			}
		}
	}
	MV1CollResultPolyDimTerminate(HitData);

	return nextPos;
}
bool Field::CheckCeiling(VECTOR pos, float& ceilingY)
{
	VECTOR top = VGet(pos.x, pos.y + 15.0f, pos.z);
	VECTOR bottom = VGet(pos.x, pos.y - 5.0f, pos.z);
	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(modelHandle, -1, top, bottom);
	if (hitPoly.HitFlag)
	{
		ceilingY = hitPoly.HitPosition.y;
		return true;
	}
	return false;
}
bool Field::IsInsideWall(VECTOR pos)
{
	float playerRadius = 3.0f;
	VECTOR checkPos = VGet(pos.x, pos.y + 10.0f, pos.z);
	// 球による当たり判定
	MV1_COLL_RESULT_POLY_DIM HitData = MV1CollCheck_Sphere(modelHandle, -1, checkPos, playerRadius);
	bool isInside = (HitData.HitNum > 0);
	// メモリ解放
	MV1CollResultPolyDimTerminate(HitData);
	return isInside;
}

bool Field::CheckCameraLine(VECTOR startPos, VECTOR endPos, VECTOR& hitPos)
{
	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(modelHandle, -1, startPos, endPos);
	if (hitPoly.HitFlag)
	{
		// 当たった座標を返す
		hitPos = hitPoly.HitPosition;
		return true;
	}
	return false;
}