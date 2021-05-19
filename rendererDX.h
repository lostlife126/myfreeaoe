#pragma once
#include "Renderer.h"
#include "ddraw.h"
//#include "dxguid.h"

#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"dxguid.lib")

/*
class Palette
{
public:

	//std::vector<SDL_Color> color;

	Palette()
	{}

	void setAge2()
	{
		std::fstream file("pal7.txt", std::ios::in);// 7 for terrain
	//	color.resize(256);

		for (int i = 0; i < 256; i++)
		{
		//	SDL_Color c;
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
*/

//// TODO: i must try directX or opengl!!!!!!!
class RendererDX7 : public Renderer
{
public:

	bool init(ResourceManager* rm_) override;
	void close() override;
	void drawAll(World* world) override;
	~RendererDX7();

	void drawTexture(Texture* tex, int x, int y, int iFrame = 0, bool flip = false); // common method for all tex
	void drawTexture2(Texture* tex, int x, int y, int iFrame = 0, bool flip = false); // common method for all tex
	void drawTextureInterface(int iTex, int x, int y, int iFrame = 0); // draw one picture (icon or interface)
	void drawInterface(); // draw main screen

protected:
	IDirectDraw7* ppiDD = nullptr;
	IDirectDrawSurface7* primSurf = nullptr;
	IDirectDrawSurface7* backSurf = nullptr;
	HWND hwnd;
};

