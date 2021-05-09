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
	SDL_Rect fillRect = { -int(pos_view_x) + wScreen_half - 2, -int(pos_view_y) - 2, 5, 5 };
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
		32,
		tex->slp->frame[iFrame]->width*4,
		SDL_PIXELFORMAT_BGRA32
	);
	SDL_SetColorKey(surface, SDL_TRUE, tex->palette->color[255].c);

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
	SDL_Color textColor; MyColor c = rm->palette.color[color].c;
	textColor.r = c.r; textColor.g = c.g; textColor.b = c.b; textColor.a = c.a;
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), textColor);
	SDL_Texture* mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
	SDL_RenderCopyEx(gRenderer, mTexture, NULL, &renderQuad, NULL, NULL, SDL_FLIP_NONE);
	SDL_DestroyTexture(mTexture);
}

/////////////////////////////////////////////////////////////////////////////////////

LRESULT WndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_CREATE:
		return 0;
	}

	return DefWindowProcW(hwnd, nMsg, wParam, lParam);
}

LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return WndProc(hwnd, nMsg, wParam, lParam);
}

bool RendererDX7::init(ResourceManager* rm_)
{
	// window registration

	WNDCLASSEXW wnd;
	wnd.cbSize = sizeof(WNDCLASSEXW);
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.lpfnWndProc = StaticWndProc;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hInstance = 0;
	wnd.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wnd.hIconSm = wnd.hIcon;
	wnd.hCursor = LoadCursor(0, IDC_ARROW);
	wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd.lpszMenuName = NULL;
	wnd.lpszClassName = L"D3D11F";
	wnd.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassExW(&wnd))
	{
		return false;
	}

	//bool bWindowed = true;
	RECT wndRect;
	RECT clientRect;

	hwnd = CreateWindowExW(NULL, L"D3D11F", L"caption", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 800, 600, NULL, NULL, NULL, NULL);

	GetWindowRect(hwnd, &wndRect);
	GetClientRect(hwnd, &clientRect);

	int iWinWid = 800 + (wndRect.right - wndRect.left) - (clientRect.right - clientRect.left);
	int iWinHei = 600 + (wndRect.bottom - wndRect.top) - (clientRect.bottom - clientRect.top);

	MoveWindow(hwnd, wndRect.left, wndRect.top, iWinWid, iWinHei, true);

	HRESULT hr = DirectDrawCreateEx(NULL, (VOID * *)& ppiDD, IID_IDirectDraw7, NULL);

	ppiDD->SetCooperativeLevel(hwnd, DDSCL_NORMAL);
//	ppiDD->SetDisplayMode(800, 600, 8, 0, 0);

	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	hr = ppiDD->CreateSurface(&desc, &primSurf, 0);

	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = 800;
	desc.dwHeight = 600;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;

	hr = ppiDD->CreateSurface(&desc, &backSurf, 0);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	rm = rm_;

	return true;
}

void RendererDX7::close() 
{
	if (backSurf != nullptr) { backSurf->Release(); backSurf = nullptr; }
	if (primSurf != nullptr) { primSurf->Release(); primSurf = nullptr; }
	if (ppiDD != nullptr) { ppiDD->Release(); ppiDD = nullptr; }
}

RendererDX7::~RendererDX7() 
{
	close();
}

void RendererDX7::drawTexture(Texture* tex, int x, int y, int iFrame, bool flip)
{
	IDirectDrawSurface7* surf;
	HANDLE h = LoadImage(0, "main.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	BITMAP bit;
	GetObject(h, sizeof(bit), &bit);

	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = bit.bmWidth;
	desc.dwHeight = bit.bmHeight;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	HRESULT hr = ppiDD->CreateSurface(&desc, &surf, 0);

	HDC hdc;
	hdc = CreateCompatibleDC(0);
	SelectObject(hdc, h);

	HDC hdc2;
	surf->GetDC(&hdc2);
	StretchBlt(hdc2, 0, 0, bit.bmWidth, bit.bmHeight, hdc, 0,0, bit.bmWidth, bit.bmHeight, SRCCOPY);
	surf->ReleaseDC(hdc2);

	DeleteDC(hdc);
	DeleteObject(h);
	backSurf->BltFast(0,0, surf, 0, DDBLTFAST_WAIT);
//	DDBLTFX bltfx;
	//ZeroMemory(&bltfx, sizeof(bltfx));
	//bltfx.dwSize = sizeof(bltfx);
	//bltfx.dwFillColor = 255;

	//primSurf->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx);


}

void RendererDX7::drawAll(World* world)
{
	drawInterface();
	//primSurf->Flip(NULL, DDFLIP_WAIT);
	RECT clientRect;
	POINT p; p.x = 0; p.y = 0;
	ClientToScreen(hwnd, &p);
	GetClientRect(hwnd, &clientRect);
	//GetWindowRect(hwnd, &clientRect);
	OffsetRect(&clientRect,p.x,p.y);
	primSurf->Blt(&clientRect, backSurf, 0, DDBLT_WAIT, 0);
}



void RendererDX7::drawTexture2(Texture* tex, int x, int y, int iFrame, bool flip)
{
	if (tex->id_slp == -1)
		return;
	tex->load();// not beautiful

	FrameSLP* frame = tex->slp->frame[iFrame];

	IDirectDrawSurface7* surf;
	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = frame->width;
	desc.dwHeight = frame->height;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;

	HRESULT hr = ppiDD->CreateSurface(&desc, &surf, 0);

	DDPIXELFORMAT pf;
	ZeroMemory(&pf, sizeof(pf));
	pf.dwSize = sizeof(pf);
	surf->GetPixelFormat(&pf);

	if (surf->Lock(0, &desc, DDLOCK_WAIT, 0) == DD_OK)
	{
		DWORD* pix = (DWORD*)desc.lpSurface;
		int pos = 0;
		for (int j = 0; j < frame->height; j++)
		{
			for (int i = 0; i < frame->width; i++)
			{
				pix[pos] = frame->picture[pos].c;
				pos++;
			}
		}
	//	memcpy(pix, frame->picture, frame->height * frame->width);
		surf->Unlock(0);
	}
	backSurf->BltFast(0, 0, surf, 0, DDBLTFAST_WAIT);

	surf->Release();
}

void RendererDX7::drawInterface()
{
	RECT topLeftViewport;
//	topLeftViewport.x = 0;
//	topLeftViewport.y = 0;
//	topLeftViewport.w = wScreen;
//	topLeftViewport.h = hScreen;
	drawTextureInterface(0, 0, 0);
}

void RendererDX7::drawTextureInterface(int iTex, int x, int y, int iFrame)
{
	auto& tex = rm->textureInterface[iTex];
	drawTexture2(tex, x, y, iFrame);
}