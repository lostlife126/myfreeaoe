#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <sstream>
#include "world.h"
#include <Windows.h>

#include "renderer.h"

// TODO: разобраться с текстами
// миникарта
// не давать камере уезжать от карты
// нормальное создание карты
// причина тормозов до 60 фпс
// смешивание разных областей


class Engine
{
public:

	bool isRun;

	HANDLE hConsole;
	World* world;
	ResourceManager* rm;

	int nCores;
	SDL_Window* window;

	Renderer* renderer;


	Engine() :
		isRun{ false },
		nCores{ SDL_GetCPUCount() } // число процов

	{
		renderer = new Renderer;
		renderer->init();

		rm = new ResourceManager;
		renderer->resource_manager = rm;
		world = new World(32, 32);
	//	world->setNormal();
		world->setVoid();
	//	world->setBase();
		//world->setBaseland(1);
		world->addObject(7, 1.1, 0.9);
	//	world->addObject(1, 1.0, 0.0);
	//	world->addObject(2, 0.75, 0.25);
	}

		~Engine()
	{

	}

	void start() 
	{
		isRun = true;
		run();
	}

	void run()
	{

		int iter = 0;
		auto tp1 = std::chrono::steady_clock::now();
		auto tp2 = std::chrono::steady_clock::now();
		double dt;
		// главный цикл
		SDL_Event e;

		while (isRun)
		{
			tp2 = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			dt = elapsedTime.count();

			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
					isRun = false;
					return;
				}
				if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						isRun = false;
						return;

					case SDLK_UP:
						renderer->pos_view_y -= 500.0 * dt;
						break;

					case SDLK_DOWN:
						renderer->pos_view_y += 500.0 * dt;
						break;

					case SDLK_RIGHT:
						renderer->pos_view_x += 500.0 * dt;
						break;

					case SDLK_LEFT:
						renderer->pos_view_x -= 500.0 * dt;
						break;

					case SDLK_0:
						world->object[0]->dir = 0;
						break;
						
					case SDLK_SPACE:

						break;
					case SDLK_d:
						world->object[0]->turnLeft();
						//world->unit[0]->frameNow = 0;
					break;
					case SDLK_a:
					//	renderer->setScreenResolution(640, 480);
						world->object[0]->turnRight();
						//world->unit[0]->frameNow = 0;
						break;
			        }
					break;
				}

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{

				break;
				}

				if (e.type == SDL_MOUSEBUTTONUP)
				{
					int x, y;
					SDL_GetMouseState(&x, &y);
					x = x - renderer->wScreen / 2 + renderer->pos_view_x; - world->TILE_SIZEx / 2;
					y = y + renderer->pos_view_y - world->TILE_SIZEy / 2;
					double xx = (1.0 * x) / world->TILE_SIZEx;
					double yy = (1.0 * y) / world->TILE_SIZEy;

					int i = (yy + xx);
					int j = yy - xx;
					int pos = i * world->h + j;
					if ((pos < 0) || (pos >= world->num_tiles))
					{
					}
					world->tile[pos].type = 1;
				}

				if (e.type == SDL_MOUSEMOTION)
				{
					int t = 0;
					break;
				}
				break;

			}
	//		if (iter % 8 == 0)
//			    world->unit[0]->frameNow++;

			renderer->draw(world, dt);
			iter++;
		//	std::cout << "fps = " << 1.0 / dt << std::endl;
		}
	}
};