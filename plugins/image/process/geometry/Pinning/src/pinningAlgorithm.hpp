#ifndef _TUTTLE_PLUGIN_PINNING_ALGORITHM_HPP_
#define	_TUTTLE_PLUGIN_PINNING_ALGORITHM_HPP_

#include "PinningPlugin.hpp"

namespace tuttle {
namespace plugin {
namespace pinning {

/**
 * @brief Perspective transformation functor.
 * @param[in] t: the transformation 3x3 matrix
 * @param[in] src: 2D source point
 */
template <typename F, typename F2>
inline boost::gil::point2<F> transform( const Perspective<F>& t, const boost::gil::point2<F2>& src )
{
	using namespace boost::numeric::ublas;
	bounded_vector<F,3> pIn;
	pIn[0] = src.x;
	pIn[1] = src.y;
	pIn[2] = 1.0;

	bounded_vector<F,3> pOut = prod( t._matrix, pIn );

	boost::gil::point2<F> res;
	res.x = pOut[0] / pOut[2];
	res.y = pOut[1] / pOut[2];
	
	return res;
}

/**
 * @brief Bilinear transformation functor.
 * @param[in] t: the transformation 2x4 matrix
 * @param[in] src: 2D source point
 *
 * @f[
 * x' = c[0,0]x + c[0,1]y + c[0,2]xy + c[0,3]
 * y' = c[1,0]x + c[1,1]y + c[1,2]xy + c[1,3]
 * @f]
 */
template <typename F, typename F2>
inline boost::gil::point2<F> transform( const Bilinear<F>& t, const boost::gil::point2<F2>& src )
{
	boost::gil::point2<F> res;
	res.x = t._matrix(0, 0) * src.x + t._matrix(0, 1) * src.y + t._matrix(0, 2) * src.x * src.y + t._matrix(0, 3);
	res.y = t._matrix(1, 0) * src.x + t._matrix(1, 1) * src.y + t._matrix(1, 2) * src.x * src.y + t._matrix(1, 3);
	return res;
}

}
}
}

#endif
