#include "WarpAlgorithm.hpp"
#include "WarpPlugin.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/resample.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle {
namespace plugin {
namespace warp {

template <typename F, typename F2>
inline boost::gil::point2<F> transform( const TPS_Morpher<F>& op, const boost::gil::point2<F2>& src )
{
    return op(src);
}

template<class View>
WarpProcess<View>::WarpProcess( WarpPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template<class View>
void WarpProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

	//TPS_Morpher<Scalar> tps( _params._inPoints, _params._outPoints , _params._rigiditeTPS);
    _tps.setup( _params._bezierIn, _params._bezierOut , _params._rigiditeTPS, _params._activateWarp, _params._nbPoints );
	TUTTLE_TCOUT_VAR( _params._rigiditeTPS );
	TUTTLE_TCOUT_VAR( _params._activateWarp );
	TUTTLE_TCOUT_VAR( _params._nbPoints );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void WarpProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

    resample_pixels_progress<bilinear_sampler>( this->_srcView, this->_dstView, _tps, procWindowRoW, this );
}

}
}
}
