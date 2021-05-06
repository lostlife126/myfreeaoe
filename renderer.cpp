#include "renderer.h"

///////////////////////////////////////////////////////////////////////////////////////

void RendererSDL::drawTextureInterface(int iTex, int x, int y, int iFrame)
{
	auto& tex = rm->textureInterface[iTex];
	drawTexture(tex, x, y, iFrame);
}

void RendererSDL::drawInterface()
{
	SDL_Rect topLeftViewport;
	topLeftViewport.x = 0;
	topLeftViewport.y = 0;
	topLeftViewport.w = wScreen;
	topLeftViewport.h = hScreen;
	SDL_RenderSetViewport(gRenderer, &topLeftViewport);
	drawTextureInterface(0, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////

void RendererSDL::drawTextureTerrain(int iTex, int x, int y, int i, int j)
{
	auto& tex = rm->textureTerrain[iTex];
	int iFr = i % tex->num_dim;
	int jFr = j % tex->num_dim;
	int iFrame = iFr * tex->num_dim + (tex->num_dim - 1) - jFr; // begin - right corner

	drawTexture(tex, x, y, iFrame);
}

void RendererSDL::drawTerrains(World* world)
{
	int size_world_x = world->w;
	int size_world_y = world->h;
	int wScreen_half = wScreen / 2;
	int pos = 0;
	for (int i = 0; i < size_world_x; i++)
		for (int j = 0; j < size_world_y; j++)
		{
			int posx = wScreen_half + (i - j) * HALF_TILE_SIZEX - pos_view_x - HALF_TILE_SIZEX;
			int posy = (i + j) * HALF_TILE_SIZEY - pos_view_y;
			if ((posx < -TILE_SIZEX) || (posx > wScreen) || (posy < -TILE_SIZEY) || (posy > hScreen))
			{
				pos++;
				continue;
			}
			int rr = world->tile[pos].type;
			drawTextureTerrain(rr, posx, posy, i, j);
			pos++;
		}
	// draw mesh lines
	
	for (int i = 0; i < size_world_x; i++)
	{
		int posx0 = wScreen_half + (i) * HALF_TILE_SIZEX - pos_view_x;
		int posy0 = (i) * HALF_TILE_SIZEY - pos_view_y;
		int posx1 = wScreen_half + (i - size_world_y) * HALF_TILE_SIZEX - pos_view_x;
		int posy1 = (i + size_world_y) * HALF_TILE_SIZEY - pos_view_y;
		SDL_RenderDrawLine(gRenderer, posx0, posy0, posx1, posy1);
	}

	for (int i = 0; i < size_world_y; i++)
	{
		int posx0 = wScreen_half + (-i)* HALF_TILE_SIZEX - pos_view_x;
		int posy0 = (i)* HALF_TILE_SIZEY - pos_view_y;
		int posx1 = wScreen_half + (-i + size_world_x) * HALF_TILE_SIZEX - pos_view_x;
		int posy1 = (i + size_world_x) * HALF_TILE_SIZEY - pos_view_y;
		SDL_RenderDrawLine(gRenderer, posx0, posy0, posx1, posy1);
	}
	

	// draw (0,0) point (just for fun)
	SDL_Rect fillRect = { -pos_view_x + wScreen_half - 2, -pos_view_y - 2, 5, 5 };
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(gRenderer, &fillRect);
}

///////////////////////////////////////////////////////////////////////////////////////

void RendererSDL::drawObjects(World* world)// todo: сделать чтобы рисовался цвет игрока
{
	/*
	int size_tile_x_half = TILE_SIZEx / 2;
	int size_tile_y_half = TILE_SIZEy / 2;
	int wScreen_half = wScreen / 2;
	for (int i = 0; i < world->num_objects; i++)
	{
		auto& obj = world->object[i];
		auto& graph = objectGraph[obj->frameObject];
		auto& tex = textureObject[graph->id_tex[obj->action]];
		if (obj->now > tex->anim_duration) // todo в будущем надо убрать отсюда
			obj->now -= tex->anim_duration;
		int posx = wScreen_half + (obj->posX - obj->posY) * size_tile_x_half - pos_view_x;
		int posy = (obj->posX + obj->posY) * size_tile_y_half - pos_view_y;
		if ((posx < -size_tile_x_half * 2) || (posx > wScreen) || (posy < -size_tile_y_half * 2) || (posy > hScreen))
		{
			continue;
		}
		//tex->draw(gRenderer, posx, posy, obj->dir, obj->now);
	}*/
}

void RendererSDL::drawTextureObject(int iTex, int x, int y, int iFrame)
{
}

void RendererSDL::drawGUI()
{
}

void RendererSDL::drawDebugInfo()
{
}
	

/////////////////////////////////////////////////////////////////


void RendererSDL::drawTexture(Texture* tex, int x, int y, int iFrame, bool flip)
{
	if (tex->id_slp == -1)
		return;
	tex->load();// not beautiful

	FrameSLP* frame = tex->slp->frame[iFrame];

	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom
	(
		(void*)(tex->slp->frame[iFrame]->picture),
		tex->slp->frame[iFrame]->width,
		tex->slp->frame[iFrame]->height,
		8,
		tex->slp->frame[iFrame]->width,
		SDL_PIXELFORMAT_INDEX8
	);
	SDL_SetColorKey(surface, SDL_TRUE, 255);

	for (int j = 0; j < 256; j++)
	{
		surface->format->palette->colors[j] = rm->palette.color[j].toSTL2();
	}

	SDL_Texture* mTexture = SDL_CreateTextureFromSurface(gRenderer, surface);

	SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
	SDL_Rect renderQuad{ 0, y - frame->hotspot_y, frame->width, frame->height };
	if (flip)
	{
		renderQuad.x = x - frame->width - frame->hotspot_x;
		SDL_RenderCopyEx(gRenderer, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_HORIZONTAL);
	}
	else
	{
		renderQuad.x = x - frame->hotspot_x;
		SDL_RenderCopyEx(gRenderer, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_NONE);
	}
	SDL_DestroyTexture(mTexture);
	SDL_FreeSurface(surface);

}


bool RendererSDL::init(ResourceManager* rm_)
{
	//Create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wScreen, hScreen, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		//Create renderer for window
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		if (gRenderer == nullptr)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			return false;
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
				return false;
			}
		}
	}
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	else
	{
		gFont = TTF_OpenFont("Gothic_Titel_Offiziell.ttf", 32);
		if (gFont == nullptr)
		{
			printf("Font not found!\n");
			return false;
		}
	}

	rm = rm_;

	return true;
}

void RendererSDL::close()
{
	TTF_CloseFont(gFont);
	gFont = nullptr;

	//Destroy window    
	SDL_FreeSurface(gScreenSurface);
	gScreenSurface = nullptr;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

RendererSDL::~RendererSDL()
{
	close();
}

void RendererSDL::drawAll(World* world)
{
	// main color is black
	SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x00, 0x0);
	SDL_RenderClear(gRenderer);

	// draw interface

	drawInterface();

	// draw main world
	SDL_Rect topLeftViewport;
	topLeftViewport.x = 0;
	topLeftViewport.y = 25;
	topLeftViewport.w = wScreen;
	topLeftViewport.h = 430;
	SDL_RenderSetViewport(gRenderer, &topLeftViewport);
	drawTerrains(world);
	//drawObjects(world);

	// draw fps
	std::stringstream ss;
	ss.precision(3);
	ss << "FPS: " << 1.0 / world->dt;
	drawText(ss.str(), 1, 1);

	// draw all
	SDL_RenderPresent(gRenderer);
}

void RendererSDL::drawText(std::string text, int x, int y, int color)
{
	SDL_Color textColor = rm->palette.color[color].toSTL2();
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), textColor);
	SDL_Texture* mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
	SDL_RenderCopyEx(gRenderer, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_NONE);
	SDL_DestroyTexture(mTexture);
}