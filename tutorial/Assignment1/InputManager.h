#pragma once   //maybe should be static class
#include "igl/opengl/glfw/Display.h"
#include "igl/opengl/glfw/renderer.h"
#include "Assignment1.h"
#include "imgui/imgui.h"


int pressedKey = 0;


void glfw_mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
        Assignment1* scn = (Assignment1*)rndr->GetScene();
        double x2, y2;

        glfwGetCursorPos(window, &x2, &y2);
        rndr->UpdatePress(x2, y2);
        if (rndr->Picking((int)x2, (int)y2))
        {

            rndr->UpdatePosition(x2, y2);
            if (button == GLFW_MOUSE_BUTTON_LEFT)
            {
                rndr->Pressed();

            }
        }
        else
        {
            rndr->UnPick(2);
        }

    }
    else
    {
        Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
        rndr->UnPick(2);
    }
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
    Assignment1* scn = (Assignment1*)rndr->GetScene();

    if (rndr->IsPicked())
    {
        rndr->UpdateZpos((int)yoffset);
        rndr->MouseProccessing(GLFW_MOUSE_BUTTON_MIDDLE);
    }
    else
    {

        rndr->MoveCamera(0, rndr->zTranslate, (float)yoffset);
    }
    scn->ZoomInOrOut(yoffset < 0);

}

void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
    Assignment1* scn = (Assignment1*)rndr->GetScene();

    rndr->UpdatePosition((float)xpos, (float)ypos);

    if (rndr->CheckViewport(xpos, ypos, 0))
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {

            rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {

            scn->MoveHorizontal(xpos);
            scn->MoveVertical(ypos);
            rndr->MoveCamera(0, rndr->xTranslate, xpos);
            rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && rndr->IsPicked() && rndr->IsMany())
            rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);

    }
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
    Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);

    rndr->resize(window, width, height);

}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
    Assignment1* scn = (Assignment1*)rndr->GetScene();

    //rndr->FreeShapes(2);
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_1:
                pressedKey = 1;
                std::cout << "You are choosing coeff a" << std::endl;
                break;
            case GLFW_KEY_2:
                pressedKey = 2;
                std::cout << "You are choosing coeff b" << std::endl;
                break;
            case GLFW_KEY_3:
                pressedKey = 3;
                std::cout << "You are choosing coeff c" << std::endl;
                break;
            case GLFW_KEY_4:
                pressedKey = 4;
                std::cout << "You are choosing coeff d" << std::endl;
                break;
            case GLFW_KEY_0:
                pressedKey = 0;
                std::cout << "Unchecked coeff" << std::endl;
                break;
            case GLFW_KEY_5:
                pressedKey = 5;
                std::cout << "You are choosing coeff e" << std::endl;
                break;
            case GLFW_KEY_6:
                pressedKey = 6;
                break;
            case GLFW_KEY_7:
                pressedKey = 7;
                break;
            case GLFW_KEY_Q:
                pressedKey = 'q';
                scn->changeMode();
                std::cout << "switched to quartic" << std::endl;
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;

            case GLFW_KEY_SPACE:
                if (scn->IsActive())
                    scn->Deactivate();
                else
                    scn->Activate();
                break;

            case GLFW_KEY_UP:
                switch (pressedKey) {
                    case(1):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[0] += 0.01;
                            if (scn->Qcoeffs[0] == 0) {
                                scn->Qcoeffs[0] += 0.01;
                            }
                        }
                        else {
                            scn->coeffs[0] += 0.01;
                        }
                        break;
                    case(2):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[1] += 0.01;
                        }
                        else {
                            scn->coeffs[1] += 0.01;
                        }
                        break;
                    case(3):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[2] += 0.01;
                        }
                        else {
                            scn->coeffs[2] += 0.01;
                        }
                        break;
                    case(4):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[3] += 0.01;
                        }
                        else {
                            scn->coeffs[3] += 0.01;
                        }
                        break;
                    case(5):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[4] += 0.01;
                        }
                        break;
                    case(6):
                        if (scn->g < 1)
                            scn->g += 0.01;
                        break;
                    case(7):
                        if (scn->b1 < 1)
                            scn->b1 += 0.01;
                        break;

                    case(8):
                        if (scn->r < 1)
                            scn->r += 0.01;
                        break;
                }
                break;
            case GLFW_KEY_DOWN:
                switch (pressedKey) {
                    case(1):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[0] -= 0.01;
                            if (scn->Qcoeffs[0] == 0) {
                                scn->Qcoeffs[0] -= 0.01;
                            }
                        }
                        else {
                            scn->coeffs[0] -= 0.01;
                        }
                        break;
                    case(2):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[1] -= 0.01;
                        }
                        else {
                            scn->coeffs[1] -= 0.01;
                        }
                        break;
                    case(3):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[2] -= 0.01;
                        }
                        else {
                            scn->coeffs[2] -= 0.01;
                        }
                        break;
                    case(4):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[3] -= 0.01;
                        }
                        else {
                            scn->coeffs[3] -= 0.01;
                        }
                        break;
                    case(5):
                        if (scn->isQuartic) {
                            scn->Qcoeffs[4] -= 0.01;
                        }
                        break;
                    case(6):
                        if (scn->g > 0)
                            scn->g -= 0.01;
                        break;
                    case(7):
                        if (scn->b1 > 0)
                            scn->b1 -= 0.01;
                        break;
                    case(8):
                        if (scn->r > 0)
                            scn->r -= 0.01;
                        break;
                }
                //scn->shapeTransformation(scn->xGlobalRotate,-5.f);
                //cout<< "down: "<<endl;
                //rndr->MoveCamera(0, scn->xRotate, -0.05f);
                break;
            case GLFW_KEY_LEFT:
                scn->iterationNumber += 1;
                std::cout << scn->iterationNumber << std::endl;
                //rndr->MoveCamera(0, scn->yRotate, 0.05f);
                break;
            case GLFW_KEY_RIGHT:
                if (scn->iterationNumber <= 1)
                    std::cout << "Cannot deceremnet IterationNum" << std::endl;
                else
                    scn->iterationNumber -= 1;
                std::cout << scn->iterationNumber << std::endl;
                break;
            case GLFW_KEY_U:
                rndr->MoveCamera(0, scn->yTranslate, 0.25f);
                break;
            case GLFW_KEY_D:
                rndr->MoveCamera(0, scn->yTranslate, -0.25f);
                break;
            case GLFW_KEY_L:
                rndr->MoveCamera(0, scn->xTranslate, -0.25f);
                break;

            case GLFW_KEY_R:
                rndr->MoveCamera(0, scn->xTranslate, 0.25f);
                break;

            case GLFW_KEY_B:
                rndr->MoveCamera(0, scn->zTranslate, 0.5f);
                break;
            case GLFW_KEY_F:
                rndr->MoveCamera(0, scn->zTranslate, -0.5f);
                break;
            default:
                break;

        }
    }
}


void Init(Display& display, igl::opengl::glfw::imgui::ImGuiMenu* menu)
{

    display.AddKeyCallBack(glfw_key_callback);
    display.AddMouseCallBacks(glfw_mouse_callback, glfw_scroll_callback, glfw_cursor_position_callback);
    display.AddResizeCallBack(glfw_window_size_callback);
    menu->init(&display);
}