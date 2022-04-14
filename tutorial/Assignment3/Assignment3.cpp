#include "Assignment3.h"
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

Assignment3::Assignment3()
{
}

//Assignment3::Assignment3(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Assignment3::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 2};
	unsigned int slots[3] = { 0 , 1, 2 };
	
	AddShader("shaders/pickingShader");
	AddShader("shaders/cubemapShader");
	AddShader("shaders/basicShader");
	AddShader("shaders/pickingShader");
	
	AddTexture("textures/box0.bmp",2);
	AddTexture("textures/cubemaps/Daylight Box_", 3);
	AddTexture("textures/grass.bmp", 2);
	//AddTexture("../res/textures/Cat_bump.jpg", 2);

	AddMaterial(texIDs,slots, 1);
	AddMaterial(texIDs+1, slots+1, 1);
	AddMaterial(texIDs + 2, slots + 2, 1);
	
	AddShape(Cube, -2, TRIANGLES);
	AddShape(Tethrahedron, -1, TRIANGLES);
	
	AddShape(Octahedron, -1, TRIANGLES);
	AddShape(Octahedron, 2, LINE_LOOP);
    AddShape(Tethrahedron, 1, LINE_LOOP);

//    AddShape(Cube, -1, TRIANGLES);
	AddShapeFromFile("data/sphere.obj", -1, TRIANGLES);
	//AddShapeFromFile("../res/objs/Cat_v1.obj", -1, TRIANGLES);
	AddShape(Plane, -2, TRIANGLES,3);

	SetShapeShader(1, 2);
	SetShapeShader(2, 2);
	SetShapeShader(5, 2);
	SetShapeShader(6, 3);
	SetShapeMaterial(1, 0);
	SetShapeMaterial(0, 1);
	SetShapeMaterial(2, 2);
	SetShapeMaterial(5, 2);
	SetShapeMaterial(6, 0);
	pickedShape = 0;
	float s = 60;
	ShapeTransformation(scaleAll, s,0);
	pickedShape = 1;
	ShapeTransformation(xTranslate, 10,0);

	pickedShape = 5;
	ShapeTransformation(xTranslate, -10,0);
	pickedShape = 6;
	ShapeTransformation(zTranslate, -1.1,0);
	pickedShape = -1;
	SetShapeStatic(0);
	SetShapeStatic(6);

	//SetShapeViewport(6, 1);
//	ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Assignment3::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((shapeIndx+1) & 0x000000FF) >>  0;
	int g = ((shapeIndx+1) & 0x0000FF00) >>  8;
	int b = ((shapeIndx+1) & 0x00FF0000) >> 16;


		s->Bind();
	s->SetUniformMat4f("Proj", Proj);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);
	if (data_list[shapeIndx]->GetMaterial() >= 0 && !materials.empty())
	{
//		materials[shapes[pickedShape]->GetMaterial()]->Bind(textures);
		BindMaterial(s, data_list[shapeIndx]->GetMaterial());
	}
	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 0.0f);
	else
		s->SetUniform4f("lightColor", 4/100.0f, 60 / 100.0f, 99 / 100.0f, 0.5f);
	//textures[0]->Bind(0);

	
	

	//s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	//s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
//	if(shaderIndx == 0)
//		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
//	else 
//		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
	s->Unbind();
}


void Assignment3::WhenRotate()
{
}

void Assignment3::WhenTranslate()
{
}

void Assignment3::Animate() {
    if(isActive)
	{
		
	}
}

void Assignment3::ScaleAllShapes(float amt,int viewportIndx)
{
	for (int i = 1; i < data_list.size(); i++)
	{
		if (data_list[i]->Is2Render(viewportIndx))
		{
            data_list[i]->MyScale(Eigen::Vector3d(amt, amt, amt));
		}
	}
}

Assignment3::~Assignment3(void)
{
}

