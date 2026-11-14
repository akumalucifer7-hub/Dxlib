#include "Winmain.h"
#include "Winmain.h"
#include "GameContext.h"
#include <algorithm>
#include <windows.h>
#include "AudioManager.h"
#include "EffectManager.h"
#include "TitleScene.h"
#include "GameOverScene.h"

#pragma warning(disable: C4819)
using namespace DxLib;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 1. グラフィックモード選択プロンプト
    int msgboxID = MessageBoxW(
        NULL,
        L"高画質モード(影あり・アンチエイリアス有効)で起動しますか？\n[いいえ]を選択すると低負荷モード(影なし)で起動します。",
        L"グラフィックモードを選択してください",
        MB_YESNO | MB_ICONQUESTION
    );

    if (msgboxID == IDCANCEL) return 0;

    int selectMode = (msgboxID == IDYES) ? 1 : 0;
    int screenWidth = (selectMode == 1) ? 1920 : 1280;
    int screenHeight = (selectMode == 1) ? 1080 : 720;

    // 2. DXライブラリ初期化
    SetUseCharSet(DX_CHARSET_UTF8);
    SetGraphMode(screenWidth, screenHeight, 32);
    SetUse3DFlag(TRUE);
    SetUseDirect3DVersion(DX_DIRECT3D_11);
    if (DxLib_Init() == -1) return -1;

    if (!EffectManager::Instance().Init(8000))
    {
        DxLib_End();
        return -1;
    }
    EffectManager::Instance().Load("MuzzleFlash", "Assets/Effects/muzzle_flash/muzzle_flash.efkefc", 0.5f);
    EffectManager::Instance().Load("HitSlash", "Assets/Effects/hit_slash/hit_slash.efkefc", 2.0f);
    SetDrawScreen(DX_SCREEN_BACK);

    // 3. ゲーム管理変数 & シーン初期化
    GameScene gameScene;
    TitleScene titleScene;
    GameOverScene gameOverScene;

    ModeState modeState = ModeState::TitleMode;
    InputValidation& input = InputValidation::Instance();
    GameContext context;
    context.input = &input;

    // ★ 重要：元の設計通り、重いロード処理(InitGame)はメインループに入る前に終わらせておく
    gameScene.InitGame(selectMode, screenWidth, screenHeight);
    titleScene.Init();

    AudioManager::Instance().Load("Run", "Assets/Sounds/Run.mp3");
    AudioManager::Instance().Load("Hit", "Assets/Sounds/Hit.mp3");
    AudioManager::Instance().Load("Parry", "Assets/Sounds/Parry.mp3");

    // 時間計測の開始
    LONGLONG prevTime = GetNowHiPerformanceCount();

    // 4. メインゲームループ
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0 && modeState != ModeState::ExitMode)
    {
        ClearDrawScreen();
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        // --- デルタタイム計算 ---
        LONGLONG currentTime = GetNowHiPerformanceCount();
        float rawDeltaTime = static_cast<float>(currentTime - prevTime) / 1000000.0f;
        prevTime = currentTime;
        context.deltaTime = (std::min)(rawDeltaTime, 0.1f);

        // --- 入力更新 ---
        input.ValidateInputUpdate();

        // --- シーン・ゲーム状態の更新 & 描画 ---
        switch (modeState)
        {
        case ModeState::TitleMode:
            modeState = titleScene.Update(input);
            titleScene.Draw(screenWidth, screenHeight);

            // ★ タイトルからゲーム開始へ切り替わった瞬間の処理
            if (modeState == ModeState::GameMode)
            {
                // カーソルを中央に戻し、その「移動した」という入力情報を即座に空読みして捨てる（カメラ暴発防止）
                SetMousePoint(screenWidth / 2, screenHeight / 2);
                input.ValidateInputUpdate();

                // 次のフレームで巨大なdeltaTimeが発生して床抜けするのを防ぐため、現在時刻を再取得
                prevTime = GetNowHiPerformanceCount();
            }
            break;

        case ModeState::GameMode:
            if (gameScene.GetIsGameOver())
            {
                modeState = ModeState::GameOverMode;
                gameOverScene.Init();
            }
            else
            {
                gameScene.UpdateGame(input, screenWidth, screenHeight);
                EffectManager::Instance().Update();
                gameScene.DrawGame();
            }
            break;

        case ModeState::GameOverMode:
        {
            ModeState nextMode = gameOverScene.Update(input);
            gameOverScene.Draw(screenWidth, screenHeight);

            // リトライを選択した場合
            if (nextMode == ModeState::GameMode)
            {
                // リトライ時は再初期化が必要
                gameScene.InitGame(selectMode, screenWidth, screenHeight);
                modeState = ModeState::GameMode;

                // 初期化ラグによるカメラ暴発と床抜けを防ぐリセット処理
                SetMousePoint(screenWidth / 2, screenHeight / 2);
                input.ValidateInputUpdate();
                prevTime = GetNowHiPerformanceCount();
            }
            else if (nextMode == ModeState::TitleMode)
            {
                titleScene.Init();
                modeState = ModeState::TitleMode;
            }
            else if (nextMode == ModeState::ExitMode)
            {
                modeState = ModeState::ExitMode;
            }
            break;
        }
        }

        DrawFormatString(10, 10, GetColor(255, 255, 255), "FPS: %.2f", GetFPS());
        ScreenFlip();
    }

    // 5. 終了処理
    EffectManager::Instance().Finalize();
    Effkseer_End();
    DxLib_End();
    return 0;
}