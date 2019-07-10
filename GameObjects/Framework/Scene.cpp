#include "pch.h"
#include "Scene.h"

void Scene::Add(const std::shared_ptr<GameObject>& obj)
{
	addingObjects.push_back(obj);
}

void Scene::Initialize(GameContext& context)
{
	for (auto& object : gameObjects)
		object->Initialize(context);
}

void Scene::Update(GameContext& context)
{
	for (auto itr = addingObjects.begin(); itr != addingObjects.end(); itr = addingObjects.erase(itr))
	{
		(*itr)->Initialize(context);
		gameObjects.push_back(std::move((*itr)));
	}

	for (auto& object : gameObjects)
		object->Update(context);

	for (auto itr = gameObjects.begin(); itr != gameObjects.end();)
	{
		if ((*itr)->destroyed)
			itr = gameObjects.erase(itr);
		else
			++itr;
	}
}

void Scene::Render(GameContext& context)
{
	for (auto& object : gameObjects)
		object->Render(context);
}

void Scene::Finalize(GameContext& context)
{
	for (auto& object : gameObjects)
		object->Finalize(context);
}

