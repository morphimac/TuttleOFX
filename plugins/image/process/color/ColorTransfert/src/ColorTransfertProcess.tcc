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

#include <boost/numeric/ublas/vector.hpp>

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

	Pixel average;
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
double ColorTransfertProcess<View>::computeAverage( const View& image )
{
	ComputeParams<View, boost::gil::bits64f> output;
	pixel_zeros_t<Pixel>( )( output.average );

	typedef typename color_space_type<View>::type Colorspace;
	typedef pixel<boost::gil::bits64f, layout<Colorspace> > CPixel;
	CPixel sum;
	pixel_zeros_t<CPixel>( )( sum );
	const std::size_t nbPixels = image.width() * image.height();

	for( int y = 0; y < image.height(); ++y )
	{
		typename View::x_iterator src_it = image.x_at( 0, y );
		for( int x = 0; x < image.width(); ++x, ++src_it )
		{
			CPixel pix;
			pixel_assigns_t<Pixel, CPixel>( )( * src_it, pix ); 
			pixel_plus_assign_t<CPixel, CPixel>( )( pix, sum );
		}
	}
	output.average  = pixel_divides_scalar_t<CPixel, double>() ( sum, nbPixels );
	
	return output.average[0];
}

template<class View>
void ColorTransfertProcess<View>::vectorRender( const View& imageSrc, const View& imageDst, const View& source, const View& output, double srcAverage, double dstAverage ){
	using namespace boost::numeric::ublas;
	const std::size_t nbPixels = imageSrc.width() * imageSrc.height();
	vector<Pixel> vec;
	vec.resize( nbPixels );
	int cptPixels = 0;

	// calcul du vecteur entre srcRef et dstRef
	for( int y = 0; y < imageSrc.height(); ++y )
	{
		typename View::x_iterator src_it = imageSrc.x_at( 0, y );
		typename View::x_iterator dst_it = imageDst.x_at( 0, y );

		for( int x = 0; x < imageSrc.width(); ++x, ++src_it, ++dst_it, ++cptPixels )
		{
			Pixel pix;
			pixel_assigns_t<Pixel, Pixel>( )( * dst_it, pix);
			pixel_minus_assign_t<Pixel, Pixel>( ) ( pix, ( * src_it) ); 
			vec( cptPixels ) = pix;
		}

                /*TUTTLE_COUT_VAR2(imageDst.x_at(367, y), imageDst.x_at(123456, y));
		TUTTLE_COUT_VAR2(imageSrc.x_at(367, y), imageSrc.x_at(123456, y));
                TUTTLE_COUT_VAR2(vec(367)[0], vec(123456)[0]);*/
	}
	
	cptPixels = 0;
	// application du vecteur 
	for( int y = 0; y < imageSrc.height(); ++y )
	{
		typename View::x_iterator src_it = source.x_at( 0, y );
		typename View::x_iterator dst_it = output.x_at( 0, y );

		for( int x = 0; x < imageSrc.width(); ++x, ++src_it, ++dst_it, ++cptPixels )
		{
			Pixel pix;
			pixel_assigns_t<Pixel, Pixel>( )( * src_it, pix);
			pixel_plus_assign_t<Pixel, Pixel>( ) ( pix, vec( cptPixels ) ); 
			//pixel_minus_assign_t<Pixel, Pixel>( ) ( pix, srcAverage );
			pixel_plus_assign_t<Pixel, Pixel>( ) ( * dst_it, pix ); 	
		}
	}	
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
	double srcRefAverage = 0.0, dstRefAverage = 0.0;

	srcRefAverage = computeAverage( this->_srcRefView );
        dstRefAverage = computeAverage( this->_dstRefView );

	TUTTLE_COUT_VAR2(srcRefAverage, dstRefAverage);

	// now analyse the differences
        vectorRender( this->_srcRefView, this->_dstRefView, this->_srcView, this->_dstView, srcRefAverage, dstRefAverage );
	
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


