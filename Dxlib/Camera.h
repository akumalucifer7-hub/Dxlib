#pragma once
#include "DxLib.h"
#include "Field.h"
#include"InputValidation.h"
#include <cmath>
#include <algorithm>

class Camera
{
public:

	void initCamera(int SCREENWIDTH, int SCREENHEIGHT);
	void updateCamera(int SCREENWIDTH, int SCREENHEIGHT, VECTOR pos, Field& field, InputValidation& input);
	float GetCameraYaw();

private:


	MATRIX cameraRotMat;
	VECTOR CameraPos;
	VECTOR TargetPos;

	VECTOR moveVec;

	float cameraYaw = 0.0f;
	float cameraPitch = 0.3f;
	float cameraAngleH = 0.0f;
	float cameraAngleV = 0.5f;
	const float cameraDistance = 30.0f;
	// ゲームパッドの入力状態を格納する構造体
	XINPUT_STATE input;

	int mouseX, mouseY;
};