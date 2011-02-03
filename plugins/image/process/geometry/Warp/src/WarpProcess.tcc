#include "WarpAlgorithm.hpp"
#include "WarpPlugin.hpp"
#include "TPS/tps.hpp"
//#include "Bezier/bezier.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/resample.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle {
namespace plugin {
namespace warp {

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
}

template <typename F, typename F2>
inline boost::gil::point2<F> transform( const TPS_Morpher<F>& op, const boost::gil::point2<F2>& src )
{
    return op(src);
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

    /*
    for( int y = procWindowOutput.y1;
                     y < procWindowOutput.y2;
                     ++y )
    {
            typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
            typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
            for( int x = procWindowOutput.x1;
                     x < procWindowOutput.x2;
                     ++x, ++src_it, ++dst_it )
            {
                    (*dst_it) = (*src_it);
            }
            if( this->progressForward() )
                    return;
    }

    const OfxRectI procWindowSrc = this->translateRegion( procWindowRoW, this->_srcPixelRod );
    OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
                                                         procWindowRoW.y2 - procWindowRoW.y1 };
    View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
                                                                      procWindowSize.x, procWindowSize.y );
    View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
                                                                      procWindowSize.x, procWindowSize.y );
    copy_pixels( src, dst );
    */

    //TPS_Morpher<Scalar> tps( _params._inPoints, _params._outPoints , _params._rigiditeTPS);
    TPS_Morpher<Scalar> tps( _params._bezierIn, _params._bezierOut , _params._rigiditeTPS, _params._activateWarp, _params._nbPoints);
    //point2<double> test(10,33);
    //tps.morphTPS(test);

    resample_pixels_progress<bilinear_sampler>( this->_srcView, this->_dstView, tps, procWindowRoW, this );
}

}
}
}
