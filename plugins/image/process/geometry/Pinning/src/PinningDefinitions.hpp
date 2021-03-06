#ifndef _TUTTLE_PLUGIN_PINNING_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_PINNING_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace pinning {

const static std::string kParamMethod = "method";
const static std::string kParamMethodAffine = "affine";
const static std::string kParamMethodPerspective = "perspective";
const static std::string kParamMethodBilinear = "bilinear";
enum EParamMethod
{
	eParamMethodAffine = 0,
	eParamMethodPerspective,
	eParamMethodBilinear
};

const static std::string kParamInterpolation = "interpolation";
const static std::string kParamInterpolationNearest = "nearest neighbor";
const static std::string kParamInterpolationBilinear = "bilinear";
enum EParamInterpolation
{
	eParamInterpolationNearest = 0,
	eParamInterpolationBilinear,
};

const static std::string kParamSetToCornersIn = "setToCornersIn";
const static std::string kParamSetToCornersOut = "setToCornersOut";
const static std::string kParamOverlay = "overlay";
const static std::string kParamInverse = "inverse";

const static std::string kParamGroupCentre = "groupCentre";
const static std::string kParamPointCentre = "pCentre";
const static std::string kParamOverlayCentre = "overlayCentre";
const static std::string kParamOverlayCentreColor = "overlayCentreColor";

const static std::string kParamGroupIn = "groupIn";
const static std::string kParamPointIn = "pIn";
const static std::string kParamOverlayIn = "overlayIn";
const static std::string kParamOverlayInColor = "overlayInColor";

const static std::string kParamGroupOut = "groupOut";
const static std::string kParamPointOut = "pOut";
const static std::string kParamOverlayOut = "overlayOut";
const static std::string kParamOverlayOutColor = "overlayOutColor";

/*
const static std::string kParamGroupSelect = "groupSelect";
const static std::string kParamPointSelect = "pSelect";
const static std::string kParamOverlaySelect = "overlaySelect";
const static std::string kParamOverlaySelectColor = "overlayInSelect";
*/

const static std::string kParamGroupPerspMatrix = "groupPerspMatrix";
const static std::string kParamPerspMatrixRow = "perpMatrix";
const static std::string kParamGroupBilinearMatrix = "groupBilinearMatrix";
const static std::string kParamBilinearMatrixRow = "bilinearMatrix";


const static std::string kParamManipulatorMode = "manipulatorMode";
const static std::string kParamManipulatorModeTranslate = "translate";
const static std::string kParamManipulatorModeRotate = "rotate";
const static std::string kParamManipulatorModeScale = "scale";
enum EParamManipulatorMode
{
        eParamManipulatorModeTranslate = 0,
        eParamManipulatorModeRotate,
        eParamManipulatorModeScale,
};

}
}
}

#endif
