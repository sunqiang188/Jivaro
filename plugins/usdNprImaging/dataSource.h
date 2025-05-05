//
// Copyright 2025 benmalartre
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#ifndef PXR_USD_IMAGING_NPR_IMAGING_DATA_SOURCE_CONTOUR_H
#define PXR_USD_IMAGING_NPR_IMAGING_DATA_SOURCE_CONTOUR_H

#include "pxr/imaging/hd/dataSource.h"
#include "pxr/imaging/hd/dataSourceTypeDefs.h"
#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/usdImaging/usdImaging/dataSourceGprim.h"
#include "pxr/usdImaging/usdImaging/dataSourceStageGlobals.h"

#include "mesh.h"
#include "stroke.h"
#include "tokens.h"

PXR_NAMESPACE_OPEN_SCOPE

struct _ContourDataSourceCache {
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

// ---------------------------------------------------------------------- //
/// \name Computation
// ---------------------------------------------------------------------- //
static void 
_ContourDataSourceBuildStrokes(_ContourDataSourceCache& datas);

///
/// \class UsdNprImagingDataSourceContour
///
/// Data source representing a USD gprim. This is the common base for geometric
/// types and includes features such as materials and primvars.
///
class UsdNprImagingDataSourceContour : public UsdImagingDataSourceGprim
{
/*
public:
    HD_DECLARE_DATASOURCE(UsdNprImagingDataSourceContour);

    /// Returns the data source representing \p name, if valid.
    ///
    USDIMAGING_API
    HdDataSourceBaseHandle Get(const TfToken &name) override;

    USDIMAGING_API
    static HdDataSourceLocatorSet Invalidate(
            UsdPrim const& prim,
            const TfToken &subprim,
            const TfTokenVector &properties,
            UsdImagingPropertyInvalidationType invalidationType);

protected:

    /// Use to construct a new UsdNprImagingDataSourceContour.
    ///
    /// \p sceneIndexPath is the path of this object in the scene index.
    ///
    /// \p usdPrim is the USD prim object that this data source represents.
    ///
    /// \p stageGlobals represents the context object for the UsdStage with
    /// which to evaluate this attribute data source.
    ///
    /// Note: client code calls this via static New.
    USDIMAGING_API
    UsdNprImagingDataSourceContour(
            const SdfPath &sceneIndexPath,
            UsdPrim usdPrim,
            const UsdImagingDataSourceStageGlobals &stageGlobals);
*/
public:
  HD_DECLARE_DATASOURCE(UsdNprImagingDataSourceContour);

  // return the names of the data-sources this object can provide
  TfTokenVector GetNames() override;

  // return the data-sources based on the names
  HdDataSourceBaseHandle Get(const TfToken& i_name) override;

  // callbacks to help which DataSource Locations need to be invalidated
  // based on the prim/subPrim/properties
  static HdDataSourceLocatorSet Invalidate(
    UsdPrim const& prim,
    const TfToken& subprim,
    const TfTokenVector& properties,
    UsdImagingPropertyInvalidationType invalidationType);

private:
  UsdNprImagingDataSourceContour(
    const SdfPath& sceneIndexPath,
    UsdPrim usdPrim,
    const UsdImagingDataSourceStageGlobals& stageGlobals);
};

HD_DECLARE_DATASOURCE_HANDLES(UsdNprImagingDataSourceContour );

PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_USD_IMAGING_NPR_IMAGING_DATA_SOURCE_CONTOUR_H
