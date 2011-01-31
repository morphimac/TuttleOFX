 #include "PinningOverlayInteract.hpp"
#include "PinningDefinitions.hpp"
#include "PinningPlugin.hpp"
#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>
#include <tuttle/plugin/interact/ParamPointRelativePoint.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

#include <vector>

namespace tuttle {
namespace plugin {
namespace pinning {

using namespace boost::numeric::ublas;

PinningOverlayInteract::PinningOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
	, _interactScene( *effect, _infos )
{
	_effect = effect;
	_plugin = static_cast<PinningPlugin*>( _effect );


        _interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut0, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut1, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut2, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	interact::AndActiveFunctor<>* activeOut3 = new interact::AndActiveFunctor<>();
	activeOut3->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
	activeOut3->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointOut3 ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut3, _plugin->_clipSrc ),
							  activeOut3,
							  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );

	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn0, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn1, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn2, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	interact::AndActiveFunctor<>* activeIn3 = new interact::AndActiveFunctor<>();
	activeIn3->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
	activeIn3->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointIn3 ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn3, _plugin->_clipSrc ),
							  activeIn3,
							  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );


        _interactScene.push_back(
              new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointCentre, _plugin->_clipSrc ),
              new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayCentre ),
              new interact::ColorRGBParam(_plugin->_paramOverlayCentreColor)
         );

}

bool PinningOverlayInteract::draw( const OFX::DrawArgs& args )
{
	if( !_plugin->_paramOverlay->getValue() || !_plugin->_clipSrc->isConnected() )
		return false;

	typedef boost::gil::point2<Scalar> Point2;
	static const float lineWidth = 2.0;
	bool displaySomething        = false;

	displaySomething |= _interactScene.draw( args );

	return displaySomething;
}

bool PinningOverlayInteract::penMotion( const OFX::PenArgs& args )
{

	//std::cout<<"pen move"<<std::endl;
	
        return _interactScene.penMotion( args );
}

bool PinningOverlayInteract::penDown( const OFX::PenArgs& args )
{
	//std::cout<<"pen down"<<std::endl;
	OfxPointD pos = args.penPosition;
        //std::cout<<pos.x<<";"<<pos.y<<std::endl;
//	double pres = args.penPressure; // lorsque l'on utilise une tablette graphique, on peut récupérer la pression du stylo
//	OfxTime time = args.time;
	
//        int width = _plugin->_clipSrc->getCanonicalRodSize(args.time, args.renderScale).x;
//      _plugin->_paramPointIn0->setValue( args.penPosition.x / width, args.penPosition.y / width );
	
        bool selObj = _interactScene.penDown( args );
        if( selObj )
        {
            return true;
        }
        else
        {
            _beginSelection = true;
            _multiSelectionRec.x1 = args.penPosition.x;
            _multiSelectionRec.y1 = args.penPosition.y;

            //if(_keyPressed_ctrl)
            {
                _multiSelection = true;
            }
        }
return _interactScene.penDown( args );
}

bool PinningOverlayInteract::penUp( const OFX::PenArgs& args )
{
    if( _multiSelection )
    {
        _multiSelectionRec.x2 = args.penPosition.x;
        _multiSelectionRec.y2 = args.penPosition.y;
        _beginSelection = false;

        // parcours Points
        //??

        _multiSelection = false;
	       
	BOOST_FOREACH( interact::InteractObject& p, _interactScene.getObjects() )
        {
            bool b = p.selectIfIsIn( _multiSelectionRec );
            _multiSelection = _multiSelection || b;
        }
    }

    // x , y
    //_interactScene.moveXYSelected( x, y );

	//std::cout<<"pen up"<<std::endl;

    return _interactScene.penUp( args );
}



void PinningOverlayInteract::calculCentre( const std::vector< bounded_vector<double, 2> > pSelect)
{
	double minX,maxX,minY,maxY = 0.0;
	
        for(int i=0 ; i<pSelect.size() ; ++i)
	{
		if(minX > pSelect[i][0] )
		{
			minX = pSelect[i][0];
		}
		if(maxX < pSelect[i][0] )
		{
			maxX = pSelect[i][0];
		}
		if(minY > pSelect[i][1] )
		{
                        minY = pSelect[i][1];
		}
		if(maxY < pSelect[i][1] )
		{
			maxY = pSelect[i][1];
		}
	}

        _plugin->_paramPointCentre->setValue((minX + maxX)/2, (minY + maxY)/2);


}

void PinningOverlayInteract::rotatePts( std::vector< bounded_vector<double, 2> > pSelect, double angle)
{
	
        for(int i=0 ; i<pSelect.size() ; ++i)
	{
            //deplace a l'origine
            pSelect[i][0] -=  _plugin->_paramPointCentre->getValue().x;
            pSelect[i][1] -=  _plugin->_paramPointCentre->getValue().y;
	
            //effectue la rotation
            pSelect[i][0] = pSelect[i][0]*cos(angle) - pSelect[i][1]*sin(angle);
            pSelect[i][1] = pSelect[i][0]*sin(angle) - pSelect[i][1]*cos(angle);
	
            //retour position initiale
            pSelect[i][0] +=  _plugin->_paramPointCentre->getValue().x;
            pSelect[i][1] +=  _plugin->_paramPointCentre->getValue().y;
        }
}

void PinningOverlayInteract::scalePts( std::vector< bounded_vector<double, 2> > pSelect, double coef)
{

        for(int i=0 ; i<pSelect.size() ; ++i)
	{
            //deplace a l'origine
            pSelect[i][0] -=  _plugin->_paramPointCentre->getValue().x;
            pSelect[i][1] -=  _plugin->_paramPointCentre->getValue().y;

            //effectue le scale
            pSelect[i][0] *= coef;
            pSelect[i][1] *= coef;

            //retour position initiale
            pSelect[i][0] +=  _plugin->_paramPointCentre->getValue().x;
            pSelect[i][1] +=  _plugin->_paramPointCentre->getValue().y;
	}
}


}
}
}
