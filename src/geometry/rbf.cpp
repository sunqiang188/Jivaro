#include "../geometry/rbf.h"


JVR_NAMESPACE_OPEN_SCOPE

RBF::RBF()
  : _epsilon(-1)
  , _type(MULTIQUADRIC)
  , _kernel(&RBF::_MultiQuadricKernel)
  , _initialized(false)
{
}

RBF::~RBF()
{
}

void RBF::SetKernelType(int type)
{
  _type = type;
  switch(_type) {
    case LINEAR:
    _kernel = (PFNRBFKERNEL)&RBF::_LinearKernel;
    break;
    case GAUSSIAN:
    _kernel = (PFNRBFKERNEL)&RBF::_GaussianKernel;
    break;
    case CUBIC:
    _kernel = (PFNRBFKERNEL)&RBF::_CubicKernel;
    break;
    case QUINTIC:
    _kernel = (PFNRBFKERNEL)&RBF::_QuinticKernel;
    break;
    case MULTIQUADRIC:
    _kernel = (PFNRBFKERNEL)&RBF::_MultiQuadricKernel;
    break;
    case INVERSE:
    _kernel = (PFNRBFKERNEL)&RBF::_InverseKernel;
    break;
    case THINPLATE:
    _kernel = (PFNRBFKERNEL)&RBF::_ThinPlateKernel;
    break;
  }
  // recompute weights
  Init(_keys, _values);
}


// automaticaly compute the best epsilon
void 
RBF::_ComputeEpsilon(const Matrix<float>& matrix)
{
  size_t numRows = matrix.NumRows();
  
  std::vector<float> minimums = matrix.GetColumnsMinimum();
  std::vector<float> maximums = matrix.GetColumnsMaximum();
  float product = 1.f;
  size_t size = 0;
  for(size_t i = 0; i < matrix.NumColumns(); ++i) {
    float edge = maximums[i] - minimums[i];
    if(!GfIsClose(edge, 0, 0.000000001)) {
      product *= edge;
      size++;
    }
  }
  _epsilon = GfPow(product / (float)numRows, 1.f / (float)size);
}

// init
void
RBF::Init(const Matrix<float>& keys, const Matrix<float>& values)
{
  _ComputeEpsilon(keys);
  _keys = keys;
  _values = values;
  
  size_t nbp = keys.NumRows();
  size_t d =  keys.NumColumns();
  float r = 0.f;
  
  _A.Resize(nbp, nbp);

  for(size_t i = 0; i < nbp; ++i) {
    for(size_t j = 0; j < nbp; ++j) {
      r = 0.f;
      for(size_t k = 0; k < d; ++k) {
        r += GfPow(keys.Get(i, k) - keys.Get(j, k), 2);
      }
      r = GfSqrt(r);
      _A.Set(i, j, _kernel(r, _epsilon));
    }
  }

  // solve the weights
  _A.InverseInPlace();
  _initialized = true;
}

// interpolate

void
RBF::Interpolate(const Matrix<float>& querys, Matrix<float>* result)
{
  result->Resize(querys.NumRows(), _values.NumColumns());

  size_t nd = _keys.NumRows();
  size_t m = _keys.NumColumns();
  size_t ni = querys.NumRows();
  std::vector<float> v(nd);
  for(size_t column = 0; column < _values.NumColumns(); ++column) {
    std::vector<float> results(ni);
    std::vector<float> value = _values.GetColumn(column);
    std::vector<float> weighted = _A.MultiplyVector(value);
    for(size_t i = 0; i < ni ; ++i) {
      for(size_t j = 0; j < nd; ++j) {
        float r = 0.f;
        for(size_t k = 0; k < m; ++k) {
          r += GfPow(querys.Get(i, k) - _keys.Get(j, k), 2);
        }
        r = GfSqrt(r);
        v[j] = _kernel(r, _epsilon);
      }
      results[i] = 0.f;
      for(size_t j = 0; j < nd; ++j) {
        results[i] += v[j] * value[j];
      }
    }
    result->SetColumn(column, results);
  }
}

JVR_NAMESPACE_CLOSE_SCOPE
