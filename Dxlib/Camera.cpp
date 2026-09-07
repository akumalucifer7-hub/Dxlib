#include"Camera.h"

void Camera::initCamera(int SCREENWIDTH, int SCREENHEIGHT)
{
	cameraYaw = 0.0f;
	cameraPitch = 0.3f;
	SetMousePoint(SCREENWIDTH / 2, SCREENHEIGHT / 2);
}

void Camera::updateCamera(int SCREENWIDTH, int SCREENHEIGHT,VECTOR pos, Field& field, InputValidation& input)
{
	SetMouseDispFlag(FALSE);

        float RX;
        float RY;
		input.GetRightStick(RX, RY, 0.2f);
        cameraYaw += RX * 0.03f;
        cameraPitch += RY * 0.03f;
		if (cameraPitch > 1.0f) cameraPitch = 1.0f;
		if (cameraPitch < -1.0f) cameraPitch = -1.0f;
		//マウスから入力
		input.GetMousePosition(mouseX, mouseY);
		float deltaYaw = 0.0f;
		float deltaPitch = 0.0f;
		// 0.002fマウス感度
		deltaYaw = (mouseX - (SCREENWIDTH / 2)) * 0.002f;   
		deltaPitch = (mouseY - (SCREENHEIGHT / 2)) * 0.002f;

		SetMousePoint(SCREENWIDTH / 2, SCREENHEIGHT / 2);
		cameraYaw += deltaYaw;
		cameraPitch += deltaPitch;
		if (cameraPitch > 1.0f) cameraPitch = 1.0f;
		if (cameraPitch < -1.0f) cameraPitch = -1.0f;
	

	TargetPos = VAdd(pos, VGet(0.0f, 10.0f, 0.0f));
	VECTOR IdealCameraPos;
	IdealCameraPos.x = pos.x - cosf(cameraPitch) * sinf(cameraYaw) * cameraDistance;
	IdealCameraPos.y = pos.y + sinf(cameraPitch) * cameraDistance + 15.0f;
	IdealCameraPos.z = pos.z - cosf(cameraPitch) * cosf(cameraYaw) * cameraDistance;
	
		VECTOR hitPos;
		if (field.CheckCameraLine(TargetPos, IdealCameraPos, hitPos))
		{
			VECTOR targetToHit = VSub(hitPos, TargetPos);
			float hitDist = VSize(targetToHit);
			float offset = 1.0f;
			if (hitDist < offset)
			{
				
				offset = hitDist * 0.5f; 
			}
			VECTOR toTarget = VNorm(VSub(TargetPos, IdealCameraPos));
			CameraPos = VAdd(hitPos, VScale(toTarget, offset));
			float currentDist = VSize(VSub(CameraPos, TargetPos));
			float minDist = 10.0f; 

			if (currentDist < minDist)
			{
				CameraPos.y += (minDist - currentDist);
			}
		}
		else
		{
			CameraPos = IdealCameraPos;
		}


	SetCameraPositionAndTarget_UpVecY(CameraPos, TargetPos);
	SetCameraNearFar(2.0f, 1000.0f);
}

float Camera::GetCameraYaw()
{
	return cameraYaw;
}

