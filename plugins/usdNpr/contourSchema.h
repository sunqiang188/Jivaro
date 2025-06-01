#ifndef USDNPR_CONTOUR_SCHEMA_H
#define USDNPR_CONTOUR_SCHEMA_H

#include "api.h"
#include "tokens.h"
#include "pxr/imaging/hd/schema.h"

PXR_NAMESPACE_OPEN_SCOPE

#define USDNPR_CONTOUR_SCHEMA_TOKENS \
	(contour) \
	(drawSilhouette) \
	(drawBoundary) \
	(drawCrease) \
	(drawSubset) \
	(silhouetteWidth) \
	(boundaryWidth) \
	(creaseWidth) \
	(creaseAngle) \
	(subsetWidth) \
	(viewPoint) \
	(targets)

TF_DECLARE_PUBLIC_TOKENS(UsdNprContourSchemaTokens, USDNPR_API, USDNPR_CONTOUR_SCHEMA_TOKENS);

class UsdNprContourSchema : public HdSchema {
public:
	UsdNprContourSchema(HdContainerDataSourceHandle container) : HdSchema(container) {}

	USDNPR_API
	HdBoolArrayDataSourceHandle GetDrawSilhouette();

	USDNPR_API
	HdBoolArrayDataSourceHandle GetDrawBoundary();

	USDNPR_API
	HdBoolArrayDataSourceHandle GetDrawCrease();

	USDNPR_API
	HdBoolArrayDataSourceHandle GetDrawSubset();

	USDNPR_API
	HdFloatArrayDataSourceHandle GetSilhouetteWidth();

	USDNPR_API
	HdFloatArrayDataSourceHandle GetBoundaryWidth();

	USDNPR_API
	HdFloatArrayDataSourceHandle GetCreaseWidth();

	USDNPR_API
	HdFloatArrayDataSourceHandle GetCreaseAngle();

	USDNPR_API
	HdFloatArrayDataSourceHandle GetSubsetWidth();

	USDNPR_API
	HdPathArrayDataSourceHandle GetViewPoint();

	USDNPR_API
	HdPathArrayDataSourceHandle GetTargets();

	USDNPR_API
	static UsdNprContourSchema GetFromParent(const HdContainerDataSourceHandle& parent);

	USDNPR_API
	static const TfToken& GetSchemaToken();

	USDNPR_API
	static const HdDataSourceLocator& GetDefaultLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetDrawSilhouetteLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetDrawBoundaryLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetDrawCreaseLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetDrawSubsetLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetSilhouetteWidthLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetBoundaryWidthLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetCreaseWidthLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetCreaseAngleLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetSubsetWidthLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetViewPointLocator();

	USDNPR_API
	static const HdDataSourceLocator& GetTargetsLocator();

	USDNPR_API
	static HdContainerDataSourceHandle BuildRetained(
		const HdBoolArrayDataSourceHandle& drawSilhouette,
		const HdBoolArrayDataSourceHandle& drawBoundary,
		const HdBoolArrayDataSourceHandle& drawCrease,
		const HdBoolArrayDataSourceHandle& drawSubset,
		const HdFloatArrayDataSourceHandle& silhouetteWidth,
		const HdFloatArrayDataSourceHandle& boundaryWidth,
		const HdFloatArrayDataSourceHandle& creaseWidth,
		const HdFloatArrayDataSourceHandle& creaseAngle,
		const HdFloatArrayDataSourceHandle& subsetWidth,
		const HdPathArrayDataSourceHandle& viewPoint,
		const HdPathArrayDataSourceHandle& targets
	);

	class Builder {
	public:
		USDNPR_API
		Builder& SetDrawSilhouette(const HdBoolArrayDataSourceHandle& drawSilhouette);

		USDNPR_API
		Builder& SetDrawBoundary(const HdBoolArrayDataSourceHandle& drawBoundary);

		USDNPR_API
		Builder& SetDrawCrease(const HdBoolArrayDataSourceHandle& drawCrease);

		USDNPR_API
		Builder& SetDrawSubset(const HdBoolArrayDataSourceHandle& drawSubset);

		USDNPR_API
		Builder& SetSilhouetteWidth(const HdFloatArrayDataSourceHandle& silhouetteWidth);

		USDNPR_API
		Builder& SetBoundaryWidth(const HdFloatArrayDataSourceHandle& boundaryWidth);

		USDNPR_API
		Builder& SetCreaseWidth(const HdFloatArrayDataSourceHandle& creaseWidth);

		USDNPR_API
		Builder& SetCreaseAngle(const HdFloatArrayDataSourceHandle& creaseAngle);

		USDNPR_API
		Builder& SetSubsetWidth(const HdFloatArrayDataSourceHandle& subsetWidth);

		USDNPR_API
		Builder& SetViewPoint(const HdPathArrayDataSourceHandle& viewPoint);

		USDNPR_API
		Builder& SetTargets(const HdPathArrayDataSourceHandle& targets);

		USDNPR_API
		HdContainerDataSourceHandle Build();

	private:
		HdBoolArrayDataSourceHandle 	_drawSilhouette;
		HdBoolArrayDataSourceHandle 	_drawBoundary;
		HdBoolArrayDataSourceHandle 	_drawCrease;
		HdBoolArrayDataSourceHandle 	_drawSubset;
		HdFloatArrayDataSourceHandle 	_silhouetteWidth;
		HdFloatArrayDataSourceHandle 	_boundaryWidth;
		HdFloatArrayDataSourceHandle 	_creaseWidth;
		HdFloatArrayDataSourceHandle 	_creaseAngle;
		HdFloatArrayDataSourceHandle 	_subsetWidth;
		HdPathArrayDataSourceHandle  	_viewPoint;
		HdPathArrayDataSourceHandle  	_targets;
	};
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif