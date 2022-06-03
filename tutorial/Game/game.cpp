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

	SetPickingShader("shaders/pickingShader", next_data_id++);

	auto shader = make_shared<Shader>("shaders/basicShader", next_data_id++);
	auto bricks = make_shared<Material>(shader);
	auto grass = make_shared<Material>(shader);

	auto daylight = make_shared<Material>("shaders/cubemapShader", next_data_id++);
	auto black = make_shared<Material>("shaders/blackShader", next_data_id++);

	bricks->AddTexture(0, "textures/bricks.bmp", 2);
	grass->AddTexture(0, "textures/grass.bmp", 2);
	daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

	CreateTex(800, 800);
	// texIDs[3] = AddTexture("../res/textures/Cat_bump.jpg", 2);

	shapeIDs[0] = AddShape(Cube, -2, TRIANGLES, daylight);
	shapeIDs[1] = AddShape(Tethrahedron, -1, TRIANGLES, grass);
	shapeIDs[2] = AddShape(Octahedron, -1, TRIANGLES, bricks);
	shapeIDs[3] = AddShape(Octahedron, 2, LINE_LOOP, black);
	shapeIDs[4] = AddShape(Tethrahedron, 1, LINE_LOOP, black);
	shapeIDs[5] = AddShapeFromFile("data/sphere.obj", -1, TRIANGLES, bricks);
	shapeIDs[6] = AddShape(Plane, -1, TRIANGLES, grass, 1);

	pickedShape = 0;
	ShapeTransformation(scaleAll, 60.0, 0);
	pickedShape = 1;
	ShapeTransformation(xTranslate, 3, 0);

	pickedShape = 5;
	ShapeTransformation(xTranslate, -3, 0);
	pickedShape = -1;
	SetShapeStatic(shapeIDs[0]);
	SetShapeStatic(shapeIDs[6]);
}

void Game::Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<Material> material)
{
	shared_ptr<Shader> s = material->Bind();

	s->SetUniformMatrix4f("Proj", &Proj);
	s->SetUniformMatrix4f("View", &View);
	s->SetUniformMatrix4f("Model", &Model);
	s->SetUniform1i("time", time);

	s->SetUniform4f("lightColor", 4 / 100.0f, 6 / 100.0f, 99 / 100.0f, 0.5f);
}


void Game::Animate() {
	if (isActive)
	{
		time = 1;
	}
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
