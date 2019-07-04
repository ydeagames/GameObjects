#include "pch.h"
#include "BuildSettings.h"
#include <Framework/SceneManager.h>
#include "BaseScene.h"

const std::wstring BuildSettings::GAME_TITLE = L"GameObjects";
const std::wstring BuildSettings::GAME_TITLE_CLASS = L"GameObjectsWindowClass";

void BuildSettings::Build(GameContext& context)
{
	context.GetSceneManager().Register<BaseScene>("BaseScene");

	context.GetSceneManager().LoadScene("BaseScene");
}
