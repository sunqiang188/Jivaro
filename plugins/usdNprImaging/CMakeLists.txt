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

