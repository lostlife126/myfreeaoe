#include "textures.h"
#include "world.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "glut.h"
#include "resourceManager.h"
#include <sstream>

struct RendererState
{
	bool drawInterface;
	bool drawTerrain;
	bool drawObjects;
	bool drawStaticObjects;
	bool drawDynamicObjects;
	bool drawGUI;
	bool drawFPS;
	bool drawDebug;
	bool drawMesh;
};

class Renderer
{

public:

	virtual bool init(ResourceManager* rm) = 0;
	virtual void close() = 0;
	virtual void drawAll(World* world, double dt) = 0;
	virtual ~Renderer() {}
	int pos_view_x, pos_view_y;

protected:

	ResourceManager* rm;

	const int TILE_SIZEX = 96;
	const int TILE_SIZEY = 48;
	const int HALF_TILE_SIZEX = TILE_SIZEX / 2;
	const int HALF_TILE_SIZEY = TILE_SIZEY / 2;
	RendererState options;

	int wScreen = 800;
	int hScreen = 600;
	int dColor = 32;
};

//// TODO: i must try directX or opengl!!!!!!!
class RendererSDL: public Renderer
{
public:

	bool init(ResourceManager* rm_) override;
	void close() override;
	void drawAll(World* world, double dt) override;
	~RendererSDL();

protected:

	SDL_Window* gWindow = nullptr;
	SDL_Surface* gScreenSurface = nullptr;
	SDL_Renderer* gRenderer = nullptr;
	TTF_Font* gFont = nullptr;

	void drawInterface();
	void drawTerrains(World* world);
	void drawObjects(World* world);

	void drawGUI();
	void drawDebugInfo();

	void drawTexture(Texture* tex, int x, int y, int iFrame = 0, bool flip = false);
	void drawTextureTerrain(int iTex, int x, int y, int i, int j);
	void drawTextureInterface(int iTex, int x, int y, int iFrame = 0);
	void drawTextureObject(int iTex, int x, int y, int iFrame = 0);

	void drawText(std::string text, int x, int y, int color = 255);

};


