#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
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

class FrameSLP
{
public:
	int width;
	int height;
	int32_t hotspot_x;
	int32_t hotspot_y;
	uint8_t* picture = nullptr;
	SDL_Surface* surface = nullptr;

	~FrameSLP();
	void free();

	FrameSLP(char* buff, int pos);
};

class FileSLP
{
public:
	std::vector<FrameSLP*> frame;

	FileSLP() {}

	void load(char* buff);

	void setPalette(Palette* pal);
};

class FileBINA
{
public:
	void* buff;
	uint32_t size;

};

class FileWAV
{
public:
	void* buff;
	uint32_t size;
};

class Texture
{
public:
	int id_slp = -1;
	FileSLP* slp = nullptr;
	std::string caption;

	Texture()
	{}

	Texture(const char* c, int id):
		caption(c),
		id_slp(id)
	{}

	void load();

};
 
class TextureTerrain: public Texture
{
public:

	int num_dim = 1;
	uint8_t minimap_color = 0;

	TextureTerrain(const char* c, int id, int dims, uint8_t color):
		Texture(c,id),
		num_dim(dims),
		minimap_color(color)
	{}

	void draw(SDL_Renderer* ren, int x, int y, int iFrame = 0)
	{
		if (id_slp == -1)
			return;
		load();
		SDL_Rect renderQuad;
		FrameSLP* frame = slp->frame[iFrame];
		SDL_Texture* mTexture = SDL_CreateTextureFromSurface(ren, frame->surface);

		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		renderQuad = { x - frame->hotspot_x, y - frame->hotspot_y, frame->width, frame->height };
		SDL_RenderCopyEx(ren, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_NONE);
		SDL_DestroyTexture(mTexture);
	}
};

class TextureObject : public Texture
{
public:

	int frames_per_angles = 1;
	int directions = 1;
	double anim_duration;

	TextureObject(const char* c, int id, int fpa=1, int dirs=0, double dur=0.0):
		Texture(c,id),
		frames_per_angles(fpa),
		directions(dirs),
		anim_duration(dur)
	{}
	
	void draw(SDL_Renderer* ren, int x, int y, int dir = 0, double now = 0.0)
	{
		if (id_slp == -1)
			return;
		load();
		bool flip = false;
		if (dir < 0)
		{
			dir = -dir;
			flip = true;
		}
		int iFrame = dir;
		if (anim_duration > 0.0)
		{
			iFrame = (dir + now / anim_duration) * frames_per_angles;
		}
		SDL_Rect renderQuad;
		FrameSLP* frame = slp->frame[iFrame];
		SDL_Texture* mTexture = SDL_CreateTextureFromSurface(ren, frame->surface);

		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		if (flip)
		{
			renderQuad = { x - (frame->width - frame->hotspot_x), y - frame->hotspot_y, frame->width, frame->height };
			SDL_RenderCopyEx(ren, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_HORIZONTAL);
		}
		else
		{
			renderQuad = { x - frame->hotspot_x, y - frame->hotspot_y, frame->width, frame->height };
			SDL_RenderCopyEx(ren, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_NONE);
		}
		SDL_DestroyTexture(mTexture);
	}
	
};

class TextureInterface: public Texture
{
public:

	TextureInterface()
	{
		id_slp = -1;
	}

	void draw(SDL_Renderer* ren, int x, int y, int iFrame = 0)
	{
		if (id_slp == -1)
			return;
		load();
		SDL_Rect renderQuad;
		FrameSLP* frame = slp->frame[iFrame];
		SDL_Texture* mTexture = SDL_CreateTextureFromSurface(ren, frame->surface);

		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		renderQuad = { x - frame->hotspot_x, y - frame->hotspot_y, frame->width, frame->height };
		SDL_RenderCopyEx(ren, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_NONE);
		SDL_DestroyTexture(mTexture);
	}

};
