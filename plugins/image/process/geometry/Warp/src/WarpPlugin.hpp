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
	std::vector< point2<Scalar> > _inPoints;
	std::vector< point2<Scalar> > _outPoints;
        std::vector< point2<Scalar> > _tgtPointsIn;
        std::vector< point2<Scalar> > _tgtPointsOut;
	std::vector< point2<Scalar> > _buildPoints;

	EParamMethod _method;
};

/**
 * @brief Warp plugin
 */
class WarpPlugin : public OFX::ImageEffect
{
public:
	typedef double Scalar;
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
        OFX::BooleanParam*  _paramInverse;
        OFX::PushButtonParam* _paramReset;
        OFX::ChoiceParam*  _paramMethod;

	OFX::IntParam*  _paramNbPoints;

        //In
	OFX::GroupParam* _paramGroupIn;
	boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointIn;

	OFX::BooleanParam*  _paramOverlayIn;
	OFX::RGBParam*  _paramOverlayInColor;

        //Out
	OFX::GroupParam* _paramGroupOut;
	boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointOut;

	OFX::BooleanParam*  _paramOverlayOut;
	OFX::RGBParam*  _paramOverlayOutColor;

        //Tgt In
        OFX::GroupParam* _paramGroupTgtIn;
        boost::array<OFX::Double2DParam*, 2*kMaxNbPoints> _paramPointTgtIn;

        OFX::BooleanParam*  _paramOverlayTgtIn;
        OFX::RGBParam*  _paramOverlayTgtInColor;

        //Tgt Out
        OFX::GroupParam* _paramGroupTgtOut;
        boost::array<OFX::Double2DParam*, 2*kMaxNbPoints> _paramPointTgtOut;

        OFX::BooleanParam*  _paramOverlayTgtOut;
        OFX::RGBParam*  _paramOverlayTgtOutColor;

private:
	OFX::InstanceChangedArgs _instanceChangedArgs;
};

}
}
}

#endif
