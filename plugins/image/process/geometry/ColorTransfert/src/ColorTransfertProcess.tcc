#include "ColorTransfertAlgorithm.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/param/gilColor.hpp>
#include <boost/gil/extension/typedefs.hpp>

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_assign.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_minmax.hpp>
#include <boost/gil/extension/color/hsl.hpp>

#include <boost/units/pow.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/erase.hpp>
#include <boost/mpl/find.hpp>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

using namespace boost::gil;

template<class View, typename CType = boost::gil::bits64f>
struct ComputeParams
{
	typedef typename View::value_type Pixel;
	typedef typename color_space_type<View>::type Colorspace;
	typedef pixel<CType, layout<Colorspace> > CPixel;
};

template<class View>
ColorTransfertProcess<View>::ColorTransfertProcess( ColorTransfertPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template<class View>
void ColorTransfertProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );			// Appel setup héritage
        _params = _plugin.getProcessParams( args.renderScale );		// Récupération des paramètres du plug

	OFX::Clip* _clipSrcRef;       ///< Source & dst ref image clip
	boost::scoped_ptr<OFX::Image> _srcRef;
	OfxRectI _srcRefPixelRod;
	//SView _srcRefView; ///< @brief source clip (filters have only one input) 

	OFX::Clip* _clipDstRef;       ///< Source & dst ref image clip
	boost::scoped_ptr<OFX::Image> _dstRef;
	OfxRectI _dstRefPixelRod;
	//SView _dstRefView; ///< @brief source clip (filters have only one input) 

	//computeAverage(_clipSrcRef);
}

void computeAverage(OFX::Clip* image){
	std::cout<<"jnljfnljen"<<std::endl;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ColorTransfertProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	
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
	/*
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
							                  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							                  procWindowSize.x, procWindowSize.y );
	copy_pixels( src, dst );
	*/

}

}
}
}
