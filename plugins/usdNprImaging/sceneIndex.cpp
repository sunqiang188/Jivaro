#include <pxr/imaging/hd/tokens.h>
#include <pxr/imaging/hd/primvarsSchema.h>
#include <pxr/imaging/hd/basisCurvesSchema.h>

#include "sceneIndex.h"

PXR_NAMESPACE_OPEN_SCOPE

UsdNprSceneIndex::UsdNprSceneIndex(const HdSceneIndexBaseRefPtr  &inputSceneIndex)
  : HdSingleInputFilteringSceneIndexBase(inputSceneIndex)
{
}

UsdNprSceneIndex::~UsdNprSceneIndex()
{
}

SdfPathVector 
UsdNprSceneIndex::GetChildPrimPaths(
    const SdfPath &primPath) const
{
  return _GetInputSceneIndex()->GetChildPrimPaths(primPath);
}

HdSceneIndexPrim 
UsdNprSceneIndex::GetPrim(const SdfPath& primPath) const {
    HdSceneIndexPrim prim = _GetInputSceneIndex()->GetPrim(primPath);
    if (prim.primType == HdPrimTypeTokens->basisCurves) {

        HdBasisCurvesSchema curveSchema = HdBasisCurvesSchema::GetFromParent(prim.dataSource);
        HdPrimvarsSchema primvarSchema = HdPrimvarsSchema::GetFromParent(prim.dataSource);

        /*
        UsdNprContourSchema contourSchema = UsdNprContourSchema::GetFromParent(prim.dataSource);

        if (curveSchema && primvarSchema && hairProcSchema) { 
            if (auto deformer = _deformerMap.find(primPath); deformer != _deformerMap.end()) {
                prim.dataSource = _HairProcDataSource::New(primPath, prim.dataSource, deformer->second);
            }
        }
        */
    }
    return prim;
}

void 
UsdNprSceneIndex::_PrimsAdded(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::AddedPrimEntries& entries) {
    if (!_IsObserved()) {
        return;
    }
    for (const HdSceneIndexObserver::AddedPrimEntry& entry: entries) {
        if (entry.primType == HdPrimTypeTokens->basisCurves) {

            auto prim = _GetInputSceneIndex()->GetPrim(entry.primPath);
            HdBasisCurvesSchema curveSchema = HdBasisCurvesSchema::GetFromParent(prim.dataSource);
            HdPrimvarsSchema primvarSchema = HdPrimvarsSchema::GetFromParent(prim.dataSource);

            /*
            UsdNprContourSchema hairProcSchema = UsdNprContourSchema::GetFromParent(prim.dataSource);

            if (curveSchema && primvarSchema && hairProcSchema) {
                _init_deformer(entry.primPath, hairProcSchema, curveSchema, primvarSchema);
            }
            */
        }
    }
    _SendPrimsAdded(entries);
}

void
UsdNprSceneIndex::_PrimsDirtied(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::DirtiedPrimEntries& entries) {

    // If any prims in entries are part of _targets, we need to also dirty their sources, ie the hairProcedural prims
    if (!_IsObserved()) {
        return;
    }

    HdSceneIndexObserver::DirtiedPrimEntries dirty = entries;
    /*
    for (const HdSceneIndexObserver::DirtiedPrimEntry& entry: entries) {
        if (auto it = _targets.find(entry.primPath); it != _targets.end()) {
            for (const SdfPath& path : it->second) {

                auto prim = _GetInputSceneIndex()->GetPrim(path);
                HdPrimvarsSchema primvarSchema = HdPrimvarsSchema::GetFromParent(prim.dataSource);
                dirty.emplace_back(path, primvarSchema.GetPointsLocator());
            }
        }
    }
    */
    _SendPrimsDirtied(dirty);
}


void 
UsdNprSceneIndex::_PrimsRemoved(
    const pxr::HdSceneIndexBase &sender,
    const pxr::HdSceneIndexObserver::RemovedPrimEntries &entries)
{
     // If any prims in entries are part of _targets, we need to also dirty their sources, ie the hairProcedural prims
     if (!_IsObserved()) {
        return;
    }

    _SendPrimsRemoved(entries);
}



PXR_NAMESPACE_CLOSE_SCOPE