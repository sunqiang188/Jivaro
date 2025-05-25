#include <sstream>
#include <iomanip>

#include <pxr/usd/sdf/fileFormat.h>
#include "pxr/base/plug/registry.h"
#include "pxr/base/tf/staticTokens.h"

#include "pxr/usd/usd/attribute.h"
#include "pxr/usd/usd/attributeQuery.h"
#include "pxr/usd/usd/modelAPI.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/schemaBase.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdGeom/imageable.h"
#include "pxr/usd/usdGeom/xformCommonAPI.h"


#include "../acceleration/bvh.h"
#include "../app/utils.h"
#include "../pbd/solver.h"

JVR_NAMESPACE_OPEN_SCOPE

namespace Utils {
std::string FindNextAvailableTokenString(std::string prefix) {
    // Find number in the prefix
    size_t end = prefix.size() - 1;
    while (end > 0 && std::isdigit(prefix[end])) {
        end--;
    }
    size_t padding = prefix.size() - 1 - end;
    const std::string number = prefix.substr(end + 1, padding);
    auto value = number.size() ? std::stoi(number) : 0;
    std::ostringstream newName;
    padding = padding == 0 ? 4 : padding; // 4: default padding
    do {
        value += 1;
        newName.seekp(0, std::ios_base::beg); // rewind
        newName << prefix.substr(0, end + 1) << std::setfill('0') << std::setw(padding) << value;
        // Looking for existing token with the same name.
        // There might be a better solution here
    } while (TfToken::Find(newName.str()) != TfToken());
    return newName.str();
}

const std::vector<std::string> GetUsdValidExtensions() {
    const auto usdExtensions = SdfFileFormat::FindAllFileFormatExtensions();
    std::vector<std::string> validExtensions;
    auto addDot = [](const std::string &str) { return "." + str; };
    std::transform(usdExtensions.cbegin(), usdExtensions.cend(), std::back_inserter(validExtensions), addDot);
    return validExtensions;
}

/*static*/
std::vector<UsdPrim> 
_GetAllPrimsOfType(UsdStagePtr const &stage, 
                                  TfType const& schemaType)
{
    std::vector<UsdPrim> result;
    UsdPrimRange range = stage->Traverse();
    std::copy_if(range.begin(), range.end(), std::back_inserter(result),
                 [schemaType](UsdPrim const &prim) {
                     return prim.IsA(schemaType);
                 });
    return result;
}

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (root)
);

PrimInfo::PrimInfo(const UsdPrim &prim, const UsdTimeCode time)
{
    hasCompositionArcs = (prim.HasAuthoredReferences() ||
                          prim.HasAuthoredPayloads() ||
                          prim.HasAuthoredInherits() ||
                          prim.HasAuthoredSpecializes() ||
                          prim.HasVariantSets());
    isActive = prim.IsActive();
    UsdGeomImageable img(prim);
    isImageable = static_cast<bool>(img);
    isDefined = prim.IsDefined();
    isAbstract = prim.IsAbstract();

    // isInPrototype is meant to guide UI to consider the prim's "source", so
    // even if the prim is a proxy prim, then unlike the core
    // UsdPrim.IsInPrototype(), we want to consider it as coming from a
    // prototype to make it visually distinctive.  If in future we need to
    // decouple the two concepts we can, but we're sensitive here to python
    // marshalling costs.
    isInPrototype = prim.IsInPrototype() || prim.IsInstanceProxy();


    // only show camera guides for now, until more guide generation logic is
    // moved into usdImaging
    supportsGuides = prim.IsA<UsdGeomCamera>();

    supportsDrawMode = isActive && isDefined && 
        !isInPrototype && prim.GetPath() != SdfPath::AbsoluteRootPath() &&
        UsdModelAPI(prim).IsModel();

    isInstance = prim.IsInstance();
    isVisibilityInherited = false;
    if (img){
        UsdAttributeQuery query(img.GetVisibilityAttr());
        TfToken visibility = UsdGeomTokens->inherited;
        query.Get(&visibility, time);
        isVisibilityInherited = (visibility == UsdGeomTokens->inherited);
        visVaries = query.ValueMightBeTimeVarying();
    }
    else {
        visVaries = false;
    }

    if (prim.GetParent())
        name = prim.GetName().GetString();
    else
        name = _tokens->root.GetString();
    typeName = prim.GetTypeName().GetString();

    displayName = prim.GetDisplayName();
}

/*static*/
PrimInfo
GetPrimInfo(const UsdPrim &prim, const UsdTimeCode time)
{
    return PrimInfo(prim, time);
}

bool HasXformOpSamples(const pxr::UsdPrim& prim, const TfToken& opName) {
    pxr::UsdGeomXformable xformable(prim);

    bool tempResetXformStack;
    std::vector<pxr::UsdGeomXformOp> ops = xformable.GetOrderedXformOps(&tempResetXformStack);
    for (const auto& op : ops) {
      if(op.GetName() == opName) {
        pxr::UsdAttribute attr = op.GetAttr();
        std::vector<double> timeSamples;
        return (attr.GetTimeSamples(&timeSamples) && !timeSamples.empty());
      }
    }
    return false;
}

bool HasCommonXformOps(const UsdGeomXformable& xformable)
{
  bool tempResetXformStack;
  std::vector<UsdGeomXformOp> xformOps =
      xformable.GetOrderedXformOps(&tempResetXformStack);
  if (xformOps.size() > 5)
      return false;

  // The expected order is:
  // ["xformOp:translate", "xformOp:translate:pivot", "xformOp:rotateABC",
  //  "xformOp:scale", "!invert!xformOp:translate:pivot"]
  auto it = xformOps.begin();

  // This holds the computed attribute name tokens so that we can avoid
  // hard-coding them.
  // The name for the rotate op is not computed here because it can vary.
  static const struct {
    TfToken translate = UsdGeomXformOp::GetOpName(
      UsdGeomXformOp::TypeTranslate);
    TfToken pivot = UsdGeomXformOp::GetOpName(
      UsdGeomXformOp::TypeTranslate, UsdGeomTokens->pivot);
    TfToken scale = UsdGeomXformOp::GetOpName(
      UsdGeomXformOp::TypeScale);
  } attrNames;

  // Search one-by-one for the ops in the correct order.
  // We can skip ops in the "expected" order (that is, all the common ops are
  // optional) but we can't skip ops in the "actual" order (that is, extra ops
  // aren't allowed).
  //
  // Note, in checks below, avoid using UsdGeomXformOp::GetOpName() because
  // it will construct strings in the case of an inverted op.
  UsdGeomXformOp t;
  if (it != xformOps.end() && it->GetName() == attrNames.translate && !it->IsInverseOp())
    ++it;

  UsdGeomXformOp p;
  if (it != xformOps.end() && it->GetName() == attrNames.pivot && !it->IsInverseOp())
    ++it;

  UsdGeomXformOp r;
  if (it != xformOps.end() && UsdGeomXformCommonAPI::CanConvertOpTypeToRotationOrder(it->GetOpType()) &&
        !it->IsInverseOp())
      ++it;

  UsdGeomXformOp s;
  if (it != xformOps.end() && it->GetName() == attrNames.scale && !it->IsInverseOp())
    ++it;

  UsdGeomXformOp pInv;
  if (it != xformOps.end() && it->GetName() == attrNames.pivot && it->IsInverseOp())
    ++it;

  // If we did not reach the end of the xformOps vector, then there were
  // extra ops that did not match any of the expected ops.
  // This means that the xformOps vector isn't XformCommonAPI-compatible.
  if (it != xformOps.end())
    return false;

  // Verify that translate pivot and inverse translate pivot are either both 
  // present or both absent.
  if ((bool) p != (bool) pInv)
    return false;

  return true;
}

} // namespace Utils
JVR_NAMESPACE_CLOSE_SCOPE