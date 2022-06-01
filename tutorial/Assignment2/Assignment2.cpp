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
	SetShapeStatic(AddShape(Plane, -1, TRIANGLES, move(make_shared<Material>("shaders/raytracingShader", false, next_data_id++))));
}

void Assignment2::Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<Material> material)
{
	shared_ptr<Shader> s = material->Bind();
	int r = ((s->m_id + 1) & 0x000000FF) >>  0;
	int g = ((s->m_id + 1) & 0x0000FF00) >>  8;
	int b = ((s->m_id + 1) & 0x00FF0000) >> 16;

	s->Bind();	
	s->SetUniform1f("time",time);
	//s->SetUniform1f("x",x);
	//s->SetUniform1f("y",y);
	
	
	s->SetUniformMat4f("Proj", Proj);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);

	s->SetUniform4fv("eye",&sceneData.eye,1);
	s->SetUniform4fv("ambient", &sceneData.ambient, 1);
	s->SetUniform4fv("objects",&sceneData.objects[0], sceneData.objects.size());
	s->SetUniform4fv("objColors", &sceneData.colors[0], sceneData.colors.size());
	s->SetUniform4fv("lightsPosition", &sceneData.lights[0], sceneData.lights.size());
	s->SetUniform4fv("lightsDirection", &sceneData.directions[0], sceneData.directions.size());
	s->SetUniform4fv("lightsIntensity", &sceneData.intensities[0], sceneData.intensities.size());
	
	s->SetUniform4i("sizes", sceneData.sizes[0], sceneData.sizes[1], sceneData.sizes[2], sceneData.sizes[3]);

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
		s->SetUniform4fv(#obj, &sceneData.obj[i], 1); \
	}

void Assignment2::LoadSceneDataToShader(Shader* s)
{
	char var[20]; // for the LoadFloatArrayToShader macro

	s->SetUniform4fv("eye", &sceneData.eye, 1);
	s->SetUniform4fv("ambient", &sceneData.ambient, 1);
	s->SetUniform4iv("sizes", &sceneData.sizes, 1);

	LoadFloatArrayToShader(objects);
	LoadFloatArrayToShader(lights);
	LoadFloatArrayToShader(directions);
	LoadFloatArrayToShader(colors);
	LoadFloatArrayToShader(intensities);
}
