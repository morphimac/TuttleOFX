#ifndef _TUTTLE_PLUGIN_WARP_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_WARP_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace warp {

const static std::size_t kMaxNbPoints = 10;

const static std::string kParamMethod = "method";
const static std::string kParamMethodCreation = "creation";
const static std::string kParamMethodDelete = "delete";
const static std::string kParamMethodMove = "move";
enum EParamMethod
{
        eParamMethodCreation = 0,
        eParamMethodDelete,
        eParamMethodMove
};

const static std::string kParamOverlay = "overlay";
const static std::string kParamNbPoints = "nbPoints";

const static std::string kParamGroupIn = "groupIn";
const static std::string kParamPointIn = "pIn";
const static std::string kParamOverlayIn = "overlayIn";
const static std::string kParamOverlayInColor = "overlayInColor";

const static std::string kParamGroupOut = "groupOut";
const static std::string kParamPointOut = "pOut";
const static std::string kParamOverlayOut = "overlayOut";
const static std::string kParamOverlayOutColor = "overlayOutColor";
}
}
}

#endif
