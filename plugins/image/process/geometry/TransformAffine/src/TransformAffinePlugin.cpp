#include "TransformAffinePlugin.hpp"
#include "TransformAffineProcess.hpp"
#include "TransformAffineDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace tuttle {
namespace plugin {
namespace transformAffine {

TransformAffinePlugin::TransformAffinePlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramPointIn0 = fetchDouble2DParam( kParamPointIn + "0" );
	_paramPointIn1 = fetchDouble2DParam( kParamPointIn + "1" );
	_paramPointIn2 = fetchDouble2DParam( kParamPointIn + "2" );

	_paramPointOut0 = fetchDouble2DParam( kParamPointOut + "0" );
	_paramPointOut1 = fetchDouble2DParam( kParamPointOut + "1" );
	_paramPointOut2 = fetchDouble2DParam( kParamPointOut + "2" );

	_paramMatrixRow0 = fetchDouble3DParam( kParamMatrixRow + "0" );
	_paramMatrixRow1 = fetchDouble3DParam( kParamMatrixRow + "1" );
	_paramMatrixRow2 = fetchDouble3DParam( kParamMatrixRow + "2" );

    _paramOverlay          = fetchBooleanParam( kParamOverlay );
    _paramOverlayIn        = fetchBooleanParam( kParamOverlayIn );
    _paramOverlayOut       = fetchBooleanParam( kParamOverlayOut );
}

TransformAffineProcessParams<TransformAffinePlugin::Scalar> TransformAffinePlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	TransformAffineProcessParams<Scalar> params;
	_paramMatrixRow0->getValue( params._matrix(0, 0), params._matrix(0, 1), params._matrix(0, 2) );
	_paramMatrixRow1->getValue( params._matrix(1, 0), params._matrix(1, 1), params._matrix(1, 2) );
	_paramMatrixRow2->getValue( params._matrix(2, 0), params._matrix(2, 1), params._matrix(2, 2) );
	return params;
}

void TransformAffinePlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	using namespace boost::numeric::ublas;
	if( boost::starts_with(paramName, kParamPointIn) ||
	    boost::starts_with(paramName, kParamPointOut) )
	{
		bounded_matrix<double,3,3> matrix;

		////////////////////////////////////////
		/// @todo: compute "matrix" from input/output points -> "_paramPoint*"
		matrix = identity_matrix<double>(3);
		////////////////////////////////////////
		
		_paramMatrixRow0->setValue( matrix(0, 0), matrix(1, 0), matrix(2, 0) );
		_paramMatrixRow1->setValue( matrix(0, 1), matrix(1, 1), matrix(2, 1) );
		_paramMatrixRow2->setValue( matrix(0, 2), matrix(1, 2), matrix(2, 2) );
	}
}

bool TransformAffinePlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	using namespace boost::numeric::ublas;
	TransformAffineProcessParams<Scalar> params = getProcessParams();

	// is the transformation matrix is an identity matrix the node is identity,
	// we perform no modification on the input image.
	if( norm_inf( params._matrix - identity_matrix<double>(3) ) == 0 )
	{
		identityClip = _clipSrc;
		identityTime = args.time;
		return true;
	}
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
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );

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
