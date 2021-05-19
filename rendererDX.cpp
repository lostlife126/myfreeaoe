#include "RendererDX.h"
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
	StretchBlt(hdc2, 0, 0, bit.bmWidth, bit.bmHeight, hdc, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);
	surf->ReleaseDC(hdc2);

	DeleteDC(hdc);
	DeleteObject(h);
	backSurf->BltFast(0, 0, surf, 0, DDBLTFAST_WAIT);
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
	OffsetRect(&clientRect, p.x, p.y);
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