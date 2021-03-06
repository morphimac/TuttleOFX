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
	return op( src );
}

template<class View>
WarpProcess<View>::WarpProcess( WarpPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	_clipSrcB = effect.fetchClip( kClipSourceB );
}

template<class View>
void WarpProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

	if( this->_clipSrcB->isConnected( ) )
	{
		// srcB initialization
		this->_srcB.reset( _clipSrcB->fetchImage( args.time ) );
		if( !this->_srcB.get( ) )
		{
			BOOST_THROW_EXCEPTION( exception::ImageNotReady( ) );
		}
		if( this->_srcB->getRowBytes( ) == 0 )
		{
			BOOST_THROW_EXCEPTION( exception::WrongRowBytes( ) );
		}
		// _srcBPixelRod = _srcB->getRegionOfDefinition(); // bug in nuke, returns bounds
		_srcBPixelRod = _clipSrcB->getPixelRod( args.time, args.renderScale );
		this->_srcBView = tuttle::plugin::getView<View > ( this->_srcB.get( ), _srcBPixelRod );
		_tpsB.setup( _params._bezierOut, _params._bezierIn, _params._rigiditeTPS, _params._activateWarp, this->_srcBPixelRod.x2 - this->_srcBPixelRod.x1, this->_srcBPixelRod.y2 - this->_srcBPixelRod.y1, ( 1.0 - _params._transition ) );
	}
	//TPS_Morpher<Scalar> tps( _params._inPoints, _params._outPoints , _params._rigiditeTPS);
	_tpsA.setup( _params._bezierIn, _params._bezierOut, _params._rigiditeTPS, _params._activateWarp, this->_srcPixelRod.x2 - this->_srcPixelRod.x1, this->_srcPixelRod.y2 - this->_srcPixelRod.y1, _params._transition );
	//TUTTLE_TCOUT_VAR( _params._rigiditeTPS );
	//TUTTLE_TCOUT_VAR( _params._activateWarp );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void WarpProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	const OfxRectI procWindowOutput = translateRegion( procWindowRoW, this->_dstPixelRod );
	const OfxRectI procWindowSrcA = translateRegion( procWindowRoW, this->_srcPixelRod );
	const OfxRectI procWindowSrcB = translateRegion( procWindowRoW, this->_srcBPixelRod );

	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
								procWindowRoW.y2 - procWindowRoW.y1 };

	if( this->_clipSrcB->isConnected( ) )
	{
		Image imgA( procWindowSize.x, procWindowSize.y );
		Image imgB( procWindowSize.x, procWindowSize.y );
		View viewA = subimage_view(
						view(imgA),
						this->_srcPixelRod.x1-procWindowRoW.x1, this->_srcPixelRod.y1-procWindowRoW.y1,
						this->_srcView.width(), this->_srcView.height() );
		View viewB = subimage_view(
						view(imgB),
						this->_srcBPixelRod.x1-procWindowRoW.x1, this->_srcBPixelRod.y1-procWindowRoW.y1,
						this->_srcBView.width(), this->_srcBView.height() );

		resample_pixels_progress<bilinear_sampler > ( this->_srcView, viewA, _tpsA, procWindowSrcA, this );
		resample_pixels_progress<bilinear_sampler > ( this->_srcBView, viewB, _tpsB, procWindowSrcB, this );

		//fondu entre imgA et imgB = this->_dstView FAITEALAMAIN
		View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
								  procWindowSize.x, procWindowSize.y );
		transform_pixels_progress(
								   view( imgA ),
								   view( imgB ),
								   dst,
								   pixel_merge_t<Pixel > ( _params._transition ),
								   *this );
	}
	else
	{
		View dst = subimage_view(
						this->_dstView,
						this->_srcPixelRod.x1-this->_dstPixelRod.x1, this->_srcPixelRod.y1-this->_dstPixelRod.y1,
						this->_srcView.width(), this->_srcView.height() );
		resample_pixels_progress<bilinear_sampler > ( this->_srcView, dst, _tpsA, procWindowSrcA, this );
	}
}

}
}
}
