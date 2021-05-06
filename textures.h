#pragma once
#include <iostream>
#include <vector>
#include <fstream>

// class for storing one picture of slp animation
class FrameSLP
{
public:
	int width;
	int height;
	int32_t hotspot_x; // center x
	int32_t hotspot_y; // center y
	uint8_t* picture = nullptr;

	~FrameSLP();
	void free();

	FrameSLP(char* buff, int pos);
};

// class for storing slp animation (array of frames)
class FileSLP
{ 
public:
	std::vector<FrameSLP*> frame;

	FileSLP() {}

	void load(char* buff);
};

// classes for woork with sound and scripts (not use now)
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

// common class for work with slp
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
 
// class of terrain textures as grass ice water sand
class TextureTerrain: public Texture
{
public:

	int num_dim = 1;
	uint8_t minimap_color = 0;

	TextureTerrain(const char* c, int id, int dims, uint8_t color=0):
		Texture(c,id),
		num_dim(dims),
		minimap_color(color)
	{}

	/*
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

	void draw2(SDL_Renderer* ren, Palette* pal, int x, int y, int iFrame = 0) // here we create and delete surface every times
	{
		if (id_slp == -1)
			return;
		load();
		SDL_Rect renderQuad;
		FrameSLP* frame = slp->frame[iFrame];

		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)(slp->frame[iFrame]->picture), slp->frame[iFrame]->width, slp->frame[iFrame]->height,
			8, slp->frame[iFrame]->width, SDL_PIXELFORMAT_INDEX8);
		SDL_SetColorKey(surface, SDL_TRUE, 255);

		for (int j = 0; j < 256; j++)
		{
			surface->format->palette->colors[j] = pal->color[j];
		}

		SDL_Texture* mTexture = SDL_CreateTextureFromSurface(ren, surface);

		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		renderQuad = { x - frame->hotspot_x, y - frame->hotspot_y, frame->width, frame->height };
		SDL_RenderCopyEx(ren, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_NONE);
		SDL_DestroyTexture(mTexture);
		SDL_FreeSurface(surface);
	}

	void draw(SDL_Renderer* ren, Palette* pal, int x, int y, int i, int j)
	{
		int iFr = i % num_dim;
		int jFr = j % num_dim;
		int iFrame = iFr * num_dim + (num_dim - 1) - jFr; // begin - right corner
		draw2(ren, pal, x, y, iFrame);
	}
	*/
};

// class of object textures as units trees, houses
class TextureObject : public Texture
{
public:

	int frames_per_angles = 1;
	int directions = 1;
	float anim_duration;

	TextureObject(const char* c, int id, int fpa=1, int dirs=0, float dur=0.0):
		Texture(c,id),
		frames_per_angles(fpa),
		directions(dirs),
		anim_duration(dur)
	{}

	/*
	
	void draw(SDL_Renderer* ren, int x, int y, int dir = 0, float now = 0.0)
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
	*/
};

// class of interfaces textures as interface, icons
class TextureInterface: public Texture
{
public:

	TextureInterface()
	{
		id_slp = -1;
	}

	/*

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

	*/
};
