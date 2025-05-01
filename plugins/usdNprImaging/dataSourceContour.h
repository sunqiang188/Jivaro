//
// Copyright 2022 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//

#ifndef PXR_USD_IMAGING_USD_NPR_IMAGING_DATA_SOURCE_CONTOUR_H
#define PXR_USD_IMAGING_USD_NPR_IMAGING_DATA_SOURCE_CONTOUR_H

#include "pxr/usdImaging/usdImaging/dataSourceGprim.h"
#include "pxr/usdImaging/usdImaging/dataSourceStageGlobals.h"

#include "pxr/imaging/hd/dataSource.h"

PXR_NAMESPACE_OPEN_SCOPE

/// \class UsdNprDataSourceContourPrim
///
/// A prim data source representing a UsdNprContour prim. 
///
class UsdNprDataSourceContourPrim : public UsdImagingDataSourceGprim
{
public:
    HD_DECLARE_DATASOURCE(UsdNprDataSourceContourPrim);

    USDIMAGING_API
    HdDataSourceBaseHandle Get(const TfToken &name) override;

    USDIMAGING_API
    static HdDataSourceLocatorSet Invalidate(
            UsdPrim const& prim,
            const TfToken &subprim,
            const TfTokenVector &properties,
            UsdImagingPropertyInvalidationType invalidationType);

private:
    UsdNprDataSourceContourPrim(
        const SdfPath &sceneIndexPath,
        UsdPrim usdPrim,
        const UsdImagingDataSourceStageGlobals &stageGlobals);
};

HD_DECLARE_DATASOURCE_HANDLES(UsdNprDataSourceContourPrim);

PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_USD_IMAGING_USD_NPR_IMAGING_DATA_SOURCE_CONTOUR_H
