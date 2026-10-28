#pragma once
#pragma once
#include <vector>
#include "GameObject.h"
#include "Player.h"
#include "Villain.h"
#include "Field.h"
#include "Camera.h"
#include"InputValidation.h"
#include"GameContext.h"
#include "Animator.h"
#include "UserInterface.h"
class GameScene
{
public:
	void InitGame(int selectMode, int SCREENWIDTH, int SCREENHEIGHT);

	void UpdateGame(InputValidation& input, int SCREENWIDTH, int SCREENHEIGHT);
	void DrawGame();
	bool GetIsGameOver() const { return isGameOver; }

private:
	std::vector<GameObject*> gameObjects;
	GameContext CreateContext();
	GameContext context;
	int StartTime;
	bool SetHighMode = false;
	Camera camera;
	Player player;
	Field field;
	Villain villain;
	GameUI gameUI;
	InputValidation inputValidation;
	Animator animator;
	int ShadowMapHandle;
	VECTOR lightDir;
	bool isDebugDrawMode;
	bool isGameOver = false;
};