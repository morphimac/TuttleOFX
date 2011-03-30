#ifndef _TUTTLE_PLUGIN_COLORTRANSFERT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFERT_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

/**
 * @brief ColorTransfert process
 *
 */
template<class View>
class ColorTransfertProcess : public ImageGilFilterProcessor<View>
{
public:
        typedef typename View::value_type Pixel;
        typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    ColorTransfertPlugin&    _plugin;            ///< Rendering plugin
        ColorTransfertProcessParams<Scalar> _params; ///< parameters

public:
    ColorTransfertProcess( ColorTransfertPlugin& effect );

	void setup( const OFX::RenderArguments& args );

        void computeAverage(OFX::Clip* image);

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ColorTransfertProcess.tcc"

#endif
