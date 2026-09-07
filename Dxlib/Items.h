#pragma once
#include"DxLib.h"

class Items
{
	public:
		void InitItem( const TCHAR* FrameName, const TCHAR* HandleFrame);
		void SetShapes(const TCHAR* Shapename, float ShapeRate);
		void AttachItem(int playerModelHandle, const char* FrameName, float offsetX, float offsetY, float offsetZ, float rotX, float rotY, float rotZ);
		void DrawItem();
		int GetModelHandle() const { return weaponModel1; }
	private:

		int weaponModel1;
		VECTOR WEAPON01pos;
		int handBoneIndex;
		int shapeIndex;
		MATRIX ItemGripInvMat;
	
};