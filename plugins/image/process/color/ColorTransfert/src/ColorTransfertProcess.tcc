#include "ColorTransfertAlgorithm.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <tuttle/plugin/param/gilColor.hpp>

#include <boost/units/pow.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/erase.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>

#include <boost/numeric/ublas/vector.hpp>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

using namespace boost::gil;

template<class View>
struct ColorParams
{
	typedef typename View::value_type Pixel;
	Pixel _srcAverage, _dstAverage, _ratio;

	ColorParams( const Pixel& srcAverage, const Pixel& dstAverage, const Pixel& srcDeviation, const Pixel & dstDeviation )
	{
		pixel_zeros_t<Pixel > ( )( _ratio );
		_ratio = pixel_divides_t<Pixel, Pixel, Pixel > ( ) ( dstDeviation, srcDeviation );

		pixel_assigns_t<Pixel, Pixel > ( )( srcAverage, _srcAverage );
		pixel_assigns_t<Pixel, Pixel > ( )( dstAverage, _dstAverage );
	}

	Pixel operator( )(const Pixel & p ) const
	{
		Pixel p2;
		pixel_assigns_t<Pixel, Pixel > ( )( p, p2 );
		// RGB to LAB
		/// @todo mel

		pixel_minus_assign_t<Pixel, Pixel > ( )( _srcAverage, p2 );
		p2 = pixel_multiplies_t<Pixel, Pixel, Pixel > ( )( _ratio, p2 );
		pixel_plus_assign_t<Pixel, Pixel > ( )( _dstAverage, p2 );

		// LAB to RGB
		/// @todo mel

		return p2;
	}
};


template<class View>
ColorTransfertProcess<View>::ColorTransfertProcess( ColorTransfertPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	_clipSrcRef = effect.fetchClip( kClipSrcRef );
	_clipDstRef = effect.fetchClip( kClipDstRef );
}

template<class View>
void ColorTransfertProcess<View>::computeAverage( const View& image, Pixel& average, Pixel& deviation )
{
	pixel_zeros_t<Pixel > ( )( average );

	typedef typename color_space_type<View>::type Colorspace;
	typedef pixel<boost::gil::bits64f, layout<Colorspace> > CPixel;
	CPixel sumAverage, sumDeviation;
	pixel_zeros_t<CPixel > ( )( sumAverage );
	pixel_zeros_t<CPixel > ( )( sumDeviation );
	const std::size_t nbPixels = image.width( ) * image.height( );

	// Average
	for( int y = 0; y < image.height( ); ++y )
	{
		typename View::x_iterator src_it = image.x_at( 0, y );
		for( int x = 0; x < image.width( ); ++x, ++src_it )
		{
			CPixel pix;
			pixel_assigns_t<Pixel, CPixel > ( )( * src_it, pix );
			pixel_plus_assign_t<CPixel, CPixel > ( )( pix, sumAverage );
		}
	}
	average = pixel_divides_scalar_t<CPixel, double>( ) ( sumAverage, nbPixels );

	// Standard deviation
	for( int y = 0; y < image.height( ); ++y )
	{
		typename View::x_iterator src_it = image.x_at( 0, y );
		for( int x = 0; x < image.width( ); ++x, ++src_it )
		{
			CPixel pix = pixel_rgb_to_lab_t<Pixel, CPixel>()( * src_it );
//			CPixel pix;
//			pixel_assigns_t<Pixel, CPixel > ( )( * src_it, pix );
			pixel_minus_assign_t<Pixel, CPixel > ( )( average, pix );
			pix = pixel_pow_t<CPixel, 2 > ( )( pix );
			pixel_plus_assign_t<CPixel, CPixel > ( )( pix, sumDeviation );
		}
	}
	deviation = pixel_divides_scalar_t<CPixel, double>( ) ( sumDeviation, nbPixels );
	deviation = pixel_sqrt_t<Pixel > ( )( deviation );

}

template<class View>
void ColorTransfertProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args ); // Call parent class setup
	_params = _plugin.getProcessParams( args.renderScale ); // Retrieve plugin parameters

	// srcRef initialization
	this->_srcRef.reset( _clipSrcRef->fetchImage( args.time ) );
	if( !this->_srcRef.get( ) )
	{
		BOOST_THROW_EXCEPTION( exception::ImageNotReady( ) );
	}
	if( this->_srcRef->getRowBytes( ) == 0 )
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes( ) );
	}
	// _srcRefPixelRod = _srcRef->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcRefPixelRod = _clipSrcRef->getPixelRod( args.time, args.renderScale );
	this->_srcRefView = tuttle::plugin::getView<View > ( this->_srcRef.get( ), _srcRefPixelRod );


	// dstRef initialization
	this->_dstRef.reset( _clipDstRef->fetchImage( args.time ) );
	if( !this->_dstRef.get( ) )
	{
		BOOST_THROW_EXCEPTION( exception::ImageNotReady( ) );
	}
	if( this->_dstRef->getRowBytes( ) == 0 )
	{
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes( ) );
	}
	// _dstPixelRod = _dst->getRegionOfDefinition(); // bug in nuke, returns bounds
	_dstRefPixelRod = _clipDstRef->getPixelRod( args.time, args.renderScale );
	this->_dstRefView = tuttle::plugin::getView<View > ( this->_dstRef.get( ), _dstRefPixelRod );

	// analyse srcRef and dstRef
	pixel_zeros_t<Pixel > ( )( _srcRefAverage );
	pixel_zeros_t<Pixel > ( )( _dstRefAverage );
	pixel_zeros_t<Pixel > ( )( _srcRefDeviation );
	pixel_zeros_t<Pixel > ( )( _dstRefDeviation );

	computeAverage( this->_srcRefView, _srcRefAverage, _srcRefDeviation );
	computeAverage( this->_dstRefView, _dstRefAverage, _dstRefDeviation );
	//TUTTLE_COUT_VAR4(_srcRefAverage[0], _srcRefDeviation[0], _dstRefAverage[0], _dstRefDeviation[0]);

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
	const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
								procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
							  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x, procWindowSize.y );

	// fill dst: modify src using analyse of srcRef and dstRef differences
	transform_pixels_progress( src, dst, ColorParams<View > ( _srcRefAverage, _dstRefAverage, _srcRefDeviation, _dstRefDeviation ), *this );
}

}
}
}


