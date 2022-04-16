#include "Assignment1.h"
#include <cmath>
#include <iostream>
#include <fstream>


static void printMat(const Eigen::Matrix4d& mat)
{
    std::cout << " matrix:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            std::cout << mat(j, i) << " ";
        std::cout << std::endl;
    }
}

Assignment1::Assignment1()
{

    x = 0;
    y = 0;
    r = 1.0;
    g = 1.0;
    b1 = 1.0;

    move = Eigen::Vector4f(0, 0, 0, 0);
    zoom = 1;
    iterationNumber = 3;
    coeffs = Eigen::Vector4cf(1, 0, 0, -1);
    isQuartic = 1;


}

//Assignment1::Assignment1(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{
//}

void Assignment1::Init()
{
    unsigned int texIDs[3] = { 0 , 1, 2 };
    unsigned int slots[3] = { 0 , 1, 2 };

    //AddShader("shaders/TomsShader");
    AddShader("shaders/SITShader");

    AddTexture("textures/plane.png", 2);
    //    AddTexture("textures/grass.bmp", 2);

    AddMaterial(texIDs, slots, 1);
    //    AddMaterial(texIDs+1, slots+1, 1);

    AddShape(Plane, -1, TRIANGLES, 0);
    SetShapeShader(0, 0);
    SetShapeMaterial(0, 0);

    SetShapeStatic(0);

    FindQuarticRoots();
    //    std::cout << "the roots are:\n" << Qroots << std::endl;
    //    Eigen::Vector3cf roots = FindCubicRoots();
    //    std::cout << "the roots are:\n" << roots << std::endl;
    std::cout << "first " << Qroots[0] << std::endl;
    std::cout << "second " << Qroots[1] << std::endl;
    std::cout << "third " << Qroots[2] << std::endl;
    std::cout << "fourth " << Qroots[3] << std::endl;
    //    SetShapeViewport(6, 1);


    //	ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Assignment1::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{

    Shader* s = shaders[shaderIndx];
    //    int r = ((shapeIndx+1) & 0x000000FF) >>  0;
    //    int g = ((shapeIndx+1) & 0x0000FF00) >>  8;
    //    int b = ((shapeIndx+1) & 0x00FF0000) >> 16;

    s->SetUniform1i("isQuartic", isQuartic);
    if (isQuartic == 1) {
        FindQuarticRoots();
        s->SetUniform1f("r1r", Qroots[0].real());
        s->SetUniform1f("r1i", Qroots[0].imag());
        s->SetUniform1f("r2r", Qroots[1].real());
        s->SetUniform1f("r2i", Qroots[1].imag());
        s->SetUniform1f("r3r", Qroots[2].real());
        s->SetUniform1f("r3i", Qroots[2].imag());
        s->SetUniform1f("r4r", Qroots[3].real());
        s->SetUniform1f("r4i", Qroots[3].imag());

        s->SetUniform1f("a", Qcoeffs[0]);
        s->SetUniform1f("b", Qcoeffs[1]);
        s->SetUniform1f("c", Qcoeffs[2]);
        s->SetUniform1f("d", Qcoeffs[3]);
        s->SetUniform1f("e", Qcoeffs[4]);

    }
    else {
        Eigen::Vector3cf roots = FindCubicRoots();
        s->SetUniform1f("r1r", roots[0].real());
        s->SetUniform1f("r1i", roots[0].imag());
        s->SetUniform1f("r2r", roots[1].real());
        s->SetUniform1f("r2i", roots[1].imag());
        s->SetUniform1f("r3r", roots[2].real());
        s->SetUniform1f("r3i", roots[2].imag());
        s->SetUniform1f("r4r", 0.0);
        s->SetUniform1f("r4i", 0.0);


        s->SetUniform1f("a", coeffs[0].real());
        s->SetUniform1f("b", coeffs[1].real());
        s->SetUniform1f("c", coeffs[2].real());
        s->SetUniform1f("d", coeffs[3].real());


    }

    s->SetUniform1f("k", iterationNumber);

    s->SetUniform1f("r", r);
    s->SetUniform1f("g", g);
    s->SetUniform1f("b1", b1);

    s->SetUniform4f("move", move[0], move[1], move[2], move[3]);
    s->SetUniform1f("zoom", zoom);

    s->Bind();

    //s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
    //s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
//	if(shaderIndx == 0)
//		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
//	else
//		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
    s->Unbind();
}


void Assignment1::WhenRotate(bool offset, bool xAxis)
{
    if (xAxis) {
        if (offset)
            move[0] += 0.1f;
        else
            move[1] -= 0.1f;

    }
    else {
        if (offset)
            move[1] += 0.1f;
        else
            move[0] -= 0.1f;

    }
}

void Assignment1::ZoomInOrOut(bool offset) {
    if (offset)
        zoom += 0.1;
    else if (!offset && zoom > 0.4)
        zoom -= 0.1;

}

void Assignment1::WhenTranslate()
{
}

void Assignment1::Animate() {
    if (isActive)
    {
        time += 0.01f;
    }
}

void Assignment1::ScaleAllShapes(float amt, int viewportIndx)
{
    for (int i = 1; i < data_list.size(); i++)
    {
        if (data_list[i]->Is2Render(viewportIndx))
        {
            data_list[i]->MyScale(Eigen::Vector3d(amt, amt, amt));
        }
    }
}
Eigen::Vector3cf Assignment1::FindCubicRoots()
{
    Eigen::Vector2cf reduceCoeffs = Eigen::Vector2cf::Zero();
    Eigen::Vector3cf roots;
    std::complex<float> bOver3a = (coeffs[1] / coeffs[0]) / 3.0f;
    reduceCoeffs[0] = coeffs[2] / coeffs[0] - 3.0f * bOver3a * bOver3a;
    reduceCoeffs[1] = coeffs[2] / coeffs[0] * bOver3a - coeffs[3] / coeffs[0] - 2.0f * bOver3a * bOver3a * bOver3a;
    //    std::cout<<"reduced\n"<<reduceCoeffs<<std::endl;
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
        roots[1] = std::complex<float>(cosf(3.14159f / 3.0f), sinf(3.14159f / 3.0f)) * bOver3a;
        roots[2] = std::complex<float>(cosf(2.0f * 3.14159f / 3.0f), sinf(2 * 3.14159f / 3.0f)) * bOver3a;
    }

    return roots;
}

std::complex<float> Assignment1::NewtonCubicRoot(std::complex<float> num)
{
    std::complex<float> root = num;
    const int iter = 9;
    bool isSmall = false;
    if (std::abs(num) < 1e-3)
    {
        if (std::abs(num) == 0)
            return num;
        isSmall = true;
        num = num * 1e6f;
        root = num;
    }
    else
    if (std::abs(num) < 0.9f)
        root = 1;
    for (int k = 0; k < iter; k++)
    {
        root = (2.0f * root * root * root + num) / root / root / 3.0f;
    }
    if (isSmall)
        root = root / 100.0f;
    return root;
}

Eigen::Vector3cf Assignment1::FindRootsOfReduceEquation(Eigen::Vector2cf reduceCoeffs)
{
    Eigen::Vector3cf roots = Eigen::Vector3cf::Zero();
    std::complex<float> sqroot = std::sqrt(reduceCoeffs[0] * reduceCoeffs[0] * reduceCoeffs[0] / 27.0f + reduceCoeffs[1] * reduceCoeffs[1] / 4.0f);
    std::complex<float> p = NewtonCubicRoot(reduceCoeffs[1] / 2.0f + sqroot);
    std::complex<float> n = NewtonCubicRoot(reduceCoeffs[1] / 2.0f - sqroot);
    roots[0] = p + n;
    roots[1] = p * std::complex<float>(cosf(2.0f * 3.14159f / 3.0f), sinf(2 * 3.14159f / 3.0f)) - n * std::complex<float>(cosf(1.0f * 3.14159f / 3.0f), sinf(1 * 3.14159f / 3.0f));
    roots[2] = -p * std::complex<float>(cosf(1.0f * 3.14159f / 3.0f), sinf(1 * 3.14159f / 3.0f)) + n * std::complex<float>(cosf(2.0f * 3.14159f / 3.0f), sinf(2 * 3.14159f / 3.0f));
    return roots;
}
Assignment1::~Assignment1(void)
{
}

void Assignment1::MoveHorizontal(float xpos) {
    if (xpos > x) {
        move[0] += 0.01;
    }
    else if (xpos < x) {
        move[0] -= 0.01;
    }
    x = xpos;

}

void Assignment1::MoveVertical(float ypos) {
    if (ypos > y) {
        move[1] += 0.01;
    }
    else if (ypos < y) {
        move[1] -= 0.01;
    }
    y = ypos;
}

std::complex<float> Assignment1::cuberoot(std::complex<float> z) {
    if (z.real() < 0) {
        return -pow(-z, 1.0f / 3.0f);
    }
    else {
        return pow(z, 1.0f / 3.0f);
    }
}

void Assignment1::FindQuarticRoots() {
    std::complex<float> a = Qcoeffs[0], b = Qcoeffs[1], c = Qcoeffs[2], d = Qcoeffs[3], e = Qcoeffs[4];
    std::complex<float> delta0 = c * c - ((std::complex<float>)3) * b * d + ((std::complex<float>)12) * a * e;
    std::complex<float> delta1 = ((std::complex<float>)2) * c * c * c - ((std::complex<float>)9) * b * c * d + ((std::complex<float>)27) * b * b * e + ((std::complex<float>)27) * a * d * d - ((std::complex<float>)72) * a * c * e;
    std::complex<float>Q, S, p, q;
    if (delta0 == ((std::complex<float>)0)) {
        Q = cuberoot(delta1);
    }
    else {
        std::complex<float> delta = std::sqrt(delta1 * delta1 - (std::complex<float>)4 * delta0 * delta0 * delta0);
        Q = cuberoot((delta1 + delta) / ((std::complex<float>)2));
    }
    try {
        p = (((std::complex<float>)8) * a * c - ((std::complex<float>)3) * b * b) / (((std::complex<float>)8) * a * a);
        q = (b * b * b - ((std::complex<float>)4) * a * b * c + ((std::complex<float>)8) * a * a * d) / (((std::complex<float>)8) * a * a * a);
        S = ((std::complex<float>)0.5) * std::sqrt(((std::complex<float>)(-2.0f / 3.0f)) * p + (1.0f / (3.0f * a)) * (Q + (delta0 / Q)));
        std::complex<float> delta3 = ((std::complex<float>)0.5) * std::sqrt(((std::complex<float>) - 4) * S * S - ((std::complex<float>)2) * p + q / S);
        std::complex<float> delta4 = ((std::complex<float>)0.5) * std::sqrt(((std::complex<float>) - 4) * S * S - ((std::complex<float>)2) * p - q / S);
        Qroots[0] = (-b / (((std::complex<float>)4) * a)) - S + delta3;
        Qroots[1] = (-b / (((std::complex<float>)4) * a)) - S - delta3;
        Qroots[2] = (-b / (((std::complex<float>)4) * a)) + S + delta4;
        Qroots[3] = (-b / (((std::complex<float>)4) * a)) + S - delta4;


    }
    catch (const std::exception& e) {
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
    }


}


void Assignment1::changeMode() {
    isQuartic = (isQuartic + 1) % 2;
}


