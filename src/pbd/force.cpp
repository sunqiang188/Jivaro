#include "../pbd/force.h"
#include "../pbd/particle.h"
#include "../pbd/solver.h"

JVR_NAMESPACE_OPEN_SCOPE

size_t GravityForce::TYPE_ID = Force::GRAVITY;

GravityForce::GravityForce(const GfVec3f& gravity) 
  : Force()
  , _gravity(gravity)
  , _attr()
{
}

GravityForce::GravityForce(const UsdAttribute& attribute) 
  : Force()
  , _attr(attribute)
{
  _attr.Get(&_gravity, UsdTimeCode::Default());
}

void GravityForce::Update(float time)
{
  if(_attr.IsValid()) _attr.Get(&_gravity, time);
}

void GravityForce::Apply(size_t begin, size_t end, Particles* particles, float dt) const
{
  const float* mass = &particles->mass[0];
  const float* invMass = &particles->invMass[0];
  GfVec3f* velocity = &particles->velocity[0];
  const short* state = &particles->state[0];

  for(size_t index = begin; index < end; ++index) {
    //if(state[index] != Particles::ACTIVE) continue;
    velocity[index] += _gravity /** invMass[index]*/ * dt;
  }
}


size_t DampForce::TYPE_ID = Force::DAMP;

DampForce::DampForce(const UsdAttribute& attr)
  : Force()
  , _attr(attr)
{
  _attr.Get(&_damp, UsdTimeCode::Default());
}

DampForce::DampForce(float damp)
  : Force()
  , _damp(damp)
  , _attr()
{
}

void DampForce::Update(float time)
{
  if(_attr.IsValid()) _attr.Get(&_damp, time);
}

void DampForce::Apply(size_t begin, size_t end, Particles* particles, float dt) const
{
  const float* mass = &particles->mass[0];
  const float* invMass = &particles->invMass[0];
  GfVec3f* velocity = &particles->velocity[0];

  for(size_t index = begin; index < end; ++index) {
    if (particles->state[index] != Particles::ACTIVE)continue;
    velocity[index] -= _damp * velocity[index] * invMass[index] * dt;
  }
}

JVR_NAMESPACE_CLOSE_SCOPE