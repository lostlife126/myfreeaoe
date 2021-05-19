#pragma once

#include <iostream>
#include <chrono>
#include <omp.h>
#include <vector>
#include <list>
#include <sstream>
#include <Windows.h>
#include "world.h"
#include "rendererSDL.h"

// TODO:
// ���������
// �� ������ ������ ������� �� �����
// ���������� �������� �����
// ������� �������� �� 5 ��� ??? � �������
// ���������� ������ ��������

// ����� ��� ������������ �������
class Timer
{
public:

};

// class of game engine
class Engine
{
public:

	bool isRun;
	World* world; // world where all proceed
	int nCores;  // cores pf processor (not used)
	float max_fps = 25.0; // limit of fps (not used because my fps very smaaaall)

	Renderer* renderer; // renderer
	ResourceManager* resourceManager; 

	Engine();

	~Engine();

	void start();

	// main cycle
	void run();
};