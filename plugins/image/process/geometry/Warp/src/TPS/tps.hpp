#ifndef _TUTTLE_PLUGIN_TPS_HPP_
#define _TUTTLE_PLUGIN_TPS_HPP_

#include "WarpDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>

double square(double x);
double base_func(double r2);
void morphTPS(OFX::Double2DParam* _paramPointIn, OFX::Double2DParam* _paramPointOut);


#endif
