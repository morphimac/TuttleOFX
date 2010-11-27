#ifndef _TUTTLE_PLUGIN_TRANSFORMAFFINE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TRANSFORMAFFINE_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace transformAffine {

template<typename Scalar>
struct TransformAffineProcessParams
{
	boost::numeric::ublas::bounded_matrix<double,3,3> _matrix;
};

/**
 * @brief TransformAffine plugin
 */
class TransformAffinePlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    TransformAffinePlugin( OfxImageEffectHandle handle );

public:
	TransformAffineProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc; ///< Source image clip
    OFX::Clip* _clipDst; ///< Destination image clip

    OFX::Double2DParam* _paramPointIn0;
    OFX::Double2DParam* _paramPointIn1;
    OFX::Double2DParam* _paramPointIn2;

    OFX::Double2DParam* _paramPointOut0;
    OFX::Double2DParam* _paramPointOut1;
    OFX::Double2DParam* _paramPointOut2;

	OFX::Double3DParam* _paramMatrixRow0;
	OFX::Double3DParam* _paramMatrixRow1;
	OFX::Double3DParam* _paramMatrixRow2;
	
	OFX::BooleanParam*  _paramOverlay;
	OFX::BooleanParam*  _paramOverlayIn;
	OFX::BooleanParam*  _paramOverlayOut;
};

}
}
}

#endif
