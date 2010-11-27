#include "transformAffineAlgorithm.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/resample.hpp>
#include <tuttle/plugin/exceptions.hpp>


namespace tuttle {
namespace plugin {
namespace transformAffine {

template<class View>
TransformAffineProcess<View>::TransformAffineProcess( TransformAffinePlugin& effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template<class View>
void TransformAffineProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void TransformAffineProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
//	
//	for( int y = procWindowOutput.y1;
//			 y < procWindowOutput.y2;
//			 ++y )
//	{
//		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
//		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
//		for( int x = procWindowOutput.x1;
//			 x < procWindowOutput.x2;
//			 ++x, ++src_it, ++dst_it )
//		{
//			(*dst_it) = (*src_it);
//		}
//		if( this->progressForward() )
//			return;
//	}
	
//	switch( _interpolation )
//	{
//		case eParamInterpolationNearest:
			resample_pixels_progress<nearest_neighbor_sampler>( this->_srcView, this->_dstView, _params, procWindowOutput, this );
//			return;
//		case eParamInterpolationBilinear:
//			<bilinear_sampler>
//			return;
//	}

}

}
}
}

