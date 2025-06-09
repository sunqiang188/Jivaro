#include <pxr/base/tf/refPtr.h>
#include <pxr/imaging/hd/sceneIndex.h>
#include "pxr/imaging/hd/filteringSceneIndex.h"

#include "api.h"

PXR_NAMESPACE_OPEN_SCOPE

class UsdNprSceneIndex;
TF_DECLARE_REF_PTRS(UsdNprSceneIndex);

class UsdNprSceneIndex : public HdSingleInputFilteringSceneIndexBase 
{
public:
    static UsdNprSceneIndexRefPtr New(
      const HdSceneIndexBaseRefPtr  inputSceneIndex){
        return TfCreateRefPtr(new UsdNprSceneIndex(inputSceneIndex));}

public:
    USDNPRIMAGING_API
    HdSceneIndexPrim GetPrim(const SdfPath& primPath) const override;
    USDNPRIMAGING_API
    SdfPathVector GetChildPrimPaths(const SdfPath& primPath) const override;

protected:
    UsdNprSceneIndex(const HdSceneIndexBaseRefPtr  &inputSceneIndex);
    ~UsdNprSceneIndex();

    void _PrimsAdded(const HdSceneIndexBase& sender, const HdSceneIndexObserver::AddedPrimEntries& entries) override;
    void _PrimsRemoved(const HdSceneIndexBase& sender, const HdSceneIndexObserver::RemovedPrimEntries& entried) override;
    void _PrimsDirtied(const HdSceneIndexBase& sender, const HdSceneIndexObserver::DirtiedPrimEntries& entries) override;
private:
    
};



PXR_NAMESPACE_CLOSE_SCOPE