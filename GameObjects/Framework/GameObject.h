#pragma once
#include "Transform.h"
#include <Utilities/TypeId.h>

class GameContext;
class Component;

namespace Layer
{
	enum Enum
	{
		LAYER_NONE,
		LAYER_BALL,
		LAYER_PLAYER,
		NUM_LAYER,
	};
};

// ゲームオブジェクト
class GameObject
{
public:
	std::shared_ptr<Transform> transform;
	Layer::Enum layer = Layer::LAYER_NONE;
	bool destroyed;

public:
	GameObject()
		: destroyed(false)
	{
		transform = std::make_shared<Transform>();
	}
	~GameObject() = default;

public:
	static void Destroy(GameObject& obj)
	{
		obj.destroyed = true;
	}

public:
	// 生成
	virtual void Initialize(GameContext& context)
	{
	}

	// 更新
	virtual void Update(GameContext& context)
	{
	}

	// 描画
	virtual void Render(GameContext& context)
	{
	}

	// 破棄
	virtual void Finalize(GameContext& context)
	{
	}
};

