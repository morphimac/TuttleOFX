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
            //Points out
            interact::AndActiveFunctor<>* activeOut = new interact::AndActiveFunctor<>();
            activeOut->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
            activeOut->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointOut[i] ) );
            _interactScene.push_back( new interact::ParamPoint<interact::FrameClip,
                                      eCoordinateSystemXY>( _infos, _plugin->_paramPointOut[i], _plugin->_clipSrc ),
                                      activeOut,
                                      new interact::ColorRGBParam(_plugin->_paramOverlayOutColor));

            interact::PointInteract* point = new interact::ParamPoint<interact::FrameClip, eCoordinateSystemXY>( _infos, _plugin->_paramPointOut[i], _plugin->_clipSrc );

            //Points In
            interact::AndActiveFunctor<>* activeIn = new interact::AndActiveFunctor<>();
            activeIn->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ) );
            activeIn->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointIn[i] ) );
            _interactScene.push_back( new interact::ParamPoint<interact::FrameClip,
                                      eCoordinateSystemXY>( _infos, _plugin->_paramPointIn[i], _plugin->_clipSrc ),
                                      activeIn,
                                      new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );

            // tangente IN A
            interact::AndActiveFunctor<>* activeTgtInA = new interact::AndActiveFunctor<>();
            activeTgtInA->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayTgtIn ) );
            activeTgtInA->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointTgtIn[i*2] ) );
            _interactScene.push_back(new interact::ParamPoint/*RelativePoint*/<interact::FrameClip,
                                       eCoordinateSystemXY>( _infos, _plugin->_paramPointTgtIn[i*2], _plugin->_clipSrc/*, point*/ ),
                                       activeTgtInA,
                                       new interact::ColorRGBParam(_plugin->_paramOverlayTgtInColor ));

            // tangente IN B
            interact::AndActiveFunctor<>* activeTgtInB = new interact::AndActiveFunctor<>();
            activeTgtInB->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayTgtIn ) );
            activeTgtInB->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointTgtIn[i*2+1] ) );
            _interactScene.push_back(new interact::ParamPoint/*RelativePoint*/<interact::FrameClip,
                                      eCoordinateSystemXY>( _infos, _plugin->_paramPointTgtIn[i*2+1], _plugin->_clipSrc/*, point*/ ),
                                      activeTgtInB,
                                      new interact::ColorRGBParam(_plugin->_paramOverlayTgtInColor ));

            // tangente Out A
            interact::AndActiveFunctor<>* activeTgtOutA = new interact::AndActiveFunctor<>();
            activeTgtOutA->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayTgtOut ) );
            activeTgtOutA->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointTgtOut[i*2] ) );
            _interactScene.push_back(new interact::ParamPoint/*RelativePoint*/<interact::FrameClip,
                                       eCoordinateSystemXY>( _infos, _plugin->_paramPointTgtOut[i*2], _plugin->_clipSrc/*, point*/ ),
                                       activeTgtOutA,
                                       new interact::ColorRGBParam(_plugin->_paramOverlayTgtOutColor ));

            // tangente Out B
            interact::AndActiveFunctor<>* activeTgtOutB = new interact::AndActiveFunctor<>();
            activeTgtOutB->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayTgtOut ) );
            activeTgtOutB->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointTgtOut[i*2+1] ) );
            _interactScene.push_back(new interact::ParamPoint/*RelativePoint*/<interact::FrameClip,
                                      eCoordinateSystemXY>( _infos, _plugin->_paramPointTgtOut[i*2+1], _plugin->_clipSrc/*, point*/ ),
                                      activeTgtOutB,
                                      new interact::ColorRGBParam(_plugin->_paramOverlayTgtOutColor ));
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
                        OfxPointD pIn1 = _plugin->_paramPointIn[c]->getValue();
                        OfxPointD pIn2 = _plugin->_paramPointIn[c+1]->getValue();

                        OfxPointD pOut1 = _plugin->_paramPointOut[c]->getValue();
                        OfxPointD pOut2 = _plugin->_paramPointOut[c+1]->getValue();

                        //Points de la tangente
                        OfxPointD tIn1 = _plugin->_paramPointTgtIn[(2*c)]->getValue();
                        OfxPointD tIn2 = _plugin->_paramPointTgtIn[(2*c)+3]->getValue();

                        OfxPointD tOut1 = _plugin->_paramPointTgtOut[(2*c)]->getValue();
                        OfxPointD tOut2 = _plugin->_paramPointTgtOut[(2*c)+3]->getValue();

                        //Création et remplissage du tableau necessaire à Bezier
                        std::vector< point2<double> > tabPtsIn;
                        tabPtsIn.push_back( point2<double>( pIn1.x, pIn1.y ) );
                        tabPtsIn.push_back( point2<double>( tIn1.x, tIn1.y ) );
                        tabPtsIn.push_back( point2<double>( tIn2.x, tIn2.y ) );
                        tabPtsIn.push_back( point2<double>( pIn2.x, pIn2.y ) );

                        std::vector< point2<double> > tabPtsOut;
                        tabPtsOut.push_back( point2<double>( pOut1.x, pOut1.y ) );
                        tabPtsOut.push_back( point2<double>( tOut1.x, tOut1.y ) );
                        tabPtsOut.push_back( point2<double>( tOut2.x, tOut2.y ) );
                        tabPtsOut.push_back( point2<double>( pOut2.x, pOut2.y ) );

                        //Mise en place des points de bezier dans un tableau
                        point2<double> ptIn;
                        point2<double> ptOut;
                        for(std::size_t i = 0; i < nbCoeffBezier ; ++i)
                        {
                            double t = (double(nbCoeffBezier)-i)/double(nbCoeffBezier);
                            ptIn = bezier::rempliTabPoint( tabPtsIn,t);
                            _tgtPointsBezierIn.push_back(point2<double> (ptIn.x,ptIn.y));

                            ptOut = bezier::rempliTabPoint( tabPtsOut,t);
                            _tgtPointsBezierOut.push_back(point2<double> (ptOut.x,ptOut.y));
                        }

                        //Choix de la couleur des tangentes dans nuke
                        double rIn,vIn,bIn;
                        _plugin->_paramOverlayTgtInColor->getValue(rIn,vIn,bIn);
                        glColor3f(rIn,vIn,bIn);

                        //Si "overlay tangente" est sur afficher
                        if(_plugin->_paramOverlayTgtIn->getValue())
                        {
                            bezier::dessinePoint( tabPtsIn, rIn, vIn, bIn);
                            //Trace les lignes des tangentes
                            glBegin(GL_LINES);
                                glVertex2f(_plugin->_paramPointTgtIn[0]->getValue().x,_plugin->_paramPointTgtIn[0]->getValue().y);
                                glVertex2f(_plugin->_paramPointTgtIn[1]->getValue().x,_plugin->_paramPointTgtIn[1]->getValue().y);
                                glVertex2f(_plugin->_paramPointTgtIn[(2*c)+2]->getValue().x,_plugin->_paramPointTgtIn[(2*c)+2]->getValue().y);
                                glVertex2f(_plugin->_paramPointTgtIn[(2*c)+3]->getValue().x,_plugin->_paramPointTgtIn[(2*c)+3]->getValue().y);
                            glEnd();

                            glPointSize(11.0);
                            glBegin(GL_POINTS);
                                glVertex2f(_plugin->_paramPointTgtIn[(2*c)+2]->getValue().x,_plugin->_paramPointTgtIn[(2*c)+2]->getValue().y);
                                glVertex2f(_plugin->_paramPointTgtIn[0]->getValue().x,_plugin->_paramPointTgtIn[0]->getValue().y);
                            glEnd();
                        }

                        //Choix de la couleur des tangentes dans nuke
                        double rOut,vOut,bOut;
                        _plugin->_paramOverlayTgtOutColor->getValue(rOut,vOut,bOut);
                        glColor3f(rOut,vOut,bOut);

                        //Si "overlay tangente" est sur afficher
                        if(_plugin->_paramOverlayTgtOut->getValue())
                        {
                            bezier::dessinePoint( tabPtsOut, rOut, vOut, bOut);
                            //Trace les lignes des tangentes
                            glBegin(GL_LINES);
                                glVertex2f(_plugin->_paramPointTgtOut[0]->getValue().x,_plugin->_paramPointTgtOut[0]->getValue().y);
                                glVertex2f(_plugin->_paramPointTgtOut[1]->getValue().x,_plugin->_paramPointTgtOut[1]->getValue().y);
                                glVertex2f(_plugin->_paramPointTgtOut[(2*c)+2]->getValue().x,_plugin->_paramPointTgtOut[(2*c)+2]->getValue().y);
                                glVertex2f(_plugin->_paramPointTgtOut[(2*c)+3]->getValue().x,_plugin->_paramPointTgtOut[(2*c)+3]->getValue().y);
                            glEnd();

                            glPointSize(11.0);
                            glBegin(GL_POINTS);
                                glVertex2f(_plugin->_paramPointTgtOut[(2*c)+2]->getValue().x,_plugin->_paramPointTgtOut[(2*c)+2]->getValue().y);
                                glVertex2f(_plugin->_paramPointTgtOut[0]->getValue().x,_plugin->_paramPointTgtOut[0]->getValue().y);
                            glEnd();
                        }
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

                _plugin->_paramPointOut[nbPoints]->setIsSecretAndDisabled(false);
                _plugin->_paramPointOut[nbPoints]->setValue(args.penPosition.x,args.penPosition.y);

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
            for( unsigned int i = 0 ; i < nbPoints ; ++i)
            {
                for(std::size_t ptSuivants = i+1; ptSuivants < nbPoints ; ++ptSuivants)
                {
                    _plugin->_paramPointIn[ptSuivants]->setValue(_plugin->_paramPointIn[ptSuivants+1]->getValue());
                    _plugin->_paramPointOut[ptSuivants]->setValue(_plugin->_paramPointOut[ptSuivants+1]->getValue());

                    _plugin->_paramPointTgtIn[2*ptSuivants]->setValue(_plugin->_paramPointTgtIn[2*ptSuivants+2]->getValue());
                    _plugin->_paramPointTgtIn[2*ptSuivants+1]->setValue(_plugin->_paramPointTgtIn[2*ptSuivants+3]->getValue());
                    _plugin->_paramPointTgtOut[2*ptSuivants]->setValue(_plugin->_paramPointTgtOut[2*ptSuivants+2]->getValue());
                    _plugin->_paramPointTgtOut[2*ptSuivants+1]->setValue(_plugin->_paramPointTgtOut[2*ptSuivants+3]->getValue());
                }
                _plugin->_paramPointIn[nbPoints-1]->setIsSecretAndDisabled(true);
                _plugin->_paramPointOut[nbPoints-1]->setIsSecretAndDisabled(true);
                _plugin->_paramPointTgtIn[2*nbPoints-2]->setIsSecretAndDisabled(true);
                _plugin->_paramPointTgtIn[2*nbPoints-1]->setIsSecretAndDisabled(true);
                _plugin->_paramPointTgtOut[2*nbPoints-2]->setIsSecretAndDisabled(true);
                _plugin->_paramPointTgtOut[2*nbPoints+1]->setIsSecretAndDisabled(true);

                _plugin->_paramNbPoints->setValue(nbPoints - 1);
            }
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
        //Tangente In

        //Point courant
        point2<double> ptCurIn;
        ptCurIn.x = _plugin->_paramPointIn[numPt-1]->getValue().x;
        ptCurIn.y = _plugin->_paramPointIn[numPt-1]->getValue().y;

        //Point numéro 1 de la tangente
        _plugin->_paramPointTgtIn[2*(numPt-1)]->setIsSecretAndDisabled(false);
        _plugin->_paramPointTgtIn[2*(numPt-1)]->setValue(args.penPosition.x,args.penPosition.y);

        //Point numéro 2 de la tangente
        _plugin->_paramPointTgtIn[2*(numPt-1)+1]->setIsSecretAndDisabled(false);
        _plugin->_paramPointTgtIn[2*(numPt-1)+1]->setValue(2*ptCurIn.x-args.penPosition.x,2*ptCurIn.y-args.penPosition.y);

        //Tangente Out

        //Point courant
        point2<double> ptCurOut;
        ptCurOut.x = _plugin->_paramPointOut[numPt-1]->getValue().x;
        ptCurOut.y = _plugin->_paramPointOut[numPt-1]->getValue().y;

        //Point numéro 1 de la tangente
        _plugin->_paramPointTgtOut[2*(numPt-1)]->setIsSecretAndDisabled(false);
        _plugin->_paramPointTgtOut[2*(numPt-1)]->setValue(args.penPosition.x,args.penPosition.y);

        //Point numéro 2 de la tangente
        _plugin->_paramPointTgtOut[2*(numPt-1)+1]->setIsSecretAndDisabled(false);
        _plugin->_paramPointTgtOut[2*(numPt-1)+1]->setValue(2*ptCurOut.x-args.penPosition.x,2*ptCurOut.y-args.penPosition.y);


        return _interactScene.penUp( args );
    }
    else if(_plugin->_paramMethod->getValue() == eParamMethodMove)
    {
        for( std::size_t i = 0 ; i < numPt ; ++i )
        {
            _plugin->_paramPointTgtIn[2*i+1]->setValue(2*_plugin->_paramPointIn[i]->getValue()-_plugin->_paramPointTgtIn[2*i]->getValue());
            _plugin->_paramPointTgtOut[2*i+1]->setValue(2*_plugin->_paramPointOut[i]->getValue()-_plugin->_paramPointTgtOut[2*i]->getValue());
        }

        return _interactScene.penUp( args );
    }
	return false;
}

}
}
}
