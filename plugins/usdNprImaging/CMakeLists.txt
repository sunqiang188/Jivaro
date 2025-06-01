#set(PXR_PREFIX pxr/usdImaging)
set(PXR_PACKAGE usdNprImaging)

usd_library(usdNprImaging
    LIBRARIES
        tf
        usd
        hd
        hdsi
        hf
        hd
        usdImaging
        usdNpr
        work

    PUBLIC_CLASSES
        utils
        mesh
        stroke
        adapter
        dataSource
        sceneIndexPlugin
        sceneIndex
        tokens

    PUBLIC_HEADERS
        api.h

    RESOURCE_FILES
        plugInfo.json
)

target_compile_definitions(usdNprImaging
    PRIVATE
        USDNPRIMAGING_EXPORTS
)

if(MSVC)
    message("THIS IS MICROSOFT VISUAL STUDIO...")
    message("${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/usdNprImaging.lib")
    message("${CMAKE_CURRENT_BINARY_DIR}/usdNprImaging.lib")
    add_custom_command(
        TARGET usdNprImaging 
        POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/usdNprImaging.lib" "${CMAKE_CURRENT_BINARY_DIR}/usdNprImaging.lib"
        COMMENT "Copying ${copy_file_name} to build directory"
)
endif()

