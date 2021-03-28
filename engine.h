#pragma once

#include <iostream>
#include <chrono>
#include <omp.h>
#include <vector>
#include <list>
#include <sstream>
#include "world.h"
#include <Windows.h>

#include "renderer.h"

// TODO:
// миникарта
// не давать камере уезжать от карты
// нормальное создание карты
// причина тормозов до 5 фпс ??? и решение
// смешивание разных областей


class Engine
{
public:

	bool isRun;
	World* world;
	int nCores;
	double max_fps = 25.0;

	Renderer* renderer;

	Engine():
		isRun{ false },
		nCores{ SDL_GetCPUCount() } // число процов
	{
		renderer = new Renderer;
		renderer->init();

		world = new World(32, 32);
		world->setVoid();
		for(int i = 0;i<40;i++)
			for (int j = 0; j < 40; j++)
				world->addObject(0, i*0.5+1.0, j*0.5+1.0);
	}

		~Engine()
	{
			delete renderer;
			delete world;
	}

	void start() 
	{
		isRun = true;
		run();
	}

	void run()
	{
//		auto tp1 = std::chrono::steady_clock::now();
//		auto tp2 = std::chrono::steady_clock::now();
		double dt = 0.0;
		// главный цикл
		SDL_Event e;
		double dt_input = 0.0;
		double dt_update = 0.0;
		double dt_draw = 0.0;
		double dt_render = 0.0;

		while (isRun)
		{
			double t0 = omp_get_wtime();
			// for dt calc
//			tp2 = std::chrono::steady_clock::now();
//			std::chrono::duration<double> elapsedTime = tp2 - tp1;
//			tp1 = tp2;
//			dt = elapsedTime.count();
		//	if (dt < 0.05)
		//	{
		//		dt = 0.05;
		//		Sleep((0.05-dt)*1000.0);
		//	}

			// action handler
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
				//	isRun = false;
					return;
				}
				
				if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
					//	isRun = false;
						return;

					case SDLK_UP:
						renderer->pos_view_y -= 500.0 *dt;
						break;

					case SDLK_DOWN:
						renderer->pos_view_y += 500.0 *dt;
						break;

					case SDLK_RIGHT:
						renderer->pos_view_x += 500.0 *dt;
						break;

					case SDLK_LEFT:
						renderer->pos_view_x -= 500.0 *dt;
						break;

					case SDLK_0:
						world->object[0]->dir = 0;
						break;
						
					case SDLK_SPACE:

						break;
					case SDLK_d:
						world->object[0]->turnLeft();
					break;
					case SDLK_a:
						world->object[0]->turnRight();
						break;
					case SDLK_w:
						world->object[0]->walk(dt);
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
					x = x - renderer->wScreen / 2 + renderer->pos_view_x; -renderer->TILE_SIZEx / 2;
					y = y + renderer->pos_view_y - renderer->TILE_SIZEy / 2;
					double xx = (1.0 * x) / renderer->TILE_SIZEx;
					double yy = (1.0 * y) / renderer->TILE_SIZEy;

					int i = (yy + xx);
					int j = yy - xx;
					int pos = i * world->h + j;
					world->time_now = 0.0;
					if (!((pos < 0) || (pos >= world->num_tiles)))
					{
						world->tile[pos].type = 1;
					}
					
					
					break;
				}

				if (e.type == SDL_MOUSEMOTION)
				{
					int t = 0;
					break;
				}
				break;

			}
			int x, y;
			SDL_GetMouseState(&x, &y);
			if(x < 10)
				renderer->pos_view_x -= 500.0 * dt;
			if (x > renderer->wScreen-10)
				renderer->pos_view_x += 500.0 * dt;
			if (y < 10)
				renderer->pos_view_y -= 500.0 * dt;
			if (y > renderer->hScreen - 10)
				renderer->pos_view_y += 500.0 * dt;

			double t1 = omp_get_wtime();
			

			world->updateAll(dt);
			double t2 = omp_get_wtime();
			renderer->drawAll(world,dt);
			double t3 = omp_get_wtime();

			std::stringstream ss;
		//	ss.precision(3);
			ss << "time: " << world->time_now;
			renderer->drawText(ss.str(), 1, 20);
			ss.precision(3);
			ss.str("");
			ss << "time input: " << dt_input/dt*100.0;
			renderer->drawText(ss.str(), 1, 40);
			ss.str("");
			ss << "time update: " << dt_update / dt * 100.0;
			renderer->drawText(ss.str(), 1, 60);
			ss.str("");
			ss << "time draw: " << dt_draw / dt * 100.0;
			renderer->drawText(ss.str(), 1, 80);
			ss.str("");
			ss << "time render: " << dt_render / dt * 100.0;
			renderer->drawText(ss.str(), 1, 100);

			ss.str("");
			ss << "(" << renderer->pos_view_x<<", "<< renderer->pos_view_y<<")";
			renderer->drawText(ss.str(), 690, 1);

			SDL_RenderPresent(renderer->gRenderer);
			double t4 = omp_get_wtime();
			dt_input = t1 - t0;
			dt_update = t2 - t1;
			dt_draw = t3 - t2;
			dt_render = t4 - t3;
			dt = t4 - t0;
			if (dt < 1.0/max_fps)
			{
				SDL_Delay((1.0 / max_fps - dt)*1000.0);
				dt = 1.0 / max_fps;
			}
		}
	}
};