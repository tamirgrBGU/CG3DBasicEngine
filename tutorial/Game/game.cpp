#include "game.h"
#include <iostream>

using std::make_shared;
using Eigen::Matrix4d;

static void printMat(const Matrix4d& mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << mat(j, i) << " ";
		std::cout << std::endl;
	}
}

Game::Game()
{
	time = 0;
}

//Game::Game(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

unsigned int Game::CreateTex(int width, int height)
{
	unsigned char* data = new unsigned char[width * height * 4];
	for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			data[(i * height + j) * 4] = (i + j) % 256;
			data[(i * height + j) * 4 + 1] = (i + j * 2) % 256;
			data[(i * height + j) * 4 + 2] = (i * 2 + j) % 256;
			data[(i * height + j) * 4 + 3] = (i * 3 + j) % 256;
		}
	}
	unsigned int texIndx = AddTexture(width, height, data, COLOR);
	delete[] data;
	return texIndx;
}

void Game::Init()
{
	unsigned int shapeIDs[8];

	auto program = make_shared<Program>("shaders/basicShader", next_data_id++);
	auto bricks = make_shared<Material>(program);
	auto grass = make_shared<Material>(program);

	auto daylight = make_shared<Material>("shaders/cubemapShader", next_data_id++);
	auto black = make_shared<Material>("shaders/blackShader", next_data_id++);

	bricks->AddTexture(0, "textures/bricks.bmp", 2);
	grass->AddTexture(0, "textures/grass.bmp", 2);
	daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

	CreateTex(800, 800);
	// texIDs[3] = AddTexture("../res/textures/Cat_bump.jpg", 2);

	shapeIDs[0] = AddShape(Cube, -2, daylight);
	shapeIDs[1] = AddShape(Tethrahedron, -1, grass);
	shapeIDs[2] = AddShape(Octahedron, -1, bricks);
	shapeIDs[3] = AddShapeFromFile("data/sphere.obj", -1, bricks);
	shapeIDs[4] = AddShape(Plane, -1, grass, 1);

	SetShapeWireframe(shapeIDs[1], true);
	SetShapeWireframe(shapeIDs[2], true);
	SetShapeWireframe(shapeIDs[3], true);

	pickedShape = 0;
	ShapeTransformation(scaleAll, 60.0, 0);
	pickedShape = 1;
	ShapeTransformation(xTranslate, 3, 0);

	pickedShape = 3;
	ShapeTransformation(xTranslate, -3, 0);
	pickedShape = -1;
	SetShapeStatic(shapeIDs[0]);
	SetShapeStatic(shapeIDs[4]);
}

void Game::Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<const Program> p)
{
	p->SetUniformMatrix4f("Proj", &Proj);
	p->SetUniformMatrix4f("View", &View);
	p->SetUniformMatrix4f("Model", &Model);
	p->SetUniform1i("time", time);

	p->SetUniform4f("lightColor", 4 / 100.0f, 6 / 100.0f, 99 / 100.0f, 0.5f);
}

void Game::Animate() 
{
	if (isActive)
		time = 1;
	else
		time = 0;
}

void Game::ScaleAllShapes(float amt, int viewportIndx)
{
	for (int i = 1; i < data_list.size(); i++)
	{
		if (data_list[i]->Is2Render(viewportIndx))
		{
			data_list[i]->MyScale(Eigen::Vector3d(amt, amt, amt));
		}
	}
}

Game::~Game(void)
{
}
