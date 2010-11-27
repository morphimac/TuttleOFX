#ifndef _TUTTLE_PLUGIN_TRANSFORMAFFINE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_TRANSFORMAFFINE_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace transformAffine {

/**
 * @brief TransformAffine process
 *
 */
template<class View>
class TransformAffineProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef double Scalar;
protected :
    TransformAffinePlugin&    _plugin;        ///< Rendering plugin

	TransformAffineProcessParams<Scalar> _params;
	
public:
    TransformAffineProcess( TransformAffinePlugin& effect );

	void setup( const OFX::RenderArguments& args );
	
    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "TransformAffineProcess.tcc"

#endif
