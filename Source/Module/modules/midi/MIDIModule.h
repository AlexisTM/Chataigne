/*
  ==============================================================================

    MIDIModule.h
    Created: 20 Dec 2016 12:35:26pm
    Author:  Ben

  ==============================================================================
*/

#ifndef MIDIMODULE_H_INCLUDED
#define MIDIMODULE_H_INCLUDED

#include "Module/Module.h"
#include "Common/MIDI/MIDIManager.h"
#include "Common/MIDI/MIDIDeviceParameter.h"

class MIDIModule :
	public Module,
	public MIDIInputDevice::MIDIInputListener
{
public:
	MIDIModule(const String &name = "MIDI", bool useGenericControls = true);
	virtual ~MIDIModule();

	MIDIDeviceParameter * midiParam;
	BoolParameter * autoAdd;

	MIDIInputDevice * inputDevice;
	MIDIOutputDevice * outputDevice;
	
	bool useGenericControls;

	virtual void sendNoteOn(int pitch, int velocity, int channel = 1);
	virtual void sendNoteOff(int pitch, int channel = 0);
	virtual void sendControlChange(int number, int value, int channel = 1);
	virtual void sendSysex(Array<uint8> data);

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
	void updateMIDIDevices();

	virtual void noteOnReceived(const int &channel, const int &pitch, const int &velocity) override;
	virtual void noteOffReceived(const int &channel, const int &pitch, const int &velocity) override;
	virtual void controlChangeReceived(const int &channel, const int &number, const int &value) override;

	void updateValue(const int &channel, const String &n, const int &val);

	//Routing
	class MIDIRouteParams :
		public RouteParams
	{
	public:
		MIDIRouteParams(Module * sourceModule, Controllable * c);
		~MIDIRouteParams() {}
		enum Type { NOTE_ON, NOTE_OFF, FULL_NOTE, CONTROL_CHANGE };
		EnumParameter * type;
		IntParameter * channel;
		IntParameter * pitchOrNumber;
	};

	virtual RouteParams * createRouteParamsForSourceValue(Module * sourceModule, Controllable * c, int /*index*/) override { return new MIDIRouteParams(sourceModule, c); }
	virtual void handleRoutedModuleValue(Controllable * c, RouteParams * p) override;


	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	static MIDIModule * create() { return new MIDIModule(); }
	virtual String getDefaultTypeString() const override { return "MIDI"; }

	//InspectableEditor * getEditor(bool isRoot) override;
};



#endif  // MIDIMODULE_H_INCLUDED
