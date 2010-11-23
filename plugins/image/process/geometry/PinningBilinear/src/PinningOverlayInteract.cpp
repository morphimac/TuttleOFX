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

/**
 * @brief create a list of points which represents a grid in image coordinates
 * @param imgSize image size (in pixels)
 * @param nbTiles number of tiles
 * @param scale scale of the grid (normalized value)
 * @param center shift to apply on the image (normalized value)
 */

PinningOverlayInteract::PinningOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
	, _interactScene( *effect, _infos )
{
	_effect = effect;
	_plugin = static_cast<PinningPlugin*>( _effect );


	//const interact::FrameOptionalClip frame( _plugin->_srcRefClip, _plugin->_clipSrc );
	//Si on veut rajouter une source a notre plugin

	//interact::PointInteract* centerPoint = new interact::ParamPoint<interact::FrameOptionalClip, eCoordonateSystemXXcn>( _infos, _plugin->_center, frame );
/*
	_interactScene.push_back( new interact::ParamPointRelativePoint<interact::FrameOptionalClip, eCoordonateSystemXXcn>( _infos, _plugin->_gridCenter, frame, centerPoint ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_gridCenterOverlay ) );
	_interactScene.push_back( centerPoint, new interact::IsActiveBooleanParamFunctor<>( _plugin->_centerOverlay ) );
*/
}

//=========================================================================================================
//
//	Fonction pour creer la grille
//
//=========================================================================================================
template<typename Point2>
std::vector<std::vector<Point2> > createGrid( const OfxRectD& rod, const unsigned int nbTiles, const Point2& scale, const Point2& center, const unsigned int steps = 50 )
{
	//	Point2 rodCorner( rod.x1, rod.y1 );
	Point2 rodSize( rod.x2 - rod.x1, rod.y2 - rod.y1 );		//Taille de l'image
	const unsigned int size = nbTiles + 1; 				//Nombre de lignes de la grille
	const double stepSize   = rodSize.x / (double) steps;		
	const double tileSize   = rodSize.x / (double) nbTiles;		//Taille des espaces de la grille
	const double halfSize   = rodSize.x / 2.0;			//Centre de l'image
	const double yShift     = ( rodSize.x - rodSize.y ) / 2.0;	// vertical shift (because it's a squared grid)
	const Point2 pixCenter  = ( center * rodSize.x ); 		// center in pixel coordinates

	std::vector<std::vector<Point2> > lines;
	lines.reserve( 2 * size );
	for( unsigned int i = 0; i < size; ++i )			//Pour chaque ligne de la grille
	{
		std::vector<Point2> horizontal, vertical;
		horizontal.reserve( steps );
		vertical.reserve( steps );
		for( unsigned int j = 0; j <= steps; ++j )		//Pour chaque pas
		{
			double si = ( i * tileSize ) - halfSize;
			Point2 diagonal( si * scale.x + halfSize + pixCenter.x, si * scale.y + halfSize + pixCenter.y - yShift );
			double sj = ( j * stepSize ) - halfSize;
			horizontal.push_back( Point2( sj * scale.x + halfSize + pixCenter.x, diagonal.y ) );
			vertical.push_back( Point2( diagonal.x, sj * scale.y + halfSize + pixCenter.y - yShift ) );
		}
		lines.push_back( horizontal );
		lines.push_back( vertical );
	}
	return lines;
}

//=========================================================================================================
//
//	Fonction pour creer la grille
//
//=========================================================================================================
bool PinningOverlayInteract::draw( const OFX::DrawArgs& args )
{
	typedef boost::gil::point2<Scalar> Point2;		//Création d'un point
	static const float lineWidth = 2.0;			//Largeur de ligne : 2 pixel
	bool displaySomething        = false;			//Affichage initial : nul

	displaySomething |= _interactScene.draw( args );	

	// drawing
	if( _plugin->_gridOverlay->getValue() && _plugin->_clipSrc->isConnected() )
	//Si l'affichage de la grille est cochée et qu'une image est connectée au plugin
	{
		displaySomething = true;
		static const unsigned int steps = 10;

		// get the project size
		OfxRectD srcRod = _plugin->_clipSrc->getCanonicalRod( args.time );		//definition de srcRod

		const Point2 imgSize( srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1 );		//taille de l'image
		const OfxRectD outputRod = _plugin->_clipDst->getCanonicalRod( args.time );	

		//parameters
		const Point2 gridCenter( ofxToGil( _plugin->_gridCenter->getValue() ) );	//centre de la grille
		const Point2 gridScale( ofxToGil( _plugin->_gridScale->getValue() ) );		//echelle de la grille
		std::vector<std::vector<Point2> > grid = createGrid( srcRod, steps, gridScale, gridCenter );
		//On cree la grille "grid"

		Point2 rodCorner( srcRod.x1, srcRod.y1 );

		glLineWidth( lineWidth );
		glColor3f( 1.0f, 1.0f, 0.0f );
		overlay::drawCurves( grid );
	}

	return displaySomething;
}



bool PinningOverlayInteract::penMotion( const OFX::PenArgs& args )
{
	return _interactScene.penMotion( args );
}

bool PinningOverlayInteract::penDown( const OFX::PenArgs& args )
{
	return _interactScene.penDown( args );
}

bool PinningOverlayInteract::penUp( const OFX::PenArgs& args )
{
	return _interactScene.penUp( args );
}

}
}
}

