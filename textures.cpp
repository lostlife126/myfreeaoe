
#include "textures.h"

FrameSLP::~FrameSLP()
{
	free();
}

void FrameSLP::free()
{
	if (picture != nullptr)
	{
		delete[] picture;
		SDL_FreeSurface(surface);
		picture = nullptr;
		surface = nullptr;
	}
}

FrameSLP::FrameSLP(char* buff, int pos)
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
	} headerSLP;

	memcpy(&headerSLP, &(buff[pos]), 32);
	width = headerSLP.width;
	height = headerSLP.height;
	hotspot_x = headerSLP.hotspot_x;
	hotspot_y = headerSLP.hotspot_y;
	picture = new uint8_t[width * height];
	std::fill_n(picture, width * height, 255); // transparent color
	pos = headerSLP.outline_table_offset;
	uint32_t* edges = new uint32_t[height];
	memcpy(edges, &(buff[pos]), 4 * height);
	pos += 4 * height;
	uint32_t* cmdTable = new uint32_t[height];
	memcpy(cmdTable, &(buff[pos]), 4 * height);
	pos += 4 * height;

	for (uint32_t i = 0; i < height; i++)
	{
		if ((edges[i] & 0x0000ffff) + ((edges[i] & 0xffff0000) >> 16) == width)
			continue;

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
					for (uint32_t j = 0; j < pix_cnt; j++)
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



void FileSLP::load(char* buff)
{
	struct
	{
		char version[4];
		uint32_t nFrames;
		char comment[24];
	} header;
	memcpy(&header, buff, 32);
	frame.resize(header.nFrames);
	for (uint32_t i = 0; i < frame.size(); i++)
	{
		frame[i] = new FrameSLP(buff, (i + 1) * 32);
	}
}

void FileSLP::setPalette(Palette* pal)
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

void Texture::load()
{
	if ((slp != nullptr) || (id_slp == -1))
		return;

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

