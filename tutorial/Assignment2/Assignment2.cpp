#include "Assignment2.h"
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

Assignment2::Assignment2()
{
	SceneParser("data/scenes/scene1.txt",&scnData);
	xResolution = 800;
	yResolution = 800;
	//x = 0.5f;
	//y = 0;
	sourceIndx = -1;
	isRightPressed = false;
	isPressed = false;
	time = 0;
}

//Assignment2::Assignment2(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Assignment2::Init()
{	
	unsigned int texIDs[3] = { 0 , 1, 2};
	unsigned int slots[3] = { 0 , 1, 2 };

	AddShader("shaders/pickingShader");
	AddShader("shaders/raytracingShader");
	
	AddTexture("textures/box0.bmp",2);
	AddTexture("textures/grass.bmp", 2);

	AddMaterial(texIDs,slots, 1);
	AddMaterial(texIDs+1, slots+1, 1);
	
	AddShape(Plane, -1, TRIANGLES,0);
	SetShapeShader(0,1);
	SetShapeMaterial(0, 0);
	// pickedShape = 0;
	// ShapeTransformation(zTranslate,-5,0);
	// pickedShape = -1;
	SetShapeStatic(0);
		
	//SetShapeViewport(6, 1);
//	ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Assignment2::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((shapeIndx+1) & 0x000000FF) >>  0;
	int g = ((shapeIndx+1) & 0x0000FF00) >>  8;
	int b = ((shapeIndx+1) & 0x00FF0000) >> 16;

	s->Bind();	
	s->SetUniform1f("time",time);
	//s->SetUniform1f("x",x);
	//s->SetUniform1f("y",y);
	
	
	s->SetUniformMat4f("Proj", Proj);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);

	s->SetUniform4fv("eye",&scnData.eye,1);
	s->SetUniform4fv("ambient", &scnData.ambient, 1);
	s->SetUniform4fv("objects",&scnData.objects[0], scnData.objects.size());
	s->SetUniform4fv("objColors", &scnData.colors[0], scnData.colors.size());
	s->SetUniform4fv("lightsPosition", &scnData.lights[0], scnData.lights.size());
	s->SetUniform4fv("lightsDirection", &scnData.directions[0], scnData.directions.size());
	s->SetUniform4fv("lightsIntensity", &scnData.intensities[0], scnData.intensities.size());
	
	s->SetUniform4i("sizes", scnData.sizes[0], scnData.sizes[1], scnData.sizes[2], scnData.sizes[3]);
	if (data_list[shapeIndx]->GetMaterial() >= 0 && !materials.empty())
	{
//		materials[shapes[pickedShape]->GetMaterial()]->Bind(textures);
		BindMaterial(s, data_list[shapeIndx]->GetMaterial());
	}
	// if (shaderIndx == 0)
	// 	s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 0.0f);
	// else
	// 	s->SetUniform4f("lightColor", 4/100.0f, 60 / 100.0f, 99 / 100.0f, 0.5f);
	//textures[0]->Bind(0);

	
	

	//s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	//s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
//	if(shaderIndx == 0)
//		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
//	else 
//		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
	s->Unbind();
}

void Assignment2::SetPosition(int x, int y)
{
	if (isPressed)
	{
		xRel = -xOldPos + (float)x / xResolution;
		yRel = yOldPos - (float)y / yResolution;

		xOldPos += (float)xRel;
		yOldPos -= (float)(yRel);
		if (isRightPressed)
		{
			if (sourceIndx >= 0)
			{
				scnData.objects[sourceIndx][0] += xRel * 2;
				scnData.objects[sourceIndx][1] += yRel * 2;
			}
		}
		else
		{
			scnData.eye[0] += xRel * 2;
			scnData.eye[1] += yRel * 2;
		}
	}
	else
	{
		xOldPos = ((float)x / xResolution);
		yOldPos = (float)(y) / yResolution;
		//isPressed = true;
	}
}


void Assignment2::WhenRotate()
{
}

void Assignment2::WhenTranslate()
{

}

void Assignment2::Animate() {
    if(isActive)
	{
		time = time + 0.125/16.0;		
	}
}

void Assignment2::ScaleAllShapes(float amt,int viewportIndx)
{
	for (int i = 1; i < data_list.size(); i++)
	{
		if (data_list[i]->Is2Render(viewportIndx))
		{
            data_list[i]->MyScale(Eigen::Vector3d(amt, amt, amt));
		}
	}
}

float Assignment2::Intersection(Eigen::Vector3f sourcePoint)
{
	Eigen::Vector3f v = (sourcePoint - Eigen::Vector3f(0, 0, scnData.eye[2] - scnData.eye[3])).normalized();
	sourcePoint = sourcePoint + Eigen::Vector3f(scnData.eye[0], scnData.eye[1], scnData.eye[3]);
	float tmin = 1.0e10;
	int indx = -1;
	for (int i = 0; i < scnData.sizes[0]; i++) //every object
	{
		if (scnData.objects[i][3] > 0) //sphere
		{
			Eigen::Vector3f p0o = (scnData.objects[i].head(3)) - sourcePoint;
			float r = scnData.objects[i][3];
			float b = v.dot(p0o);
			float delta = b * b - p0o.dot(p0o) + r * r;
			float t;
			if (delta >= 0)
			{
				if (b >= 0)
					t = b - sqrt(delta);
				else
					t = b + sqrt(delta);
				if (t < tmin && t>0)
				{
					tmin = t;
					indx = i;
				}
			}
		}
		else  //plane
		{
			Eigen::Vector3f n = ((scnData.objects[i]).head(3)).normalized();
			Eigen::Vector3f p0o = ( - scnData.objects[i][3] * n) - scnData.objects[i].head(3);
			
			float t = (n.dot(p0o)) / (n.dot(v));
			if (t > 0 && t < tmin)
			{
				tmin = t;
				indx = i;
			}
		}
	}
	sourceIndx = indx;
	//std::cout<<"indx "<<indx<<std::endl;
	return tmin;
}

void Assignment2::RotateEye(float amt, bool upDown)
{
	float n = scnData.eye.norm();
	if (upDown)
		scnData.eye[1] += amt;
	else
		scnData.eye[0] += amt;
	//scnData.eye = scnData.eye.normalized()*n;
}

Assignment2::~Assignment2(void)
{
}

