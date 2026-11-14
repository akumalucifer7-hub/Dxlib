#include "GameOverScene.h"
#include "InputValidation.h"
#include "DxLib.h"

void GameOverScene::Init()
{
    selectedIndex = 0; // 画面遷移時にカーソル位置をリセット
}

ModeState GameOverScene::Update(const InputValidation& input)
{
    static bool prevUp = false;
    static bool prevDown = false;
    static bool prevEnter = false; // ★ 追加：エンターキーの前フレーム状態

    bool currUp = (CheckHitKey(KEY_INPUT_UP) == 1 || CheckHitKey(KEY_INPUT_W) == 1);
    bool currDown = (CheckHitKey(KEY_INPUT_DOWN) == 1 || CheckHitKey(KEY_INPUT_S) == 1);
    bool currEnter = (CheckHitKey(KEY_INPUT_RETURN) == 1); // ★ 追加

    if (currUp && !prevUp) { selectedIndex = 0; } // 上へ
    if (currDown && !prevDown) { selectedIndex = 1; } // 下へ

    bool enterTrigger = (currEnter && !prevEnter); // ★ 追加：エンターキーを押した瞬間

    prevUp = currUp;
    prevDown = currDown;
    prevEnter = currEnter; // ★ 追加

    // 左クリック または エンターキーを押した瞬間に決定
    if (input.IsMouseTrigger(InputValidation::Mouse::Left) || enterTrigger)
    {
        if (selectedIndex == 0) return ModeState::GameMode; // もう一度遊ぶ
        else return ModeState::TitleMode;                   // タイトルに戻る
    }

    return ModeState::GameOverMode;
}

void GameOverScene::Draw(int screenWidth, int screenHeight) const
{
    // ... (描画処理は変更なし) ...
    DrawString(screenWidth / 2 - 50, screenHeight / 2 - 50, "Game Over", GetColor(255, 0, 0));

    unsigned int colorRetry = (selectedIndex == 0) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
    unsigned int colorTitle = (selectedIndex == 1) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

    DrawString(screenWidth / 2 - 30, screenHeight / 2 + 10, "Retry", colorRetry);
    DrawString(screenWidth / 2 - 30, screenHeight / 2 + 40, "Title", colorTitle);

    int cursorY = (selectedIndex == 0) ? (screenHeight / 2 + 10) : (screenHeight / 2 + 40);
    DrawString(screenWidth / 2 - 60, cursorY, "->", GetColor(255, 255, 0));
}