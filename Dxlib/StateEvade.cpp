#include"StateEvade.h"
#include"Player.h"
#include"StateIdle.h"
#include"StateRun.h"
#include"Camera.h"
void StateEvade::OnEnter(Character& character, const GameContext& context)
{
    // 1. 回避アニメーションの再生（非ループ）
    character.SetAnimation(static_cast<int>(Character::CharacterStateID::Evade), false);

    // 2. 移動方向の決定
    float lx = 0.0f;
    float ly = 0.0f;
    context.input->GetLeftStick(lx, ly, 0.2f);
    float keyX = 0.0f;
    float keyY = 0.0f;
    if (context.input->IsKeyPressed(KEY_INPUT_W) || context.input->IsKeyPressed(KEY_INPUT_UP)) { keyY = 1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_S) || context.input->IsKeyPressed(KEY_INPUT_DOWN)) { keyY = -1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_A) || context.input->IsKeyPressed(KEY_INPUT_LEFT)) { keyX = -1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_D) || context.input->IsKeyPressed(KEY_INPUT_RIGHT)) { keyX = 1.0f; }

    // キーボード入力があれば優先して置き換え
    if (keyX != 0.0f || keyY != 0.0f)
    {
        lx = keyX;
        ly = keyY;
    }
    if (lx != 0.0f || ly != 0.0f)
    {
        // 入力方向がある場合：カメラ向きに応じた入力方向へ回転して進む
        MATRIX cameraRotMat = MGetRotY(context.camera->GetCameraYaw());
        VECTOR localMoveVec = VGet(lx, 0.0f, ly);
        moveDir = VNorm(VTransform(localMoveVec, cameraRotMat));

        float newAngle = atan2f(moveDir.x, moveDir.z) + DX_PI_F;
        character.SetAngle(newAngle);
    }
    else
    {
        // 入力がない場合：プレイヤーが現在向いている正面方向へ進む
        MATRIX rotMat = MGetRotY(character.GetAngle());
        moveDir = VTransform(VGet(0.0f, 0.0f, -1.0f), rotMat);
    }

    // 3. 回避初速の設定
    evadeSpeed = 6.0f;

    // 4. 無敵フラグを有効化（※Player側で被弾判定をスキップするフラグ）
    //player.SetIsInvincible(true);
}

void StateEvade::Update(Character& character, const GameContext& context)
{
    float deltaTime = character.GetDeltaTime(); // または context から取得
    float normTime = character.GetAnimNormalizedTime();

    // --- 1. 高速移動と減衰処理 ---
    if (evadeSpeed > 0.0f)
    {
        VECTOR moveOffset = VScale(moveDir, evadeSpeed);
        VECTOR currentPos = character.GetPos();
        VECTOR finalPos = context.field->WallCollision(currentPos, moveOffset);

        currentPos.x = finalPos.x;
        currentPos.z = finalPos.z;
        character.SetPos(currentPos);

        // フレーム経過とともに減速
        evadeSpeed -= 25.0f * deltaTime;
        if (evadeSpeed < 0.0f) evadeSpeed = 0.0f;
    }

    // --- 2. 無敵区間の制御 ---
    // 例: アニメーションの 10% 〜 60% の間のみ無敵を付与
   /* if (normTime >= 0.1f && normTime <= 0.6f)
    {
        player.SetIsInvincible(true);
    }
    else
    {
        player.SetIsInvincible(false);
    }*/


    // --- 4. 回避終了判定 ---
    if (character.GetIsAnimationEnd() || normTime >= 0.95f)
    {
        // 終了時に入力があれば Run、なければ Idle へ戻る
        float lx = 0.0f;
        float ly = 0.0f;
        context.input->GetLeftStick(lx, ly, 0.2f);
        bool isMoveKeyPressed =
            context.input->IsKeyPressed(KEY_INPUT_W) || context.input->IsKeyPressed(KEY_INPUT_UP) ||
            context.input->IsKeyPressed(KEY_INPUT_S) || context.input->IsKeyPressed(KEY_INPUT_DOWN) ||
            context.input->IsKeyPressed(KEY_INPUT_A) || context.input->IsKeyPressed(KEY_INPUT_LEFT) ||
            context.input->IsKeyPressed(KEY_INPUT_D) || context.input->IsKeyPressed(KEY_INPUT_RIGHT);

        if (isMoveKeyPressed || lx != 0.0f || ly != 0.0f)
        {
            character.ChangeState(std::make_unique<StateRun>(), context);
        }
        else
        {
            character.ChangeState(std::make_unique<StateIdle>(), context);
        }
        return;
    }
}

void StateEvade::OnExit(Character& character, const GameContext& context)
{
	// 回避状態終了時の処理（必要に応じて）
}