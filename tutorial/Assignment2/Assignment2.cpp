#include "Assignment2.h"
#include <iostream>

using namespace std;

using Eigen::Matrix4f;
using Eigen::Vector4cf;
using Eigen::Vector3cf;

Assignment2::Assignment2()
{
	SceneParser("data/scenes/scene1.txt", &sceneData);
	xResolution = 800;
	yResolution = 800;
	//x = 0.5f;
	//y = 0;
	sourceIndx = -1;
	isRightPressed = false;
	isPressed = false;
	time = 0;
}

void Assignment2::Init()
{
	SetShapeStatic(AddShape(Plane, -1, move(make_shared<Material>("shaders/raytracingShader", next_data_id++))));
}

void Assignment2::Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<const Program> p)
{
	int r = ((p->GetId() + 1) & 0x000000FF) >> 0;
	int g = ((p->GetId() + 1) & 0x0000FF00) >>  8;
	int b = ((p->GetId() + 1) & 0x00FF0000) >> 16;

	p->SetUniform1f("time",time);
	//p->SetUniform1f("x",x);
	//p->SetUniform1f("y",y);	
	
	p->SetUniformMatrix4f("Proj", &Proj);
	p->SetUniformMatrix4f("View", &View);
	p->SetUniformMatrix4f("Model", &Model);

	p->SetUniform4fv("eye",1,&sceneData.eye);
	p->SetUniform4fv("ambient", 1, &sceneData.ambient);
	p->SetUniform4fv("objects",sceneData.objects.size(), &sceneData.objects[0]);
	p->SetUniform4fv("objColors", sceneData.colors.size(), &sceneData.colors[0]);
	p->SetUniform4fv("lightsPosition", sceneData.lights.size(), &sceneData.lights[0]);
	p->SetUniform4fv("lightsDirection", sceneData.directions.size(), &sceneData.directions[0]);
	p->SetUniform4fv("lightsIntensity", sceneData.intensities.size(), &sceneData.intensities[0]);
	
	p->SetUniform4i("sizes", sceneData.sizes[0], sceneData.sizes[1], sceneData.sizes[2], sceneData.sizes[3]);

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
				sceneData.objects[sourceIndx][0] += xRel * 2;
				sceneData.objects[sourceIndx][1] += yRel * 2;
			}
		}
		else
		{
			sceneData.eye[0] += xRel * 2;
			sceneData.eye[1] += yRel * 2;
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
	Eigen::Vector3f v = (sourcePoint - Eigen::Vector3f(0, 0, sceneData.eye[2] - sceneData.eye[3])).normalized();
	sourcePoint = sourcePoint + Eigen::Vector3f(sceneData.eye[0], sceneData.eye[1], sceneData.eye[3]);
	float tmin = 1.0e10;
	int indx = -1;
	for (int i = 0; i < sceneData.sizes[0]; i++) //every object
	{
		if (sceneData.objects[i][3] > 0) //sphere
		{
			Eigen::Vector3f p0o = (sceneData.objects[i].head(3)) - sourcePoint;
			float r = sceneData.objects[i][3];
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
			Eigen::Vector3f n = ((sceneData.objects[i]).head(3)).normalized();
			Eigen::Vector3f p0o = ( -sceneData.objects[i][3] * n) - sceneData.objects[i].head(3);
			
			float t = (n.dot(p0o)) / (n.dot(v));
			if (t > 0 && t < tmin)
			{
				tmin = t;
				indx = i;
			}
		}
	}
	sourceIndx = indx;
	//cout<<"indx "<<indx<<endl;
	return tmin;
}

void Assignment2::RotateEye(float amt, bool upDown)
{
	float n = sceneData.eye.norm();
	if (upDown)
		sceneData.eye[1] += amt;
	else
		sceneData.eye[0] += amt;
	//scnData.eye = scnData.eye.normalized()*n;
}

Assignment2::~Assignment2(void)
{
}

#define LoadFloatArrayToShader(obj) \
	for (int i = 0; i < sceneData.obj.size(); i++) \
	{ \
		sprintf(var, "%s[%d]", #obj, i); \
		p->SetUniform4fv(#obj, 1, &sceneData.obj[i]); \
	}

void Assignment2::LoadSceneDataToProgram(Program* p)
{
	char var[20]; // for the LoadFloatArrayToShader macro

	p->SetUniform4fv("eye", 1, &sceneData.eye);
	p->SetUniform4fv("ambient", 1, &sceneData.ambient);
	p->SetUniform4iv("sizes", 1, &sceneData.sizes);

	LoadFloatArrayToShader(objects);
	LoadFloatArrayToShader(lights);
	LoadFloatArrayToShader(directions);
	LoadFloatArrayToShader(colors);
	LoadFloatArrayToShader(intensities);
}
