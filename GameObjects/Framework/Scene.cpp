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
	for (auto& object : addingObjects)
		object->Initialize(context);
	gameObjects.splice(gameObjects.end(), std::move(addingObjects));

	for (auto& object : gameObjects)
		object->Update(context);

	gameObjects.remove_if(std::mem_fn(&GameObject::destroyed));
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

