#ifndef _TUTTLE_PLUGIN_WARP_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_WARP_PLUGIN_HPP_

#include "WarpDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/array.hpp>

namespace tuttle {
namespace plugin {
namespace warp {

using namespace boost::gil;
template<typename Scalar>
struct WarpProcessParams
{
	std::vector< point2<double> > _inPoints;
	std::vector< point2<double> > _outPoints;
};

/**
 * @brief Warp plugin
 */
class WarpPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    WarpPlugin( OfxImageEffectHandle handle );

public:
	WarpProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );


//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	

public:
	OFX::Clip* _clipSrc; ///< Source image clip
   	OFX::Clip* _clipDst; ///< Destination image clip

	OFX::BooleanParam*  _paramOverlay;

	OFX::IntParam*  _paramNbPoints;

	OFX::GroupParam* _paramGroupIn;
	boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointIn;

	OFX::BooleanParam*  _paramOverlayIn;
	OFX::RGBParam*  _paramOverlayInColor;

	OFX::GroupParam* _paramGroupOut;
	boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointOut;

	OFX::BooleanParam*  _paramOverlayOut;
	OFX::RGBParam*  _paramOverlayOutColor;

private:
	OFX::InstanceChangedArgs _instanceChangedArgs;
};

}
}
}

#endif
