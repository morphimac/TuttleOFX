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
        _paramInverse       = fetchBooleanParam( kParamInverse );

        _paramMethod        = fetchChoiceParam( kParamMethod );
	_paramNbPoints       = fetchIntParam( kParamNbPoints );

        //Param IN
	_paramGroupIn        = fetchGroupParam( kParamGroupIn );
	for( std::size_t cptIn = 0; cptIn < kMaxNbPoints; ++cptIn )
	{
                _paramPointIn[cptIn] = fetchDouble2DParam( kParamPointIn + boost::lexical_cast<std::string>(cptIn) );
        }
        _paramOverlayIn      = fetchBooleanParam( kParamOverlayIn );
	_paramOverlayInColor = fetchRGBParam( kParamOverlayInColor );

        //Param OUT
	_paramGroupOut        = fetchGroupParam( kParamGroupIn );
	for( std::size_t cptOut = 0; cptOut < kMaxNbPoints; ++cptOut )
	{
                _paramPointOut[cptOut] = fetchDouble2DParam( kParamPointOut + boost::lexical_cast<std::string>(cptOut) );
	}
        _paramOverlayOut      = fetchBooleanParam( kParamOverlayOut );
	_paramOverlayOutColor = fetchRGBParam( kParamOverlayOutColor );

        //Param TGT IN
        _paramGroupTgtIn        = fetchGroupParam( kParamGroupTgtIn );
        for( std::size_t cptTgtIn = 0; cptTgtIn < kMaxNbPoints; ++cptTgtIn )
        {
                _paramPointTgtIn[2*cptTgtIn] = fetchDouble2DParam( kParamPointTgtIn + boost::lexical_cast<std::string>(2*cptTgtIn) );
                _paramPointTgtIn[2*cptTgtIn+1] = fetchDouble2DParam( kParamPointTgtIn + boost::lexical_cast<std::string>(2*cptTgtIn+1) );
        }
        _paramOverlayTgtIn      = fetchBooleanParam( kParamOverlayTgtIn );
        _paramOverlayTgtInColor = fetchRGBParam( kParamOverlayTgtInColor );

        //Param TGT Out
        _paramGroupTgtOut        = fetchGroupParam( kParamGroupTgtOut );
        for( std::size_t cptTgtOut = 0; cptTgtOut < kMaxNbPoints; ++cptTgtOut )
        {
                _paramPointTgtOut[2*cptTgtOut] = fetchDouble2DParam( kParamPointTgtOut + boost::lexical_cast<std::string>(2*cptTgtOut) );
                _paramPointTgtOut[2*cptTgtOut+1] = fetchDouble2DParam( kParamPointTgtOut + boost::lexical_cast<std::string>(2*cptTgtOut+1) );
        }
        _paramOverlayTgtOut      = fetchBooleanParam( kParamOverlayTgtOut );
        _paramOverlayTgtOutColor = fetchRGBParam( kParamOverlayTgtOutColor );

        //Param speciaux
	_instanceChangedArgs.time          = 0;
	_instanceChangedArgs.renderScale.x = 1;
	_instanceChangedArgs.renderScale.y = 1;
        _instanceChangedArgs.reason        = OFX::eChangePluginEdit;
        changedParam( _instanceChangedArgs, kParamNbPoints ); // init IsSecret property for each pair of points parameter
}

WarpProcessParams<WarpPlugin::Scalar> WarpPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	WarpProcessParams<Scalar> params;
        std::size_t size = _paramNbPoints->getValue();

        if(!_paramInverse->getValue())
        {
                for( std::size_t i = 0; i < size; ++i )
                {
                        point2<double> pIn = ofxToGil( _paramPointIn[i]->getValue() );
                        params._inPoints.push_back( pIn );

                        point2<double> pOut = ofxToGil( _paramPointOut[i]->getValue() );
                        params._outPoints.push_back( pOut );

                        point2<double> pTgtIn  = ofxToGil( _paramPointTgtIn[2*i]->getValue() );
                        point2<double> pTgtIn2 = ofxToGil( _paramPointTgtIn[2*i+1]->getValue() );
                        params._tgtPointsIn.push_back( pTgtIn );
                        params._tgtPointsIn.push_back( pTgtIn2 );

                        point2<double> pTgtOut  = ofxToGil( _paramPointTgtOut[2*i]->getValue() );
                        point2<double> pTgtOut2 = ofxToGil( _paramPointTgtOut[2*i+1]->getValue() );
                        params._tgtPointsOut.push_back( pTgtOut );
                        params._tgtPointsOut.push_back( pTgtOut2 );

                        params._method        = static_cast<EParamMethod>( _paramMethod->getValue() );
                }
                return params;
        }
        else
        {
                for( std::size_t i = 0; i < size; ++i )
                {
                        point2<double> pIn = ofxToGil( _paramPointOut[i]->getValue() );
                        params._inPoints.push_back( pIn );

                        point2<double> pOut = ofxToGil( _paramPointIn[i]->getValue() );
                        params._outPoints.push_back( pOut );

                        point2<double> pTgtIn  = ofxToGil( _paramPointTgtOut[2*i]->getValue() );
                        point2<double> pTgtIn2 = ofxToGil( _paramPointTgtOut[2*i+1]->getValue() );
                        params._tgtPointsIn.push_back( pTgtIn );
                        params._tgtPointsIn.push_back( pTgtIn2 );

                        point2<double> pTgtOut  = ofxToGil( _paramPointTgtIn[2*i]->getValue() );
                        point2<double> pTgtOut2 = ofxToGil( _paramPointTgtIn[2*i+1]->getValue() );
                        params._tgtPointsOut.push_back( pTgtOut );
                        params._tgtPointsOut.push_back( pTgtOut2 );

                        params._method        = static_cast<EParamMethod>( _paramMethod->getValue() );
                }
                return params;
        }
}

void WarpPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{        if( boost::starts_with( paramName, kParamPointIn ) ||
	    boost::starts_with( paramName, kParamPointOut ) ||
            boost::starts_with( paramName, kParamPointTgtIn ) ||
            boost::starts_with( paramName, kParamPointTgtOut ) ||
		paramName == kParamNbPoints )
        {
                switch( static_cast < EParamMethod >( _paramMethod->getValue() ) )
                {
                        case eParamMethodCreation:
                        {
                                std::size_t size = _paramNbPoints->getValue();
                                std::size_t i = 0;
                                for(; i < size; ++i )
                                {
                                        _paramPointIn[i]->setIsSecretAndDisabled( false );
                                        _paramPointOut[i]->setIsSecretAndDisabled( false );
                                        _paramPointTgtIn[2*i]->setIsSecretAndDisabled( false );
                                        _paramPointTgtIn[2*i+1]->setIsSecretAndDisabled( false );
                                        _paramPointTgtOut[2*i]->setIsSecretAndDisabled( false );
                                        _paramPointTgtOut[2*i+1]->setIsSecretAndDisabled( false );
                                }
                                for( ; i < kMaxNbPoints; ++i )
                                {
                                        _paramPointIn[i]->setIsSecretAndDisabled( true );
                                        _paramPointOut[i]->setIsSecretAndDisabled( true );
                                        _paramPointTgtIn[2*i]->setIsSecretAndDisabled( true );
                                        _paramPointTgtIn[2*i+1]->setIsSecretAndDisabled( true );
                                        _paramPointTgtOut[2*i]->setIsSecretAndDisabled( true );
                                        _paramPointTgtOut[2*i+1]->setIsSecretAndDisabled( true );
                                }
                                break;
                        }
                        case eParamMethodDelete:
                        {
                                break;
                        }
                        case eParamMethodMove:
                        {
                                break;
                        }
                        case eParamMethodReset:
                        {
                                break;
                        }
                }
        }
}

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
