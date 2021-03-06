cmake_minimum_required( VERSION 3.14 )

project( lab1 LANGUAGES CXX )

set( CMAKE_INCLUDE_CURRENT_DIR ON )


if( NOT DEFINED CMAKE_BUILD_TYPE )

    set( CMAKE_BUILD_TYPE "Release" )

endif()

message( STATUS "Build type for ${PROECT_NAME} is \"${CMAKE_BUILD_TYPE}\"")


find_library( gmsh REQUIRED )


set( ${PROJECT_NAME}_TASKS torus read_stl )

foreach( TASK_NAME ${${PROJECT_NAME}_TASKS} )
    add_executable( ${TASK_NAME} )

    target_sources( ${TASK_NAME} PRIVATE
        "${PROJECT_SOURCE_DIR}/${TASK_NAME}.cpp" )
    
    target_include_directories( ${TASK_NAME} PRIVATE
        "/usr/include"
        "${PROJECT_SOURCE_DIR}/../include"
        )

    target_link_libraries( ${TASK_NAME} gmsh )

    if( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )

        target_compile_definitions( ${TASK_NAME} PRIVATE _CS_LABS_DEBUG=1 )

    endif()

endforeach()
