#include "igl/opengl/glfw/renderer.h"
#include "Assignment1.h"
#include "InputManager.h"

int main(int argc, char* argv[])
{
    const int DISPLAY_WIDTH = 1200;
    const int DISPLAY_HEIGHT = 800;
    const float CAMERA_ANGLE = 0;
    const float NEAR = 1.0f;
    const float FAR = 120.0f;
    const int infoIndx = 2;
    std::list<int> x, y;
    x.push_back(DISPLAY_WIDTH);
    y.push_back(DISPLAY_HEIGHT);
    Display disp = Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OPENGL");
    igl::opengl::glfw::imgui::ImGuiMenu* menu = new igl::opengl::glfw::imgui::ImGuiMenu();
    Renderer* rndr = new Renderer(CAMERA_ANGLE, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, NEAR, FAR);
    Assignment1* scn = new Assignment1();  //initializing scene

    Init(disp, menu); //adding callback functions
    scn->Init();    //adding shaders, textures, shapes to scene
    rndr->Init(scn, x, y, 1, nullptr); // adding scene and viewports to the renderer
    //rndr->SetViewport(-1, -1, 1, 1, 0); //?
    disp.SetRenderer(rndr);

    disp.launch_rendering(rndr);

    delete scn;
    delete rndr;
    delete menu;

    return 0;
}