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

const static std::string kParamOverlay = "overlay";
const static std::string kParamInverse = "inverse";


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


}
}
}

#endif
