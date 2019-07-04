#pragma once
#include "Transform.h"
#include <Utilities/TypeId.h>

class GameContext;
class Component;

// �Q�[���I�u�W�F�N�g
class GameObject
{
public:
	std::shared_ptr<Transform> transform;

public:
	GameObject()
	{
		transform = std::make_shared<Transform>();
	}
	~GameObject() = default;

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

