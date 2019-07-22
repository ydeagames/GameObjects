#include "pch.h"
#include "Scene.h"

void Scene::Add(const std::shared_ptr<GameObject>& obj)
{
	addingObjects.push_back(obj);
}

void Scene::Initialize(GameContext& context)
{
	for (auto& layer : gameObjects)
		for (auto& object : layer)
			object->Initialize(context);
}

void Scene::Update(GameContext& context)
{
	for (auto& object : addingObjects)
	{
		object->Initialize(context);
		gameObjects[object->layer].emplace_back(std::move(object));
	}
	addingObjects.clear();

	for (auto& layer : gameObjects)
		for (auto& object : layer)
			object->Update(context);

	for (auto& layer : gameObjects)
		layer.remove_if(std::mem_fn(&GameObject::destroyed));
}

void Scene::Render(GameContext& context)
{
	for (auto& layer : gameObjects)
		for (auto& object : layer)
			object->Render(context);
}

void Scene::Finalize(GameContext& context)
{
	for (auto& layer : gameObjects)
		for (auto& object : layer)
			object->Finalize(context);
}

