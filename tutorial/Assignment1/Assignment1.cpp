#include "Assignment1.h"
#include <iostream>
#include "glfw\include\GLFW\glfw3.h"

// local helper functions

using namespace std;

using Eigen::Vector2cf;

//using namespace igl::opengl;


complex<float> NewtonCubicRoot(complex<float> num)
{
	complex<float> root = num;
	const int iter = 9;
	bool isSmall = false;
	if (abs(num) < 1e-3)
	{
		if (abs(num) == 0)
			return num;
		isSmall = true;
		num = num * 1e6f;
		root = num;
	}
	else
		if (abs(num) < 0.9f)
			root = 1;
	for (int k = 0; k < iter; k++)
	{
		root = (2.0f * root * root * root + num) / root / root / 3.0f;
	}
	if (isSmall)
		root = root / 100.0f;
	return root;
}

Vector3cf FindRootsOfReduceEquation(Vector2cf reduceCoeffs)
{
	Vector3cf roots = Vector3cf::Zero();
	complex<float> sqroot = sqrt(reduceCoeffs[0] * reduceCoeffs[0] * reduceCoeffs[0] / 27.0f + reduceCoeffs[1] * reduceCoeffs[1] / 4.0f);
	complex<float> p = NewtonCubicRoot(reduceCoeffs[1] / 2.0f + sqroot);
	complex<float> n = NewtonCubicRoot(reduceCoeffs[1] / 2.0f - sqroot);
	roots[0] = p + n;
	roots[1] = p * complex<float>(cosf(2.0f * 3.14159f / 3.0f), sinf(2 * 3.14159f / 3.0f)) - n * complex<float>(cosf(1.0f * 3.14159f / 3.0f), sinf(1 * 3.14159f / 3.0f));
	roots[2] = -p * complex<float>(cosf(1.0f * 3.14159f / 3.0f), sinf(1 * 3.14159f / 3.0f)) + n * complex<float>(cosf(2.0f * 3.14159f / 3.0f), sinf(2 * 3.14159f / 3.0f));
	return roots;
}

Vector3cf FindCubicRoots(Vector4cf coeffs)
{
	Vector3cf roots;
	Vector2cf reduceCoeffs = Vector2cf::Zero();
	complex<float> bOver3a = (coeffs[1] / coeffs[0]) / 3.0f;
	reduceCoeffs[0] = coeffs[2] / coeffs[0] - 3.0f * bOver3a * bOver3a;
	reduceCoeffs[1] = coeffs[2] / coeffs[0] * bOver3a - coeffs[3] / coeffs[0] - 2.0f * bOver3a * bOver3a * bOver3a;
	if (reduceCoeffs.norm() > 0.000001)
	{
		roots = FindRootsOfReduceEquation(reduceCoeffs);
		roots[0] -= bOver3a;
		roots[1] -= bOver3a;
		roots[2] -= bOver3a;
	}
	else
	{
		roots[0] = -1.0f * bOver3a;
		roots[1] = complex<float>(cosf(3.14159f / 3.0f), sinf(3.14159f / 3.0f)) * bOver3a;
		roots[2] = complex<float>(cosf(2.0f * 3.14159f / 3.0f), sinf(2 * 3.14159f / 3.0f)) * bOver3a;
	}

	return roots;
}

// Assignment1 methods

Assignment1::Assignment1()
{
	coeffs = Vector4cf::Zero();
}

void Assignment1::Init(int init_width, int init_height)
{
	width = init_width;
	height = init_height;
	
	AddShape(Plane, -1, move(make_shared<igl::opengl::Material>("shaders/shader", next_data_id++)));

	cout.precision(3);

	coeffs[0] = 1;
	coeffs[1] = 0;
	coeffs[2] = 0;
	coeffs[3] = 1;

	CalcRoots();
}

void Assignment1::CalcRoots()
{
	roots = FindCubicRoots(coeffs);

	cout << "coeffs: " << coeffs[0].real() << ", " << coeffs[1].real() << ", " << coeffs[2].real() << ", " << coeffs[3].real() 
		<< " roots: " << roots[0] << ", " << roots[1] << ", " << roots[2] << endl;
}

void Assignment1::Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<const igl::opengl::Program> p)
{
	p->SetUniformMatrix4f("Proj", &Proj);
	p->SetUniformMatrix4f("View", &View);
	p->SetUniformMatrix4f("Model", &Model);

	p->SetUniform4f("coeffs", coeffs[0].real(), coeffs[1].real(), coeffs[2].real(), coeffs[3].real());
	p->SetUniform4f("roots[0]", roots[0].real(), roots[0].imag(), 0.0f, 0.0f);
	p->SetUniform4f("roots[1]", roots[1].real(), roots[1].imag(), 0.0f, 0.0f);
	p->SetUniform4f("roots[2]", roots[2].real(), roots[2].imag(), 0.0f, 0.0f);
	p->SetUniform1i("iterations", iterations);
	p->SetUniform1f("scale", scale);
	p->SetUniform1f("xpos", xpos);
	p->SetUniform1f("ypos", ypos);
}

void Assignment1::KeyPressed(int key)
{
	switch (key) 
	{
	case GLFW_KEY_1:
	case GLFW_KEY_2:
	case GLFW_KEY_3:
	case GLFW_KEY_4:
		active_coeff = key - GLFW_KEY_1;
		cout << "active cooefficient is: " << active_coeff << endl;
		break;
	case GLFW_KEY_UP:
		coeffs[active_coeff] += 0.01f;
		CalcRoots();
		break;
	case GLFW_KEY_DOWN:
		coeffs[active_coeff] -= 0.01f;
		CalcRoots();
		break;
	case GLFW_KEY_LEFT:
		iterations = iterations == 1 ? 1 : iterations - 1;
		cout << "iterations: " << iterations << endl;
		break;
	case GLFW_KEY_RIGHT:
		iterations++;
		cout << "iterations: " << iterations << endl;
		break;
	}
}

void Assignment1::Scroll(int yoffset)
{
	scale *= yoffset > 0 ? 0.9f : 1.0f + 1.0f / 9.0f;

	cout << "pixel width: " << scale * 2.0 / width << " height: " << scale * 2.0 / height << ", scale: " << scale << endl;
}

void Assignment1::UpdateCursorPosition(int x, int y)
{
	cursor_xpos = x;
	cursor_ypos = y;
}

void Assignment1::DragTo(int x, int y)
{
	double x_rel = cursor_xpos - x;
	double y_rel = y - cursor_ypos;
	xpos += x_rel * 2.0 * scale / width;
	ypos += y_rel * 2.0 * scale / height;

	cout << "xpos, ypos: " << xpos << ", " << ypos << endl;

	UpdateCursorPosition(x, y);
}

void Assignment1::Resize(int new_width, int new_height)
{
	width = new_width;
	height = new_height;

	cout << "pixel width: " << scale * 2.0 / width << " height: " << scale * 2.0 / height << endl;
}