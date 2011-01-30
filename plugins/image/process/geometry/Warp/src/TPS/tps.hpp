#ifndef _TUTTLE_PLUGIN_TPS_HPP_
#define _TUTTLE_PLUGIN_TPS_HPP_

#include "WarpDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace warp {

using namespace boost::gil;
using namespace boost::numeric::ublas;
typedef	matrix<double> Matrix;
typedef matrix_row<Matrix> Matrix_Row;
typedef matrix_column<Matrix> Matrix_Col;

template<typename SCALAR>
class TPS_Morpher
{
public:
    typedef SCALAR Scalar;
    typedef point2<Scalar> Point2;

    const std::vector< Point2> _pIn;
    const std::vector< Point2 > _pOut;
    //std::vector< Point2> _pToBuild;

    Matrix mtx_l, mtx_v, mtx_orig_k;

    TPS_Morpher( const std::vector< Point2 > pIn, const std::vector< Point2 > pOut );

    template<typename S2>
    Point2 operator()( const point2<S2>& pt ) const;

private:

};

}
}
}

#include "tps.tcc"

#endif
