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

	CPixel sum;
	Pixel average;
};

template<class View>
ColorTransfertProcess<View>::ColorTransfertProcess( ColorTransfertPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
        _clipSrcRef = effect.fetchClip( kOfxImageEffectSimpleSourceClipName );
        _clipDstRef = effect.fetchClip( kOfxImageEffectSimpleSourceClipName );
}

template<class View>
void ColorTransfertProcess<View>::computeAverage( const View& image )
{
        ComputeParams<View, boost::gil::bits64f> output;
       	//pixel_zeros_t<Pixel>( )( output.sum );
	pixel_zeros_t<Pixel>( )( output.average );
        //std::cout<< output.average.value <<std::endl;

}

template<class View>
void ColorTransfertProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );		// Call parent class setup
        _params = _plugin.getProcessParams( args.renderScale ); // Retrieve plugin parameters

	// srcRef initialization
	this->_srcRef.reset( _clipSrcRef->fetchImage( args.time ) );
	if( !this->_srcRef.get() )
	{
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	}
	if( this->_srcRef->getRowBytes() == 0 )
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	}
	// _srcRefPixelRod = _srcRef->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcRefPixelRod   = _clipSrcRef->getPixelRod( args.time, args.renderScale );
	this->_srcRefView = tuttle::plugin::getView<View>( this->_srcRef.get(), _srcRefPixelRod );


	// dstRef initialization
	this->_dstRef.reset( _clipDstRef->fetchImage( args.time ) );
	if( !this->_dstRef.get() )
	{
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	}
	if( this->_dstRef->getRowBytes() == 0 )
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	}
	// _dstPixelRod = _dst->getRegionOfDefinition(); // bug in nuke, returns bounds
	_dstRefPixelRod   = _clipDstRef->getPixelRod( args.time, args.renderScale );
	this->_dstRefView = tuttle::plugin::getView<View>( this->_dstRef.get(), _dstRefPixelRod );

        // analyse srcRef and dstRef
        computeAverage( this->_srcRefView );
        computeAverage( this->_dstRefView );

	// now analyse the differences
	
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
	
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcRefView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y );
	//View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	//copy_pixels( src, dst );

	// fill dst: modify src using analyse of srcRef and dstRef differences
	
}

}
}
}


