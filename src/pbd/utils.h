#ifndef JVR_PBD_UTILS_H
#define JVR_PBD_UTILS_H



#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/matrix3d.h>
#include <pxr/base/gf/quatd.h>
#include <pxr/base/gf/transform.h>

#include "../common.h"


JVR_NAMESPACE_OPEN_SCOPE

/*
void ExtractRotation(const GfMatrix3d &A, GfQuaternion &q,
  const unsigned int maxIter)
{
  for (unsigned int iter = 0; iter < maxIter; ++iter)
  {
    GfMatrix3d R = q.Matrix();
    GfVec3d omega = 
      (R.GetColumn(0) ^ A.GetColumn(0) + 
       R.GetColumn(1) ^ A.GetColumn(1) + 
       R.GetColumn(2) ^ A.GetColumn(2) 
      ) * (
        1.0 / GfAbs(
          GfDot(R.GetColumn(0), A.GetColumn(0)) + 
          GfDot(R.GetColumn(1), A.GetColumn(1)) + 
          GfDot(R.GetColumn(2), A.GetColumn(2))) + 1.0e-9);
    double w = omega.GetLength();
    if (w < 1.0e-9) break;
    q = GfQuaterniond(AngleAxisd(w, (1.0 / w) * omega)) * q;
    q.Normalize();
  }
}
*/

GfMatrix4d InterpolateMatrices(const GfMatrix4d& m1, const GfMatrix4d& m2, float t)
{
  GfTransform x1(m1);
  GfTransform x2(m2);

  GfVec3d t1(x1.GetTranslation());
  GfVec3d t2(x2.GetTranslation());

  GfVec3d s1(x1.GetScale());
  GfVec3d s2(x2.GetScale());

  GfQuaternion r1(x1.GetRotation().GetQuaternion());
  GfQuaternion r2(x2.GetRotation().GetQuaternion());

  GfVec3d translation = (1.f - t) * t1 + t * t2;
  GfVec3d scale = (1.f - t) * s1 + t * s2;
  GfQuaternion rotation = pxr::GfSlerp(r1, r2, t);

  GfTransform result;
  result.SetTranslation(translation);
  result.SetScale(scale);
  result.SetRotation(rotation);

  return result.GetMatrix();
}

JVR_NAMESPACE_CLOSE_SCOPE

#endif // JVR_PBD_UTILS_H