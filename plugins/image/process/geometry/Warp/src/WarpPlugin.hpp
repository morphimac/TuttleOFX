#ifndef _TUTTLE_PLUGIN_WARP_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_WARP_PLUGIN_HPP_

#include "WarpDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace warp {

using namespace boost::gil;
template<typename Scalar>
struct WarpProcessParams
{
	//std::vector<point2<double> > inPoints;
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
    	// do not need to delete these, the ImageEffect is managing them for us
    	OFX::Clip* _clipSrc; ///< Source image clip
   	OFX::Clip* _clipDst; ///< Destination image clip

	OFX::BooleanParam*  _paramOverlay;

    	OFX::GroupParam* _paramGroupIn;
    	OFX::Double2DParam* _paramPointIn[nbPoints];

	OFX::BooleanParam*  _paramOverlayIn;
	OFX::RGBParam*  _paramOverlayInColor;

    	OFX::GroupParam* _paramGroupOut;
    	OFX::Double2DParam* _paramPointOut[nbPoints];

	OFX::BooleanParam*  _paramOverlayOut;
	OFX::RGBParam*  _paramOverlayOutColor;
};

}
}
}

#endif
