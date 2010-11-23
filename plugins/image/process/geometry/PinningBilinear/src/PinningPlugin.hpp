#ifndef _TUTTLE_PLUGIN_PINNING_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_PINNING_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace pinning {

template<typename Scalar>
struct PinningProcessParams
{
	
};

/**
 * @brief Pinning plugin
 */
class PinningPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
	typedef boost::gil::point2<double> Point2;

public:
    PinningPlugin( OfxImageEffectHandle handle );

public:
	PinningProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;
	void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );
	void render( const OFX::RenderArguments &args );
	//bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	//void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc; ///< Source image clip
	OFX::Clip* _clipDst; ///< Destination image clip

	OFX::Double2DParam* _center; ///< center coordonnates

	OFX::GroupParam* _groupDisplayParams; ///< groupe des options d'affichage
	OFX::BooleanParam*  _gridOverlay; ///< grid overlay

	OFX::Double2DParam* _gridCenter; ///< Centre de la grille
	OFX::Double2DParam* _gridScale; ///< Echelle de la grille

	OFX::Double2DParam* _paramSrc[4];
	OFX::Double2DParam* _paramDst[4];
};

}
}
}

#endif
