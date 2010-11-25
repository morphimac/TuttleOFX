#include "TransformAffinePlugin.hpp"
#include "TransformAffineProcess.hpp"
#include "TransformAffineDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace transformAffine {

    OfxRectD TransformAffinePlugin::_dstRoi     = { 0, 0, 0, 0 };
    OfxRectD TransformAffinePlugin::_srcRoi     = { 0, 0, 0, 0 };
    OfxRectD TransformAffinePlugin::_srcRealRoi = { 0, 0, 0, 0 };

TransformAffinePlugin::TransformAffinePlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );
    _srcRefClip = fetchClip( kClipOptionalSourceRef );
    _groupDisplayParams   = fetchGroupParam( kParamDisplayOptions );
    _gridOverlay          = fetchBooleanParam( kParamGridOverlay );
    _gridCenter           = fetchDouble2DParam( kParamGridCenter );
    _gridCenterOverlay    = fetchBooleanParam( kParamGridCenterOverlay );
    _gridScale            = fetchDouble2DParam( kParamGridScale );
    _center               = fetchDouble2DParam( kParamCenter );
    _centerOverlay        = fetchBooleanParam( kParamCenterOverlay );
}

TransformAffineProcessParams<TransformAffinePlugin::Scalar> TransformAffinePlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	TransformAffineProcessParams<Scalar> params;
	return params;
}

void TransformAffinePlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{


}

bool TransformAffinePlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	TransformAffineProcessParams<Scalar> params = getProcessParams();
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
void TransformAffinePlugin::render( const OFX::RenderArguments &args )
{
	using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _dstClip->getPixelDepth( );
    OFX::EPixelComponent dstComponents = _dstClip->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                TransformAffineProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                TransformAffineProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                TransformAffineProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
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
                TransformAffineProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                TransformAffineProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                TransformAffineProcess<gray32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
	}
}


}
}
}
