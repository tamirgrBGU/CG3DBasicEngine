#pragma once
#include <igl/igl_inline.h>
#include <vector>
#include <functional>
//#include <igl/opengl/ViewerCore.h>
#include "DrawInfo.h"
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <../imgui/imgui.h>
#include "../DrawBuffer.h"
#include "../Camera.h"
#include "../ViewerData.h"

struct GLFWwindow;



class Renderer 
{
public:
    Renderer(float angle, float relationWH, float near, float far);

    enum buffersMode {
        COLOR, DEPTH, STENCIL, BACK, FRONT, NONE
    };
    enum transformations {
        xTranslate,
        yTranslate,
        zTranslate,
        xRotate,
        yRotate,
        zRotate,
        xScale,
        yScale,
        zScale,
        scaleAll,
        xCameraTranslate,
        yCameraTranslate,
        zCameraTranslate
    };
    enum drawFlags {
        toClear = 1,
        is2D = 2,
        inAction = 4,
        scissorTest = 8,
        depthTest = 16,
        stencilTest = 32,
        blend = 64,
        blackClear = 128,
        clearDepth = 256,
        backdraw = 512,
        clearStencil = 1024,
        passStencil = 2048,
        inAction2 = 4096,
        none = 8192,
        scaleAbit = 16384,
        stencil2 = 32768,
        onPicking = 65536
    };
    enum {
        preRot, postRot, phiRot, thetaRot, psiRot, psiPhiRot
    };
    enum {
        non, viewport, camera, shader, buffer,
    };

	~Renderer();
	IGL_INLINE void draw( GLFWwindow* window);

	//IGL_INLINE bool key_pressed(unsigned int unicode_key, int modifiers);

		// Returns **true** if action should be cancelled.
	std::function<bool(GLFWwindow* window)> callback_init;
	std::function<bool(GLFWwindow* window)> callback_pre_draw;
	std::function<bool(GLFWwindow* window)> callback_post_draw;
	std::function<bool(GLFWwindow* window, int button, int modifier)> callback_mouse_down;
	std::function<bool(GLFWwindow* window, int button, int modifier)> callback_mouse_up;
	std::function<bool(GLFWwindow* window, int mouse_x, int mouse_y)> callback_mouse_move;
	std::function<bool(GLFWwindow* window, float delta_y)> callback_mouse_scroll;
	std::function<bool(GLFWwindow* window, unsigned int key, int modifiers)> callback_key_pressed;
	std::function<bool(GLFWwindow* window, int w, int h)> callback_post_resize;
	// THESE SHOULD BE DEPRECATED:
	std::function<bool(GLFWwindow* window, unsigned int key, int modifiers)> callback_key_down;
	std::function<bool(GLFWwindow* window, unsigned int key, int modifiers)> callback_key_up;
	// Pointers to per-callback data
	void* callback_init_data;
	void* callback_pre_draw_data;
	void* callback_post_draw_data;
	void* callback_mouse_down_data;
	void* callback_mouse_up_data;
	void* callback_mouse_move_data;
	void* callback_mouse_scroll_data;
	void* callback_key_pressed_data;
	void* callback_key_down_data;
	void* callback_key_up_data;

// Callbacks
//	 double Picking(double x, double y);
	 inline void Animate() { scn->Animate(); };
	IGL_INLINE bool key_pressed(unsigned int unicode_key, int modifier);
	IGL_INLINE void resize(GLFWwindow* window,int w, int h); // explicitly set window size
	IGL_INLINE void post_resize(GLFWwindow* window, int w, int h); // external resize due to user interaction
    void SetScene(igl::opengl::glfw::Viewer* scn);
	void UpdatePosition(double xpos, double ypos);
	inline igl::opengl::glfw::Viewer* GetScene() {
		return scn;
	}

	void TranslateCamera(Eigen::Vector3f amt);

    float UpdatePosition(float xpos, float ypos);

    void UpdatePress(float xpos, float ypos);

    void AddCamera(const Eigen::Vector3d &pos, float fov, float relationWH, float zNear, float zFar,
                   int infoIndx = -1);

    void AddViewport(int left, int bottom, int width, int height);

    unsigned int AddBuffer(int infoIndx, bool stencil = false);

    int Create2Dmaterial(int texsNum);

    void AddDraw(int viewportIndx, int cameraIndx, int shaderIndx, int buffIndx, unsigned int flags);

    void CopyDraw(int infoIndx, int property, int indx);

    void SetViewport(int left, int bottom, int width, int height, int indx);

    inline void BindViewport2D(int indx) { drawInfos[indx]->SetFlags(is2D); }

    void MoveCamera(int cameraIndx, int type, float amt);

    bool Picking(int x, int y);

    void OutLine();

    void PickMany(int viewportIndx);

    void MouseProccessing(int button, int mode = 0, int viewportIndx = 0);

    inline float GetNear(int cameraIndx) { return cameras[cameraIndx]->GetNear(); }

    inline float GetFar(int cameraIndx) { return cameras[cameraIndx]->GetFar(); }

    inline float GetAngle(int cameraIndx) { return cameras[cameraIndx]->GetAngle(); }

    inline void SetDrawFlag(int infoIndx, unsigned int flag) { drawInfos[infoIndx]->SetFlags(flag); }

    inline void ClearDrawFlag(int infoIndx, unsigned int flag) { drawInfos[infoIndx]->ClearFlags(flag); }

    inline void Pressed() { isPressed = !isPressed; }

    inline bool IsPressed() const { return isPressed; }

    inline void FreeShapes(int viewportIndx) { scn->ClearPickedShapes(viewportIndx); };

    bool CheckViewport(int x, int y, int viewportIndx);

    bool UpdateViewport(int viewport);

    inline int GetViewportsSize() { return viewports.size(); }

    float CalcMoveCoeff(int cameraIndx, int width);

    inline void UpdateZpos(int ypos) { zrel = ypos; }

    inline void ClearPickedShapes(int viewportIndx) {
        scn->ClearPickedShapes(viewportIndx);
        isMany = false;
    }

    inline void UnPick(int viewportIndx) {
        isPicked = false;
        scn->ClearPickedShapes(viewportIndx);
    }
    inline bool IsPicked() { return isPicked; }
    inline bool IsMany() const { return isMany; }
    void Init(igl::opengl::glfw::Viewer *scene, std::list<int> xViewport, std::list<int> yViewport, int pickingBits,igl::opengl::glfw::imgui::ImGuiMenu *_menu);


private:
    // Stores all the viewing options
//    std::vector<igl::opengl::ViewerCore> core_list;
    std::vector<igl::opengl::Camera*> cameras;
    igl::opengl::glfw::Viewer* scn;
    std::vector<Eigen::Vector4i> viewports;
    std::vector<DrawInfo *> drawInfos;
    std::vector<igl::opengl::DrawBuffer*> buffers;
	size_t selected_core_index;
	int next_core_id;
    int xold, yold, xrel, yrel, zrel;
    int maxPixX, maxPixY;
    int xWhenPress, yWhenPress;
    bool isMany;
    bool isPicked;
    int materialIndx2D;
    bool isPressed;
    int currentViewport;
	unsigned int next_property_id = 1;
	float highdpi;
	float depth;
	unsigned int left_view, right_view;
	double doubleVariable;
	igl::opengl::glfw::imgui::ImGuiMenu* menu;
	double z;

    void draw_by_info(int info_index = 1);

    void ActionDraw(int viewportIndx);

    void Clear(float r, float g, float b, float a, unsigned int flags);

};

