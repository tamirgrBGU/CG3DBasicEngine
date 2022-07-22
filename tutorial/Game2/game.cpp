#include "game.h"
#include <iostream>

using namespace Eigen;
using namespace std;
using namespace igl::opengl;

void Game::Init()
{
	auto program = make_shared<Program>("shaders/basicShader", next_data_id++);
	
	auto bricks = make_shared<Material>(program);
	auto grass = make_shared<Material>(program);
	auto daylight = make_shared<Material>("shaders/cubemapShader", next_data_id++);

	bricks->AddTexture(0, "textures/bricks.bmp", 2);
	grass->AddTexture(0, "textures/grass.bmp", 2);
	daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

	auto background = AddShape(Cube, daylight, -2);
	auto cube = AddShape(Cube, bricks, -1);
	auto sphere = make_shared<const Mesh>("data/sphere.obj");
	auto brickSphere = AddShapeFromMesh(sphere, grass, -1);
	auto grassSphere = AddShapeFromMesh(sphere, grass, -1);

	shapes.emplace_back(brickSphere);

	background->SetStatic();
	background->Scale(60.0);

	cube->SetShowWireframe();
	cube->Rotate(0.7853982, Axis::Z);

	brickSphere->SetShowWireframe();
	brickSphere->Scale(1.5);
	brickSphere->Translate(-2.0, Axis::X);

	grassSphere->SetShowWireframe();
	grassSphere->Scale(1.5);
	grassSphere->Translate(2.0, Axis::X);
}

void Game::Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<const igl::opengl::Program> p)
{
	p->SetUniformMatrix4f("Proj", &Proj);
	p->SetUniformMatrix4f("View", &View);
	p->SetUniformMatrix4f("Model", &Model);
	p->SetUniform4f("lightColor", 4 / 100.0f, 6 / 100.0f, 99 / 100.0f, 0.5f);
}
