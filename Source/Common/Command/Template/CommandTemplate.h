/*
  ==============================================================================

    CommandTemplate.h
    Created: 31 May 2018 11:23:16am
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "../CommandContext.h"
#include "CommandTemplateParameter.h"

class BaseCommand;
class Module;
class CommandDefinition;
class CustomValuesCommandArgumentManager;

class CommandTemplate :
	public BaseItem
{
public:
	CommandTemplate(Module * m, var params = var());
	CommandTemplate(var params = var());
	~CommandTemplate();

	ControllableContainer paramsContainer;
	CommandDefinition * sourceDef;
	OwnedArray<CommandTemplateParameter> templateParams;
	ScopedPointer<CustomValuesCommandArgumentManager> customValuesManager;

	void generateParametersFromDefinition(CommandDefinition * def);

	BaseCommand * createCommand(Module * m, CommandContext context, var params);

	void onContainerParameterChangedInternal(Parameter * p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
	void onContainerNiceNameChanged() override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	class TemplateListener
	{
	public:
		virtual ~TemplateListener() {}
		virtual void templateParameterChanged(CommandTemplateParameter * ctp) {}
		virtual void templateNameChanged(CommandTemplate * ct) {}
	};

	ListenerList<TemplateListener> templateListeners;
	void addCommandTemplateListener(TemplateListener* newListener) { templateListeners.add(newListener); }
	void removeCommandTemplateListener(TemplateListener* listener) { templateListeners.remove(listener); }
	 


	static CommandTemplate * create(var params) { return new CommandTemplate(params); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandTemplate)
};

/*
class CommandTemplateDefinition :
	public CommandDefinition,
	public ControllableContainerListener
{
public:
	CommandTemplateDefinition(CommandTemplate * ct);
	~CommandTemplateDefinition();

	CommandTemplate * commandTemplate;

	void childAddressChanged(ControllableContainer * cc) override;

	static CommandTemplateDefinition * createDef(ControllableContainer * container, CommandTemplate * ct, CreateCommandFunc createFunc);

};
*/
