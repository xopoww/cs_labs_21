cmake_minimum_required( VERSION 3.14 )

project( lab2 LANGUAGES CXX )

set( CMAKE_INCLUDE_CURRENT_DIR ON )


if( NOT DEFINED CMAKE_BUILD_TYPE )

    set( CMAKE_BUILD_TYPE "Release" )

endif()

message( STATUS "Build type for ${PROJECT_NAME} is \"${CMAKE_BUILD_TYPE}\"")


set( ${PROJECT_NAME}_LIBS gmsh VTK )

find_library( gmsh REQUIRED )

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})


add_executable( ${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/main.cpp" )

target_include_directories( ${PROJECT_NAME} PRIVATE
    "/usr/include"
    "/usr/local/include/vtk-9.0"
    "${PROJECT_SOURCE_DIR}/../../include"
    )

target_link_libraries( ${PROJECT_NAME} gmsh ${VTK_LIBRARIES} )

if( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )

    target_compile_definitions( ${PROJECT_NAME} PRIVATE _CS_LABS_DEBUG=1 )

endif()
