#ifndef _TUTTLE_PLUGIN_TRANSFORMAFFINE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TRANSFORMAFFINE_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace transformAffine {

template<typename Scalar>
struct TransformAffineProcessParams
{
	
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
    OFX::Clip* _srcClip; ///< Source image clip
    OFX::GroupParamDescriptor* groupSource;
    OFX::Double2DParamDescriptor* pointEntree1;
    OFX::Double2DParamDescriptor* pointEntree2;
    OFX::Double2DParamDescriptor* pointEntree3;

    OFX::Clip* _srcRefClip; ///< source ref image clip

    OFX::Clip* _dstClip; ///< Destination image clip
    OFX::GroupParamDescriptor* groupSortie;
    OFX::Double2DParamDescriptor* pointSortie1;
    OFX::Double2DParamDescriptor* pointSortie2;
    OFX::Double2DParamDescriptor* pointSortie3;

    OFX::GroupParam* _groupDisplayParams; ///< group of all overlay options (don't modify the output image)
    OFX::BooleanParam*  _gridOverlay; ///< grid overlay
    OFX::Double2DParam* _gridCenter; ///< grid center
    OFX::BooleanParam*  _gridCenterOverlay; ///< grid center overlay
    OFX::Double2DParam* _gridScale; ///< grid scale

    OFX::Double2DParam* _center; ///< center coordonnates
    OFX::BooleanParam*  _centerOverlay; ///< lens center overlay

    static OfxRectD _dstRoi;
    static OfxRectD _srcRoi;
    static OfxRectD _srcRealRoi;

};

}
}
}

#endif
