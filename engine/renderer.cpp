#include "renderer.h"
#include <GLFW/glfw3.h>

#include <igl/unproject_onto_mesh.h>
#include <utility>
#include "igl/look_at.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

using namespace std;

namespace igl
{

namespace opengl
{

namespace glfw
{

Renderer::Renderer(float angle, float relationWH, float near, float far)
{

    callback_init = nullptr;
    callback_pre_draw = nullptr;
    callback_post_draw = nullptr;
    callback_mouse_down = nullptr;
    callback_mouse_up = nullptr;
    callback_mouse_move = nullptr;
    callback_mouse_scroll = nullptr;
    callback_key_down = nullptr;
    callback_key_up = nullptr;

    callback_init_data = nullptr;
    callback_pre_draw_data = nullptr;
    callback_post_draw_data = nullptr;
    callback_mouse_down_data = nullptr;
    callback_mouse_up_data = nullptr;
    callback_mouse_move_data = nullptr;
    callback_mouse_scroll_data = nullptr;
    callback_key_down_data = nullptr;
    callback_key_up_data = nullptr;
    glLineWidth(5);
    cameras.push_back(new Camera(angle, relationWH, near, far));
    isPressed = false;
    isMany = false;
    xold = 0;
    yold = 0;
    xrel = 0;
    yrel = 0;
    zrel = 0;
    currentViewport = 0;
    isPicked = false;
}

void Renderer::Clear(float r, float g, float b, float a,unsigned int flags)
{
    glClearColor(r, g, b, a);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT & flags);
}

void Renderer::SwapDrawInfo(int indx1, int indx2)
{
    DrawInfo* info = drawInfos[indx1];
    drawInfos[indx1] = drawInfos[indx2];
    drawInfos[indx2] = info;
}

IGL_INLINE void Renderer::draw_by_info(int info_index){
    DrawInfo* info = drawInfos[info_index];
    buffers[info->buffer]->Bind();
    glViewport(viewports[info->viewportIndx].x(), viewports[info->viewportIndx].y(), viewports[info->viewportIndx].z(), viewports[info->viewportIndx].w());
    if (info->flags & scissorTest)
    {
        glEnable(GL_SCISSOR_TEST);
        int x = min(xWhenPress, xold);
        int y = min(viewports[info->viewportIndx].w() - yWhenPress, viewports[info->viewportIndx].w() - yold);
        glScissor(x, y, abs(xWhenPress - xold), abs( yWhenPress - yold));
    }
    else
        glDisable(GL_SCISSOR_TEST);

    if (info->flags & stencilTest)
    {
        glEnable(GL_STENCIL_TEST);
        if (info->flags & passStencil)
        {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
        }
        else
        {
            if (info->flags & stencil2)
            {
                glStencilFunc(GL_EQUAL, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
            }
            else
            {
                glStencilFunc(GL_EQUAL, 0, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            }
        }
    }
    else
        glDisable(GL_STENCIL_TEST);

    if (info->flags & depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if (info->flags & blend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    }
    else
        glDisable(GL_BLEND);

    Matrix4f Proj = cameras[info->cameraIndx]->GetViewProjection().cast<float>();
    Matrix4f View = cameras[info->cameraIndx]->MakeTransScaled().inverse().cast<float>();

    if (info->flags & toClear)
    {
        if (info->flags & blackClear)
            Clear(0, 0, 0, 0,info->flags);
        else
            Clear(info->Clear_RGBA.x(), info->Clear_RGBA.y(), info->Clear_RGBA.z(), info->Clear_RGBA.w(),info->flags);
    }

    scn->Draw(Proj, View, info->viewportIndx, info->flags,info->property_id);
}

IGL_INLINE void Renderer::draw( GLFWwindow* window)
{
	using namespace std;
	using namespace Eigen;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	int width_window, height_window;
	glfwGetWindowSize(window, &width_window, &height_window);
	
	auto highdpi_tmp = (width_window == 0 || width == 0) ? highdpi : (width / width_window);

	if (fabs(highdpi_tmp - highdpi) > 1e-8)
	{
		post_resize(window,width, height);
		highdpi = highdpi_tmp;
	}
	//if (menu)
	//{
	//	menu->pre_draw();
	//	menu->callback_draw_viewer_menu();
 //   }
    int indx = 0;
    for (auto& info : drawInfos)
    {
        if (!(info->flags & (inAction | inAction2)) || ((info->flags & inAction2) && !(info->flags & stencilTest) && isPressed && !isPicked) || ((info->flags & inAction2) && (info->flags & stencilTest)  && isPicked ))
            draw_by_info(indx);
        indx++;
    }

 //   if (menu)
	//{
	//	menu->post_draw();

	//}


}

void Renderer::SetScene(igl::opengl::glfw::Viewer* viewer)
{
	scn = viewer;
}

void Renderer::TranslateCamera(Vector3f amt)
{
	cameras[0]->Translate(amt.cast<double>(), true);
}

//void Renderer::RotateCamera(float amtX, float amtY)
//{
//	core().camera_eye = core().camera_eye + Vector3f(0,amtY,0);
//	Matrix3f Mat;
//		Mat << cos(amtY),0,sin(amtY),  0, 1, 0 ,  -sin(amtY), 0, cos(amtY) ;
//	core().camera_eye = Mat* core().camera_eye;
//
//}


void Renderer::UpdatePosition(double xpos, double ypos)
{
    xrel = xold - xpos;
    yrel = yold - ypos;
    xold = xpos;
    yold = ypos;
}

void Renderer::UpdatePress(double xpos, double ypos)
{
    xWhenPress = xpos;
    yWhenPress = ypos;
}

void Renderer::AddCamera(const Vector3d& pos, float fov, float relationWH, float zNear, float zFar, int infoIndx)
{
    if (infoIndx > 0 && infoIndx < drawInfos.size())
    {
        drawInfos[infoIndx]->SetCamera(cameras.size());
    }
    cameras.push_back(new igl::opengl::Camera(fov, relationWH, zNear, zFar));
    cameras.back()->Translate(pos, false);
}

void Renderer::AddViewport(int left, int bottom, int width, int height)
{
    viewports.emplace_back(Vector4i(left, bottom, width, height));
    glViewport(left, bottom, width, height);

}

void Renderer::AddDraw(int viewportIndx, int cameraIndx, int shaderIndx, int buffIndx, unsigned int flags)
{
    drawInfos.emplace_back(new DrawInfo(viewportIndx, cameraIndx, shaderIndx, buffIndx, flags,next_property_id));
    next_property_id <<= 1;
}

void Renderer::CopyDraw(int infoIndx, int property, int indx)
{
    DrawInfo* info = drawInfos[infoIndx];
    switch (property)
    {
        case non:
            drawInfos.emplace_back(new DrawInfo(info->viewportIndx, info->cameraIndx, info->shaderIndx, info->buffer, info->flags,next_property_id));
            break;
        case viewport:
            drawInfos.emplace_back(new DrawInfo(indx, info->cameraIndx, info->shaderIndx, info->buffer, info->flags,next_property_id));
            break;
        case camera:
            drawInfos.emplace_back(new DrawInfo(info->viewportIndx, indx, info->shaderIndx, info->buffer, info->flags,next_property_id));
            break;
        case shader:
            drawInfos.emplace_back(new DrawInfo(info->viewportIndx, info->cameraIndx, indx, info->buffer, info->flags,next_property_id));
            break;
        case buffer:
            drawInfos.emplace_back(new DrawInfo(info->viewportIndx, info->cameraIndx, info->shaderIndx, indx, info->flags,next_property_id));
            break;
    }
    next_property_id <<= 1;
}

void Renderer::SetViewport(int left, int bottom, int width, int height, int indx)
{
    viewports[indx] = Vector4i(left, bottom, width, height);
    glViewport(left, bottom, width, height);
}



Renderer::~Renderer()
{
	//if (scn)
	//	delete scn;
}


bool Renderer::Picking(int x, int y)
{

    Vector4d pos;

    unsigned char data[4];
    //glGetIntegerv(GL_VIEWPORT, viewport); //reading viewport parameters
    int i = 0;
    isPicked =  scn->Picking(data,i);
    return isPicked;

}

void Renderer::OutLine()
{
    ActionDraw(0);
}

void Renderer::PickMany(int viewportIndx)
{
    if (!isPicked)
    {
        int viewportCurrIndx = 0;
        int xMin = min(xWhenPress, xold);
        int yMin = min(viewports[viewportCurrIndx].w() - yWhenPress, viewports[viewportCurrIndx].w() - yold);
        int xMax = max(xWhenPress, xold);
        int yMax = max(viewports[viewportCurrIndx].w() - yWhenPress, viewports[viewportCurrIndx].w() - yold);
		depth = scn->AddPickedShapes(cameras[0]->GetViewProjection().cast<double>() * (cameras[0]->MakeTransd()).inverse(), viewports[viewportCurrIndx], viewportCurrIndx, xMin, xMax, yMin, yMax,viewportIndx);
        if (depth != -1)
        {
            depth = (depth*2.0f - cameras[0]->GetFar()) / (cameras[0]->GetNear() - cameras[0]->GetFar());
            isMany = true;
            isPicked = true;
        }
        else
            depth = 0;

    }
}

void Renderer::ActionDraw(int viewportIndx)
{
    //if (menu)
    //{
    //    menu->pre_draw();
    //    menu->callback_draw_viewer_menu();
    //}
    for (int i = 0; i < drawInfos.size(); i++)
    {
        if ((drawInfos[i]->flags & inAction) && viewportIndx == drawInfos[i]->viewportIndx)
            draw_by_info(i);
    }
    //if (menu)
    //{
    //    menu->post_draw();

    //}
}
IGL_INLINE void Renderer::resize(GLFWwindow* window,int w, int h)
	{
		post_resize(window,w, h);
	}

IGL_INLINE void Renderer::post_resize(GLFWwindow* window, int w, int h)
	{
        // hold old windows size
        int x = 0;
        int y = 0;
        for(auto & viewport : viewports){
            x = max(x,viewport.x()+viewport.z());
            y = max(y,viewport.y()+viewport.w());
        }
        float ratio_x = (float)w/(float)x;
        float ratio_y = (float)h/(float)y;
        for(auto & viewport : viewports){
            viewport = Vector4i((int)((float)viewport.x()*ratio_x),(int)((float)viewport.y()*ratio_y),(int)((float)viewport.z()*ratio_x),(int)((float)viewport.w()*ratio_y));
        }
		if (callback_post_resize)
		{
			callback_post_resize(window, w, h);
		}
	}


void Renderer::MoveCamera(int cameraIndx, int type, float amount)
{
    switch (type)
    {
        case xTranslate:
            cameras[cameraIndx]->TranslateInSystem(cameras[cameraIndx]->MakeTransd().block<3,3>(0,0), Vector3d(amount, 0, 0));
            break;
        case yTranslate:
            cameras[cameraIndx]->TranslateInSystem(cameras[cameraIndx]->MakeTransd().block<3,3>(0,0), Vector3d(0, amount, 0));
            break;
        case zTranslate:
            cameras[cameraIndx]->TranslateInSystem(cameras[cameraIndx]->MakeTransd().block<3,3>(0,0), Vector3d(0, 0, amount));
            break;
        case xRotate:
            cameras[cameraIndx]->Rotate(amount, Viewer::Axis::X);
            break;
        case yRotate:
            cameras[cameraIndx]->Rotate(amount, Viewer::Axis::Y);
            break;
        case zRotate:
            cameras[cameraIndx]->Rotate(amount, Viewer::Axis::Z);
            break;
        case scaleAll:
            cameras[cameraIndx]->Scale(amount);
            break;
        default:
            break;
    }
}

bool Renderer::CheckViewport(int x, int y, int viewportIndx)
{
    return (viewports[viewportIndx].x() < x && viewports[viewportIndx].y() < y && viewports[viewportIndx].z() + viewports[viewportIndx].x() > x && viewports[viewportIndx].w() + viewports[viewportIndx].y() > y);
}

bool Renderer::UpdateViewport(int viewport)
{
    if (viewport != currentViewport)
    {
        isPicked = false;
        currentViewport = viewport;
        Pressed();
		scn->UnPick();
        return false;
    }
    return true;
}

void Renderer::MouseProccessing(int button, int mode, int viewportIndx)
{
    if (isPicked || button == 0)
    {

		if(button == 2)
			scn->MouseProccessing(button, zrel, zrel, CalcMoveCoeff(mode & 7, viewports[viewportIndx].w()), cameras[0]->MakeTransd(), viewportIndx);
		else
			scn->MouseProccessing(button, xrel, yrel, CalcMoveCoeff(mode & 7, viewports[viewportIndx].w()), cameras[0]->MakeTransd(), viewportIndx);
    }

}

float Renderer::CalcMoveCoeff(int cameraIndx, int width)
{
    return cameras[cameraIndx]->CalcMoveCoeff(depth,width);
}

unsigned int Renderer::AddBuffer(int infoIndx)
{
    CopyDraw(infoIndx, buffer, buffers.size());

    DrawInfo* info = drawInfos.back();
    info->SetFlags(stencilTest );
   // info->ClearFlags(depthTest);
    info->SetFlags( clearDepth | clearStencil);
    int width = viewports[info->viewportIndx].z(), height = viewports[info->viewportIndx].w();

    unsigned int texId;
    texId = scn->AddTexture(width, height, 0, COLOR);
    scn->AddTexture(width, height, 0, DEPTH);
    scn->BindTexture(texId, texId);
    buffers.push_back(new igl::opengl::DrawBuffer(width, height, texId));

    return texId;
}

//int Renderer::Create2Dmaterial(int infoIndx, int code)
//{
//    vector<unsigned int> texIds;
//    vector<unsigned int> slots;
//    
//    unsigned int texId = AddBuffer(infoIndx);
//    texIds.push_back(texId);
//    slots.push_back(texId);
//    texIds.push_back(texId + 1);
//    slots.push_back(texId + 1);
//    
//    materialIndx2D = scn->AddMaterial((unsigned int*)&texIds[0], (unsigned int*)&slots[0], 2);
//
//    return materialIndx2D;
//}


//void Renderer::SetBuffers()
//{
//    AddCamera(Vector3d(0, 0, 1), 0, 1, 1, 10,2);
//    int materialIndx = Create2Dmaterial(1,1);
//    scn->SetShapeMaterial(6, materialIndx);
//    SwapDrawInfo(2, 3);
//}

IGL_INLINE void Renderer::Init(igl::opengl::glfw::Viewer* scene, list<int>xViewport, list<int>yViewport, int pickingBits)
{
    scn = scene;
    //menu = _menu;
    MoveCamera(0, zTranslate, 10);
    Vector4i viewport;
    glGetIntegerv(GL_VIEWPORT, viewport.data());
    buffers.push_back(new igl::opengl::DrawBuffer());
    maxPixX = viewport.z();
    maxPixY = viewport.w();
    xViewport.push_front(0);
    yViewport.push_front(0);
    list<int>::iterator xit = xViewport.begin();
    int indx = 0;

    for (++xit; xit != xViewport.end(); ++xit)
    {
        list<int>::iterator yit = yViewport.begin();
        for (++yit; yit != yViewport.end(); ++yit)
        {
            viewports.emplace_back(*prev(xit), *prev(yit), *xit - *prev(xit), *yit - *prev(yit));

            if ((1 << indx) & pickingBits) {
                DrawInfo* new_draw_info = new DrawInfo(indx, 0, 0, 0,
                    1 | inAction | depthTest | stencilTest | passStencil | blackClear |
                    clearStencil | clearDepth | onPicking,
                    next_property_id);
                next_property_id <<= 1;
                for (auto& data : scn->data_list)
                {
                    new_draw_info->set(data->is_visible, true);
                }
                drawInfos.emplace_back(new_draw_info);
            }
            DrawInfo* temp = new DrawInfo(indx, 0, 1, 0, (int)(indx < 1) | depthTest | clearDepth, next_property_id);
            next_property_id <<= 1;
            drawInfos.emplace_back(temp);
            indx++;
        }
    }

    //if (menu)
    //{
    //    menu->callback_draw_viewer_menu = [&]()
    //    {
    //        // Draw parent menu content
    //        auto temp = Vector4i(0, 0, 0, 0); // set imgui to min size and top left corner
    //        menu->draw_viewer_menu(scn, cameras, temp, drawInfos);
    //    };
    //}
}

} // glfw

} // opengl

} // igl