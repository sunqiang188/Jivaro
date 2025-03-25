#ifndef JVR_PBD_COLLISION_H
#define JVR_PBD_COLLISION_H

#include <pxr/base/vt/array.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/matrix4f.h>

#include "../common.h"
#include "../acceleration/hashGrid.h"
#include "../acceleration/bvh.h"
#include "../pbd/element.h"

JVR_NAMESPACE_OPEN_SCOPE

class Geometry;

struct Particles;
struct Body;
class Constraint;
class CollisionConstraint;
class Points;
class Solver;
class HashGrid;


class Collision : public Element
{
public:

  static const float TOLERANCE_MARGIN;

  enum Type {
    PLANE = 1,
    BOX,
    SPHERE,
    CAPSULE,
    MESH,
    SELF
  };

  Collision(Geometry* collider, const SdfPath& path, 
    float restitution=0.5f, float friction=0.5f) 
    : Element(Element::COLLISION)
    , _collider(collider)
    , _restitution(restitution)
    , _friction(friction){};

  /*
  void AddBody(Particles* particles, Body* body);
  void RemoveBody(Particles* particles, Body* body);
  */
  virtual ~Collision() {};
  virtual size_t GetTypeId() const override = 0; // pure virtual

  virtual void Init(size_t numParticles);
  virtual void Update(const UsdPrim& prim, double time);
  virtual void FindContacts(Particles* particles, const std::vector<Body*>& bodies,
    std::vector<Constraint*>& constraints, float ft);

  virtual Geometry* GetGeometry(){return _collider;};
  virtual void SetTime(float time){_t = time;};

  //virtual bool Compute(Particles* particles, size_t index, Contact& contact){return false;};

  virtual float GetValue(Particles* particles, size_t index) = 0;
  virtual GfVec3f GetGradient(Particles* particles, size_t index) = 0; // pure virtual
  virtual GfVec3f GetVelocity(Particles* particles, size_t index);

  inline bool CheckHit(size_t index) const {
    return _hits[index] > 0;
  };
  inline void SetHit(size_t index, bool hit) {
    _hits[index] = (int)hit;
  };

  inline bool CheckCorrected(size_t index) {
    return _corrected[index];
  };
  inline void SetCorrected(size_t index, bool corrected) {
    _corrected[index] = corrected;
  };

  float GetFriction() const {return _friction;};
  float GetRestitution() const {return _restitution;};
  float GetDamp() const {return _damp;};
  float GetMargin()const {return _margin;};
  float GetStiffness() const {return _stiffness;};

  void Reset(Particles* particles);

  // for visual debugging
  virtual void GetPoints(Particles* particles, VtArray<GfVec3f>& points,
    VtArray<float>& radius, VtArray<GfVec3f>& colors);
  virtual void GetNormals(Particles* particles, VtArray<GfVec3f>& points,
    VtArray<float>& radius, VtArray<GfVec3f>& colors, VtArray<int>& counts);
  virtual void GetVelocities(Particles* particles, VtArray<GfVec3f>& points,
    VtArray<float>& radius, VtArray<GfVec3f>& colors, VtArray<int>& counts);

protected:

  static const size_t PACKET_SIZE;

  virtual void _UpdateParameters(const UsdPrim& prim, double time);
  
  virtual void _ResetContacts(Particles* particles);
  
  virtual void _BuildContacts(Particles* particles, const std::vector<Body*>& bodies,
    std::vector<Constraint*>& constraints);
  virtual void _FindContacts(Particles* particles, size_t begin, size_t end, float ft);
  /*
  virtual void _UpdateContacts(Particles* particles, size_t begin, size_t end);
  */
  virtual void _FindContact(Particles* particles, size_t index, float ft) = 0; // pure virtual
  /*
  virtual void _StoreContactLocation(Particles* particles, int elem, Contact* contact);
  */

  // hits encode vertex hit in the int list bits
  std::vector<int>                  _hits;
  std::vector<int>                  _corrected;
  VtArray<GfVec3f>                  _correction;
  //std::vector<int>                  _c2p;
  size_t                            _numParticles;
  //Contacts                          _contacts;

  bool                              _enabled;
  float                             _restitution;
  float                             _friction;
  float                             _damp;
  float                             _stiffness;
  float                             _margin;
  float                             _maxSeparationVelocity;
  float                             _t;
  Geometry*                         _collider;
  TfToken                           _key;

};

class PlaneCollision : public Collision
{
public:
  PlaneCollision(Geometry* collider, const SdfPath& path, 
    float restitution=0.5f, float friction= 0.5f);
  size_t GetTypeId() const override { return TYPE_ID; };

  //virtual bool Compute(Particles* particles, size_t index, Contact& contact);

  float GetValue(Particles* particles, size_t index) override;
  GfVec3f GetGradient(Particles* particles, size_t index) override;
  void Update(const UsdPrim& prim, double time) override;

protected:
  void _UpdatePositionAndNormal();
  void _FindContact(Particles* particles, size_t index, float ft) override;

private:
  static size_t            TYPE_ID;
  GfVec3f                  _position;
  GfVec3f                  _normal;
  GfVec3f                  _prevPosition;
  GfVec3f                  _prevNormal;

};

class BoxCollision : public Collision
{
public:
  BoxCollision(Geometry* collider, const SdfPath& path, 
    float restitution=0.5f, float friction= 0.5f);
  size_t GetTypeId() const override { return TYPE_ID; };

  float GetValue(Particles* particles, size_t index) override;
  GfVec3f GetGradient(Particles* particles, size_t index) override;
  void Update(const UsdPrim& prim, double time) override;
  
protected:
  void _UpdateSize();
  void _FindContact(Particles* particles, size_t index, float ft) override;  

private:
  static size_t                 TYPE_ID;
  float                         _size;
};


class SphereCollision : public Collision
{
public:
  SphereCollision(Geometry* collider, const SdfPath& path, 
    float restitution=0.5f, float friction= 0.5f);
  size_t GetTypeId() const override { return TYPE_ID; };

  float GetValue(Particles* particles, size_t index) override;
  GfVec3f GetGradient(Particles* particles, size_t index) override;
  void Update(const UsdPrim& prim, double time) override;
  
protected:
  void _UpdateCenterAndRadius();
  void _FindContact(Particles* particles, size_t index, float ft) override;  

private:
  static size_t                 TYPE_ID;
  GfVec3f                  _center;
  float                         _radius;
};

class CapsuleCollision : public Collision
{
public:
  CapsuleCollision(Geometry* collider, const SdfPath& path, 
    float restitution=0.5f, float friction= 0.5f);
  size_t GetTypeId() const override { return TYPE_ID; };

  float GetValue(Particles* particles, size_t index) override;
  GfVec3f GetGradient(Particles* particles, size_t index) override;
  void Update(const UsdPrim& prim, double time) override;
  
protected:
  void _UpdateRadiusAndHeight();
  void _FindContact(Particles* particles, size_t index, float ft) override;  

private:
  static size_t                 TYPE_ID;
  GfVec3f                       _center;
  float                         _radius;
  float                         _height;
};


class MeshCollision : public Collision
{
public:
  MeshCollision(Geometry* collider, const SdfPath& path, 
    float restitution=0.5f, float friction= 0.5f);
  ~MeshCollision();
  size_t GetTypeId() const override { return TYPE_ID; };

  virtual void Init(size_t numParticles) override;

  float GetValue(Particles* particles, size_t index) override;
  GfVec3f GetGradient(Particles* particles, size_t index) override;
  GfVec3f GetVelocity(Particles* particles, size_t index) override;
  void Update(const UsdPrim& prim, double time) override;

  // for visual debugging
  void GetPoints(Particles* particles, VtArray<GfVec3f>& points,
    VtArray<float>& radius, VtArray<GfVec3f>& colors) override;
  void GetNormals(Particles* particles, VtArray<GfVec3f>& points,
    VtArray<float>& radius, VtArray<GfVec3f>& colors, VtArray<int>& counts) override;
  void GetVelocities(Particles* particles, VtArray<GfVec3f>& points,
    VtArray<float>& radius, VtArray<GfVec3f>& colors, VtArray<int>& counts) override;

protected:
  void _CreateAccelerationStructure();
  void _UpdateAccelerationStructure();
  void _FindContact(Particles* particles, size_t index, float ft) override;  
  //void _StoreContactLocation(Particles* particles, int index, Contact* contact) override;

private:
  static size_t                 TYPE_ID;
  BVH                           _bvh;
  std::vector<Location>         _closest;
};

class SelfCollision : public Collision
{

public:

  SelfCollision(Particles* particles, const SdfPath& path,  
    float restitution=0.5f, float friction= 0.5f);
  ~SelfCollision();
  size_t GetTypeId() const override { return TYPE_ID; };

  float GetValue(Particles* particles, size_t index) override{return 0.f;};
  GfVec3f GetGradient(Particles* particles, size_t index) override{return GfVec3f(0.f);};
  float GetValue(Particles* particles, size_t index, size_t other);;
  GfVec3f GetGradient(Particles* particles, size_t index, size_t other);
  GfVec3f GetVelocity(Particles* particles, size_t index, size_t other);

  void Update(const UsdPrim& prim, double time) override;

  void FindContacts(Particles* particles, const std::vector<Body*>& bodies, 
    std::vector<Constraint*>& constraints, float ft)override;

protected:
  void _UpdateParameters( const UsdPrim& prim, double time) override;
  void _ComputeNeighbors(const std::vector<Body*>& bodies);
  void _UpdateAccelerationStructure();
  void _ResetContacts(Particles* particles) override;
  void _FindContacts(Particles* particles, size_t begin, size_t end, float ft) override;
  //void _UpdateContacts(Particles* particles, size_t begin, size_t end) override;
  void _FindContact(Particles* particles, size_t index, float ft) override;
  //void _StoreContactLocation(Particles* particles, int index, int other, Contact* contact);

  void _BuildContacts(Particles* particles, const std::vector<Body*>& bodies,
    std::vector<Constraint*>& constraints)override;

  inline bool _AreConnected(size_t lhs, size_t rhs);

private:
  static size_t                     TYPE_ID;
  HashGrid                          _grid;
  Particles*                        _particles;

  bool                              _neighborsInitialized;
  std::vector<int>                  _neighbors;
  std::vector<short>                _neighborsCounts;
  std::vector<int>                  _neighborsOffsets;

  
};

JVR_NAMESPACE_CLOSE_SCOPE

#endif // JVR_PBD_COLLISION_H
