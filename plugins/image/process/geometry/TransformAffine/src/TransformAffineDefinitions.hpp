#ifndef _TUTTLE_PLUGIN_TRANSFORMAFFINE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_TRANSFORMAFFINE_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace transformAffine {


const static std::string kParamGroupIn = "groupIn";
const static std::string kParamPointIn = "pIn";

const static std::string kParamGroupOut = "groupOut";
const static std::string kParamPointOut = "pOut";

const static std::string kParamGroupMatrix = "groupMatrix";
const static std::string kParamMatrixRow = "matrix";

const static std::string kParamOverlay = "overlay";
const static std::string kParamOverlayIn = "overlayIn";
const static std::string kParamOverlayInColor = "overlayInColor";
const static std::string kParamOverlayOut = "overlayOut";
const static std::string kParamOverlayOutColor = "overlayOutColor";

}
}
}

#endif
