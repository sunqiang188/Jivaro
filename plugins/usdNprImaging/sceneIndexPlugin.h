#include "pxr/pxr.h"
#include "pxr/imaging/hd/sceneIndex.h"
#include "pxr/imaging/hd/sceneIndexPlugin.h"
#include "pxr/imaging/hd/sceneIndexPluginRegistry.h"

#include "api.h"
#include "stroke.h"
#include "mesh.h"

PXR_NAMESPACE_OPEN_SCOPE

class UsdPrim;

class UsdNprSceneIndexPlugin : public HdSceneIndexPlugin {
public:

	struct _ContourAdapterComputeDatas {
		const UsdPrim* prim;
		double time;
		const UsdNprStrokeParams* strokeParams;
		UsdNprStrokeGraph* graph;

		UsdNprHalfEdgeMesh* halfEdgeMesh;
		GfMatrix4d viewPointMatrix;
		UsdNprEdgeClassification classification;

		bool drawSilhouette;
		bool drawCrease;
		bool drawBoundary;
	};
  
	typedef TfHashMap<SdfPath, UsdNprHalfEdgeMeshSharedPtr, SdfPath::Hash> 
		UsdNprHalfEdgeMeshMap;

		static const HdSceneIndexPluginRegistry::InsertionPhase
		GetInsertionPhase()
		{
				// XXX Until we have a better way to declare ordering/dependencies b/w
				//     scene index plugins, allow plugins to run before and after this
				//     plugin (i.e., don't use 0).
				return 2;
		}

		UsdNprSceneIndexPlugin();
    
protected:
	HdSceneIndexBaseRefPtr _AppendSceneIndex(
			const HdSceneIndexBaseRefPtr& inputScene,
			const HdContainerDataSourceHandle& inputArgs) override;
};

PXR_NAMESPACE_CLOSE_SCOPE