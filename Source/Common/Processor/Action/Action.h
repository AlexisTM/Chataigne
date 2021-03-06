/*
  ==============================================================================

    Action.h
    Created: 28 Oct 2016 8:05:02pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

#include "../Processor.h"
#include "Condition/ConditionManager.h"
#include "Consequence/ConsequenceManager.h"

class Action :
	public Processor,
	public ConditionManager::ConditionManagerListener,
	public ConditionManager::Listener
{
public:
	Action(const String &name = "Action", var params = var());
	virtual ~Action();

	enum Role { STANDARD, ACTIVATE, DEACTIVATE };
	Role actionRole;

	bool autoTriggerWhenAllConditionAreActives; //default true, but if false, let use Actions as user check tool without auto behavior (like TimeTriggers)
	
	bool forceNoOffConsequences; // independent of roles
	bool hasOffConsequences;

	ConditionManager cdm;
	ScopedPointer<ConsequenceManager> csmOn;
	ScopedPointer<ConsequenceManager> csmOff;

	BoolParameter * isActive;
	Trigger * triggerOn;
	Trigger * triggerOff;

	void updateConditionRole();
	void setRole(Role role, bool force = false);

	virtual void setForceDisabled(bool value, bool force = false) override;
	void setHasOffConsequences(bool value);


	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void onContainerParameterChangedInternal(Parameter * p) override;
    void onContainerTriggerTriggered(Trigger *) override;
	void conditionManagerValidationChanged(ConditionManager *) override;

	void itemAdded(Condition *) override;
	void itemRemoved(Condition *) override;

	ProcessorUI * getUI() override;

	class ActionListener
	{
	public:
		virtual ~ActionListener() {}
		virtual void actionEnableChanged(Action *) {}
		virtual void actionRoleChanged(Action *) {}
		virtual void actionValidationChanged(Action *) {}
	};

	ListenerList<ActionListener> actionListeners;
	void addActionListener(ActionListener* newListener) { actionListeners.add(newListener); }
	void removeActionListener(ActionListener* listener) { actionListeners.remove(listener); }


	class ActionEvent {
	public:
		enum Type { ENABLED_CHANGED, ROLE_CHANGED, VALIDATION_CHANGED };
		ActionEvent(Type type, Action * c) : type(type), action(c) {}
		Type type;
		Action * action;
	};
	QueuedNotifier<ActionEvent> actionAsyncNotifier;
	typedef QueuedNotifier<ActionEvent>::Listener AsyncListener;


	void addAsyncActionListener(AsyncListener* newListener) { actionAsyncNotifier.addListener(newListener); }
	void addAsyncCoalescedActionListener(AsyncListener* newListener) { actionAsyncNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncActionListener(AsyncListener* listener) { actionAsyncNotifier.removeListener(listener); }


	static Action * create(var params) { return new Action("Action", params); }
	String getTypeString() const override { return "Action"; };

	//InspectableEditor * getEditor(bool /*isRoot*/) override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Action)
};





#endif  // ACTION_H_INCLUDED
