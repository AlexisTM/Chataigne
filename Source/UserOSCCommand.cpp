/*
  ==============================================================================

    UserOSCCommand.cpp
    Created: 15 Jan 2017 4:43:14pm
    Author:  Ben

  ==============================================================================
*/

#include "UserOSCCommand.h"
#include "UserOSCCommandEditor.h"

UserOSCCommand::UserOSCCommand(CustomOSCModule * _module, CommandContext context, var params) :
	OSCCommand(_module, context, params),
	cModule(_module)
{
	model = cModule->getModelForName(params.getProperty("model", ""));
	jassert(model != nullptr);
	
	address->setValue(model->addressParam->stringValue());
	rebuildArgsFromModel();
}

UserOSCCommand::~UserOSCCommand()
{
}

void UserOSCCommand::rebuildArgsFromModel()
{
	argumentsContainer.clear();

	//TODO : SlaveParameter ?
	Parameter * p = nullptr;
	for (auto &a : model->arguments)
	{
		Parameter * ap = a->param;
		switch (ap->type)
		{
		case Controllable::BOOL: p = new BoolParameter(a->argumentName->stringValue(), ap->description, ap->value); break;
		case Controllable::INT: p = new IntParameter(a->argumentName->stringValue(), ap->description, ap->value,ap->minimumValue,ap->maximumValue); break;
		case Controllable::FLOAT: p = new FloatParameter(a->argumentName->stringValue(), ap->description, ap->value, ap->minimumValue, ap->maximumValue); break;
		case Controllable::STRING: p = new StringParameter(a->argumentName->stringValue(), ap->description, ap->value); break;
		}
		
		argumentsContainer.addParameter(p);
	}
}

InspectableEditor * UserOSCCommand::getEditor(bool isRoot)
{
	return new UserOSCCommandEditor(this, isRoot);
}