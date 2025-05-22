#ifndef JVR_PBD_CONTACT_H
#define JVR_PBD_CONTACT_H

#include "../geometry/location.h"

JVR_NAMESPACE_OPEN_SCOPE

class Particles;
class Plane;
class Sphere;
class Mesh;
class Collision;
class SelfCollision;


class Contact : public Location {
public:
  enum State {
    DISCARD,
    ACTIVE,
    TOUCHING
  };
  Contact() : _state(DISCARD){};
  virtual ~Contact(){};

  void Init(Collision* collision, Particles* particles, size_t index);
  void Update(Collision* collision, Particles* particles, size_t index);

  void SetActive(bool active){_state = active;};
  bool IsActive() const {return _state != DISCARD;};
  void SetTouching(bool touching){touching ? TOUCHING : ACTIVE;};
  bool IsTouching() const {return _state == TOUCHING;};
  const GfVec3f& GetNormal() const {return _normal;};
  const GfVec3f& GetVelocity() const {return _velocity;};
  float GetDepth() const {return _depth;};
  float GetInitDepth() const {return _initDepth;};

  void SetNormal(const GfVec3f& normal){_normal = normal;};
  void SetDepth(float depth){_depth = depth;};
  void SetInitDepth(float depth){_initDepth = depth;};

private:      
  GfVec3f           _normal;   // contact normal
  GfVec3f           _velocity; // relative velocity

  float             _initDepth;// start frame penetration depth
  float             _depth;    // current substep penetration depth

  size_t            _state;
};


static const size_t PARTICLE_MAX_CONTACTS = 16;

class Contacts {

public:
  Contacts() : n(0), m(1), data(NULL), used(NULL){};
  virtual ~Contacts() { delete[] data; delete[] used;};

  Contact* Get(size_t index, size_t second=0) const {
    return &data[index * m + second];
  };

  void Resize(size_t n, size_t m=PARTICLE_MAX_CONTACTS);
  void ResetUsed(size_t index);
  void ResetAllUsed();

  bool IsUsed(size_t index){return used[index] > 0;};
  bool IsActive(size_t index, size_t second=0){return Get(index, second)->IsActive(); };

  Contact* Use(size_t index);
  Contact* LastUsed(size_t index);
  size_t GetNumUsed(size_t index) const;
  size_t GetTotalNumUsed() const;
  size_t GetSize() const {return n;};

private:
  size_t                n;
  size_t                m;
  int*                  used;
  Contact*              data;
};


JVR_NAMESPACE_CLOSE_SCOPE

#endif // JVR_PBD_CONTACT_H