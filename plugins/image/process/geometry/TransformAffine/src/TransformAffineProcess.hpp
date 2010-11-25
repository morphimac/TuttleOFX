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
	typedef float Scalar;
protected :
    TransformAffinePlugin&    _plugin;        ///< Rendering plugin

public:
    TransformAffineProcess( TransformAffinePlugin& effect );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "TransformAffineProcess.tcc"

#endif
