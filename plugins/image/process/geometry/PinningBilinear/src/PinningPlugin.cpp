#include "PinningPlugin.hpp"
#include "PinningProcess.hpp"
#include "PinningDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace pinning {


PinningPlugin::PinningPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
	_clipSrc 		= fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst 		= fetchClip( kOfxImageEffectOutputClipName );

	_center			= fetchDouble2DParam( kParamCenter );

	_groupDisplayParams	= fetchGroupParam( kParamDisplayOptions );
	_gridOverlay		= fetchBooleanParam( kParamGridOverlay );

	_gridCenter		= fetchDouble2DParam( kParamGridCenter );
	_gridScale		= fetchDouble2DParam( kParamGridScale );



    for( unsigned int i = 0; i<4; ++i )
    {
    	_paramSrc[i] = fetchDouble2DParam( kParamPointSource + boost::lexical_cast<std::string>(i) );
    	//boost::lexical_cast<std::string>(i)  -> converti un double en chaine de caractere
    	_paramDst[i] = fetchDouble2DParam( kParamPointDestination + boost::lexical_cast<std::string>(i) );
    }
}

PinningProcessParams<PinningPlugin::Scalar> PinningPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	PinningProcessParams<Scalar> params;
	return params;
}

void PinningPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kParamHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kParamHelpString );
    }
}

bool PinningPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	PinningProcessParams<Scalar> params = getProcessParams();
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
void PinningPlugin::render( const OFX::RenderArguments &args )
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
                PinningProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                PinningProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                PinningProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
	default:
	{
		TUTTLE_COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
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
                PinningProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                PinningProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                PinningProcess<gray32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				TUTTLE_COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
	else
	{
		TUTTLE_COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
	}
}

}
}
}
