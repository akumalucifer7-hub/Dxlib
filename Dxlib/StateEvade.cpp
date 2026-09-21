#include"StateEvade.h"
#include"Player.h"
#include"StateIdle.h"
#include"StateRun.h"

void StateEvade::OnEnter(Player& player, const GameContext& context)
{
    // 1. 回避アニメーションの再生（非ループ）
    player.SetAnimation(Player::State::Evade, false);

    // 2. 移動方向の決定
    float lx = 0.0f;
    float ly = 0.0f;
    context.input->GetLeftStick(lx, ly, 0.2f);

    if (context.input->IsKeyPressed(KEY_INPUT_W) || context.input->IsKeyPressed(KEY_INPUT_UP)) { ly = 1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_S) || context.input->IsKeyPressed(KEY_INPUT_DOWN)) { ly = -1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_A) || context.input->IsKeyPressed(KEY_INPUT_LEFT)) { lx = -1.0f; }
    if (context.input->IsKeyPressed(KEY_INPUT_D) || context.input->IsKeyPressed(KEY_INPUT_RIGHT)) { lx = 1.0f; }

    if (lx != 0.0f || ly != 0.0f)
    {
        // 入力方向がある場合：カメラ向きに応じた入力方向へ回転して進む
        MATRIX cameraRotMat = MGetRotY(context.camera->GetCameraYaw());
        VECTOR localMoveVec = VGet(lx, 0.0f, ly);
        moveDir = VNorm(VTransform(localMoveVec, cameraRotMat));

        float newAngle = atan2f(moveDir.x, moveDir.z) + DX_PI_F;
        player.SetPlayerAngle(newAngle);
    }
    else
    {
        // 入力がない場合：プレイヤーが現在向いている正面方向へ進む
        MATRIX rotMat = MGetRotY(player.GetPlayerAngle());
        moveDir = VTransform(VGet(0.0f, 0.0f, -1.0f), rotMat);
    }

    // 3. 回避初速の設定
    evadeSpeed = 8.0f;

    // 4. 無敵フラグを有効化（※Player側で被弾判定をスキップするフラグ）
    //player.SetIsInvincible(true);
}

void StateEvade::Update(Player& player, const GameContext& context)
{
    float deltaTime = player.GetDeltaTime(); // または context から取得
    float normTime = player.GetAnimNormalizedTime();

    // --- 1. 高速移動と減衰処理 ---
    if (evadeSpeed > 0.0f)
    {
        VECTOR moveOffset = VScale(moveDir, evadeSpeed);
        VECTOR currentPos = player.GetPos();
        VECTOR finalPos = context.field->WallCollision(currentPos, moveOffset);

        currentPos.x = finalPos.x;
        currentPos.z = finalPos.z;
        player.SetPos(currentPos);

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
    if (player.GetIsAnimationEnd() || normTime >= 0.95f)
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
            player.ChangeState(std::make_unique<StateRun>(), context);
        }
        else
        {
            player.ChangeState(std::make_unique<StateIdle>(), context);
        }
        return;
    }
}
void StateEvade::OnExit(Player& player, const GameContext& context)
{
	// 回避状態終了時の処理（必要に応じて）
}