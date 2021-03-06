/*
  ==============================================================================

    OSPowerCommand.h
    Created: 5 Jan 2018 4:05:45pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "Common/Command/BaseCommand.h"
#include "../OSModule.h"

class OSPowerCommand :
	public BaseCommand
{
public:
	OSPowerCommand(OSModule * _module, CommandContext context, var params);
	~OSPowerCommand();

	enum ActionType { SHUTDOWN, REBOOT };
	ActionType actionType;

	void trigger() override;

	static OSPowerCommand * create(ControllableContainer * module, CommandContext context, var params) { return new OSPowerCommand((OSModule *)module, context, params); }

};