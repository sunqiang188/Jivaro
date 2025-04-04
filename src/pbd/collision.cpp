
#include <pxr/base/work/loops.h>

#include "../utils/color.h"
#include "../geometry/geometry.h"
#include "../geometry/implicit.h"
#include "../geometry/mesh.h"
#include "../geometry/points.h"
#include "../acceleration/bvh.h"
#include "../acceleration/hashGrid.h"
#include "../pbd/utils.h"
#include "../pbd/collision.h"
#include "../pbd/particle.h"
#include "../pbd/contact.h"
#include "../pbd/solver.h"
#include "../pbd/constraint.h"

#include <usdPbd/collisionAPI.h>

JVR_NAMESPACE_OPEN_SCOPE

const size_t Collision::PACKET_SIZE = 64;
const float Collision::TOLERANCE_MARGIN = 0.01f;


void Collision::Reset()
{
  _flip = false;
  _contacts.ResetAllUsed();
}

void Collision::GetPoints(Particles* particles, VtArray<GfVec3f>& points, 
  VtArray<float>& radius, VtArray<GfVec3f>& colors)
{
  Mask::Iterator iterator(this, 0, particles->GetNumParticles());
  const GfVec3f color(RANDOM_0_1, RANDOM_0_1, RANDOM_0_1);
  const float r = 0.05f;
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next())
    if(_contacts.IsUsed(index)) {
      Contact* contact = _contacts.Get(index);
      points.push_back(GfVec3f(contact->GetPoint()));
      colors.push_back(color);
      radius.push_back(r);
    }
}

void Collision::GetNormals(Particles* particles, VtArray<GfVec3f>& points, 
  VtArray<float>& radius, VtArray<GfVec3f>& colors, VtArray<int>& counts)
{
  Mask::Iterator iterator(this, 0, particles->GetNumParticles());
  const GfVec3f color(RANDOM_0_1, RANDOM_0_1, RANDOM_0_1);
  const float r = 0.05f;
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next())
    if(_contacts.IsUsed(index)) {
      Contact* contact = _contacts.Get(index);
      if(contact) {
        const GfVec3f position = GfVec3f(contact->GetPoint());
        points.push_back(position);
        points.push_back(position + GfVec3f(contact->GetNormal()));
        colors.push_back(color);
        colors.push_back(color);
        radius.push_back(r);
        radius.push_back(r);
        counts.push_back(2);
      }
    }
}

void Collision::GetVelocities(Particles* particles, VtArray<GfVec3f>& points, 
  VtArray<float>& radius, VtArray<GfVec3f>& colors, VtArray<int>& counts)
{
}

// 
// Contacts
//
void Collision::UpdateContacts(Particles* particles, float t)
{
  _t = t;
  WorkParallelForN(
      particles->GetNumParticles(),
      std::bind(&Collision::_UpdateContacts, this, particles, 
        std::placeholders::_1, std::placeholders::_2), PACKET_SIZE);
}

void Collision::_UpdateContacts(Particles* particles, size_t begin, size_t end)
{
  Mask::Iterator iterator(this, begin, end);
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next())
    if(_contacts.IsActive(index))
      _contacts.Get(index)->Update(this, particles, index);
}

void Collision::FindContacts(Particles* particles, float ft)
{
  if(!_enabled)return; 
  WorkParallelForN(particles->GetNumParticles(),
    std::bind(&Collision::_FindContacts, this, particles,
      std::placeholders::_1, std::placeholders::_2, ft), PACKET_SIZE);
}

void Collision::_FindContacts(Particles* particles, size_t begin, size_t end, float ft)
{
  Mask::Iterator iterator(this, begin, end);
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next()) {
    Contact* contact = _contacts.Get(index);
    //if(!contact->IsTouching() || (contact->GetPoint() - particles->position[index]).GetLength() > _margin)
    _FindContact(particles, index, ft);
  }
}


void Collision::_BuildContacts(Particles* particles, const std::vector<Body*>& bodies,
  std::vector<Constraint*>& constraints)
{
  Constraint* constraint = NULL;

  VtArray<int> elements;
  Body* currentBody = nullptr;
  Mask::Iterator iterator(this, 0, particles->GetNumParticles());
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next()) {

    if (particles->body[index] != currentBody || elements.size() >= Constraint::BlockSize) {
      if (elements.size()) {
        constraint = new CollisionConstraint(currentBody, this, elements, _stiffness, _damp);
        constraints.push_back(constraint);
        elements.clear();
      }
      currentBody = particles->body[index];
    } 
    elements.push_back(index);
  } 
  
  if (elements.size()) {
    constraint = new CollisionConstraint(currentBody, this, elements);
    constraints.push_back(constraint);
  }
}

void 
Collision::CreateContactConstraints(Particles* particles, const std::vector<Body*>& bodies,
    std::vector<Constraint*>& constraints)
{
  /*
  Constraint* constraint = nullptr;
  if(_contacts.GetTotalNumUsed()) {
    if(GetTypeId() == Collision::MESH) {
      const Mesh* mesh = (const Mesh*)_collider;
      const GfVec3f* positions = mesh->GetPositionsCPtr();
      const GfVec3f* normals = mesh->GetNormalsCPtr();

      VtArray<int> elements;
      VtArray<Contact*> contacts;

      Mask::Iterator iterator(this, 0, particles->GetNumParticles());
      size_t index = iterator.Begin();
      for (; index != Mask::INVALID_INDEX; index = iterator.Next()) {
        if(!_contacts.GetNumUsed(index)) continue;
        
        Contact* contact = _contacts.Use(index);
        
        if(contact->IsTouching()) {
          //const GfVec3f velocity = mesh->GetTriangleVelocity(contact->GetComponentIndex());
          //particles->position[p] = desired;
          //particles->predicted[p] = desired;
          contact->Update(this, particles, index);
          elements.push_back(index);
          contacts.push_back(contact);
          particles->color[index] = GfVec3f(1.f, 0.f, 0.f);
        }  
        else
          particles->color[index] = GfVec3f(0.5f, 0.5f, 0.5f);

        if ((elements.size() >= Constraint::BlockSize) || iterator.End()) {
          if (elements.size()) {
            constraint = new ContactConstraint(particles->body[index], elements, this, contacts, _stiffness, _damp);
            constraints.push_back(constraint);
            elements.clear();
            contacts.clear();
          } 
        }
      }
    }
    // TODO implement other collision types
    
  }


  
  CollisionConstraint* constraint = NULL;
  size_t numParticles = particles->GetNumParticles();

  size_t numContacts = _contacts.GetTotalNumUsed();

  VtArray<int> elements;
 
  size_t contactsOffset = 0;
  size_t contactIdx = 0;
  Mask::Iterator iterator(this, 0, numParticles);
  size_t index = iterator.Begin();
  for (; index != Mask::INVALID_INDEX; index = iterator.Next()) {
    size_t numUsed = _contacts.GetNumUsed(index);
    if(numUsed) {
      elements.push_back(index);      
    } 
    
    if ((elements.size() >= Constraint::BlockSize) || iterator.End()) {
      if (elements.size()) {
        constraint = new CollisionConstraint(particles, this, elements);
        constraints.push_back(constraint);
        elements.clear();
      } 
    }
  }
  */
}


// 
// Init
//
void Collision::Init(Particles* particles, const std::vector<Body*>& bodies,
  std::vector<Constraint*>& constraints) 
{
  std::cout << "Collision INITIALIZE : " << this << ":" << particles->GetNumParticles() << std::endl;
  _contacts.Resize(particles->GetNumParticles(), 1);
  _contacts.ResetAllUsed();

  _BuildContacts(particles, bodies, constraints);
  std::cout << "created  : " << constraints.size() << " constraints " << std::endl;

}


// 
// Update
//
void Collision::Update(const UsdPrim& prim, double time){}

void Collision::_UpdateParameters(const UsdPrim& prim, double time)
{
  UsdPbdCollisionAPI api(prim);
  api.GetCollisionEnabledAttr().Get(&_enabled, time);
  api.GetRestitutionAttr().Get(&_restitution, time);
  api.GetFrictionAttr().Get(&_friction, time);
  api.GetDampAttr().Get(&_damp, time);
  api.GetMarginAttr().Get(&_margin, time);
  api.GetMaxSeparationVelocityAttr().Get(&_maxSeparationVelocity, time);
  api.GetCollisionStiffnessAttr().Get(&_stiffness, time);
}


// Velocity
GfVec3f Collision::GetVelocity(Particles* particles, size_t index)
{  

  const GfVec3f torque = _collider->GetTorque();
  const GfVec3f tangent =
    (GetGradient(particles, index) ^ torque).GetNormalized();

  return _collider->GetVelocity() + tangent * torque.GetLength();
};

size_t Collision::GetContactComponent(size_t index, size_t c) const
{
  return _contacts.Get(index, c)->GetComponentIndex();
}

GfVec3f Collision::GetContactNormal(size_t index,size_t c) const 
{
  return _contacts.Get(index, c)->GetNormal();
}

GfVec3f Collision::GetContactVelocity(size_t index, size_t c) const 
{
  return _contacts.Get(index, c)->GetVelocity();
}

float Collision::GetContactDepth(size_t index, size_t c) const
{
  return _contacts.Get(index, c)->GetDepth();
}

float Collision::GetContactInitDepth(size_t index, size_t c) const
{
  return _contacts.Get(index, c)->GetInitDepth();
}

void Collision::SetContactTouching(size_t index, bool touching, size_t c)
{
  _contacts.Get(index, c)->SetTouching(touching);
}

bool Collision::IsContactTouching(size_t index, size_t c) const
{
  return _contacts.Get(index, c)->IsTouching();
}

void Collision::SetContactActive(size_t index, bool active, size_t c)
{
  _contacts.Get(index, c)->SetActive(active);
}

bool Collision::IsContactActive(size_t index, size_t c) const
{
  return _contacts.Get(index, c)->IsActive();
}


//----------------------------------------------------------------------------------------
// Plane Collision
//----------------------------------------------------------------------------------------
size_t PlaneCollision::TYPE_ID = Collision::PLANE;

PlaneCollision::PlaneCollision(Geometry* collider, const SdfPath& path, 
  float restitution, float friction) 
  : Collision(collider, path, restitution, friction)
{
  Plane* plane = (Plane*)collider;
  _UpdatePositionAndNormal();
}

bool PlaneCollision::Compute(Particles* particles, size_t index, Contact& contact)
{
  contact.SetPoint(_prevPosition * (1.f - _t) + _position * _t);
  contact.SetNormal(GfSlerp(_t, _prevNormal, _normal));
  contact.SetDistance(GfDot(_normal, particles->predicted[index] - _position) -(particles->radius[index]));

  return contact.GetDistance() < _margin;
}

float PlaneCollision::GetValue(Particles* particles, size_t index)
{
  GfVec3f position = _prevPosition * (1.f - _t) + _position * _t;
  GfVec3f normal = GfSlerp(_t, _prevNormal, _normal);

  return GfDot(normal, particles->predicted[index] - position) -(particles->radius[index]);
}

GfVec3f PlaneCollision::GetGradient(Particles* particles, size_t index)
{
  return GfSlerp(_t, _prevNormal, _normal);
}

void PlaneCollision::Update(const UsdPrim& prim, double time) 
{
  _UpdatePositionAndNormal();
  _UpdateParameters(prim, time);
}

void PlaneCollision::_UpdatePositionAndNormal()
{
  Plane* plane = (Plane*)_collider;
  _position =  plane->GetOrigin();
  _normal = plane->GetNormal();

  const GfMatrix4d& invMatrix(_collider->GetInverseMatrix());
  const GfMatrix4d& prevMatrix(_collider->GetPreviousMatrix());
  const GfMatrix4d deltaMatrix = prevMatrix * invMatrix;

  _prevPosition = GfVec3f(deltaMatrix.Transform(_position));
  _prevNormal = GfVec3f(deltaMatrix.TransformDir(_normal));

}

void PlaneCollision::_FindContact(Particles* particles, size_t index, float ft)
{
  const GfVec3f predicted(particles->predicted[index] + particles->velocity[index] * ft);
  float d = GfDot(_normal, predicted - _position)  - particles->radius[index];
  _contacts.Get(index)->SetActive(d < _margin);
}

//----------------------------------------------------------------------------------------
// Box Collision
//----------------------------------------------------------------------------------------
size_t BoxCollision::TYPE_ID = Collision::BOX;

BoxCollision::BoxCollision(Geometry* collider, const SdfPath& path, 
  float restitution, float friction)
  : Collision(collider, path, restitution, friction)
{
  _UpdateSize();
}

void BoxCollision::Update(const UsdPrim& prim, double time)
{
  _UpdateSize();
  _UpdateParameters(prim, time);
}

void BoxCollision::_UpdateSize()
{
  Cube* cube = (Cube*)_collider;
  _size = cube->GetSize();
} 


GfVec3f _PointOnBox(const GfVec3f& local, double size, const GfMatrix4d& m)
{
  const double halfSize = size * 0.5f;
  const double xDiff = GfAbs(local[0]) - halfSize;
  const double yDiff = GfAbs(local[1]) - halfSize;
  const double zDiff = GfAbs(local[2]) - halfSize;

  GfVec3f normal;
  double d;
  if(xDiff <= yDiff && xDiff <= zDiff) {
    d = xDiff;
    normal = local[0] > 0.f ? GfVec3f(-1.f, 0.f, 0.f) : GfVec3f(1.f, 0.f, 0.f);
  } else if(yDiff <= zDiff) {
    d = yDiff;
    normal = local[1] > 0.f ? GfVec3f(0.f, -1.f, 0.f) : GfVec3f(0.f, 1.f, 0.f);
  } else {
    d = zDiff;
    normal = local[2] > 0.f ? GfVec3f(0.f, 0.f, -1.f) : GfVec3f(0.f, 0.f, 1.f);
  }

  return GfVec3f(m.Transform(normal * d));
}

void BoxCollision::_FindContact(Particles* particles, size_t index, float ft)
{
  const GfVec3f velocity = particles->velocity[index] * ft;
  const GfVec3f predicted(particles->predicted[index] + velocity);
  Cube* cube = (Cube*) _collider;

  const GfVec3d scale = _collider->GetScale();
  const float scaleFactor = (scale[0] + scale[1] + scale[2]) / 3.f + 1e-9;

  _contacts.Get(index)->SetActive(
    cube->SignedDistance(predicted) - particles->radius[index] / scaleFactor < Collision::TOLERANCE_MARGIN);
}

float BoxCollision::GetValue(Particles* particles, size_t index)
{
  Cube* cube = (Cube*) _collider;
  const GfVec3d scale = _collider->GetScale();
  const float scaleFactor = (scale[0] + scale[1] + scale[2]) / 3.f + 1e-9;
  return cube->SignedDistance(particles->predicted[index]) - particles->radius[index] / scaleFactor;
}
  
GfVec3f BoxCollision::GetGradient(Particles* particles, size_t index)
{
  const GfVec3f local(_collider->GetInverseMatrix().Transform(particles->predicted[index]));

  const GfVec3f closest = _PointOnBox(local, _size, _collider->GetMatrix());

  return (particles->predicted[index] - closest).GetNormalized();
}


//----------------------------------------------------------------------------------------
// Sphere Collision
//----------------------------------------------------------------------------------------
size_t SphereCollision::TYPE_ID = Collision::SPHERE;

SphereCollision::SphereCollision(Geometry* collider, const SdfPath& path, 
  float restitution, float friction)
  : Collision(collider, path, restitution, friction)
{
  Sphere* sphere = (Sphere*)collider;
  _UpdateCenterAndRadius();
}

void SphereCollision::Update(const UsdPrim& prim, double time)
{
  _UpdateCenterAndRadius();
  _UpdateParameters(prim, time);
}

void SphereCollision::_UpdateCenterAndRadius()
{
  Sphere* sphere = (Sphere*)_collider;
  _center = sphere->GetCenter();
  _radius = sphere->GetRadius();
} 


void SphereCollision::_FindContact(Particles* particles, size_t index, float ft)
{
  const GfVec3f velocity = particles->velocity[index] * ft;
  GfVec3f predicted(_collider->GetInverseMatrix().Transform(particles->predicted[index] + velocity));
  _contacts.Get(index)->SetActive(predicted.GetLength() - particles->radius[index] < _radius);
}

float SphereCollision::GetValue(Particles* particles, size_t index)
{
  Sphere* sphere = (Sphere*)_collider;

  const GfVec3d scale = _collider->GetScale();
  const float scaleFactor = (scale[0] + scale[1] + scale[2]) / 3.f + 1e-9;

  return sphere->SignedDistance(particles->predicted[index]) - particles->radius[index] / scaleFactor;
}
  
GfVec3f SphereCollision::GetGradient(Particles* particles, size_t index)
{
  return (particles->predicted[index] - _center).GetNormalized();
}


//----------------------------------------------------------------------------------------
// Capsule Collision
//----------------------------------------------------------------------------------------
size_t CapsuleCollision::TYPE_ID = Collision::CAPSULE;

CapsuleCollision::CapsuleCollision(Geometry* collider, const SdfPath& path, 
  float restitution, float friction)
  : Collision(collider, path, restitution, friction)
{
  Capsule* capsule = (Capsule*)collider;
  _UpdateRadiusAndHeight();
}

void CapsuleCollision::Update(const UsdPrim& prim, double time)
{
  _UpdateRadiusAndHeight();
  _UpdateParameters(prim, time);
}

void CapsuleCollision::_UpdateRadiusAndHeight()
{
  Capsule* capsule = (Capsule*)_collider;
  _radius = capsule->GetRadius();
  _height = capsule->GetHeight();
} 


static GfVec3f _PointOnCapsuleSegment(const GfVec3f &p, 
  const TfToken &axis, double height)
{
  GfVec3f a, b;
  if(axis == UsdGeomTokens->x) {
    a = GfVec3f(-height*0.5f, 0.f, 0.f);
    b = GfVec3f(height*0.5f, 0.f, 0.f);
  } else if(axis == UsdGeomTokens->y) {
    a = GfVec3f(0.f, -height*0.5f, 0.f);
    b = GfVec3f(0.f, height*0.5f, 0.f);
  } else if(axis == UsdGeomTokens->z) {
    a = GfVec3f(0.f, 0.f, -height*0.5f);
    b = GfVec3f(0.f, 0.f, height*0.5f);
  }

  const GfVec3f pa(p - a), ba(b - a);
  return a + ba * GfClamp(GfDot(pa, ba) / GfDot(ba, ba), 0.f, 1.f );
}
 
void CapsuleCollision::_FindContact(Particles* particles, size_t index, float ft)
{
  Capsule* capsule = (Capsule*)_collider;
  GfVec3f predicted(particles->predicted[index] + particles->velocity[index] * ft);

  const GfVec3d scale = _collider->GetScale();
  const float scaleFactor = (scale[0] + scale[1] + scale[2]) / 3.f + 1e-9;
  const float d = capsule->SignedDistance(predicted) - (particles->radius[index] + _margin) / scaleFactor;

  _contacts.Get(index)->SetActive(d < Collision::TOLERANCE_MARGIN);
}

float CapsuleCollision::GetValue(Particles* particles, size_t index)
{
  Capsule* capsule = (Capsule*)_collider;

  const GfVec3d scale = _collider->GetScale();
  const float scaleFactor = (scale[0] + scale[1] + scale[2]) / 3.f + 1e-9;

  return capsule->SignedDistance(particles->predicted[index]) - particles->radius[index] / scaleFactor;
}
  
GfVec3f CapsuleCollision::GetGradient(Particles* particles, size_t index)
{
  Capsule* capsule = (Capsule*)_collider;
  const GfVec3f local(_collider->GetInverseMatrix().Transform(particles->predicted[index]));
  const GfVec3f closest = _PointOnCapsuleSegment(local, capsule->GetAxis(), capsule->GetHeight());

  const GfVec3f surface = closest + (local - closest).GetNormalized() * _radius;
  GfVec3f world(_collider->GetMatrix().Transform(surface));

  if((local - closest).GetLengthSq() < _radius * _radius)
    return (world - particles->predicted[index]).GetNormalized();
  else
    return (particles->predicted[index] - world).GetNormalized();
}


//----------------------------------------------------------------------------------------
// Mesh Collision
//----------------------------------------------------------------------------------------
size_t MeshCollision::TYPE_ID = Collision::MESH;

MeshCollision::MeshCollision(Geometry* collider, const SdfPath& path, 
  float restitution, float friction)
  : Collision(collider, path, restitution, friction)
{
  _CreateAccelerationStructure();
}

MeshCollision::~MeshCollision()
{
}

void MeshCollision::Init(Particles* particles, const std::vector<Body*>& bodies,
  std::vector<Constraint*>& constraints)
{
  size_t numParticles = particles->GetNumParticles();
  std::cout << "Mesh Collision INITIALIZE : " << numParticles << std::endl;

  _contacts.Resize(numParticles, 1);
  _contacts.ResetAllUsed();

  _BuildContacts(particles, bodies, constraints);
  std::cout << "created  : " << constraints.size() << " constraints " << std::endl;
}

void MeshCollision::Update(const UsdPrim& prim, double time)
{
  _UpdateParameters(prim, time);
  _UpdateAccelerationStructure();
}

void MeshCollision::_CreateAccelerationStructure()
{
  _bvh.Init({_collider});
} 

void MeshCollision::_UpdateAccelerationStructure()
{
  _bvh.Update();
} 

void MeshCollision::_FindContact(Particles* particles, size_t index, float ft)
{
  Mesh* mesh = (Mesh*)_collider;
  const GfVec3f* positions = mesh->GetPositionsCPtr();
  const GfVec3f* normals = mesh->GetNormalsCPtr();

  Contact* contact = _contacts.Get(index);
  *contact = Contact();

  const GfVec3f predicted = particles->predicted[index] + particles->velocity[index] * ft;
  const float maxDistance = particles->velocity[index].GetLength() * ft + particles->radius[index] + _margin;

  GfRay ray(particles->predicted[index], particles->velocity[index] * ft);
  if(_bvh.Raycast(ray, contact, maxDistance)) {
    contact->SetActive(true);
  } else if(_bvh.Closest(predicted, contact, FLT_MAX)) {
    const Triangle* triangle = mesh->GetTriangle(contact->GetComponentIndex());

    const GfVec3f position = 
      contact->ComputePosition(positions, &triangle->vertices[0], 3, &mesh->GetMatrix());

    const GfVec3f normal = 
      contact->ComputeNormal(normals, &triangle->vertices[0], 3, &mesh->GetMatrix());

    const GfVec3f delta = predicted - position;
    contact->SetActive(GfDot(delta, normal) < 0.f || delta.GetLength() < maxDistance);
  }
}

float 
MeshCollision::GetValue(Particles* particles, size_t index)
{
  Contact* contact = _contacts.Get(index);
  if(!contact->IsValid())return 0.f;
  Mesh* mesh = (Mesh*)GetGeometry();
  const GfVec3f* positions = mesh->GetPositionsCPtr();
  const GfVec3f* previous = mesh->GetPreviousCPtr();
  const GfVec3f* normals = mesh->GetNormalsCPtr();
  
  const Triangle* triangle = mesh->GetTriangle(contact->GetComponentIndex());

  const GfVec3f position = 
    contact->ComputeInterpolatedPosition(positions, previous, _t, &triangle->vertices[0], 3, &mesh->GetMatrix());
    //contact->ComputePosition(positions, &triangle->vertices[0], 3, &mesh->GetMatrix());

  const GfVec3f normal = 
    contact->ComputeInterpolatedNormal(normals, positions, previous, _t, &triangle->vertices[0], 3, &mesh->GetMatrix());
    //contact->ComputeNormal(normals, &triangle->vertices[0], 3, &mesh->GetMatrix());

  return GfDot(particles->predicted[index] - position, normal)  - particles->radius[index];

}
  
GfVec3f 
MeshCollision::GetGradient(Particles* particles, size_t index)
{
  Contact* contact = _contacts.Get(index);
  if(!contact->IsValid())return GfVec3f(0.f);
  Mesh* mesh = (Mesh*)GetGeometry();
  const GfVec3f* positions = mesh->GetPositionsCPtr();
  const GfVec3f* previous = mesh->GetPreviousCPtr();
  const GfVec3f* normals = mesh->GetNormalsCPtr();
  const Triangle* triangle = mesh->GetTriangle(contact->GetComponentIndex());
  
  return contact->ComputeInterpolatedNormal(normals, positions, previous, _t, &triangle->vertices[0], 3, &mesh->GetMatrix());
  //return contact->ComputeNormal(normals, &triangle->vertices[0], 3, &mesh->GetMatrix());
}


GfVec3f 
MeshCollision::GetVelocity(Particles* particles, size_t index)
{
  Contact* contact = _contacts.Get(index);
  if(!contact->IsValid())return GfVec3f(0.f);
  Mesh* mesh = (Mesh*)GetGeometry();

  const GfVec3f torque = _collider->GetTorque();
  const GfVec3f tangent = (GetGradient(particles, index) ^ torque).GetNormalized();

  return _collider->GetVelocity() + tangent * torque.GetLength() +
    mesh->GetTriangleVelocity(contact->GetComponentIndex());
}

void 
MeshCollision::GetPoints(Particles* particles, VtArray<GfVec3f>& points, 
  VtArray<float>& radius, VtArray<GfVec3f>& colors)
{
  Mask::Iterator iterator(this, 0, particles->GetNumParticles());
  const GfVec3f color(1.f, 0.f, 0.f);
  const float r = 0.05f;
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next())
    if(_contacts.Get(index)->IsTouching()) {
      points.push_back(GfVec3f(_contacts.Get(index)->GetPoint()));
      colors.push_back(color);
      radius.push_back(r);
    }
}

void 
MeshCollision::GetNormals(Particles* particles, VtArray<GfVec3f>& points,
  VtArray<float>& radius, VtArray<GfVec3f>& colors, VtArray<int>& counts)
{
  Mask::Iterator iterator(this, 0, particles->GetNumParticles());
  const GfVec3f red(1.f, 0.f, 0.f);
  const float r = 0.05f;
  Mesh* mesh = (Mesh*)_collider;
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next()) {
    Contact* contact = _contacts.Get(index);
    if(contact->IsValid()) {
      Triangle* triangle = mesh->GetTriangle(contact->GetComponentIndex());
      const GfVec3f position =
        contact->ComputePosition(mesh->GetPositionsCPtr(), &triangle->vertices[0], 3, &mesh->GetMatrix());
      const GfVec3f normal =
        contact->ComputeNormal(mesh->GetNormalsCPtr(), &triangle->vertices[0], 3, &mesh->GetMatrix());

      points.push_back(position);
      points.push_back(position + normal);
      colors.push_back(red);
      colors.push_back(red);
      radius.push_back(r);
      radius.push_back(r);
      counts.push_back(2);
    }
  }
}

void 
MeshCollision::GetVelocities(Particles* particles, VtArray<GfVec3f>& points,
  VtArray<float>& radius, VtArray<GfVec3f>& colors, VtArray<int>& counts)
{
  Mask::Iterator iterator(this, 0, particles->GetNumParticles());
  const GfVec3f green(0.f, 1.f, 0.f);
  const float r = 0.05f;

  bool particleVelocity = true;
  Mesh* mesh = (Mesh*)_collider;
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next()) 
  {
    Contact* contact = _contacts.Get(index);
    if(contact->IsValid()) {
      if(particleVelocity) {

        points.push_back(particles->position[index]);
        points.push_back(particles->position[index] + particles->velocity[index] * Geometry::FrameDuration);
        colors.push_back(green);
        colors.push_back(green);
        radius.push_back(r);
        radius.push_back(r);
        counts.push_back(2);
      } else {
        Triangle* triangle = mesh->GetTriangle(contact->GetComponentIndex());
        const GfVec3f velocity(mesh->GetMatrix().TransformDir(
          triangle->GetVelocity(mesh->GetPositionsCPtr(), mesh->GetPreviousCPtr())));
        const GfVec3f position(contact->ComputePosition(mesh->GetPositionsCPtr(), 
          &triangle->vertices[0], 3, &mesh->GetMatrix()));

        points.push_back(position);
        points.push_back(position + velocity);
        colors.push_back(green);
        colors.push_back(green);
        radius.push_back(r);
        radius.push_back(r);
        counts.push_back(2);
      }
    }
  }
}

//----------------------------------------------------------------------------------------
// Self Collision
//----------------------------------------------------------------------------------------
size_t SelfCollision::TYPE_ID = Collision::SELF;

SelfCollision::SelfCollision(Particles* particles, const SdfPath& path, 
  float restitution, float friction)
  : Collision(NULL, path, restitution, friction)
  , _particles(particles)
  , _grid(NULL)
  , _neighborsInitialized(false)
{
  double avgRadius = 0.0;
  size_t numParticles = _particles->GetNumParticles();

  for (size_t p = 0; p < numParticles; ++p) {
    Body* body = particles->body[p];
    avgRadius += particles->radius[p] * body->GetSelfCollisionRadius();
  }
  avgRadius /= static_cast<float>(numParticles);
  _grid.Init(numParticles, &_particles->predicted[0], avgRadius * 2.f);
}

SelfCollision::~SelfCollision()
{
  
}

// 
// Init
//
void SelfCollision::Init(Particles* particles, const std::vector<Body*>& bodies,
  std::vector<Constraint*>& constraints) 
{
  std::cout << "SelfCollision INITIALIZE : " << particles->GetNumParticles() << std::endl;
  _contacts.Resize(particles->GetNumParticles(), PARTICLE_MAX_CONTACTS);
  _contacts.ResetAllUsed();

  _BuildContacts(particles, bodies, constraints);
  std::cout << "created  : " << constraints.size() << " constraints " << std::endl;
}

void SelfCollision::_UpdateParameters(const UsdPrim& prim, double time)
{
  UsdPbdCollisionAPI api(prim);
  api.GetCollisionEnabledAttr().Get(&_enabled, time);
  api.GetRestitutionAttr().Get(&_restitution, time);
  api.GetFrictionAttr().Get(&_friction, time);
  api.GetDampAttr().Get(&_damp, time);
  api.GetMarginAttr().Get(&_margin, time);
  api.GetMaxSeparationVelocityAttr().Get(&_maxSeparationVelocity, time);
  api.GetCollisionStiffnessAttr().Get(&_stiffness);
}

void SelfCollision::Update(const UsdPrim& prim, double time)
{
  _UpdateAccelerationStructure();
  _contacts.ResetAllUsed();

}

// 
// Contacts
//

void SelfCollision::FindContacts(Particles* particles, float ft)
{
  //if(!_neighborsInitialized)_ComputeNeighbors(bodies);
  std::cout << "self collision find contacts" << std::endl;
  WorkParallelForN(particles->GetNumParticles(),
    std::bind(&SelfCollision::_FindContacts, this, particles,
      std::placeholders::_1, std::placeholders::_2, ft), PACKET_SIZE);
  std::cout << "self collision find contacts DONE" << std::endl;
  

}

void SelfCollision::UpdateContacts(Particles* particles, float t)
{
  _t = t;
  WorkParallelForN(
      particles->GetNumParticles(),
      std::bind(&SelfCollision::_UpdateContacts, this, particles, 
        std::placeholders::_1, std::placeholders::_2), PACKET_SIZE);
}

void SelfCollision::_UpdateContacts(Particles* particles, size_t begin, size_t end)
{
  Mask::Iterator iterator(this, begin, end);
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next()) {
    //GfVec3f color = RandomColorByIndex(index);
    if (_contacts.IsUsed(index))
      for (size_t c = 0; c < _contacts.GetNumUsed(index); ++c) {
        Contact* contact = _contacts.Get(index, c);
        size_t other = contact->GetComponentIndex();

        contact->Update(this, particles, index);
        /*
        if (index % 32 == 0) {
          particles->color[index] = color;
          particles->color[other] = color;
        }
        */
      }
      
  } 
}



void SelfCollision::_FindContacts(Particles* particles, size_t begin, size_t end, float ft)
{
  Mask::Iterator iterator(this, begin, end);
  for (size_t index = iterator.Begin(); index != Mask::INVALID_INDEX; index = iterator.Next()) {
    _FindContact(particles, index, ft);
  }
}

void SelfCollision::_FindContact(Particles* particles, size_t index, float ft)
{
  return;
  std::vector<int> closests;

  Body* body = particles->body[index];
  if(!body->GetSelfCollisionEnabled())return;
  const float radiusMultiplier = body->GetSelfCollisionRadius();
  size_t numCollide = 0;
  _grid.Closests(index, &particles->predicted[0], /*&particles->velocity[0], ft,*/
    closests,  2.f * ( particles->radius[index] * radiusMultiplier + TOLERANCE_MARGIN));
  
  for(int closest: closests) {
    if(numCollide >= PARTICLE_MAX_CONTACTS)break;

    Body* other = particles->body[closest];
    if(other != body) continue;
    if(_AreConnected(index, closest))continue;

    GfVec3f ip(particles->position[index] + particles->velocity[index] * ft);
    GfVec3f cp(particles->position[closest] + particles->velocity[closest] * ft);
    
    if((ip - cp).GetLength() < ((particles->radius[index] + particles->radius[closest]) * radiusMultiplier)) {
      Contact* contact = _contacts.Use(index);
      contact->SetComponentIndex(closest);
      contact->SetActive(true);
      numCollide++;
    }
  }
}

void SelfCollision::_BuildContacts(Particles* particles, const std::vector<Body*>& bodies,
  std::vector<Constraint*>& constraints)
{

  CollisionConstraint* constraint = NULL;
  size_t numParticles = particles->GetNumParticles();

  size_t numContacts = _contacts.GetTotalNumUsed();

  VtArray<int> elements;
 
  size_t contactsOffset = 0;
  size_t contactIdx = 0;
  Mask::Iterator iterator(this, 0, numParticles);
  size_t index = iterator.Begin();
  for (; index != Mask::INVALID_INDEX; index = iterator.Next()) {
    elements.push_back(index);   
    
    if ((elements.size() >= Constraint::BlockSize) || iterator.End()) {
      if (elements.size()) {
        constraint = new CollisionConstraint(particles, this, elements);
        constraints.push_back(constraint);
        elements.clear();
      } 
    }
  }
}

void SelfCollision::_UpdateAccelerationStructure()
{  
  if(_particles->num)
    _grid.Update(&_particles->predicted[0]);
} 

float SelfCollision::GetValue(Particles* particles, size_t index, size_t other)
{
  Body* body = particles->body[index];
  const float radiusMultiplier = body->GetSelfCollisionRadius();

  return (particles->predicted[index] - particles->predicted[other]).GetLength() - 
    (particles->radius[index] + particles->radius[other]) * radiusMultiplier;
}
  
GfVec3f SelfCollision::GetGradient(Particles* particles, size_t index, size_t other)
{
  return (particles->predicted[index] - particles->predicted[other]).GetNormalized();
}

// Velocity
GfVec3f SelfCollision::GetVelocity(Particles* particles, size_t index, size_t other)
{
  return particles->velocity[other];
}


void 
SelfCollision::_ComputeNeighbors(const std::vector<Body*> &bodies)
{

  size_t numParticles = _particles->GetNumParticles();
  _neighborsCounts.resize(numParticles, 0);
  _neighborsOffsets.resize(numParticles, 0);
  _neighbors.clear();

  size_t neighborsOffset = 0;
  for(size_t b = 0; b < bodies.size(); ++b) {
    const Geometry* geometry = bodies[b]->GetGeometry();
    size_t offset = bodies[b]->GetOffset();
    switch (geometry->GetType()) {
      case Geometry::MESH:
      {
        Mesh* mesh = (Mesh*)geometry;
        if(!(mesh->GetFlags() & Mesh::NEIGHBORS))
          mesh->ComputeNeighbors();
        size_t numPoints = mesh->GetNumPoints();
        for (size_t p = 0; p < numPoints; ++p) {
          size_t numNeighbors = mesh->GetNumNeighbors(p);
          for (size_t n = 0; n < numNeighbors; ++n)
            _neighbors.push_back(offset + mesh->GetNeighbor(p, n));
      
          _neighborsCounts[offset + p] = numNeighbors;
          _neighborsOffsets[offset + p] = neighborsOffset;
          neighborsOffset += numNeighbors;
        }
        break;
      }
    }
  }

  _neighborsInitialized = true;
}


bool 
SelfCollision::_AreConnected(size_t index, size_t other)
{
  int* neighbors = &_neighbors[_neighborsOffsets[index]];
  for(size_t n = 0; n < _neighborsCounts[index]; ++n) {
    if(neighbors[n] == other)return true;
  }
  return false;
}


JVR_NAMESPACE_CLOSE_SCOPE