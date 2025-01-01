#pragma once

#include <vector>
#include "Model.h"

class Scene
{
public:
	Scene();
	~Scene();

private:
	std::vector<Model> sceneModels;
};

Scene::Scene()
{
}

Scene::~Scene()
{
}