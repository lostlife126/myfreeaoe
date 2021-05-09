#pragma once
#include <vector>



// resource manager stored all textures (and sounds in further and...)
class ResourceManager
{
public:
	Palette palette;

	std::vector<TextureTerrain*> textureTerrain;
	std::vector<TextureInterface*> textureInterface;
	std::vector<TextureObject*> textureObject;

	// method for init textures (in future will be loaded from file. now this piece of shit....)
	void loadTextures() // todo: remove it when make resource manager
	{
		palette.setAge2();
		textureInterface.push_back(new TextureInterface);
		textureInterface[0]->id_slp = 51117;
		textureInterface[0]->palette = &palette;

		
		textureTerrain.push_back(new TextureTerrain("shallow", 15014, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("shallow_water", 15002, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("water", 15016, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("deep_water", 15015, 10, &palette));

		textureTerrain.push_back(new TextureTerrain("dirt", 15000, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("grass1", 15006, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("grass2", 15007, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("grass3", 15001, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("grass4", 15009, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("grass5", 15008, 10, &palette));

		textureTerrain.push_back(new TextureTerrain("road", 15018, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("broken_road", 15019, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("snow_road", 15030, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("moss_road", 15031, 10, &palette));

		textureTerrain.push_back(new TextureTerrain("farm0", 15021, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("farm1", 15022, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("farm2", 15023, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("farm3", 15004, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("farm_dead", 15005, 10, &palette));

		textureTerrain.push_back(new TextureTerrain("beach", 15017, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("desert", 15010, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("leaves", 15011, 10, &palette));

		textureTerrain.push_back(new TextureTerrain("ice", 15024, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("snow", 15026, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("snow_dirt", 15027, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("snow_grass1", 15028, 10, &palette));
		textureTerrain.push_back(new TextureTerrain("snow_grass2", 15029, 10, &palette));

		textureObject.push_back(new TextureObject("bamboo", 2293, &palette, 1, 13));// bamboo
		textureObject.push_back(new TextureObject("oak", 4652, &palette, 1, 13));// oak forest tree
		textureObject.push_back(new TextureObject("pine", 4654, &palette, 1, 13));// pine
		textureObject.push_back(new TextureObject("palm", 4653, &palette, 1, 13));// palm
		textureObject.push_back(new TextureObject("snow_pine", 4731, &palette, 1, 13));// snow pine
		textureObject.push_back(new TextureObject("jungle", 4728, &palette, 1, 13));// jungle
		textureObject.push_back(new TextureObject("cactus", 4475, &palette, 1, 13));// cactus

		textureObject.push_back(new TextureObject("archer_s", 8, &palette, 10, 8, 2.0));// archer stand
		textureObject.push_back(new TextureObject("archer_w", 12, &palette, 10, 8, 0.6975));// archer walk
		textureObject.push_back(new TextureObject("archer_a", 2, &palette, 10, 8, 0.7));// archer attack
		textureObject.push_back(new TextureObject("archer_d", 5, &palette, 10, 8, 1.0));// archer die
		textureObject.push_back(new TextureObject("archer_c", 9, &palette, 10, 8));// archer corpse
		
	//	objectGraph.push_back(new ObjectGraph("archer", 7, 8, 9, 10, 11)); // archer


	}
};