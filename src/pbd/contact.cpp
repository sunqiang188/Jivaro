#include "../geometry/geometry.h"
#include "../geometry/implicit.h"
#include "../geometry/deformable.h"
#include "../geometry/mesh.h"

#include "../pbd/contact.h"
#include "../pbd/particle.h"
#include "../pbd/collision.h"


JVR_NAMESPACE_OPEN_SCOPE

void Contact::Init(Collision* collision, Particles* particles, size_t index)
{
  /*
  if(IsValid() && collision->GetTypeId() == Collision::MESH) {
    Mesh* mesh = (Mesh*)collision->GetGeometry();
    const Triangle* triangle = mesh->GetTriangle(GetComponentIndex());
    SetPoint(ComputeInterpolatedPosition(mesh->GetPositionsCPtr(), 
      mesh->GetPreviousCPtr(), 0.f, &triangle->vertices[0], 3, &mesh->GetMatrix()));
  }
  */
  
  _normal = collision->GetGradient(particles, index);
  _initDepth = collision->GetValue(particles, index);
  _depth = _initDepth;

  
  if(collision->GetTypeId() != Collision::SELF) 
    _velocity = collision->GetVelocity(particles, index);
  else
    _velocity = ((SelfCollision*)collision)->GetVelocity(particles, index, GetComponentIndex());
  _state = _depth < collision->GetMargin() ? ACTIVE : DISCARD;
  if(_depth < 0.f)_state = TOUCHING;
  
}

void Contact::Update(Collision* collision, Particles* particles, size_t index)
{
  if(!IsActive())return;
  /*
  if(IsValid() && collision->GetTypeId() == Collision::MESH) {
    Mesh* mesh = (Mesh*)collision->GetGeometry();
    const Triangle* triangle = mesh->GetTriangle(GetComponentIndex());
    SetPoint(ComputeInterpolatedPosition(mesh->GetPositionsCPtr(), 
      mesh->GetPreviousCPtr(), collision->GetStepTime(), &triangle->vertices[0], 3, &mesh->GetMatrix()));
  }
  */
  
  _normal = collision->GetGradient(particles, index);
  _depth = collision->GetValue(particles, index);
  if(_depth < 0.f)_state = TOUCHING;
  else _state = ACTIVE;

  
  if(collision->GetTypeId() != Collision::SELF) 
    _velocity = collision->GetVelocity(particles, index);
  else
    _velocity = ((SelfCollision*)collision)->GetVelocity(particles, index, GetComponentIndex());

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
  Contact contact;
  for(size_t i = 0; i < m * n; ++i)
    data[i] = contact;
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