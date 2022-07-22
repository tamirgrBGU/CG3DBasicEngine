#pragma once

#include "../../igl_inline.h"
#include "../MeshGL.h"

#include "../ViewerData.h"
#include "ViewerPlugin.h"
#include "igl/opengl/Movable.h"
#include "igl/opengl/Material.h"


#include <Eigen/Core>
#include <Eigen/Geometry>

#include <vector>
#include <string>
#include <cstdint>

#define IGL_MOD_SHIFT           0x0001
#define IGL_MOD_CONTROL         0x0002
#define IGL_MOD_ALT             0x0004
#define IGL_MOD_SUPER           0x0008

using namespace std;

namespace igl
{

namespace opengl
{

namespace glfw
{

class Viewer abstract : public Movable
{
public:
	enum transformations { xTranslate, yTranslate, zTranslate, xRotate, yRotate, zRotate, xScale, yScale, zScale, scaleAll, reset };
	enum modes { POINTS, LINES, LINE_LOOP, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, QUADS };
	enum shapes { Axis, Plane, Cube, Octahedron, Tethrahedron, LineCopy, MeshCopy, Sphere };
	enum buffers { COLOR, DEPTH, STENCIL, BACK, FRONT, NONE };

	virtual void Init() {}
	virtual void Init(const string config) {}
	virtual void Animate() {}
	virtual void WhenTranslate() {}
	virtual Eigen::Vector3d GetCameraPosition() { return Eigen::Vector3d(0, 0, 0); }
	virtual Eigen::Vector3d GetCameraForward() { return Eigen::Vector3d(0, 0, -1); }
	virtual Eigen::Vector3d GetCameraUp() { return Eigen::Vector3d(0, 1, 0); }

	Viewer();
	virtual ~Viewer() {};
	virtual void Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, shared_ptr<const igl::opengl::Program> program) {};
	virtual void Update_overlay(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int shapeIndx, bool is_points);
	shared_ptr<Shape> AddShape(int type, shared_ptr<const Material> material, int parent, int viewport = 0);
	shared_ptr<Shape> AddShapeFromFile(const string& fileName, shared_ptr<const Material> material, int parent, int viewport = 0);
	shared_ptr<Shape> AddShapeFromMesh(shared_ptr<const Mesh> mesh, shared_ptr<const Material> material, int parent, int viewport = 0);
	string ShapeTypeToFileName(int type); // TODO: TAL
	
	Eigen::Matrix4d CalcParentsTrans(int indx);
	inline bool IsActive() const { return isActive; }
	inline void Activate() { isActive = true; }
	inline void Deactivate() { isActive = false; }

	// Alec: I call this data_list instead of just data to avoid confusion with
	// old "data" variable.
	// Stores all the data that should be visualized
	vector<shared_ptr<Shape>> shapes;
	vector<ViewerData*> data_list;
	vector<int> pShapes;
	vector<int> parents;
	vector<Texture*> textures;
	vector<Material*> materials;
	int pickedShape;
	Eigen::Vector3d pickedNormal;
	size_t selected_data_index;
	int next_data_id;
	int next_shader_id; // for flags to make sure all shaders are initialized with data
	bool isActive;
	unsigned int staticScene;

	Program* overlay_program;
	Program* overlay_point_program;
	vector<Program*> programs; // TAL: new in AddProgram without a delete

	// Keep track of the global position of the scrollwheel
	float scroll_position;

public:

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	void Draw(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, int viewportIndx, unsigned int flgs, unsigned int property_id);

	int AddMaterial(Material* material);

	void ClearPickedShapes(int viewportIndx);

	Eigen::Matrix4d GetPreviousTrans(const Eigen::Matrix4d& View, unsigned int index);

	float AddPickedShapes(const Eigen::Matrix4d& PV, const Eigen::Vector4i& viewport, int viewportIndx, int left,
		int right, int up, int bottom, int newViewportIndx);

	int AddTexture(const std::string& textureFileName, int dim);

	void MouseProccessing(int button, int xrel, int yrel, float movCoeff, Eigen::Matrix4d cameraMat, int viewportIndx);

	virtual void WhenTranslate(float dx, float dy) {}
	virtual void WhenRotate(float dx, float dy) {}
	virtual void WhenScroll(float dy) {}

	virtual void WhenTranslate(const Eigen::Matrix4d& preMat, float dx, float dy);
	virtual void WhenScroll(const Eigen::Matrix4d& preMat, float dy);
	virtual void WhenRotate(const Eigen::Matrix4d& preMat, float dx, float dy);
	
	int AddTexture(int width, int height, unsigned char* data, int mode);
	void BindTexture(int texIndx, int slot) { textures[texIndx]->Bind(slot); }

	ViewerData* AddViewerData(shared_ptr<const Mesh> mesh, shared_ptr<const Material> material, int parent, int viewport);

	void ShapeTransformation(int type, float amt, int mode);

	virtual bool Picking(unsigned char data[4], int newViewportIndx);
	inline void UnPick() { pickedShape = -1; }

	void SetParent(int indx, int newValue, bool savePosition);
};

} // glfw

} // opengl

} // igl
