#include "PlayerController.h"
#include "InputValidation.h"
#include "Character.h"
#include "Camera.h"

CommandState PlayerController::GetCommand(const Character& owner, const GameContext& context) const 
{
    CommandState cmd;
    auto& input = InputValidation::Instance();

    // 1. 移動入力 (WASD / パッドの左スティック)
    float stickX = 0.0f, stickY = 0.0f;
    input.GetLeftStick(stickX, stickY);

    if (stickX != 0.0f || stickY != 0.0f)
    {
        cmd.moveX = stickX;
        cmd.moveY = stickY;
    }
    else
    {
        if (input.IsKeyPressed(KEY_INPUT_W)) cmd.moveY += 1.0f;
        if (input.IsKeyPressed(KEY_INPUT_S)) cmd.moveY -= 1.0f;
        if (input.IsKeyPressed(KEY_INPUT_A)) cmd.moveX -= 1.0f;
        if (input.IsKeyPressed(KEY_INPUT_D)) cmd.moveX += 1.0f;
    }
	if (cmd.moveX != 0.0f || cmd.moveY != 0.0f)
	{
        if (context.camera)
        {
            // カメラのY軸回転行列を作成
            MATRIX cameraRotMat = MGetRotY(context.camera->GetCameraYaw());

            // ローカル入力ベクトル (X, 0, Y)
            VECTOR localMoveVec = VGet(cmd.moveX, 0.0f, cmd.moveY);

            // カメラ回転を行列乗算してワールド空間の移動ベクトルへ変換
            VECTOR worldMoveVec = VTransform(localMoveVec, cameraRotMat);

            cmd.moveX = worldMoveVec.x;
            cmd.moveY = worldMoveVec.z;
        }
        else
        {
            // カメラがない場合のフォールバック（そのまま代入）
			cmd.moveX = cmd.moveX;
			cmd.moveY = cmd.moveY;
        }
	}
    // 2. 行動入力
    // 攻撃 (左クリック または XInput Xボタン)
    if (input.IsMouseTrigger(InputValidation::Mouse::Left) ||
        input.IsButtonTrigger(XINPUT_BUTTON_X))
    {
        cmd.isAttack = true;
    }

    // 回避 (右クリック または XInput Bボタン)
    if (input.IsMouseTrigger(InputValidation::Mouse::Right) ||
        input.IsButtonTrigger(XINPUT_BUTTON_B))
    {
        cmd.isEvade = true;
    }

    // ジャンプ (Spaceキー または XInput Aボタン)
    if (input.IsKeyTrigger(KEY_INPUT_SPACE) ||
        input.IsButtonTrigger(XINPUT_BUTTON_A))
    {
        cmd.isJump = true;
    }

    // 特殊行動 (Shiftキー または XInput Yボタン)
    if (input.IsKeyTrigger(KEY_INPUT_LSHIFT) ||
        input.IsButtonTrigger(XINPUT_BUTTON_Y))
    {
        cmd.isSpecial = true;
    }



    return cmd;
}