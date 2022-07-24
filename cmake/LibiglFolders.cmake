# Sort projects inside the solution
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(igl_folder_targets FOLDER_NAME)
    foreach(target IN ITEMS ${ARGN})
        if(TARGET ${target})
            get_target_property(TYPE ${target} TYPE)
            if(NOT (TYPE STREQUAL "INTERFACE_LIBRARY"))
                set_target_properties(${target} PROPERTIES FOLDER "${FOLDER_NAME}")
            endif()
        endif()
    endforeach()
endfunction()

function(igl_set_folders)

igl_folder_targets("ThirdParty/Embree"
    algorithms
    embree
    lexers
    math
    simd
    sys
    tasking
)

igl_folder_targets("ThirdParty"
    CoMISo
    glad
    glfw
    imgui
    predicates
    tetgen
    tinyxml2
    triangle
)

igl_folder_targets("Libigl"
    igl
    igl_comiso
    igl_embree
    igl_opengl
    igl_opengl_glfw
    igl_opengl_glfw_imgui
    igl_png
    igl_predicates
    igl_stb_image
    igl_tetgen
    igl_triangle
    igl_xml
)

endfunction()
