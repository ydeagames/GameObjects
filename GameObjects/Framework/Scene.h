#pragma once
#include "GameObject.h"

// ゲームオブジェクトコンテナ
class Scene
{
public:
	// 子ゲームオブジェクト
	std::list<std::shared_ptr<GameObject>> gameObjects;
	// 子ゲームオブジェクト
	std::list<std::shared_ptr<GameObject>> addingObjects;

public:
	void Add(const std::shared_ptr<GameObject>& obj);

public:
	// 生成
	void Initialize(GameContext& context);
	// 更新
	void Update(GameContext& context);
	// 描画
	void Render(GameContext& context);
	// 破棄
	void Finalize(GameContext& context);
};