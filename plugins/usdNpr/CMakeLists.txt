set(PXR_PREFIX pxr/usd)
set(PXR_PACKAGE usdNpr)

usd_library(usdNpr
    LIBRARIES
        tf
        vt
        sdf
        usd
        usdGeom

    PUBLIC_CLASSES
        contour
        tokens
				contourSchema

    PUBLIC_HEADERS
        api.h
        contour.h

    PYTHON_CPPFILES
        moduleDeps.cpp

    PYMODULE_CPPFILES
        module.cpp
        wrapContour.cpp
        wrapTokens.cpp
    
    PYTHON_FILES
        __init__.py

    RESOURCE_FILES
        plugInfo.json
        generatedSchema.usda
        schema.usda
)

target_compile_definitions(usdNpr
    PRIVATE
        USDNPR_EXPORTS
)

add_custom_command(
    TARGET usdNpr
    PRE_BUILD
    COMMAND python ${CMAKE_SOURCE_DIR}/plugins/usdGenSchema.py ${CMAKE_SOURCE_DIR}/plugins/usdNpr/schema.usda
    COMMENT "Generate UsdNpr plugin sources"
) 

if(MSVC)
    message("THIS IS MICROSOFT VISUAL STUDIO...")
    message("${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/usdNpr.lib")
    message("${CMAKE_CURRENT_BINARY_DIR}/usdNpr.lib")
    add_custom_command(
        TARGET usdNpr 
        POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/usdNpr.lib" "${CMAKE_CURRENT_BINARY_DIR}/usdNpr.lib"
        COMMENT "Copying ${copy_file_name} to build directory"
)
endif()



