#pragma once
#include <iostream>
#include <vector>
#include <fstream>

// class for storing color in format rgba
struct Color
{
	union
	{
		uint32_t c;
		struct
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t a;
		};
	};

	Color() :c(0) {}
	Color(uint8_t  r_, uint8_t  g_, uint8_t  b_, uint8_t  a_ = 255) : r(r_), g(g_), b(b_), a(a_) {}
	Color(uint32_t c_) : c(c_) {}
};

class Palette
{
public:

	std::vector<Color> color;

	Palette()
	{}

	void setAge2() // load from file (bad method... need to refact)
	{
		std::fstream file("pal7.txt", std::ios::in);// 7 for terrain
		color.reserve(256);
		Color c;
		for (int i = 0; i < 256; i++)
		{
			int temp;
			file >> temp;
			c.r = temp;
			file >> temp;
			c.g = temp;
			file >> temp;
			c.b = temp;
			c.a = 255;
			color.push_back(c);
		}
		file.close();
	}

};

// class for storing one picture of slp animation
class FrameSLP
{
public:
	int32_t width;
	int32_t height;
	int32_t hotspot_x; // center x
	int32_t hotspot_y; // center y
	uint8_t* pic_low = nullptr;
	Color* picture = nullptr;

	~FrameSLP();
	void free();

	FrameSLP(char* buff, int pos, Palette* p_);
};

// class for storing slp animation (array of frames)
class FileSLP
{ 
public:
	Palette* palette = nullptr;
	std::vector<FrameSLP*> frame;


	FileSLP() {}

	void load(char* buff, Palette* p);
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
	int32_t id_slp = -1;
	FileSLP* slp = nullptr;
	Palette* palette = nullptr;
	std::string caption;

	Texture()
	{}

	Texture(const char* c, int id, Palette* p_):
		caption(c),
		id_slp(id),
		palette(p_)
	{}

	void load();

};
 
// class of terrain textures as grass ice water sand
class TextureTerrain: public Texture
{
public:

	int32_t num_dim = 1;
	uint8_t minimap_color = 0;

	TextureTerrain(const char* c, int id, int dims, Palette* p, uint8_t color=0):
		Texture(c,id, p),
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

	int32_t frames_per_angles = 1;
	int32_t directions = 1;
	float anim_duration;

	TextureObject(const char* c, int id, Palette* p, int fpa=1, int dirs=0, float dur=0.0):
		Texture(c, id, p),
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
