#include "textures.h"
#include "world.h"

class Renderer
{
public:

	int wScreen = 800;
	int hScreen = 600;
	int dColor = 32;

	SDL_Window* gWindow = nullptr;
	SDL_Surface* gScreenSurface = nullptr;
	SDL_Renderer* gRenderer = nullptr;

	void setScreenResolution(int w, int h)
	{
		wScreen = w;
		hScreen = h;
		close();
		init();
	}

	bool init()
	{
		bool success = true;

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wScreen, hScreen, SDL_WINDOW_SHOWN);
		if (gWindow == nullptr)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == nullptr)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
		return success;
	}

	void close()
	{

		//Destroy window    
		SDL_FreeSurface(gScreenSurface);
		gScreenSurface = nullptr;

		SDL_DestroyRenderer(gRenderer);
		gRenderer = nullptr;

		SDL_DestroyWindow(gWindow);
		gWindow = nullptr;

		IMG_Quit();
		SDL_Quit();
	}

	~Renderer()
	{
		close();
	}

	void draw(World* world, double dt)
	{
		SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x00, 0x0);
		SDL_RenderClear(gRenderer);
		SDL_Rect topLeftViewport;
		topLeftViewport.x = 0;
		topLeftViewport.y = 0;
		topLeftViewport.w = wScreen;
		topLeftViewport.h = hScreen;
		SDL_RenderSetViewport(gRenderer, &topLeftViewport);
		resource_manager->textureInterface[0]->draw(gRenderer, 0, 0);
		topLeftViewport.x = 0;
		topLeftViewport.y = 25;
		topLeftViewport.w = wScreen;
		topLeftViewport.h = 430;
		SDL_RenderSetViewport(gRenderer, &topLeftViewport);
		drawTerrain(world);
		drawObjects(world, dt);

		SDL_RenderPresent(gRenderer);
	}

	void drawTerrain(World* world)
	{
		int size_tile_x_half = world->TILE_SIZEx / 2;
		int size_tile_y_half = world->TILE_SIZEy / 2;
		int size_tile_x = world->TILE_SIZEx;
		int size_tile_y = world->TILE_SIZEy;
		int size_world_x = world->w;
		int size_world_y = world->h;
		int wScreen_half = wScreen / 2;
		auto& tex = resource_manager->textureTerrain;
		int pos = 0;
		for (int i = 0; i < size_world_x; i++)
			for (int j = 0; j < size_world_y; j++)
			{
				int posx = wScreen_half + (i - j) * size_tile_x_half - pos_view_x - size_tile_x_half;
				int posy = (i + j) * size_tile_y_half - pos_view_y;
				if ((posx < -size_tile_x) || (posx > wScreen) || (posy < -size_tile_y) || (posy > hScreen))
				{
					pos++;
					continue;
				}
				int rr = world->tile[pos].type;
				int iFr = i % 10;
				int jFr = j % 10;
				int iFrame = iFr * 10 + 9 - jFr; // begin - right corner
				tex[rr]->draw(gRenderer, posx, posy, iFrame);
				pos++;
			}
		
		for (int i = 0; i < size_world_x; i++)
		{
			int posx0 = wScreen_half + (i) * size_tile_x_half - pos_view_x;
			int posy0 = (i) * size_tile_y_half - pos_view_y;
			int posx1 = wScreen_half + (i - size_world_y) * size_tile_x_half - pos_view_x;
			int posy1 = (i + size_world_y) * size_tile_y_half - pos_view_y;
			SDL_RenderDrawLine(gRenderer, posx0, posy0, posx1, posy1);
		}

		for (int i = 0; i < size_world_y; i++)
		{
			int posx0 = wScreen_half + (-i)* size_tile_x_half - pos_view_x;
			int posy0 = (i)* size_tile_y_half - pos_view_y;
			int posx1 = wScreen_half + (-i + size_world_x) * size_tile_x_half - pos_view_x;
			int posy1 = (i + size_world_x) * size_tile_y_half - pos_view_y;
			SDL_RenderDrawLine(gRenderer, posx0, posy0, posx1, posy1);
		}
		

		SDL_Rect fillRect = { -pos_view_x+wScreen_half-2, -pos_view_y-2, 5, 5 };
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &fillRect);
	}

	void drawObjects(World* world, double dt)
	{
		int size_tile_x_half = world->TILE_SIZEx / 2;
		int size_tile_y_half = world->TILE_SIZEy / 2;
		int wScreen_half = wScreen / 2;
		for (int i = 0; i < world->num_objects; i++)
		{
			auto& obj = world->object[i];
			obj->update(dt);
			if (obj->now > resource_manager->textureObject[obj->type]->anim_duration)
			{
				obj->now -= resource_manager->textureObject[obj->type]->anim_duration;
			}
			int posx = wScreen_half + (obj->posX - obj->posY) * size_tile_x_half - pos_view_x;
			int posy = (obj->posX + obj->posY) * size_tile_y_half - pos_view_y;
			if ((posx < -size_tile_x_half * 2) || (posx > wScreen) || (posy < -size_tile_y_half * 2) || (posy > hScreen))
			{
				continue;
			}
			resource_manager->textureObject[obj->type]->draw(gRenderer, posx, posy, obj->dir, obj->now);

		}




	}

	ResourceManager* resource_manager;
	int pos_view_x, pos_view_y;

};