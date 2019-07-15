#pragma once
#include "GameObject.h"

// �Q�[���I�u�W�F�N�g�R���e�i
class Scene
{
public:
	// �q�Q�[���I�u�W�F�N�g
	std::list<std::shared_ptr<GameObject>> gameObjects;
	// �q�Q�[���I�u�W�F�N�g
	std::list<std::shared_ptr<GameObject>> addingObjects;

public:
	void Add(const std::shared_ptr<GameObject>& obj);

public:
	// ����
	void Initialize(GameContext& context);
	// �X�V
	void Update(GameContext& context);
	// �`��
	void Render(GameContext& context);
	// �j��
	void Finalize(GameContext& context);
};