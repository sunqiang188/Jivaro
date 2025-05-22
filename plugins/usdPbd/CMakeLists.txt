set(PXR_PREFIX pxr/usdPbd)
set(PXR_PACKAGE usdPbd)  

usd_library(usdPbd
    LIBRARIES
        arch
        tf
        gf
        vt
        sdf
        usd
        usdGeom

    PUBLIC_CLASSES
        bodyAPI
        collisionAPI
        constraintAPI
        solver
        tokens

    PUBLIC_HEADERS
        api.h
        bodyAPI.h
        collisionAPI.h
        constraintAPI.h
        solver.h 
        tokens.h

   PYTHON_CPPFILES
       moduleDeps.cpp

    PYMODULE_CPPFILES
        module.cpp
        wrapCollisionAPI.cpp
        wrapConstraintAPI.cpp
        wrapBodyAPI.cpp
        wrapSolver.cpp
        wrapTokens.cpp
    
    PYTHON_FILES
        __init__.py

    RESOURCE_FILES
        plugInfo.json
        generatedSchema.usda
        schema.usda
)

add_custom_command(
    TARGET usdPbd
    PRE_BUILD
    COMMAND python ${CMAKE_SOURCE_DIR}/plugins/usdGenSchema.py ${CMAKE_SOURCE_DIR}/plugins/usdPbd/schema.usda
    COMMENT "Generate UsdPbd plugin sources"
) 

if(MSVC)
    message("THIS IS MICROSOFT VISUAL STUDIO...")
    message("${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/usdPbd.lib")
    message("${CMAKE_CURRENT_BINARY_DIR}/usdPbd.lib")
    add_custom_command(
        TARGET usdPbd 
        POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/usdPbd.lib" "${CMAKE_CURRENT_BINARY_DIR}/usdPbd.lib"
        COMMENT "Copying ${copy_file_name} to build directory"
)
endif()


