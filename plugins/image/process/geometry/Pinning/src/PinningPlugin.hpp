#ifndef _TUTTLE_PLUGIN_PINNING_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_PINNING_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>

#include "PinningDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace pinning {

template<typename Scalar>
struct Perspective
{
	boost::numeric::ublas::bounded_matrix<Scalar,3,3> _matrix;
};

template<typename Scalar>
struct Bilinear
{
	boost::numeric::ublas::bounded_matrix<Scalar,2,4> _matrix;
};

template<typename Scalar>
struct PinningProcessParams
{
	Perspective<Scalar> _perspective;
	Bilinear<Scalar> _bilinear;

	EParamInterpolation _interpolation;
	EParamMethod _method;
};

/**
 * @brief Pinning plugin
 */
class PinningPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    PinningPlugin( OfxImageEffectHandle handle );

public:
	PinningProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc; ///< Source image clip
    OFX::Clip* _clipDst; ///< Destination image clip

	OFX::ChoiceParam*  _paramMethod;
	OFX::ChoiceParam*  _paramInterpolation;
	OFX::BooleanParam*  _paramOverlay;

    OFX::GroupParam* _paramGroupIn;
    OFX::Double2DParam* _paramPointIn0;
    OFX::Double2DParam* _paramPointIn1;
    OFX::Double2DParam* _paramPointIn2;
    OFX::Double2DParam* _paramPointIn3;
	OFX::BooleanParam*  _paramOverlayIn;
	OFX::RGBParam*  _paramOverlayInColor;

    OFX::GroupParam* _paramGroupOut;
    OFX::Double2DParam* _paramPointOut0;
    OFX::Double2DParam* _paramPointOut1;
    OFX::Double2DParam* _paramPointOut2;
    OFX::Double2DParam* _paramPointOut3;
	OFX::BooleanParam*  _paramOverlayOut;
	OFX::RGBParam*  _paramOverlayOutColor;

    OFX::GroupParam* _paramGroupPerspMatrix;
	OFX::Double3DParam* _paramPerspMatrixRow0;
	OFX::Double3DParam* _paramPerspMatrixRow1;
	OFX::Double3DParam* _paramPerspMatrixRow2;

    OFX::GroupParam* _paramGroupBilMatrix;
	OFX::Double2DParam* _paramBilMatrixRow0;
	OFX::Double2DParam* _paramBilMatrixRow1;
	OFX::Double2DParam* _paramBilMatrixRow2;
	OFX::Double2DParam* _paramBilMatrixRow3;
};

}
}
}

#endif