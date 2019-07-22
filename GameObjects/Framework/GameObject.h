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

// �Q�[���I�u�W�F�N�g
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
	// ����
	virtual void Initialize(GameContext& context)
	{
	}

	// �X�V
	virtual void Update(GameContext& context)
	{
	}

	// �`��
	virtual void Render(GameContext& context)
	{
	}

	// �j��
	virtual void Finalize(GameContext& context)
	{
	}
};

