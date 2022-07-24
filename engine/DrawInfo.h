//
// Created by hedi1 on 04/02/2022.
//

#ifndef LIBIGL_DRAWINFO_H
#define LIBIGL_DRAWINFO_H
#include <Eigen/Core>
struct DrawInfo
{
    int viewportIndx;
    int cameraIndx;
    int shaderIndx;
    int buffer;
    unsigned int flags;
    unsigned int property_id;
    Eigen::Vector4f Clear_RGBA;


    DrawInfo(int view, int camera, int shader, int buff,unsigned int _flags,unsigned int _property_id)
    {
        viewportIndx = view;
        cameraIndx = camera;
        shaderIndx = shader;
        buffer = buff;
        flags = _flags;
        property_id = _property_id;
        Clear_RGBA = Eigen::Vector4f (1,1,1,1);
    }

    inline void SetCamera(int indx)
    {
        cameraIndx = indx;
    }

    inline void SetFlags(unsigned int value) { flags = flags | value; }
    inline void ClearFlags(unsigned int value) { flags = flags & ~value; }

    inline void set(unsigned int &property_mask, bool value)
    {
        if (!value)
            unset(property_mask);
        else
            property_mask |= property_id;
    }

    inline void unset(unsigned int &property_mask) const
    {
        property_mask &= ~property_id;
    }

    inline void toggle(unsigned int &property_mask) const
    {
        property_mask ^= property_id;
    }

    inline bool is_set(unsigned int property_mask) const
    {
        return (property_mask & property_id);
    }

};
#endif //LIBIGL_DRAWINFO_H
