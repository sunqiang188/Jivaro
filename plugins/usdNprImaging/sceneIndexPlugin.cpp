#include "pxr/base/tf/token.h"
#include "pxr/base/tf/staticTokens.h"

#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/imaging/hd/sceneIndexPluginRegistry.h"
#include "pxr/imaging/hd/tokens.h"

#include "sceneIndex.h"
#include "sceneIndexPlugin.h"

#include <iostream>


PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS (
    _tokens,
    ((sceneIndexPluginName, "UsdNprSceneIndexPlugin")));

TF_REGISTRY_FUNCTION(TfType) {
    HdSceneIndexPluginRegistry::Define<UsdNprSceneIndexPlugin>();
}

TF_REGISTRY_FUNCTION(HdSceneIndexPlugin) {
    HdSceneIndexPluginRegistry::GetInstance().RegisterSceneIndexForRenderer(
        TfToken(),
        _tokens->sceneIndexPluginName,
        nullptr,
        0,
        HdSceneIndexPluginRegistry::InsertionOrderAtStart);
}

UsdNprSceneIndexPlugin::UsdNprSceneIndexPlugin() = default;

HdSceneIndexBaseRefPtr
UsdNprSceneIndexPlugin::_AppendSceneIndex(
    const HdSceneIndexBaseRefPtr &inputScene,
    const HdContainerDataSourceHandle &inputArgs)
{
    return UsdNprSceneIndex::New(inputScene);
}



PXR_NAMESPACE_CLOSE_SCOPE