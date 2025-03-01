#include "../geometry/geometry.h"
#include "../geometry/implicit.h"
#include "../geometry/deformable.h"
#include "../pbd/contact.h"
#include "../pbd/particle.h"
#include "../pbd/collision.h"


JVR_NAMESPACE_OPEN_SCOPE

void Contact::Init(Collision* collision, Particles* particles, size_t index)
{
  _normal = collision->GetGradient(particles, index);
  _initDepth = collision->GetValue(particles, index);
  _depth = _initDepth;
  if(collision->GetTypeId() != Collision::SELF) 
    _velocity = collision->GetVelocity(particles, index);
  else
    _velocity = ((SelfCollision*)collision)->GetVelocity(particles, index, GetComponentIndex());
  _rotationAlongFrame = GfRotation();

/*
  Geometry* geometry = collision->GetGeometry();
  switch(geometry->GetType()) {
    case Collision::PLANE:
    {
      Plane* plane = (Plane*)geometry;
      const GfVec3f position = plane->GetOrigin();
      _normal = plane->GetNormal();
      _initDepth = GfDot(_normal, point - position)  - radius;
      _depth = _initDepth;
    }
    case Geometry::BOX:
    {
      Cube* cube = (Cube*)geometry;

      const GfVec3d scale = _collider->GetScale();
      const float scaleFactor = (scale[0] + scale[1] + scale[2]) / 3.f + 1e-9;
      
      const GfVec3f local(_collider->GetInverseMatrix().Transform(point));
      
      const GfVec3f closest = _PointOnBox(local, _size, _collider->GetMatrix());

      _normal = (point - closest).GetNormalized();
      _initDepth = cube->SignedDistance(predicted) - radius / scaleFactor;
      _depth = _initDepth;
      
    }
    case Geometry::MESH:
    {
      Mesh* mesh = (Mesh*)geometry;
      const GfVec3f* positions = mesh->GetPositionsCPtr();
      const GfVec3f* previous = mesh->GetPreviousCPtr();
      const GfVec3f* normals = mesh->GetNormalsCPtr();

      const Triangle* triangle = mesh->GetTriangle(GetComponentIndex());

      const GfVec3f position = ComputePosition(positions, &triangle->vertices[0], 3, &mesh->GetMatrix());
      const GfVec3f next = ComputePredictedPosition(positions, previous, 1.f, &triangle->vertices[0], 3, &mesh->GetMatrix());

      _prevNormal = ComputeInterpolatedNormal(normals, positions, previous, 0.f, &triangle->vertices[0], 3, &mesh->GetMatrix());
      _normal = ComputeInterpolatedNormal(normals, positions, previous, 1.f, &triangle->vertices[0], 3, &mesh->GetMatrix());

      _rotationAlongFrame = GfRotation(_prevNormal,  _normal);

      _normal = _prevNormal;

      _initDepth = GfDot(point - position, _normal)  - radius;
      
      const GfVec3f intersection = position + _normal * _initDepth;

      SetPoint(intersection);
      SetDistance(_initDepth);
      _velocity = mesh->GetTriangleVelocity(GetComponentIndex());
    }
  }
*/
  _touching = _depth <= 0.0;
  
}

void Contact::Update(Collision* collision, Particles* particles, size_t index)
{
  _normal = collision->GetGradient(particles, index);
  _depth = collision->GetValue(particles, index);
  /*
  if(collision->GetTypeId() != Collision::SELF) 
    _velocity = collision->GetVelocity(particles, index);
    */

}


void Contacts::Resize(size_t N, size_t M) {
  if(data && n == N && m == M){ResetAllUsed(); return;}
  else if(data) {delete [] data; data=nullptr; delete [] used; used=nullptr;}

  if(N) {
    n = N;
    m = M;
    data = new Contact[n * m];
    used = new int[n];
    ResetAllUsed();
  }

};

void Contacts::ResetUsed(size_t index)
{
  used[index] = 0;
}

void 
Contacts::ResetAllUsed() { 
  memset(&used[0], 0, n * sizeof(int));
};

Contact* 
Contacts::Use(size_t index) {
  size_t available = used[index];
  used[index]++;
  return &data[index * m + available];
}

Contact*
Contacts::LastUsed(size_t index){
  if(used[index] > 0)
    return &data[index * m + used[index] - 1];
  return nullptr;
}

size_t 
Contacts::GetNumUsed(size_t index) const
{
  return used[index];
}

size_t 
Contacts::GetTotalNumUsed() const {
  size_t numContacts = 0;
  for(size_t x=0; x < n; ++x) numContacts += used[x];
  return numContacts;
}

JVR_NAMESPACE_CLOSE_SCOPE