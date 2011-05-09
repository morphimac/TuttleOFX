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
        //std::vector< point2<Scalar> > _inPointsCurve2;
        //std::vector< point2<Scalar> > _outPointsCurve2;

	std::vector< point2<Scalar> > _tgtPointsIn;
	std::vector< point2<Scalar> > _tgtPointsOut;
        //std::vector< point2<Scalar> > _tgtPointsInCurve2;
        //std::vector< point2<Scalar> > _tgtPointsOutCurve2;

	std::vector< point2<Scalar> > _buildPoints;

	std::vector< point2<Scalar> > _bezierIn;
	std::vector< point2<Scalar> > _bezierOut;
        //std::vector< point2<Scalar> > _bezierInCurve2;
        //std::vector< point2<Scalar> > _bezierOutCurve2;

        bool _activateWarp;
	double _rigiditeTPS;
	std::size_t _nbPoints;
        //std::size_t _nbPointsCurve2;
	double _transition;

	EParamMethod _method;
        EParamCurve _curve;
};

/**
 * @brief Warp plugin
 */
class WarpPlugin : public OFX::ImageEffect
{

public:
	typedef double Scalar;
	typedef boost::gil::point2<Scalar> Point2;
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
	OFX::Clip* _clipSrcB; ///< Source image clip
	OFX::Clip* _clipDst; ///< Destination image clip

	OFX::BooleanParam* _paramOverlay;
	OFX::BooleanParam* _paramInverse;
	OFX::PushButtonParam* _paramReset;
        OFX::PushButtonParam* _paramSetKey;
	OFX::ChoiceParam* _paramMethod;
        OFX::ChoiceParam* _paramCurve;

	OFX::IntParam* _paramNbPoints;
        //OFX::IntParam* _paramNbPointsCurve2;
	OFX::DoubleParam* _transition;

	OFX::GroupParam* _paramGroupSettings;
	OFX::DoubleParam* _paramRigiditeTPS;
	OFX::IntParam* _paramNbPointsBezier;

	//In
	OFX::GroupParam* _paramGroupIn;
	boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointIn;

	OFX::BooleanParam* _paramOverlayIn;
	OFX::RGBParam* _paramOverlayInColor;

        //In Curve2
        /*OFX::GroupParam* _paramGroupInCurve2;
        boost::array<OFX::Double2DParam*, kMaxNbPoints/2> _paramPointInCurve2;

        OFX::BooleanParam* _paramOverlayInCurve2;
        OFX::RGBParam* _paramOverlayInColorCurve2;*/

	//Out
	OFX::GroupParam* _paramGroupOut;
        boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointOut;

	OFX::BooleanParam* _paramOverlayOut;
	OFX::RGBParam* _paramOverlayOutColor;

        //Out Curve2
        /*OFX::GroupParam* _paramGroupOutCurve2;
        boost::array<OFX::Double2DParam*, kMaxNbPoints/2> _paramPointOutCurve2;

        OFX::BooleanParam* _paramOverlayOutCurve2;
        OFX::RGBParam* _paramOverlayOutColorCurve2;*/

	//Tgt In
	OFX::GroupParam* _paramGroupTgtIn;
	boost::array<OFX::Double2DParam*, 2 * kMaxNbPoints> _paramPointTgtIn;

	OFX::BooleanParam* _paramOverlayTgtIn;
	OFX::RGBParam* _paramOverlayTgtInColor;

        //Tgt In Curve2
        /*OFX::GroupParam* _paramGroupTgtInCurve2;
        boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointTgtInCurve2;

        OFX::BooleanParam* _paramOverlayTgtInCurve2;
        OFX::RGBParam* _paramOverlayTgtInColorCurve2;*/

	//Tgt Out
	OFX::GroupParam* _paramGroupTgtOut;
	boost::array<OFX::Double2DParam*, 2 * kMaxNbPoints> _paramPointTgtOut;

	OFX::BooleanParam* _paramOverlayTgtOut;
	OFX::RGBParam* _paramOverlayTgtOutColor;

        //Tgt Out Curve2
        /*OFX::GroupParam* _paramGroupTgtOutCurve2;
        boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointTgtOutCurve2;

        OFX::BooleanParam* _paramOverlayTgtOutCurve2;
        OFX::RGBParam* _paramOverlayTgtOutColorCurve2;*/

private:
	OFX::InstanceChangedArgs _instanceChangedArgs;
};

}
}
}

#endif
