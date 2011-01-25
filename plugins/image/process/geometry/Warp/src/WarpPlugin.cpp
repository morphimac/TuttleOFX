#include "WarpPlugin.hpp"
#include "WarpProcess.hpp"
#include "WarpDefinitions.hpp"

#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include <cstddef>

namespace tuttle {
namespace plugin {
namespace warp {

WarpPlugin::WarpPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramOverlay       = fetchBooleanParam( kParamOverlay );

	_paramNbPoints       = fetchIntParam( kParamNbPoints );

	_paramGroupIn        = fetchGroupParam( kParamGroupIn );
	for( std::size_t cptIn = 0; cptIn < kMaxNbPoints; ++cptIn )
	{
		_paramPointIn[cptIn] = fetchDouble2DParam( kParamPointIn + boost::lexical_cast<std::string>(cptIn) );
	}
	_paramOverlayIn      = fetchBooleanParam( kParamOverlayIn );
	_paramOverlayInColor = fetchRGBParam( kParamOverlayInColor );

	_paramGroupOut        = fetchGroupParam( kParamGroupIn );
	for( std::size_t cptOut = 0; cptOut < kMaxNbPoints; ++cptOut )
	{
		_paramPointOut[cptOut] = fetchDouble2DParam( kParamPointOut + boost::lexical_cast<std::string>(cptOut) );
	}
	_paramOverlayOut      = fetchBooleanParam( kParamOverlayOut );
	_paramOverlayOutColor = fetchRGBParam( kParamOverlayOutColor );

	_instanceChangedArgs.time          = 0;
	_instanceChangedArgs.renderScale.x = 1;
	_instanceChangedArgs.renderScale.y = 1;
	_instanceChangedArgs.reason        = OFX::eChangePluginEdit;
	changedParam( _instanceChangedArgs, kParamNbPoints ); // init IsSecret property for each pair of points parameters
}

WarpProcessParams<WarpPlugin::Scalar> WarpPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	WarpProcessParams<Scalar> params;
	std::size_t size = _paramNbPoints->getValue();

	for( std::size_t i = 0; i < size; ++i )
	{
		point2<double> pIn = ofxToGil( _paramPointIn[i]->getValue() );
		params._inPoints.push_back( pIn );
		point2<double> pOut = ofxToGil( _paramPointOut[i]->getValue() );
		params._outPoints.push_back( pOut );
	}

	return params;
}

void WarpPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( boost::starts_with( paramName, kParamPointIn ) ||
	    boost::starts_with( paramName, kParamPointOut ) ||
		paramName == kParamNbPoints )
	{
		std::size_t size = _paramNbPoints->getValue();
		std::size_t i = 0;
		for(; i < size; ++i )
		{
			_paramPointIn[i]->setIsSecretAndDisabled( false );
			_paramPointOut[i]->setIsSecretAndDisabled( false );
		}
		for( ; i < kMaxNbPoints; ++i )
		{
			_paramPointIn[i]->setIsSecretAndDisabled( true );
			_paramPointOut[i]->setIsSecretAndDisabled( true );
		}
	}
}

//bool WarpPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	WarpProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}
//
//void WarpPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	WarpProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool WarpPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	WarpProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void WarpPlugin::render( const OFX::RenderArguments &args )
{
	using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                WarpProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                WarpProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                WarpProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				//COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                WarpProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                WarpProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                WarpProcess<gray32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				//COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
	else
	{
		//COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
	}
}


}
}
}
