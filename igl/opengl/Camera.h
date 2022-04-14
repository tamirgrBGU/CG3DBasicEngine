//
// Created by hedi1 on 24/01/2022.
//

#ifndef ENIGEREWORK_CAMERA_H
#define ENIGEREWORK_CAMERA_H
#include "Movable.h"
#include <igl/igl_inline.h>
#include <Eigen/Geometry>
#include <Eigen/Core>
namespace igl {
    namespace opengl {
        class Camera : public Movable {
        public:
            IGL_INLINE Camera();
            IGL_INLINE Camera(float fov, float relationWH, float zNear, float zFar);

            IGL_INLINE Eigen::Matrix4f GetViewProjection() const {
                return _projection;
            }

            IGL_INLINE float GetAngle() const {
                return _fov;
            }

            IGL_INLINE float GetNear() const {
                return _near;
            }

            IGL_INLINE float GetFar() const {
                return _far;
            }

            IGL_INLINE float GetRelationWH() const {
                return _relationWH;
            }

            IGL_INLINE void SetProjection(float fov, float relationWH);

            IGL_INLINE float CalcMoveCoeff(float depth, int width) const;

            Eigen::Matrix4f _projection;
            float _fov, _relationWH;
            float _far, _near, _length;
            bool _ortho;

        };
    }
}


//#include <igl/serialize.h>
//namespace igl {
//    namespace serialization {
//
//        IGL_INLINE void serialization(bool s, igl::opengl::Camera& obj, std::vector<char>& buffer)
//        {
//
//            SERIALIZE_MEMBER(projection);
//            SERIALIZE_MEMBER(fov);
//            SERIALIZE_MEMBER(relationWH);
//            SERIALIZE_MEMBER(_far);
//            SERIALIZE_MEMBER(_near);
//
//        }
//
//        template<>
//        inline void serialize(const igl::opengl::Camera& obj, std::vector<char>& buffer)
//        {
//            serialization(true, const_cast<igl::opengl::Camera&>(obj), buffer);
//        }
//
//        template<>
//        inline void deserialize(igl::opengl::Camera& obj, const std::vector<char>& buffer)
//        {
//            serialization(false, obj, const_cast<std::vector<char>&>(buffer));
//        }
//    }
//}

#endif //ENIGEREWORK_CAMERA_H
