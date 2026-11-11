#include"Weapon.h"

void Weapon::InitWeapon( const TCHAR* ModelSource, const TCHAR* HandleFrame)
{
	weaponModel1 = MV1LoadModel(ModelSource);
	int ItemFrameIndex = MV1SearchFrame(weaponModel1, HandleFrame);
	if (ItemFrameIndex != -1)
	{
		MATRIX ItemGripMat = MV1GetFrameLocalWorldMatrix(weaponModel1, ItemFrameIndex);
		WeaponGripInvMat = MInverse(ItemGripMat);
	}
	else 
	{
		WeaponGripInvMat = MGetIdent();
	}
	MuzzleBoneIndex = MV1SearchFrame(weaponModel1, "銃口");
}
void Weapon::SetShapes(const TCHAR* Shapename,float ShapeRate)
{
	shapeIndex = MV1SearchShape(weaponModel1, Shapename);
	if (shapeIndex != -1)
	{
		MV1SetShapeRate(weaponModel1, shapeIndex, ShapeRate);
	}

}
void Weapon::AttachWeapon(int playerModelHandle, const char* FrameName, float offsetX, float offsetY,float offsetZ, float rotX, float rotY, float rotZ)
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
	MATRIX finalMat = WeaponGripInvMat;
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

void Weapon::DrawWeapon()
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

// 銃口からの水平な発射方向を取得（プレイヤーの正面向きで補正）
VECTOR Weapon::GetMuzzleForwardVector(float playerAngle) const
{
	if (weaponModel1 == -1 || MuzzleBoneIndex == -1)
	{
		// ボーンが見つからない場合はプレイヤーの向きから水平に飛ばす
		return VNorm(VGet(sinf(playerAngle), 0.0f, cosf(playerAngle)));
	}

	// 銃口ボーンの行列を取得
	MATRIX muzzleMat = MV1GetFrameLocalWorldMatrix(weaponModel1, MuzzleBoneIndex);

	// ボーンの方向ベクトル（Z軸成分）を取得
	VECTOR dir = VGet(muzzleMat.m[2][0], muzzleMat.m[2][1], muzzleMat.m[2][2]);

	// ★ 1. 上下への傾きをカットして「平行（水平）」にする
	dir.y = 0.0f;

	// ★ 2. プレイヤーの正面方向と内積を取り、逆向き（背中側）を向いていたら反転させる
	VECTOR playerForward = VGet(sinf(playerAngle), 0.0f, cosf(playerAngle));
	if (VDot(dir, playerForward) < 0.0f)
	{
		dir = VScale(dir, -1.0f); // 逆向きなら反転
	}

	// ベクトルがゼロベクトル（真上・真下を向いていた場合など）でなければ正規化
	if (VSize(dir) < 0.001f)
	{
		dir = playerForward;
	}

	return VNorm(dir);
}