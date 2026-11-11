#pragma once
#include"DxLib.h"
#include<cmath>
class Weapon
{
	public:
		void InitWeapon( const TCHAR* FrameName, const TCHAR* HandleFrame);
		void SetShapes(const TCHAR* Shapename, float ShapeRate);
		void AttachWeapon(int playerModelHandle, const char* FrameName, float offsetX, float offsetY, float offsetZ, float rotX, float rotY, float rotZ);
		void DrawWeapon();
		int GetModelHandle() const { return weaponModel1; }
		VECTOR GetPos() const { return WEAPON01pos; }
        VECTOR GetMuzzleForwardVector(float playerAngle) const;


        // 銃口のワールド行列を取得
        MATRIX GetMuzzleWorldMatrix() const
        {
            if (weaponModel1 == -1 || MuzzleBoneIndex == -1) return MGetIdent();
            return MV1GetFrameLocalWorldMatrix(weaponModel1, MuzzleBoneIndex);
        }

        // 銃口のワールド座標を取得
        VECTOR GetMuzzlePosition() const
        {
            MATRIX mat = GetMuzzleWorldMatrix();
            return VGet(mat.m[3][0], mat.m[3][1], mat.m[3][2]);
        }

        // 銃口の正面方向ベクトル（正規化済み）を取得
        VECTOR GetMuzzleForward() const
        {
            MATRIX mat = GetMuzzleWorldMatrix();
            // モデルの軸に合わせる（一般的なMMD武器モデルでは+Z方向または+Y方向）
            VECTOR dir = VGet(mat.m[2][0], mat.m[2][1], mat.m[2][2]);
            return VNorm(dir);
        }
	private:

		int weaponModel1;
		VECTOR WEAPON01pos;
		int handBoneIndex;
		int MuzzleBoneIndex;
		int shapeIndex;
		MATRIX WeaponGripInvMat;
	
};