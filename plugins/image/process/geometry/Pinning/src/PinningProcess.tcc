#include "pinningAlgorithm.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/resample.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle {
namespace plugin {
namespace pinning {

template<class View>
PinningProcess<View>::PinningProcess( PinningPlugin& effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template<class View>
void PinningProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void PinningProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

	switch( _params._interpolation )
	{
		case eParamInterpolationNearest:
			resample<nearest_neighbor_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			break;
		case eParamInterpolationBilinear:
			resample<bilinear_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			break;
	}
}

template<class View>
template<class Sampler>
void PinningProcess<View>::resample( View& srcView, View& dstView, const OfxRectI& procWindow )
{
	using namespace boost::gil;
	switch( _params._method )
	{
		case eParamMethodAffine:
		case eParamMethodPerspective:
			resample_pixels_progress<Sampler>( srcView, dstView, _params._perspective, procWindow, this );
			return;
		case eParamMethodBilinear:
			resample_pixels_progress<Sampler>( srcView, dstView, _params._bilinear, procWindow, this );
			return;
	}
}

}
}
}

