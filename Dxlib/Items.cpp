#include"Items.h"

void Items::InitItem( const TCHAR* ModelSource, const TCHAR* HandleFrame)
{
	weaponModel1 = MV1LoadModel(ModelSource);
	int ItemFrameIndex = MV1SearchFrame(weaponModel1, HandleFrame);
	if (ItemFrameIndex != -1)
	{
		MATRIX ItemGripMat = MV1GetFrameLocalWorldMatrix(weaponModel1, ItemFrameIndex);
		ItemGripInvMat = MInverse(ItemGripMat);
	}
	else 
	{
		ItemGripInvMat = MGetIdent();
	}
}
void Items::SetShapes(const TCHAR* Shapename,float ShapeRate)
{
	shapeIndex = MV1SearchShape(weaponModel1, Shapename);
	if (shapeIndex != -1)
	{
		MV1SetShapeRate(weaponModel1, shapeIndex, ShapeRate);
	}

}
void Items::AttachItem(int playerModelHandle, const char* FrameName, float offsetX, float offsetY,float offsetZ, float rotX, float rotY, float rotZ)
{
	handBoneIndex = MV1SearchFrame(playerModelHandle, FrameName);
	if (handBoneIndex == -1)
	{
		return;
	}
	// ボーンの行列を取得
	MATRIX boneMat = MV1GetFrameLocalWorldMatrix(playerModelHandle, handBoneIndex);


	MATRIX translateMat = MGetTranslate(VGet(offsetX, offsetY, offsetZ));

	// 回転行列（ここではY軸を中心に20度回転）
	MATRIX rotMatX = MGetRotX(rotX * DX_PI_F / 180.0f);
	MATRIX rotMatY = MGetRotY(rotY * DX_PI_F / 180.0f);
	MATRIX rotMatZ = MGetRotZ(rotZ * DX_PI_F / 180.0f);
	// 行列を正しい順番で合成する
	// まず持ち手を原点に合わせる
	MATRIX finalMat = ItemGripInvMat;
	//持ち手を中心に回転させる

	finalMat = MMult(finalMat, rotMatZ);
	finalMat = MMult(finalMat, rotMatX);
	finalMat = MMult(finalMat, rotMatY);
	// 回転した状態から指定方向にずらす
	finalMat = MMult(finalMat, translateMat);
	// 最後にプレイヤーの手の位置・向きに合わせる
	finalMat = MMult(finalMat, boneMat);

	// 武器モデルに反映
	MV1SetMatrix(weaponModel1, finalMat);
}

void Items::DrawItem()
{
	if (weaponModel1 != -1)
	{
		MV1DrawModel(weaponModel1);
	}
	else
	{
		// 読み込みに失敗したらエラーログを出す
		DrawString(260, 300, "Item Model Load Failed", GetColor(255, 0, 0));
	}
}	
