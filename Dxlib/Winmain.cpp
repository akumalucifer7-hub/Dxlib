#include "Winmain.h"
#include "GameContext.h"
#include <algorithm>
#include <windows.h>
#include"AudioManager.h"
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

    if (DxLib_Init() == -1) return -1;

    SetDrawScreen(DX_SCREEN_BACK);

    // 3. ゲーム管理変数 & シーン初期化
    GameScene gameScene;
    ModeState modeState = ModeState::GameMode;
    InputValidation& input = InputValidation::Instance();
    GameContext context;

    context.input = &input;
    gameScene.InitGame(selectMode, screenWidth, screenHeight);

    LONGLONG prevTime = GetNowHiPerformanceCount();
    AudioManager::Instance().Load("Run", "Assets/Sounds/Run.mp3");
    AudioManager::Instance().Load("Attack", "Assets/Sounds/Attack.mp3");
    AudioManager::Instance().Load("Hit", "Assets/Sounds/Hit.mp3");
    AudioManager::Instance().Load("Parry", "Assets/Sounds/Parry.mp3");
    // 4. メインゲームループ
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        // 画面クリア
        ClearDrawScreen();
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        // --- デルタタイム計算 (秒単位) ---
        LONGLONG currentTime = GetNowHiPerformanceCount();
        float rawDeltaTime = static_cast<float>(currentTime - prevTime) / 1000000.0f; // マイクロ秒 -> 秒
        prevTime = currentTime;

        // 極端なコマ落ち対策（上限 0.1秒 = 10FPS相当）
        context.deltaTime = (std::min)(rawDeltaTime, 0.1f);

        // --- 入力更新 ---
        input.ValidateInputUpdate();

        // --- シーン・ゲーム状態の更新 & 描画 ---
        switch (modeState)
        {
        case ModeState::TitleMode:
            // TODO: タイトル描画・更新
            break;

        case ModeState::GameMode:
            // ゲームオーバー判定
            if (gameScene.GetIsGameOver())
            {
                modeState = ModeState::GameOverMode;
            }
            else
            {
                // GameContext をそのままゲームシーンへ渡す
                gameScene.UpdateGame(input, screenWidth, screenHeight);
                gameScene.DrawGame();
            }
            break;

        case ModeState::GameOverMode:
            // ゲームオーバー画面表示
            DrawString(screenWidth / 2 - 50, screenHeight / 2, "Game Over", GetColor(255, 0, 0));
            DrawString(screenWidth / 2 - 120, screenHeight / 2 + 30, "Click Left Mouse to Restart", GetColor(255, 255, 255));

            // 左クリックで再スタート
            if (input.IsMouseTrigger(InputValidation::Mouse::Left))
            {
                gameScene.InitGame(selectMode, screenWidth, screenHeight);
                modeState = ModeState::GameMode;
            }
            break;
        }

        // デバッグ用 FPS 表示
        DrawFormatString(10, 10, GetColor(255, 255, 255), "FPS: %.2f", GetFPS());

        // 裏画面の内容を表画面に反映
        ScreenFlip();
    }

    // 5. 終了処理
    DxLib_End();
    return 0;
}