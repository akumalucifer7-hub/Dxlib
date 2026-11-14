#include "TitleScene.h"
#include "InputValidation.h"
#include "DxLib.h"

void TitleScene::Init()
{
    selectedIndex = 0; // タイトル画面に来るたびに「Start Game」にカーソルを合わせる
}

ModeState TitleScene::Update(const InputValidation& input)
{
    static bool prevUp = false;
    static bool prevDown = false;
    static bool prevEnter = false;

    bool currUp = (CheckHitKey(KEY_INPUT_UP) == 1 || CheckHitKey(KEY_INPUT_W) == 1);
    bool currDown = (CheckHitKey(KEY_INPUT_DOWN) == 1 || CheckHitKey(KEY_INPUT_S) == 1);
    bool currEnter = (CheckHitKey(KEY_INPUT_RETURN) == 1);

    // 上下移動
    if (currUp && !prevUp) { selectedIndex = 0; }
    if (currDown && !prevDown) { selectedIndex = 1; }

    bool enterTrigger = (currEnter && !prevEnter);

    prevUp = currUp;
    prevDown = currDown;
    prevEnter = currEnter;

    // 決定処理
    if (input.IsMouseTrigger(InputValidation::Mouse::Left) || enterTrigger)
    {
        if (selectedIndex == 0) return ModeState::GameMode; // ゲーム開始
        else return ModeState::ExitMode;                    // ゲーム終了
    }

    return ModeState::TitleMode;
}

void TitleScene::Draw(int screenWidth, int screenHeight) const
{
    DrawString(screenWidth / 2 - 50, screenHeight / 2 - 50, "Title", GetColor(255, 0, 0));

    // 選択中の項目を黄色、それ以外を白色で描画
    unsigned int colorStart = (selectedIndex == 0) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
    unsigned int colorExit = (selectedIndex == 1) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

    DrawString(screenWidth / 2 - 30, screenHeight / 2 + 10, "Start Game", colorStart);
    DrawString(screenWidth / 2 - 30, screenHeight / 2 + 40, "Exit", colorExit);

    // カーソルの描画
    int cursorY = (selectedIndex == 0) ? (screenHeight / 2 + 10) : (screenHeight / 2 + 40);
    DrawString(screenWidth / 2 - 60, cursorY, "->", GetColor(255, 255, 0));
}