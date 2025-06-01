#include "pxr/imaging/hd/retainedDataSource.h"

#include "contourSchema.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(UsdNprContourSchemaTokens, USDNPR_SCHEMA_TOKENS);

HdBoolArrayDataSourceHandle UsdNprContourSchema::GetDrawSilhouette() {
  return _GetTypedDataSource<HdBoolArrayDataSource>(UsdNprContourSchemaTokens->drawSilhouette);
}

HdBoolArrayDataSourceHandle UsdNprContourSchema::GetDrawBoundary() {
  return _GetTypedDataSource<HdBoolArrayDataSource>(UsdNprContourSchemaTokens->drawBoundary);
}

HdBoolArrayDataSourceHandle UsdNprContourSchema::GetDrawCrease() {
  return _GetTypedDataSource<HdBoolArrayDataSource>(UsdNprContourSchemaTokens->drawCrease);
}

HdBoolArrayDataSourceHandle UsdNprContourSchema::GetDrawSubset() {
  return _GetTypedDataSource<HdBoolArrayDataSource>(UsdNprContourSchemaTokens->drawSubset);
}

HdFloatArrayDataSourceHandle UsdNprContourSchema::GetSilhouetteWidth() {
  return _GetTypedDataSource<HdFloatArrayDataSource>(UsdNprContourSchemaTokens->silhouetteWidth);
}

HdFloatArrayDataSourceHandle UsdNprContourSchema::GetBoundaryWidth() {
  return _GetTypedDataSource<HdFloatArrayDataSource>(UsdNprContourSchemaTokens->boundaryWidth);
}

HdFloatArrayDataSourceHandle UsdNprContourSchema::GetCreaseWidth() {
  return _GetTypedDataSource<HdFloatArrayDataSource>(UsdNprContourSchemaTokens->creaseWidth);
}

HdFloatArrayDataSourceHandle UsdNprContourSchema::GetCreaseAngle() {
  return _GetTypedDataSource<HdFloatArrayDataSource>(UsdNprContourSchemaTokens->creaseAngle);
}

HdFloatArrayDataSourceHandle UsdNprContourSchema::GetSubsetWidth() {
  return _GetTypedDataSource<HdFloatArrayDataSource>(UsdNprContourSchemaTokens->subsetWidth);
}

HdPathArrayDataSourceHandle UsdNprContourSchema::GetViewPoint() {
	return _GetTypedDataSource<HdPathArrayDataSource>(UsdNprContourSchemaTokens->viewPoint);
}

HdPathArrayDataSourceHandle UsdNprContourSchema::GetTargets() {
	return _GetTypedDataSource<HdPathArrayDataSource>(UsdNprContourSchemaTokens->targets);
}

HdContainerDataSourceHandle UsdNprContourSchema::BuildRetained(
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
	const HdPathArrayDataSourceHandle& targets) {

	TfToken names[11];
	HdDataSourceBaseHandle values[11];

	size_t count = 0;
	if (drawSilhouette) {
		names[count] = UsdNprContourSchemaTokens->drawSilhouette;
		values[count++] = drawSilhouette;
	}
	if (drawBoundary) {
		names[count] = UsdNprContourSchemaTokens->drawBoundary;
		values[count++] = drawBoundary;
	}
	if (drawCrease) {
		names[count] = UsdNprContourSchemaTokens->drawCrease;
		values[count++] = drawCrease;
	}
	if (drawSubset) {
		names[count] = UsdNprContourSchemaTokens->drawSubset;
		values[count++] = drawSubset;
	}
	if (silhouetteWidth) {
		names[count] = UsdNprContourSchemaTokens->silhouetteWidth;
		values[count++] = silhouetteWidth;
	}
	if (boundaryWidth) {
		names[count] = UsdNprContourSchemaTokens->boundaryWidth;
		values[count++] = boundaryWidth;
	}
	if (creaseWidth) {
		names[count] = UsdNprContourSchemaTokens->creaseWidth;
		values[count++] = creaseWidth;
	}
	if (creaseAngle) {
		names[count] = UsdNprContourSchemaTokens->creaseAngle;
		values[count++] = creaseAngle;
	}
	if (subsetWidth) {
		names[count] = UsdNprContourSchemaTokens->subsetWidth;
		values[count++] = subsetWidth;
	}
	if (viewPoint) {
		names[count] = UsdNprContourSchemaTokens->viewPoint;
		values[count++] = viewPoint;
	}
	if (targets) {
		names[count] = UsdNprContourSchemaTokens->targets;
		values[count++] = targets;
	}
	
	return HdRetainedContainerDataSource::New(count, names, values);
}

const TfToken& UsdNprContourSchema::GetSchemaToken() {
	return UsdNprContourSchemaTokens->contour;
}

UsdNprContourSchema
UsdNprContourSchema::GetFromParent(const HdContainerDataSourceHandle& parent) {    
	return UsdNprContourSchema(
		parent 
		? HdContainerDataSource::Cast(parent->Get(UsdNprContourSchemaTokens->contour))
		: nullptr);
}

const HdDataSourceLocator& UsdNprContourSchema::GetDefaultLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetDrawSilhouetteLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->drawSilhouette);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetDrawBoundaryLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->drawBoundary);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetDrawCreaseLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->drawCrease);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetDrawSubsetLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->drawSubset);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetSilhouetteWidthLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->silhouetteWidth);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetBoundaryWidthLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->boundaryWidth);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetCreaseWidthLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->creaseWidth);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetCreaseAngleLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->creaseAngle);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetSubsetWidthLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->subsetWidth);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetViewPointLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->viewPoint);
	return locator;
}

const HdDataSourceLocator& UsdNprContourSchema::GetTargetsLocator() {
	static const HdDataSourceLocator locator(
		UsdNprContourSchemaTokens->contour,
		UsdNprContourSchemaTokens->targets);
	return locator;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetDrawSilhouette(const HdBoolArrayDataSourceHandle& drawSilhouette) {
	_drawSilhouette = drawSilhouette;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetDrawBoundary(const HdBoolArrayDataSourceHandle& drawBoundary) {
	_drawBoundary = drawBoundary;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetDrawCrease(const HdBoolArrayDataSourceHandle& drawCrease) {
	_drawCrease = drawCrease;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetDrawSubset(const HdBoolArrayDataSourceHandle& drawSubset) {
	_drawSubset = drawSubset;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetSilhouetteWidth(const HdFloatArrayDataSourceHandle& silhouetteWidth) {
	_silhouetteWidth = silhouetteWidth;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetBoundaryWidth(const HdFloatArrayDataSourceHandle& boundaryWidth) {
	_boundaryWidth = boundaryWidth;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetCreaseWidth(const HdFloatArrayDataSourceHandle& creaseWidth) {
	_creaseWidth = creaseWidth;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetCreaseAngle(const HdFloatArrayDataSourceHandle& creaseAngle) {
	_creaseAngle = creaseAngle;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetSubsetWidth(const HdFloatArrayDataSourceHandle& subsetWidth) {
	_subsetWidth = subsetWidth;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetViewPoint(const HdPathArrayDataSourceHandle& viewPoint) {
	_viewPoint = viewPoint;
	return *this;
}

UsdNprContourSchema::Builder&
UsdNprContourSchema::Builder::SetTargets(const HdPathArrayDataSourceHandle& targets) {
	_targets = targets;
	return *this;
}

HdContainerDataSourceHandle UsdNprContourSchema::Builder::Build() {
	return UsdNprContourSchema::BuildRetained(
		_drawSilhouette,
		_drawBoundary,
		_drawCrease,
		_drawSubset,
		_silhouetteWidth,
		_boundaryWidth,
		_creaseWidth,
		_creaseAngle,
		_subsetWidth,
		_viewPoint,
		_targets
	);
}


PXR_NAMESPACE_CLOSE_SCOPE