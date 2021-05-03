#include "textures.h"
#include "world.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "glut.h"


class Palette
{
public:

	std::vector<SDL_Color> color;

	Palette()
	{}

	void setAge2()
	{
		std::fstream file("pal7.txt", std::ios::in);// 7 for terrain
		color.resize(256);

		for (int i = 0; i < 256; i++)
		{
			SDL_Color c;
			int temp;
			file >> temp;
			c.r = temp;
			file >> temp;
			c.g = temp;
			file >> temp;
			c.b = temp;
			c.a = 255;
			color[i] = c;
		}
		file.close();
	}

};

class Renderer
{
public:
	const int TILE_SIZEX = 96;
	const int TILE_SIZEY = 48;
	const int HALF_TILE_SIZEX = TILE_SIZEX / 2;
	const int HALF_TILE_SIZEY = TILE_SIZEY / 2;

	int wScreen = 800;
	int hScreen = 600;
	int dColor = 32;

	// todo: make class resource manager
	std::vector<TextureTerrain*> textureTerrain;
	std::vector<TextureInterface*> textureInterface;
	std::vector<TextureObject*> textureObject;

	int pos_view_x, pos_view_y;

	Palette palette;

	virtual bool init() = 0;
	virtual void close() = 0;
	virtual void drawAll(World* world, double dt) = 0;

	//virtual ~Renderer() = 0;

	virtual void drawText(std::string text, int x, int y, int color = 255) = 0;

	void loadTextures() // todo: remove it when make resource manager
	{
		palette.setAge2();
		textureInterface.push_back(new TextureInterface);
		textureInterface[0]->id_slp = 51117;

		textureTerrain.push_back(new TextureTerrain("shallow", 15014, 10, 0));
		textureTerrain.push_back(new TextureTerrain("shallow_water", 15002, 10, 0));
		textureTerrain.push_back(new TextureTerrain("water", 15016, 10, 0));
		textureTerrain.push_back(new TextureTerrain("deep_water", 15015, 10, 0));

		textureTerrain.push_back(new TextureTerrain("dirt", 15000, 10, 0));
		textureTerrain.push_back(new TextureTerrain("grass1", 15006, 10, 0));
		textureTerrain.push_back(new TextureTerrain("grass2", 15007, 10, 0));
		textureTerrain.push_back(new TextureTerrain("grass3", 15001, 10, 0));
		textureTerrain.push_back(new TextureTerrain("grass4", 15009, 10, 0));
		textureTerrain.push_back(new TextureTerrain("grass5", 15008, 10, 0));

		textureTerrain.push_back(new TextureTerrain("road", 15018, 10, 0));
		textureTerrain.push_back(new TextureTerrain("broken_road", 15019, 10, 0));
		textureTerrain.push_back(new TextureTerrain("snow_road", 15030, 10, 0));
		textureTerrain.push_back(new TextureTerrain("moss_road", 15031, 10, 0));

		textureTerrain.push_back(new TextureTerrain("farm0", 15021, 10, 0));
		textureTerrain.push_back(new TextureTerrain("farm1", 15022, 10, 0));
		textureTerrain.push_back(new TextureTerrain("farm2", 15023, 10, 0));
		textureTerrain.push_back(new TextureTerrain("farm3", 15004, 10, 0));
		textureTerrain.push_back(new TextureTerrain("farm_dead", 15005, 10, 0));

		textureTerrain.push_back(new TextureTerrain("beach", 15017, 10, 0));
		textureTerrain.push_back(new TextureTerrain("desert", 15010, 10, 0));
		textureTerrain.push_back(new TextureTerrain("leaves", 15011, 10, 0));

		textureTerrain.push_back(new TextureTerrain("ice", 15024, 10, 0));
		textureTerrain.push_back(new TextureTerrain("snow", 15026, 10, 0));
		textureTerrain.push_back(new TextureTerrain("snow_dirt", 15027, 10, 0));
		textureTerrain.push_back(new TextureTerrain("snow_grass1", 15028, 10, 0));
		textureTerrain.push_back(new TextureTerrain("snow_grass2", 15029, 10, 0));

		textureObject.push_back(new TextureObject("bamboo", 2293, 1, 13));// bamboo
		textureObject.push_back(new TextureObject("oak", 4652, 1, 13));// oak forest tree
		textureObject.push_back(new TextureObject("pine", 4654, 1, 13));// pine
		textureObject.push_back(new TextureObject("palm", 4653, 1, 13));// palm
		textureObject.push_back(new TextureObject("snow_pine", 4731, 1, 13));// snow pine
		textureObject.push_back(new TextureObject("jungle", 4728, 1, 13));// jungle
		textureObject.push_back(new TextureObject("cactus", 4475, 1, 13));// cactus

		textureObject.push_back(new TextureObject("archer_s", 8, 10, 8, 2.0));// archer stand
		textureObject.push_back(new TextureObject("archer_w", 12, 10, 8, 0.6975));// archer walk
		textureObject.push_back(new TextureObject("archer_a", 2, 10, 8, 0.7));// archer attack
		textureObject.push_back(new TextureObject("archer_d", 5, 10, 8, 1.0));// archer die
		textureObject.push_back(new TextureObject("archer_c", 9, 10, 8));// archer corpse

	//	objectGraph.push_back(new ObjectGraph("archer", 7, 8, 9, 10, 11)); // archer
	}

};

//// TODO: i must try directX or opengl!!!!!!!
class RendererSDL: public Renderer
{
public:

	SDL_Window* gWindow = nullptr;
	SDL_Surface* gScreenSurface = nullptr;
	SDL_Renderer* gRenderer = nullptr;
	TTF_Font* gFont = nullptr;

	// so ugly and slow method
	void drawTexture(Texture* tex, int x, int y, int iFrame = 0, bool flip = false)
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
			surface->format->palette->colors[j] = palette.color[j];
		}

		SDL_Texture* mTexture = SDL_CreateTextureFromSurface(gRenderer, surface);

		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		SDL_Rect renderQuad{0, y - frame->hotspot_y, frame->width, frame->height};
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

	void drawTextureTer(int iTex, int x, int y, int i, int j)
	{
		auto& tex = textureTerrain[iTex];
		int iFr = i % tex->num_dim;
		int jFr = j % tex->num_dim;
		int iFrame = iFr * tex->num_dim + (tex->num_dim - 1) - jFr; // begin - right corner

		drawTexture(tex,x,y,iFrame);
	}

	void drawTextureInt(int iTex, int x, int y, int iFrame = 0)
	{
		auto& tex = textureInterface[iTex];

		drawTexture(tex, x, y, iFrame);
	}

	bool init() override
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

		loadTextures(); // it will remove from here

		return true;
	}

	void close() override
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

	~RendererSDL()
	{
		close();
	}

	void drawAll(World* world, double dt) override
	{
		// main color is black
		SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x00, 0x0);
		SDL_RenderClear(gRenderer);

		// draw interface
		SDL_Rect topLeftViewport;
		topLeftViewport.x = 0;
		topLeftViewport.y = 0;
		topLeftViewport.w = wScreen;
		topLeftViewport.h = hScreen;
		SDL_RenderSetViewport(gRenderer, &topLeftViewport);
		drawTextureInt(0, 0, 0);

		// draw main world
		topLeftViewport.x = 0;
		topLeftViewport.y = 25;
		topLeftViewport.w = wScreen;
		topLeftViewport.h = 430;
		SDL_RenderSetViewport(gRenderer, &topLeftViewport);
		//drawTerrain(world);
		//drawObjects(world);

		// draw fps
		std::stringstream ss;
		ss.precision(3);
		ss <<"FPS: "<< 1.0/dt;
		drawText(ss.str(),1,1);

		// draw all
		SDL_RenderPresent(gRenderer);
	}

	void drawText(std::string text, int x, int y, int color=255)
	{
		SDL_Color textColor = palette.color[color];
		SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), textColor);
		SDL_Texture* mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
		SDL_RenderCopyEx(gRenderer, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_NONE);
		SDL_DestroyTexture(mTexture);
	}

	void drawTerrain(World* world)
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
				if ((posx < - TILE_SIZEX) || (posx > wScreen) || (posy < -TILE_SIZEY) || (posy > hScreen))
				{
					pos++;
					continue;
				}
				int rr = world->tile[pos].type;
				drawTextureTer(rr, posx, posy, i, j);
				pos++;
			}
		// draw mesh lines
		/*
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
		*/
		
		// draw (0,0) point (just for fun)
		SDL_Rect fillRect = { -pos_view_x+wScreen_half-2, -pos_view_y-2, 5, 5 };
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &fillRect);
	}

	void drawObjects(World* world)// todo: сделать чтобы рисовался цвет игрока
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
		}
		*/
	}

};