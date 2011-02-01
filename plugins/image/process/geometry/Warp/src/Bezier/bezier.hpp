#ifndef _TUTTLE_PLUGIN_BEZIER_HPP_
#define _TUTTLE_PLUGIN_BEZIER_HPP_

#include "WarpDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>
#include <tuttle/plugin/opengl/gl.h>

namespace tuttle {
namespace plugin {
namespace warp {
namespace bezier {

using namespace boost::gil;
using namespace boost::numeric::ublas;

    point2<double> barycentre(std::vector< point2<double> > quatrePoints, double t);
    void tracerPoint(const point2<double> p);
    void tracePoint(std::vector< point2<double> > tabPts, int nbPointsTraces, double t);
    void dessinePoint(std::vector< point2<double> > tabPts, const double r, const double v, const double b);
    point2<double> rempliTabPoint(std::vector< point2<double> > tabPts, double t);


}
}
}
}

#endif
