#ifndef _TUTTLE_PLUGIN_INTERACTSCENE_HPP_
#define _TUTTLE_PLUGIN_INTERACTSCENE_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "IsActiveFunctor.hpp"
#include "Color.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>

#include <ofxsParam.h>

#include <boost/ptr_container/ptr_vector.hpp>
#include <utility>

namespace tuttle {
namespace plugin {
namespace interact {

/**
 * @brief To create a group of overlay/interact objects.
 * Inherit from InteractObject, because a group of overlay/interact objects is itself an overlay/interact object.
 */
class InteractScene : public InteractObject
{
typedef boost::gil::point2<double> Point2;
typedef boost::ptr_vector<InteractObject> InteractObjectsVector;
typedef boost::ptr_vector<IsActiveFunctor> IsActiveFunctorVector;
typedef boost::ptr_vector<IColor> ColorVector;
typedef std::pair<InteractObject*, Point2> SelectedObject;
typedef std::vector<SelectedObject> SelectedObjectsLinkVector;

public:
	InteractScene( OFX::ParamSet& params, const InteractInfos& infos );
	virtual ~InteractScene();

private:
	OFX::ParamSet& _params;
	const InteractInfos& _infos;
	bool _multiSelectionEnabled;
	bool _hasSelection;

	InteractObjectsVector _objects;
	IsActiveFunctorVector _isActive;
	ColorVector _colors;

	SelectedObjectsLinkVector _selected;
	EMoveType _moveType;
	bool _mouseDown;
	bool _beginSelection;
	OfxRectD _selectionRect;

public:
	InteractObjectsVector&       getObjects()       { return _objects; }
	const InteractObjectsVector& getObjects() const { return _objects; }
	
	SelectedObjectsLinkVector&       getSelectedObjects()       { return _selected; }
	const SelectedObjectsLinkVector& getSelectedObjects() const { return _selected; }

	void push_back( InteractObject* obj, IsActiveFunctor* isActive = new AlwaysActiveFunctor<>(), IColor* color = new Color() )
	{
		_objects.push_back( obj );
		_isActive.push_back( isActive );
		_colors.push_back( color );
	}

	bool draw( const OFX::DrawArgs& args );

	bool penMotion( const OFX::PenArgs& args );

	bool penDown( const OFX::PenArgs& args );

	bool penUp( const OFX::PenArgs& args );

        bool keyDown( const OFX::KeyArgs& args );

   /*     bool moveXYSelected( const Point2& p )
        {
            bool bb = false;
            BOOST_FOREACH( const interact::InteractObject& p, _interactScene.getObjects() )
            {
                bool b = p.moveXYSelected( p );
                bb = (bb || b);
            }
            return bb;
        }
   */
        bool moveXSelected( const Point2& )
        {
            return false;
        }
        bool moveYSelected( const Point2& )
        {
            return false;
        }

        void beginMove()                     {}
        void endMove()                       {}
};

}
}
}

#endif
