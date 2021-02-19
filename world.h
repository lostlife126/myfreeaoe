#pragma once
#include <vector>

enum TypeTiles
{
	MARSHLAND = 0,
	SHALLOWWATER,
	WATER,
	DEEPWATER,

	DIRT,
	GRASS0,
	GRASS1,
	GRASS2,
	GRASS3,
	GRASS4,

	ROAD,
	BROKENROAD,
	SNOWROAD,
	MOSEYROAD,

	FARM0,
	FARM1,
	FARM2,
	FARM,
	DEADFARM,

	BEACH,
	DESERT,

	LEAVES,

	ICE,
	SNOW,
	SNOWDIRT,
	SNOWGRASS,
	SNOWFOREST

};

static int climat[8][8] =
{
	TypeTiles::SNOW,
	TypeTiles::ICE,
	TypeTiles::ICE,
	TypeTiles::ICE,
	TypeTiles::MARSHLAND,
	TypeTiles::MARSHLAND,
	TypeTiles::MARSHLAND,
	TypeTiles::MARSHLAND,

	TypeTiles::SNOW,
	TypeTiles::SNOW,
	TypeTiles::SNOWGRASS,
	TypeTiles::GRASS2,
	TypeTiles::GRASS3,
	TypeTiles::GRASS4,
	TypeTiles::GRASS4,
	TypeTiles::ROAD,

	TypeTiles::SNOW,
	TypeTiles::SNOWGRASS,
	TypeTiles::GRASS1,
	TypeTiles::GRASS2,
	TypeTiles::GRASS3,
	TypeTiles::GRASS3,
	TypeTiles::ROAD,
	TypeTiles::ROAD,

	TypeTiles::SNOWDIRT,
	TypeTiles::GRASS0,
	TypeTiles::GRASS1,
	TypeTiles::GRASS2,
	TypeTiles::GRASS2,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,

	TypeTiles::DIRT,
	TypeTiles::GRASS0,
	TypeTiles::GRASS1,
	TypeTiles::GRASS1,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,

	TypeTiles::DIRT,
	TypeTiles::GRASS0,
	TypeTiles::GRASS0,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,

	TypeTiles::DIRT,
	TypeTiles::DIRT,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,

	TypeTiles::DESERT,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,
	TypeTiles::ROAD,

};

class Tile
{
public:

	int i; //left
	int j; // right
	int type;
	int id_slp;
	double hei;
	double temper;
	double precip;
	double humid;

	void calcType()
	{
	//	humid = precip / (59.0 * temper);
		double humidr = 1.0 / humid;
		int i = log2(humidr) + 3;
		int j = log2(precip / 62.5);
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		if (i > 7) i = 7;
		if (j > 7) j = 7;
		type = climat[i][j];
		if (hei < 0)
			type = TypeTiles::SHALLOWWATER;
		if (hei < -32.0)
			type = TypeTiles::WATER;
		if (hei < -64.0)
			type = TypeTiles::DEEPWATER;
	}

	Tile(int t_):
		type(t_),
		id_slp(0)
	{}

	Tile() :
		type(0),
		id_slp(0)
	{}
};

class Object
{
public:
	int type;
	double posX;
	double posY;
	double now = 0.0;
	int dir = 0; // 0 - ����, 1 - ����� ����, 2 - �����, 3 - ����� �����, -1 - ������ ���� � ��
	int action = 0; // 0 - stand, 1 - walk, 2 - attack, 3 - die, 4 - decay

	void turnLeft()
	{
		dir++;
		if (dir == 5)
			dir = -3;
	}
	void turnRight()
	{
		dir--;
		if (dir == -4)
			dir = 4;
	}

	void update(double dt)
	{
		now += dt;
	}
};

class Unit : public Object
{
public:
	int slp_action[5];

	Unit(int type_, double x_, double y_)
	{
		type = type_;
		posX = x_;
		posY = y_;
		slp_action[0] = 7;
		slp_action[1] = 8;
		slp_action[2] = 9;
		slp_action[3] = 10;
		slp_action[4] = 11;
	}



};

double linear(double a, double b, double x);

class Perlin2D
{
public:

	int nx;
	int ny;

	double** data;

	Perlin2D(int nx_, int ny_):
		nx(nx_),
		ny(ny_)
	{
		data = new double*[nx];
		for (int i = 0; i < nx; i++)
		{
			data[i] = new double [ny];
			for (int j = 0; j < ny; j++)
			{
				data[i][j] = 0.0;
			}
		}
	}

	void addLin(double amp, double len)
	{
		int nx_ = len + 1.99999;
		int ny_ = len + 1.99999;
		double** data1 = new double* [nx_];
		for (int i = 0; i < nx_; i++)
		{
			data1[i] = new double[ny_];
			for (int j = 0; j < ny_; j++)
			{
				double rr = (rand() % 1024) / 1024.0;
				data1[i][j] = rr;
			}
		}
		for (int i = 0; i < nx; i++)
		{
			for (int j = 0; j < ny; j++)
			{
				int i_tl = (i+0.5) * len / nx;
				int j_tl = (j+0.5) * len / ny;
				double xx = (i + 0.5) / nx - i_tl / len;
				double yy = (j + 0.5) / ny - j_tl / len;
				double xt = linear(data1[i_tl][j_tl], data1[i_tl+1][j_tl], xx);
				double xb = linear(data1[i_tl][j_tl+1], data1[i_tl+1][j_tl + 1], xx);
				data[i][j] = linear(xt, xb, yy)*amp +(1-amp) * data[i][j];
			}
		}
	}

	void addLog(double amp, double k)
	{
		int nx_ = k + 1.99999;
		int ny_ = k + 1.99999;
		double** data1 = new double* [nx_];
		for (int i = 0; i < nx_; i++)
		{
			data1[i] = new double[ny_];
			for (int j = 0; j < ny_; j++)
			{
				data1[i][j] = (rand() / 1024) / 1024.0;
			}
		}
		for (int i = 0; i < nx; i++)
		{
			for (int j = 0; j < ny; j++)
			{
				int i_tl = (i + 0.5) * k / nx;
				int j_tl = (j + 0.5) * k / ny;
				double xx = ((i + 0.5) / nx - i_tl / k) * k;
				double yy = ((j + 0.5) / ny - j_tl / k) * k;
				double xt = linear(log(data1[i_tl][j_tl]), log(data1[i_tl + 1][j_tl]), xx);
				double xb = linear(log(data1[i_tl][j_tl + 1]), log(data1[i_tl + 1][j_tl + 1]), xx);
				data[i][j] = exp(linear(xt, xb, yy) * amp + (1.0 - amp) * data[i][j]);
			}
		}
	}
};

class World
{
public:

	const int TILE_SIZEx = 96;
	const int TILE_SIZEy = 48;
	int w;
	int h;
	int num_tiles;
	int num_objects;
	double time_now = 0.0;

	std::vector<Tile> tile;
	std::vector<Object*> object; // units (human), buildings (walls, homes), resource (trees, bush), machines (carts, boats), other (???)

	World(int w_, int h_) :
		w(w_),
		h(h_)
	{
		num_tiles = w * h;
		tile.resize(num_tiles);
	}
	/*
	void setw()
	{
		int iTile = 0;
		Perlin2D p1(64, 64);
		p1.addLin(1.0, 25.0);
		for(int i = 0;i<w;i++)
			for (int j = 0; j < h; j++)
			{
				tile[iTile].precip = 1000;
				tile[iTile].temper = 15;
				double hh = fabs(p1.data[i][j] - 0.5) * 2.0;
				tile[iTile].hei = (hh - 0.5) * 2.0 * 64.0;
				tile[iTile].calcType();
				iTile++;
			}
	}

	void setNormal()
	{
		int iTile = 0;
		for (int i = 0; i < wWorld; i++)
			for (int j = 0; j < hWorld; j++)
			{
				int iSlp = i % 10;
				int jSlp = j % 10;
				tile[iTile].x = i;
				tile[iTile].y = j;
				tile[iTile].id_slp = (9 - iSlp) + 10 * jSlp;
				tile[iTile].precip = 62.5* pow(2.0,j);
				tile[iTile].humid = 8.0 / pow(2.0,i);

				tile[iTile].hei = 1.0;
				tile[iTile].calcType();
				if (tile[iTile].type == TypeTiles::GRASS4)
				{
					object.push_back(new Object);
					object[nObjects]->idFrame = 0;
					object[nObjects]->type = 1;
					object[nObjects]->posX = i;
					object[nObjects]->posY = j;
					nObjects++;
				}
				if (tile[iTile].type == TypeTiles::SNOWGRASS)
				{
					object.push_back(new Object);
					object[nObjects]->idFrame = 0;
					object[nObjects]->type = 4;
					object[nObjects]->posX = i;
					object[nObjects]->posY = j;
					nObjects++;
				}
				iTile++;
			}
	}

	void setRandom()
	{
		int iTile = 0;
		for (int i = 0; i < wWorld; i++)
			for (int j = 0; j < hWorld; j++)
			{
				tile[iTile].precip = (j - i) * 0.5 * 130.0 + 500.0;
				tile[iTile].temper = 3.0;//i + j;
				tile[iTile].hei = (i + 0.5) / wWorld;
				tile[iTile].calcType();
				iTile++;
			}
	}

	void setBase()
	{
		int iTile = 0;
		for (int i = 0; i < wWorld; i++)
			for (int j = 0; j < hWorld; j++)
			{
				int iSlp = i % 10;
				int jSlp = j % 10;
				tile[iTile].id_slp =  (9-iSlp) +10*jSlp;

				tile[iTile].precip = 900;
				tile[iTile].humid = 1.2;
				tile[iTile].hei = 1.0;
				tile[iTile].calcType();
				iTile++;
			}
	}
	*/
	void setVoid()
	{
		int iter = 0;
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++)
			{
				tile[iter].i = i;
				tile[iter].j = j;
			    tile[iter].type = 9;
				iter++;
		    }
	}

	void addObject(int t_, double x_, double y_)
	{
		x_ = int(x_) + 0.5;
		y_ = int(y_) + 0.5;
		object.push_back(new Unit(t_,x_, y_));
		num_objects++;
	}

};