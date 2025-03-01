#include "../geometry/location.h"


JVR_NAMESPACE_OPEN_SCOPE

//=================================================================================================
// LOCATION CLASS
//=================================================================================================
void 
Location::Set(const Location& other) {
  _geomId = other._geomId;
  _compId = other._compId;
  _coords = other._coords;
  _point  = other._point;
}

GfVec3f 
Location::ComputePosition(const GfVec3f* positions, const int* elements, size_t sz,
 const GfMatrix4d* m) const 
{
  GfVec3f result(0.f);
  if(elements)
    for(size_t d = 0; d < sz; ++d) 
      result += positions[elements[d]] * _coords[d];
  else 
    result += positions[_compId];

  if(m)return GfVec3f(m->Transform(result));
  else return result;
}

GfVec3f
Location::ComputeNormal(const GfVec3f* normals, const int* elements, size_t sz, 
  const GfMatrix4d* m) const
{
  GfVec3f result(0.f);
  if (elements)
    for (size_t d = 0; d < sz; ++d)
      result += normals[elements[d]] * _coords[d];
  else
    result += normals[_compId];

  if(m)return GfVec3f(m->TransformDir(result).GetNormalized());
  else return result.GetNormalized();
}

GfVec3f
Location::ComputeVelocity(const GfVec3f* positions, const GfVec3f* previous,
  const int* elements, size_t sz, const GfMatrix4d* m) const
{
  GfVec3f result(0.f);
  if (elements)
    for (size_t d = 0; d < sz; ++d)
      result += (positions[elements[d]] - previous[elements[d]]) * _coords[d];
  else
    result += positions[_compId] - previous[_compId];

  if(m)return GfVec3f(m->TransformDir(result));
  else return result;
}

GfVec3f 
Location::ComputeInterpolatedPosition(const GfVec3f* positions, const GfVec3f* previous, float t, 
  const int* elements, size_t sz, const GfMatrix4d* m) const 
{
  GfVec3f result(0.f);
  if(elements)
    for(size_t d = 0; d < sz; ++d) 
      result += (previous[elements[d]] * (1.f - t) + positions[elements[d]] * t) * _coords[d];
      
  else {
    result += (previous[_compId] * (1.f - t) + positions[_compId] * t);
  }

  if(m)return GfVec3f(m->Transform(result));
  else return result;

}

GfVec3f 
Location::ComputeInterpolatedNormal(const GfVec3f* normals, const GfVec3f* positions, const GfVec3f* previous, float t, 
  const int* elements, size_t sz, const GfMatrix4d* m) const 
{
  GfVec3f result(0.f);
  if(sz == 3){
    GfVec3f cur0 = positions[elements[1]] - positions[elements[0]];
    GfVec3f cur1 = positions[elements[2]] - positions[elements[0]];

    GfVec3f prev0 = previous[elements[1]] - previous[elements[0]];
    GfVec3f prev1 = previous[elements[2]] - previous[elements[0]];

    GfRotation rotation = GfRotation((cur0 ^ cur1), (prev0 ^ prev1));

    GfVec3f normal = normals[elements[0]] * _coords[0] + normals[elements[1]] * _coords[1] + normals[elements[2]] * _coords[2];
    GfVec3f prevNormal(rotation.TransformDir(normal.GetNormalized()));

    result = GfSlerp(t, prevNormal, normal).GetNormalized();
  }
  
  if(m)return GfVec3f(m->Transform(result));
  else return result;
}




PXR_NAMESPACE_CLOSE_SCOPE
