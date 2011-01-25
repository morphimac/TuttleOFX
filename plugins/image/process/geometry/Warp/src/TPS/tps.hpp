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

void morphTPS( const std::vector< point2<double> > pIn, const std::vector< point2<double> > pOut );

}
}
}

#endif
