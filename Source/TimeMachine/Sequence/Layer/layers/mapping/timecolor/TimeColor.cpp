/*
  ==============================================================================

    TimeColor.cpp
    Created: 11 Apr 2017 11:40:21am
    Author:  Ben

  ==============================================================================
*/

#include "TimeColor.h"


TimeColor::TimeColor(float _time, const Colour & _color, const String & name) :
	BaseItem(name,false),
	gradientIndex(-1)
{
	position = addFloatParameter("Time", "Time for the color", 0, 0, 3600);
	color = new ColorParameter("Color", "Color of the item", Colours::black);
	position->setValue(_time);
	color->setColor(_color);
	addParameter(color);
}

TimeColor::~TimeColor()
{

}