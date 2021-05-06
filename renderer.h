#include "textures.h"
#include "world.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "glut.h"
#include "resourceManager.h"
#include <sstream>

// struct for storing renderer option (in future)
struct RendererOptions
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

// abstract class of renderer for render picture
class Renderer
{
public:

	virtual bool init(ResourceManager* rm) = 0;
	virtual void close() = 0;
	virtual void drawAll(World* world) = 0;
	virtual ~Renderer() {}
	float pos_view_x, pos_view_y; // screen position

protected:

	RendererOptions options;
	ResourceManager* rm;
	// may be all this params should move to resource manager
	const int TILE_SIZEX = 96;
	const int TILE_SIZEY = 48;
	const int HALF_TILE_SIZEX = TILE_SIZEX / 2;
	const int HALF_TILE_SIZEY = TILE_SIZEY / 2;
	int wScreen = 800;
	int hScreen = 600;
	int dColor = 32;
};

//// TODO: i must try directX or opengl!!!!!!!
// realization of sdl renderer (VERY SLOW in this realization)
class RendererSDL: public Renderer
{
public:

	bool init(ResourceManager* rm_) override;
	void close() override;
	void drawAll(World* world) override;
	~RendererSDL();

protected:

	SDL_Window* gWindow = nullptr;
	SDL_Surface* gScreenSurface = nullptr;
	SDL_Renderer* gRenderer = nullptr;
	TTF_Font* gFont = nullptr;


	void drawInterface(); // draw main screen
	void drawTerrains(World* world); // draw terrains of world
	void drawObjects(World* world); // draw objects
	void drawGUI(); // draw gui (not work now)
	void drawDebugInfo(); // in future there will be debug info 

	void drawTexture(Texture* tex, int x, int y, int iFrame = 0, bool flip = false); // common method for all tex
	void drawTextureTerrain(int iTex, int x, int y, int i, int j); // draw one terrain tile
	void drawTextureInterface(int iTex, int x, int y, int iFrame = 0); // draw one picture (icon or interface)
	void drawTextureObject(int iTex, int x, int y, int iFrame = 0); // draw one object
	void drawText(std::string text, int x, int y, int color = 255); // draw text

};


