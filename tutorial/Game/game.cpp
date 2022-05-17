#include "game.h"
#include <iostream>


static void printMat(const Eigen::Matrix4d& mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat(j,i)<<" ";
		std::cout<<std::endl;
	}
}

Game::Game()
{
	time = 0;
}

//Game::Game(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

unsigned int Game::CreateTex(int width,int height)
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
	delete [] data;
	return texIndx;
}

void Game::Init()
{		
	unsigned int shaderIDs[4], texIDs[4], matIDs[4], shapeIDs[8], slotIDs[4] = { 0, 1, 2, 3 };
	
	shaderIDs[0] = AddShader("shaders/pickingShader");
	shaderIDs[1] = AddShader("shaders/cubemapShader");
	shaderIDs[2] = AddShader("shaders/basicShader");
	shaderIDs[3] = AddShader("shaders/basicShader2");
	//shaderIDs[3] = AddShader("shaders/pickingShader");

	texIDs[0] = AddTexture("textures/grass.bmp", 2);
	texIDs[1] = AddTexture("textures/cubemaps/Daylight Box_", 3);
	texIDs[2] = AddTexture("textures/box0.bmp", 2);
	texIDs[3] = CreateTex(800, 800);
	// texIDs[3] = AddTexture("../res/textures/Cat_bump.jpg", 2);

	matIDs[0] = AddMaterial(&texIDs[0], &slotIDs[0], 1);
	matIDs[1] = AddMaterial(&texIDs[1], &slotIDs[1], 1);
	matIDs[2] = AddMaterial(&texIDs[2], &slotIDs[2], 1);
	matIDs[3] = AddMaterial(&texIDs[3], &slotIDs[3], 1);
	
	shapeIDs[0] = AddShape(Cube, -2, TRIANGLES);
	shapeIDs[1] = AddShape(Tethrahedron, -1, TRIANGLES);
	shapeIDs[2] = AddShape(Octahedron, -1, TRIANGLES);
	shapeIDs[3] = AddShape(Octahedron, 2, LINE_LOOP);
	shapeIDs[4] = AddShape(Tethrahedron, 1, LINE_LOOP);
	shapeIDs[5] = AddShapeFromFile("data/sphere.obj", -1, TRIANGLES);
	shapeIDs[6] = AddShape(Plane, -1, TRIANGLES, 1);
	//shapeIDs[7] = AddShape(Cube, -1, TRIANGLES);
	//shapeIDs[8] = AddShapeFromFile("../res/objs/Cat_v1.obj", -1, TRIANGLES);

	SetShapeShader(shapeIDs[1], shaderIDs[2]);
	SetShapeShader(shapeIDs[2], shaderIDs[2]);
	SetShapeShader(shapeIDs[5], shaderIDs[2]);
	SetShapeShader(shapeIDs[6], shaderIDs[3]);

	SetShapeMaterial(shapeIDs[1], matIDs[0]);
	SetShapeMaterial(shapeIDs[0], matIDs[1]);
	SetShapeMaterial(shapeIDs[2], matIDs[3]);
	SetShapeMaterial(shapeIDs[5], matIDs[2]);
	//SetShapeMaterial(shapeIDs[6], matIDs[0]);

	pickedShape = 0;
	float s = 60;
	ShapeTransformation(scaleAll, s, 0);
	pickedShape = 1;
	ShapeTransformation(xTranslate, 10, 0);

	//pickedShape = 2;
	//ShapeTransformation(yTranslate, 10, 0);
	pickedShape = 5;
	ShapeTransformation(xTranslate, -10, 0);
	//pickedShape = 6;
	//ShapeTransformation(zTranslate, -1.1,0);
	pickedShape = -1;
	SetShapeStatic(shapeIDs[0]);
	SetShapeStatic(shapeIDs[6]);

	//SetShapeViewport(0, -1);
	//SetShapeViewport(6, 1);
	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((shapeIndx+1) & 0x000000FF) >>  0;
	int g = ((shapeIndx+1) & 0x0000FF00) >>  8;
	int b = ((shapeIndx+1) & 0x00FF0000) >> 16;


	s->Bind();
	s->SetUniformMat4f("Proj", Proj);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);
	s->SetUniform1i("time", time);
	if (data_list[shapeIndx]->GetMaterial() >= 0 && !materials.empty())
	{
//		materials[shapes[pickedShape]->GetMaterial()]->Bind(textures);
		BindMaterial(s, data_list[shapeIndx]->GetMaterial());
	}

	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 0.0f);
	else
		s->SetUniform4f("lightColor", 4/100.0f, 6 / 100.0f, 99 / 100.0f, 0.5f);
	//textures[0]->Bind(0);

	
	

	//s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	//s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
//	if(shaderIndx == 0)
//		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
//	else 
//		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
	s->Unbind();
}


void Game::WhenRotate()
{
}

void Game::WhenTranslate()
{
}

void Game::Animate() {
	if (isActive)
	{
		time = 1;
	}
	else
		time = 0;
}

void Game::ScaleAllShapes(float amt,int viewportIndx)
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
