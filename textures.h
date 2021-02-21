#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "glut.h"
#include  <unordered_map>


class Palette
{
public:

	std::vector<SDL_Color> color;

	Palette()
	{}

	Palette(int n)
	{
		color.resize(n);
	}

	void setAge2()
	{
		std::fstream file("pal7.txt", std::ios::in);// 7 for terrain
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
			color.push_back(c);
		}
		file.close();
	}

};

class FrameSLP
{
public:
	int32_t width;
	int32_t height;
	int32_t hotspot_x;
	int32_t hotspot_y;
	uint8_t* picture = nullptr;
	SDL_Surface* surface = nullptr;

	~FrameSLP()
	{
		free();
	}
	void free()
	{
		if (picture != nullptr)
		{
			delete[] picture;
			picture = nullptr;
			SDL_FreeSurface(surface);
			surface = nullptr;
		}
	}

	FrameSLP(char* buff, int pos)
	{
		struct
		{
			uint32_t cmd_table_offset;
			uint32_t outline_table_offset;
			uint32_t palette_offset;
			uint32_t properties;
			int32_t width;
			int32_t height;
			int32_t hotspot_x;
			int32_t hotspot_y;
		}header;
		memcpy(&header, &(buff[pos]), 32);
		width = header.width;
		height = header.height;
		hotspot_x = header.hotspot_x;
		hotspot_y = header.hotspot_y;
		picture = new uint8_t[width * height];
		std::fill_n(picture, width * height, 255); // transparent color
		pos = header.outline_table_offset;
		uint32_t* edges = new uint32_t [height];
		memcpy(edges, &(buff[pos]), 4 * height);
		pos += 4 * height;
		uint32_t* cmdTable = new uint32_t[height];
		memcpy(cmdTable, &(buff[pos]), 4 * height);
		pos += 4 * height;

		for (uint32_t i = 0; i < height; i++)
		{
			if ((edges[i] & 0x0000ffff) + ((edges[i] & 0xffff0000) >> 16) == width)
			{
				continue;
			}
			if (pos != cmdTable[i])
			{
				std::cout << "BAD " << i << " row..." << std::endl;
				pos = cmdTable[i];
			}
			uint8_t data = 0;
			uint32_t pix_pos = (edges[i] & 0x0000ffff);
			uint8_t addOff;
			uint8_t dataAdd;
			// TODO: make right color of players
			while (data != 0x0F)
			{
				memcpy(&data, &(buff[pos]), 1);
				pos++;

				if (data == 0x0F)
					break;

				uint8_t cmd = data & 0xF;
				uint32_t pix_cnt = 0;
				uint8_t color_index = 255;
				uint8_t tmp = 0;

				switch (cmd)
				{
				case 0:// leser draw
				case 4:
				case 8:
				case 0xC:
					pix_cnt = data >> 2;
					memcpy(&(picture[i * width + pix_pos]), &(buff[pos]), pix_cnt);
					pos += pix_cnt;
					pix_pos += pix_cnt;
					break;

				case 1: // leser skip
				case 5:
				case 9:
				case 0xD:
					pix_cnt = data >> 2;
					pix_pos += pix_cnt;
					break;

				case 2: // Greater Draw
					memcpy(&addOff, &(buff[pos]), 1);
					pos++;
					pix_cnt = ((data & 0xf0) << 4) + addOff;
					memcpy(&(picture[i * width + pix_pos]), &(buff[pos]), pix_cnt);
					pos += pix_cnt;
					pix_pos += pix_cnt;
					break;

				case 3: // Greater Skip
					memcpy(&addOff, &(buff[pos]), 1);
					pos++;
					pix_cnt = ((data & 0xF0) << 4) + addOff;
					pix_pos += pix_cnt;
					break;

				case 6:// Player color draw
					dataAdd = data >> 4;
					if (dataAdd == 0)
					{
						uint8_t tmp;
						memcpy(&tmp, &(buff[pos]), 1);
						pos++;
						pix_cnt = tmp;
					}
					else
						pix_cnt = dataAdd;
					for (int j = 0; j < pix_cnt; j++)
					{
						uint8_t ff = 255;
						memcpy(&ff, &(buff[pos]), 1);
						pos++;
						ff += 16 * 2;
						picture[i * width + pix_pos] = ff;
						pix_pos++;
					}
					break;

				case 7: //Fill
					dataAdd = data >> 4;
					if (dataAdd == 0)
					{
						uint8_t tmp;
						memcpy(&tmp, &(buff[pos]), 1);
						pos++;
						pix_cnt = tmp;
					}
					else
						pix_cnt = dataAdd;
					memcpy(&color_index, &(buff[pos]), 1);
					pos++;
					for (uint32_t j = 0; j < pix_cnt; j++)
					{
						picture[i * width + pix_pos] = color_index;
						pix_pos++;
					}
					break;

				case 0xa: //Fill player color
					dataAdd = (data & 0xF0) >> 4;
					if (dataAdd == 0)
					{
						uint8_t tmp;
						memcpy(&tmp, &(buff[pos]), 1);
						pos++;
						pix_cnt = tmp;
					}
					else
						pix_cnt = dataAdd;
					memcpy(&color_index, &(buff[pos]), 1);
					pos++;
					for (uint32_t j = 0; j < pix_cnt; j++)
					{
						picture[i * width + pix_pos] = color_index;
						pix_pos++;
					}
					break;
				case 0x0b:////Shadow draw
					dataAdd = (data & 0xF0) >> 4;
					if (dataAdd == 0)
					{
						uint8_t tmp;
						memcpy(&tmp, &(buff[pos]), 1);
						pix_cnt = tmp;
						pos++;
					}
					else
						pix_cnt = dataAdd;
					pix_pos += pix_cnt;
					break;

				case 0x0E:
					switch (data)
					{
					case 0x0E:
						i = i;
						break;
					case 0x1E:
						i = i;
						break;
					case 0x2E:
						i = i;
						break;
					case 0x3E:
						i = i;
						break;

					case 0x4E: //Outline pixel TODO player color
						color_index = 16 * 2;
						picture[i * width + pix_pos] = color_index;
						pix_pos += 1;
						break;
					case 0x6E:
						color_index = 16 * 3;
						picture[i * width + pix_pos] = color_index;
						pix_pos += 1;
						break;

					case 0x5E: //Outline run TODO player color
						memcpy(&tmp, &(buff[pos]), 1);
						pos++;
						pix_cnt = tmp;
						color_index = 16 * 2;
						for (uint32_t j = 0; j < pix_cnt; j++)
						{
							picture[i * width + pix_pos] = color_index;
							pix_pos++;
						}
						break;
					case 0x7E:
						memcpy(&tmp, &(buff[pos]), 1);
						pos++;
						pix_cnt = tmp;
						color_index = 16 * 3;
						for (uint32_t j = 0; j < pix_cnt; j++)
						{
							picture[i * width + pix_pos] = color_index;
							pix_pos++;
						}
						break;

					case 0x8E:
						break;
					case 0x9E:
						break;
					case 0xaE:
						break;

					default:
						break;
					}
					break;
				default:
					std::cout << "SlpFrame: Unknown cmd at " << std::hex <<
						pos << ": " << (int)data << std::endl;
					break;
				}
			}
		}
		delete[] edges;
		delete[] cmdTable;
		surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)picture, width, height,
			8, width, SDL_PIXELFORMAT_INDEX8);
		SDL_SetColorKey(surface, SDL_TRUE, 255);
	}
};

class FileSLP
{
public:
	std::vector<FrameSLP*> frame;

	FileSLP()
	{}

	void load(char* buff)
	{
		struct
		{
			char version[4];
			uint32_t nFrames;
			char comment[24];
		} header;
		memcpy(&header, buff, 32);
		frame.resize(header.nFrames);

		for (int i = 0; i < frame.size(); i++)
		{
			frame[i] = new FrameSLP(buff, (i + 1) * 32);
		}
	}

	void setPalette(Palette* pal)
	{
		for (int j = 0; j < 256; j++)
		{
			frame[0]->surface->format->palette->colors[j] = pal->color[j];
		}
		for (int i = 1; i < frame.size(); i++)
		{
			frame[i]->surface->format = frame[0]->surface->format;
		}
	}
};

class FileBINA
{
public:
	void* buff;
	uint32_t size;

	void load(void* buff, uint32_t size)
	{}

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

	void load()
	{
		if (slp != nullptr)
		{
			return;
		}

		if (id_slp == -1)
		{
			return;
		}
		std::string path;
		if (id_slp < 15000)
		{
			path = "graphics.drs";
		}
		else if (id_slp < 50000)
		{
			path = "terrain.drs";
		}
		else
		{
			path = "interfac.drs";
		}

		std::fstream file;
		file.open(path, std::ios::binary | std::ios::in);
		file.seekg(0, std::ios::end);
		file.seekg(0, std::ios::beg);
		struct
		{
			char info[40];
			char version[4];
			char file_type[12];
			int num_tables;
			int offset_first;
		}header_drs;
		struct
		{
			char ext[4];
			int offset;
			int num_files;
		}header_table;
		struct
		{
			int id_file;
			int offset;
			int size;
		}headerFile;
		file.read(reinterpret_cast<char*>(&header_drs), 64);
		for (int i = 0; i < header_drs.num_tables; i++)
		{
			file.read(reinterpret_cast<char*>(&header_table), 12);
			if (header_table.ext[1] == 'p' && header_table.ext[2] == 'l' && header_table.ext[3] == 's')
			{
				file.seekg(header_table.offset, std::ios::beg);
				for (int j = 0; j < header_table.num_files; j++)
				{
					file.read(reinterpret_cast<char*>(&headerFile), 12);
					if (headerFile.id_file == id_slp)
					{
						Palette pal_;
						pal_.setAge2();
						slp = new FileSLP;
						char* buff = new char[headerFile.size];
						file.seekg(headerFile.offset, std::ios::beg);
						file.read(buff, headerFile.size);
						slp->load(buff);
						slp->setPalette(&pal_);
						delete[] buff;
						file.close();
						return;
					}
				}
			}
		}
	}

};
 
class TextureTerrain: public Texture
{
public:

	int num_dim;
	uint8_t minimap_color;

	TextureTerrain(int id, int dims, uint8_t color):
		num_dim(dims),
		minimap_color(color)
	{
		id_slp = id;
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

class TextureObject : public Texture
{
public:

	int frames_per_angles = 1;
	int directions = 1;
	double anim_duration = 0.0;

	TextureObject(int id, int fpa, int dirs, double dur=0.0)
	{
		id_slp = id;
		frames_per_angles = fpa;
		directions = dirs;
		anim_duration = dur;
	}

	TextureObject()
	{
		id_slp = -1;
	}
	
	void draw(SDL_Renderer* ren, int x, int y, int dir, double now)
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

class ResourceManager
{
public:
	//std::unordered_map<int, FileBINA*> bina;
	//std::unordered_map<int, FileWAV*> wav;
	std::vector<TextureTerrain*> textureTerrain;
	std::vector<TextureInterface*> textureInterface;
	std::vector<TextureObject*> textureObject;


	ResourceManager()
	{
		textureInterface.push_back(new TextureInterface);
		textureInterface[0]->id_slp = 51117;


		textureTerrain.push_back(new TextureTerrain(15014, 10, 0));// shallow
		textureTerrain.push_back(new TextureTerrain(15002, 10, 0));// shallow water
		textureTerrain.push_back(new TextureTerrain(15016, 10, 0));// water
		textureTerrain.push_back(new TextureTerrain(15015, 10, 0));// deep water

		textureTerrain.push_back(new TextureTerrain(15000, 10, 0));// dirt
		textureTerrain.push_back(new TextureTerrain(15006, 10, 0));// grass0
		textureTerrain.push_back(new TextureTerrain(15007, 10, 0));// grass1
		textureTerrain.push_back(new TextureTerrain(15001, 10, 0));// grass2
		textureTerrain.push_back(new TextureTerrain(15009, 10, 0));// grass3
		textureTerrain.push_back(new TextureTerrain(15008, 10, 0));// grass4

		textureTerrain.push_back(new TextureTerrain(15018, 10, 0));// road
		textureTerrain.push_back(new TextureTerrain(15019, 10, 0));// broken road
		textureTerrain.push_back(new TextureTerrain(15030, 10, 0));// snowroad
		textureTerrain.push_back(new TextureTerrain(15031, 10, 0));// grassroad

		textureTerrain.push_back(new TextureTerrain(15021, 10, 0));// farm 0
		textureTerrain.push_back(new TextureTerrain(15022, 10, 0));// farm 1
		textureTerrain.push_back(new TextureTerrain(15023, 10, 0));// farm 2
		textureTerrain.push_back(new TextureTerrain(15004, 10, 0));// farm
		textureTerrain.push_back(new TextureTerrain(15005, 10, 0));// dead farm

		textureTerrain.push_back(new TextureTerrain(15017, 10, 0));// beach
		textureTerrain.push_back(new TextureTerrain(15010, 10, 0));// desert

		textureTerrain.push_back(new TextureTerrain(15011, 10, 0));// leaves

		textureTerrain.push_back(new TextureTerrain(15024, 10, 0));// ice
		textureTerrain.push_back(new TextureTerrain(15026, 10, 0));// snow
		textureTerrain.push_back(new TextureTerrain(15027, 10, 0));// snowdirt
		textureTerrain.push_back(new TextureTerrain(15028, 10, 0));// snowgrass
		textureTerrain.push_back(new TextureTerrain(15029, 10, 0));// snowgrass too

		textureObject.push_back(new TextureObject(2293, 1, 13));// bamboo
		textureObject.push_back(new TextureObject(4652, 1, 13));// oak forest tree
		textureObject.push_back(new TextureObject(4654, 1, 13));// pine
		textureObject.push_back(new TextureObject(4653, 1, 13));// palm
		textureObject.push_back(new TextureObject(4731, 1, 13));// snow pine
		textureObject.push_back(new TextureObject(4728, 1, 13));// jungle
		textureObject.push_back(new TextureObject(4475, 1, 13));// cactus

		textureObject.push_back(new TextureObject(8, 10, 8, 2.0));// archer stand
		textureObject.push_back(new TextureObject(12, 10, 8));// archer walk
		textureObject.push_back(new TextureObject(2, 10, 8));// archer attack
		textureObject.push_back(new TextureObject(5, 10, 8));// archer die
		textureObject.push_back(new TextureObject(9, 10, 8));// archer corpse
	}

};