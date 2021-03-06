#ifndef _TUTTLE_PLUGIN_WARP_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_WARP_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace warp {

const static std::size_t kMaxNbPoints = 20;
//const static std::size_t kPasBezier = 0.2;
static const float lineWidth = 0.5;
static const float pointWidth = 3.0;
static const int seuil = 15;

static const float positionOrigine = -200.0;

//static const int nbCoeffBezier = 50;

//const static std::string kClipSourceA = "A";
const static std::string kClipSourceB = "B";

const static std::string kParamMethod = "method";
const static std::string kParamMethodCreation = "creation";
const static std::string kParamMethodDelete = "delete";
const static std::string kParamMethodMove = "move";
enum EParamMethod
{
        eParamMethodCreation = 0,
        eParamMethodDelete,
        eParamMethodMove,

};


const static std::string kParamOverlay = "overlay";
const static std::string kParamInverse = "inverse";
const static std::string kParamNbPoints = "nbPoints";
const static std::string kParamTransition = "transition";
const static std::string kParamReset = "reset";
const static std::string kParamNextCurve = "nextCurve";
const static std::string kParamSetKey = "setKey";

const static std::string kParamGroupSettings = "settings";
const static std::string kParamNbPointsBezier = "Points Bezier";
const static std::string kParamRigiditeTPS = "Rigidite TPS";

const static std::string kParamGroupIn = "groupIn";
const static std::string kParamPointIn = "pIn";
const static std::string kParamOverlayIn = "overlayIn";
const static std::string kParamOverlayInColor = "overlayInColor";

const static std::string kParamGroupOut = "groupOut";
const static std::string kParamPointOut = "pOut";
const static std::string kParamOverlayOut = "overlayOut";
const static std::string kParamOverlayOutColor = "overlayOutColor";

const static std::string kParamGroupTgtIn = "groupTgtIn";
const static std::string kParamPointTgtIn = "pTangenteIn";
const static std::string kParamOverlayTgtIn = "overlayTgtIn";
const static std::string kParamOverlayTgtInColor = "overlayTangentInColor";

const static std::string kParamGroupTgtOut = "groupTgtOut";
const static std::string kParamPointTgtOut = "pTangenteOut";
const static std::string kParamOverlayTgtOut = "overlayTgtOut";
const static std::string kParamOverlayTgtOutColor = "overlayTangentOutColor";

const static std::string kParamGroupCurveBegin = "groupCurveBegin";
const static std::string kParamCurveBegin = "pCurveBegin";

const static std::string kParamGroupOverlay = "groupOverlay";

}
}
}

#endif
