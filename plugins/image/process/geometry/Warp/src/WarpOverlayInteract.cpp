#include "WarpOverlayInteract.hpp"
#include "WarpDefinitions.hpp"
#include "WarpPlugin.hpp"
#include "Bezier/bezier.hpp"
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
namespace warp {

WarpOverlayInteract::WarpOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
	, _interactScene( *effect, _infos )
{
	_effect = effect;
        _plugin = static_cast<WarpPlugin*>( _effect );

        //Points out
        for( std::size_t i = 0; i < kMaxNbPoints; ++i )
        {
            // main point
            interact::AndActiveFunctor<>* activeOut = new interact::AndActiveFunctor<>();
            activeOut->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
            activeOut->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointIn[i] ) );
            _interactScene.push_back( new interact::ParamPoint<interact::FrameClip,
                                      eCoordonateSystemXY>( _infos, _plugin->_paramPointOut[i], _plugin->_clipSrc ),
                                      activeOut,
                                      new interact::ColorRGBParam(_plugin->_paramOverlayOutColor));

            interact::PointInteract* point = new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXY>( _infos, _plugin->_paramPointOut[i], _plugin->_clipSrc );

            // tangente A
            interact::AndActiveFunctor<>* activeTgtA = new interact::AndActiveFunctor<>();
            activeTgtA->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayTgt ) );
            activeTgtA->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointTgt[i*2] ) );
            _interactScene.push_back(new interact::ParamPoint/*RelativePoint*/<interact::FrameClip,
                                       eCoordonateSystemXY>( _infos, _plugin->_paramPointTgt[i*2], _plugin->_clipSrc/*, point*/ ),
                                       activeTgtA,
                                       new interact::ColorRGBParam(_plugin->_paramOverlayTgtColor ));

            // tangente B
            interact::AndActiveFunctor<>* activeTgtB = new interact::AndActiveFunctor<>();
            activeTgtB->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayTgt ) );
            activeTgtB->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointTgt[i*2+1] ) );
            _interactScene.push_back(new interact::ParamPoint/*RelativePoint*/<interact::FrameClip,
                                      eCoordonateSystemXY>( _infos, _plugin->_paramPointTgt[i*2+1], _plugin->_clipSrc/*, point*/ ),
                                      activeTgtB,
                                      new interact::ColorRGBParam(_plugin->_paramOverlayTgtColor ));

        }

        // Points in
        for(unsigned int cptIn = 0; cptIn < kMaxNbPoints; ++cptIn)
	{
                interact::AndActiveFunctor<>* activeIn = new interact::AndActiveFunctor<>();
                activeIn->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ) );
                activeIn->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointIn[cptIn] ) );
                _interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXY>( _infos, _plugin->_paramPointIn[cptIn], _plugin->_clipSrc ),
                                                                  activeIn,
                                                                  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
        }
}

bool WarpOverlayInteract::draw( const OFX::DrawArgs& args )
{
	if( !_plugin->_paramOverlay->getValue() || !_plugin->_clipSrc->isConnected() )
		return false;

	typedef boost::gil::point2<Scalar> Point2;
	bool displaySomething        = false;

        //Recuperation du nombre de points positionnés
	const std::size_t nbPoints = _plugin->_paramNbPoints->getValue();

        if(nbPoints >1)
        {
                for( std::size_t c = 0 ; c < nbPoints-1 ; ++c )
		{
                        // Création des points et des ptTangente et recupération des valeurs
                        //points à relier
                        OfxPointD p1 = _plugin->_paramPointIn[c]->getValue();
                        OfxPointD p2 = _plugin->_paramPointIn[c+1]->getValue();

                        //Points de la tangente
                        OfxPointD t1 = _plugin->_paramPointTgt[(2*c)]->getValue();
                        OfxPointD t2 = _plugin->_paramPointTgt[(2*c)+3]->getValue();

                        //Choix de la couleur des tangentes dans nuke
                        double r,v,b;
                        _plugin->_paramOverlayTgtColor->getValue(r,v,b);
                        glColor3f(r,v,b);

                        //Trace les lignes des tangentes
                        glBegin(GL_LINES);
                            glVertex2f(_plugin->_paramPointTgt[(2*c)]->getValue().x,_plugin->_paramPointTgt[(2*c)]->getValue().y);
                            glVertex2f(_plugin->_paramPointTgt[(2*c)+1]->getValue().x,_plugin->_paramPointTgt[(2*c)+1]->getValue().y);
                        glEnd();

                        glPointSize(11.0);
                        glBegin(GL_POINTS);
                            glVertex2f(_plugin->_paramPointTgt[(2*c)]->getValue().x,_plugin->_paramPointTgt[(2*c)]->getValue().y);
                        glEnd();

                        //Création et remplissage du tableau necessaire à Bezier
                        std::vector< point2<double> > tabPts;
                        tabPts.push_back( point2<double>( p1.x, p1.y ) );
                        tabPts.push_back( point2<double>( t1.x, t1.y ) );
                        tabPts.push_back( point2<double>( t2.x, t2.y ) );
                        tabPts.push_back( point2<double>( p2.x, p2.y ) );

                        //Utilisation de Bezier
			bezier::dessinePoint( tabPts, 4 );
                }
        }

	displaySomething |= _interactScene.draw( args );

        return displaySomething;
}

bool WarpOverlayInteract::penMotion( const OFX::PenArgs& args )
{
        if(_plugin->_paramMethod->getValue() == eParamMethodMove)
        {
            return _interactScene.penMotion( args );
        }
}

bool WarpOverlayInteract::penDown( const OFX::PenArgs& args )
{
	unsigned int nbPoints = _plugin->_paramNbPoints->getValue();

        //S'il reste des points à placer et si le mode est "creation"
	if( (nbPoints < kMaxNbPoints) && (_plugin->_paramMethod->getValue() == eParamMethodCreation) )
	{
		_plugin->_paramPointIn[nbPoints]->setIsSecretAndDisabled(false);
		_plugin->_paramPointIn[nbPoints]->setValue(args.penPosition.x,args.penPosition.y);
		_plugin->_paramNbPoints->setValue(nbPoints+1);

		return _interactScene.penDown( args );
	}
        //Si le mode est "move"
	else if(_plugin->_paramMethod->getValue() == eParamMethodMove)
	{
		return _interactScene.penDown( args );
	}
        //Si le mode est delete
	else if(_plugin->_paramMethod->getValue() == eParamMethodDelete)
        {
	}
	return false;
}

bool WarpOverlayInteract::keyDown( const OFX::KeyArgs& args )
{
	if(args.keySymbol == kOfxKey_space)
		TUTTLE_COUT("test");
	else
		TUTTLE_COUT("test raté");
	return false;
}

//bool WarpOverlayInteract::keyUp( const KeyArgs& args );

//bool WarpOverlayInteract::keyRepeat( const KeyArgs& args );

//if(PinningOverlayInteract::keyDown(args))
  //  TUTTLE_COUT("control est pressé comme une orange");

bool WarpOverlayInteract::penUp( const OFX::PenArgs& args )
{
    unsigned int numPt = _plugin->_paramNbPoints->getValue();

    if(_plugin->_paramMethod->getValue() == eParamMethodCreation)
    {
        //Point courant
        point2<double> ptCur;
        ptCur.x = _plugin->_paramPointIn[numPt-1]->getValue().x;
        ptCur.y = _plugin->_paramPointIn[numPt-1]->getValue().y;

        //Point numéro 1 de la tangente
        point2<double> ptTgt1;
        _plugin->_paramPointTgt[2*(numPt-1)]->setIsSecretAndDisabled(false);
        _plugin->_paramPointTgt[2*(numPt-1)]->setValue(args.penPosition.x,args.penPosition.y);

        //Point numéro 2 de la tangente
        point2<double> ptTgt2;
        _plugin->_paramPointTgt[2*(numPt-1)+1]->setIsSecretAndDisabled(false);
        _plugin->_paramPointTgt[2*(numPt-1)+1]->setValue(2*ptCur.x-args.penPosition.x,2*ptCur.y-args.penPosition.y);

        return _interactScene.penUp( args );
    }
    else if(_plugin->_paramMethod->getValue() == eParamMethodMove)
    {
        for( std::size_t i = 0 ; i < numPt ; ++i )
        {
            _plugin->_paramPointTgt[2*i+1]->setValue(2*_plugin->_paramPointIn[i]->getValue()-_plugin->_paramPointTgt[2*i]->getValue());
            //_plugin->_paramPointTgt[2*i]->setValue(2*_plugin->_paramPointIn[i]->getValue()-_plugin->_paramPointTgt[2*i+1]->getValue());
        }
        return _interactScene.penUp( args );
    }
}

}
}
}
