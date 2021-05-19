#include "textures.h"
#include "world.h"

#include "resourceManager.h"


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






