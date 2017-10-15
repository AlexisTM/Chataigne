/*
  ==============================================================================

    TimeTrigger.cpp
    Created: 20 Nov 2016 3:18:20pm
    Author:  Ben Kuper

  ==============================================================================
*/

#include "TimeTrigger.h"
#include "TimeTriggerEditor.h"


TimeTrigger::TimeTrigger(float _time, float flagYPos, const String &name) :
	Action(name)
{
	time = addFloatParameter("Time", "Time at which the action is triggered", _time, 0, 3600);
	time->defaultUI = FloatParameter::TIME;
	flagY = addFloatParameter("Flag Y", "Position of the trigger's flag", flagYPos);
	isTriggered = addBoolParameter("Is Triggered", "Is this Time Trigger already triggered during this playing ?", false);

	autoTriggerWhenAllConditionAreActives = false;
}

TimeTrigger::~TimeTrigger()
{

}

InspectableEditor * TimeTrigger::getEditor(bool isRoot)
{
	return new TimeTriggerEditor(this,isRoot);
}

void TimeTrigger::onContainerTriggerTriggered(Trigger * t)
{
	Action::onContainerTriggerTriggered(t);
	if (t == trigger)
	{
		isTriggered->setValue(true);
	}
}
