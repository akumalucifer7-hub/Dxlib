#include"ICharacterState.h"
#include"StateRun.h"
#include"StateIdle.h"
#include"StateJump.h"
#include"StateFall.h"
#include"Character.h"
#include"Player.h"
#include"InputValidation.h"
#include"Camera.h"
#include"PlayerController.h"
void StateRun::OnEnter(Character& character, const GameContext& context)
{
	// Runアニメーションの再生設定など
	character.SetAnimation(static_cast<int>(Character::CharacterStateID::Run), true);
}

void StateRun::Update(Character& character, const GameContext& context)
{
    // --- 1. 落下判定 ---
    if (!character.GetIsGround() && character.GetVelocityY() < 0.0f)
    {
        character.ChangeState(std::make_unique<StateFall>(), context);
        return;
    }

    // --- 2. ジャンプ判定 ---
    if (cmd.isJump() && character.GetIsGround())
    {
        character.ChangeState(std::make_unique<StateJump>(), context);
        return;
    }

    // --- 4. 移動入力と処理 ---
    float lx = 0.0f;
    float ly = 0.0f;
    context.input->GetLeftStick(lx, ly, 0.2f);

    // スティック判定の「後」ではなく「前」にキー入力を反映させてからチェックする
    if (context.input->IsKeyPressed(KEY_INPUT_W) || context.input->IsKeyPressed(KEY_INPUT_UP)) { ly = 1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_S) || context.input->IsKeyPressed(KEY_INPUT_DOWN)) { ly = -1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_A) || context.input->IsKeyPressed(KEY_INPUT_LEFT)) { lx = -1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_D) || context.input->IsKeyPressed(KEY_INPUT_RIGHT)) { lx = 1.0f; }

    // 入力が一切ない場合のみ Idle へ遷移
    if (lx == 0.0f && ly == 0.0f)
    {
        character.ChangeState(std::make_unique<StateIdle>(), context);
        return;
    }

    // 歩き切り替え（LSHIFTが押されている場合）
    if (context.input->IsKeyPressed(KEY_INPUT_LSHIFT))
    {
        // player.ChangeState(std::make_unique<StateWalk>(), context);
        // return;
    }

    // カメラのY軸回転行列を取得して移動方向を決定
    MATRIX cameraRotMat = MGetRotY(context.camera->GetCameraYaw());
    VECTOR localMoveVec = VGet(lx, 0.0f, ly);
    VECTOR moveVec = VTransform(localMoveVec, cameraRotMat);

    float length = sqrtf(moveVec.x * moveVec.x + moveVec.z * moveVec.z);
    if (length > 0.0f)
    {
        moveVec = VNorm(moveVec);

        // プレイヤーの進行方向（角度）を更新
        float newAngle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
        character.SetAngle(newAngle);

        // 走りの速度（2.5f）を適用して位置を計算
        float runSpeed = 2.0f;
        VECTOR moveOffset = VScale(moveVec, runSpeed);

        VECTOR currentPos = character.GetPos();
        VECTOR finalPos = context.field->WallCollision(currentPos, moveOffset);

        currentPos.x = finalPos.x;
        currentPos.z = finalPos.z;
        character.SetPos(currentPos);
    }
}

void StateRun::OnExit(Character& character, const GameContext& context)
{
	// Run状態終了時の処理（もしあれば）
}