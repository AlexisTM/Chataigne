/*
  ==============================================================================

	OSCModule.cpp
	Created: 29 Oct 2016 7:07:07pm
	Author:  bkupe

  ==============================================================================
*/

#include "OSCModule.h"
#include "Module/modules/common//ui/EnablingNetworkControllableContainerEditor.h"

OSCModule::OSCModule(const String & name, int defaultLocalPort, int defaultRemotePort, bool canHaveInput, bool canHaveOutput) :
	Module(name),
	Thread("OSCZeroconf"),
	localPort(nullptr),
    servus("_osc._udp"),
	receiveCC(nullptr)
{
	
	setupIOConfiguration(canHaveInput, canHaveOutput);
	canHandleRouteValues = canHaveOutput;
	
	//Receive
	if (canHaveInput)
	{
		receiveCC = new EnablingControllableContainer("OSC Input");
		receiveCC->customGetEditorFunc = &EnablingNetworkControllableContainerEditor::create;
		moduleParams.addChildControllableContainer(receiveCC);

		localPort = receiveCC->addIntParameter("Local Port", "Local Port to bind to receive OSC Messages", defaultLocalPort, 1024, 65535);
		localPort->hideInOutliner = true;
		localPort->isTargettable = false;

		receiver.addListener(this);
		setupReceiver();
	} else
	{
		if (receiveCC != nullptr) moduleParams.removeChildControllableContainer(receiveCC);
		receiveCC = nullptr;
	}

	//Send
	if (canHaveOutput)
	{
		outputManager = new BaseManager<OSCOutput>("OSC Outputs");
		moduleParams.addChildControllableContainer(outputManager);

		outputManager->setCanBeDisabled(true);
		if (!Engine::mainEngine->isLoadingFile)
		{
			OSCOutput * o = outputManager->addItem(nullptr, var(), false);
			o->remotePort->setValue(defaultRemotePort);
			setupSenders();
		}
	} else
	{
		if (outputManager != nullptr) removeChildControllableContainer(outputManager);
		outputManager = nullptr;
	}

	//Script
	scriptObject.setMethod(sendOSCId, OSCModule::sendOSCFromScript);
}

OSCModule::~OSCModule()
{
	if (isThreadRunning())
	{
		signalThreadShouldExit();
		waitForThreadToExit(1000);
		stopThread(100);
	}
}

void OSCModule::setupReceiver()
{
	receiver.disconnect();
	if (receiveCC == nullptr) return;

	if (!receiveCC->enabled->boolValue()) return;
	DBG("Local port set to : " << localPort->intValue());
	bool result = receiver.connect(localPort->intValue());

	if (result)
	{
		NLOG(niceName, "Now receiving on port : " + localPort->stringValue());
		if(!isThreadRunning() && !Engine::mainEngine->isLoadingFile) startThread();

		Array<IPAddress> ad;
		IPAddress::findAllAddresses(ad);

		Array<String> ips;
		for (auto &a : ad) ips.add(a.toString());
		ips.sort();
		String s = "Local IPs:";
		for (auto &ip : ips) s += String("\n > ") + ip;

		NLOG(niceName, s);
	} else
	{
		NLOGERROR(niceName, "Error binding port " + localPort->stringValue());
	}
	
}

float OSCModule::getFloatArg(OSCArgument a)
{
	if (a.isFloat32()) return a.getFloat32();
	if (a.isInt32()) return (float)a.getInt32();
	if (a.isString()) return a.getString().getFloatValue();
	return 0;
}

int OSCModule::getIntArg(OSCArgument a)
{
	if (a.isInt32()) return a.getInt32();
	if (a.isFloat32()) return roundf(a.getFloat32());
	if (a.isString()) return a.getString().getIntValue();
	return 0;
}

String OSCModule::getStringArg(OSCArgument a)
{
	if (a.isString()) return a.getString();
	if (a.isInt32()) return String(a.getInt32());
	if (a.isFloat32()) return String(a.getFloat32());
	return "";
}

void OSCModule::processMessage(const OSCMessage & msg)
{
	if (logIncomingData->boolValue())
	{
		String s = "";
		for (auto &a : msg) s += String(" ") + getStringArg(a);
		NLOG(niceName, msg.getAddressPattern().toString() << " :" << s);
	}

	inActivityTrigger->trigger();
	processMessageInternal(msg);

	if (scriptManager->items.size() > 0)
	{
		Array<var> params;
		params.add(msg.getAddressPattern().toString());
		var args;
		for (auto &a : msg) args.append(OSCModule::argumentToVar(a));
		params.add(args);
		scriptManager->callFunctionOnAllItems(oscEventId, params);
	}
}

void OSCModule::setupModuleFromJSONData(var data)
{
	Module::setupModuleFromJSONData(data);

	if (receiveCC != nullptr)
	{
		receiveCC->enabled->setValue(hasInput);
		receiveCC->hideInEditor = !hasInput;
	}
	if (outputManager != nullptr)
	{
		outputManager->enabled->setValue(hasOutput);
		outputManager->hideInEditor = !hasOutput;
	}
}

void OSCModule::setupSenders()
{
	for (auto &o : outputManager->items) o->setupSender();
}

void OSCModule::sendOSC(const OSCMessage & msg)
{
	if (!enabled->boolValue()) return;
	if (outputManager == nullptr) return;

	if (!outputManager->enabled->boolValue()) return;

	if (logOutgoingData->boolValue())
	{
		NLOG(niceName, "Send OSC : " << msg.getAddressPattern().toString());
		for (auto &a : msg)
		{
			LOG(getStringArg(a));
		}
	}

	outActivityTrigger->trigger();
	for (auto &o : outputManager->items) o->sendOSC(msg);
}

void OSCModule::setupZeroConf()
{
	if (Engine::mainEngine->isClearing || localPort == nullptr) return;

	String nameToAdvertise;
	int portToAdvertise = 0;
	while (nameToAdvertise != niceName || portToAdvertise != localPort->intValue())
	{
		nameToAdvertise = niceName;
		portToAdvertise = localPort->intValue();

		servus.withdraw();
		
		if (!hasInput) return;

		DBG("ADVERTISE");
		servus.announce(portToAdvertise, ("Chataigne - " + nameToAdvertise).toStdString());
		
		if (nameToAdvertise != niceName || localPort->intValue() != portToAdvertise || !hasInput)
		{
			DBG("Name or port changed during advertise, readvertising");
		}
	}
	
	NLOG(niceName,"Zeroconf service created : " << nameToAdvertise << ":" << portToAdvertise);
}

var OSCModule::sendOSCFromScript(const var::NativeFunctionArgs & a)
{
	OSCModule * m = getObjectFromJS<OSCModule>(a);
	if (!m->enabled->boolValue()) return var();

	if (a.numArguments == 0) return var();

	OSCMessage msg(a.arguments[0].toString());

	for (int i = 1; i < a.numArguments; i++)
	{
		if (a.arguments[i].isArray())
		{
			Array<var> * arr = a.arguments[i].getArray();
			for (auto &aa : *arr) msg.addArgument(varToArgument(aa));
		}else
		{
			msg.addArgument(varToArgument(a.arguments[i]));
		}
	}

	m->sendOSC(msg);

	return var();
}


OSCArgument OSCModule::varToArgument(const var & v)
{
	if (v.isBool()) return OSCArgument(((bool)v) ? 1 : 0);
	else if (v.isInt()) return OSCArgument((int)v);
	else if (v.isInt64()) return OSCArgument((int)v);
	else if (v.isDouble()) return OSCArgument((float)v);
	else if (v.isString()) return OSCArgument(v.toString());

	jassert(false);
	return OSCArgument("error");
}

var OSCModule::argumentToVar(const OSCArgument & a)
{
	if (a.isFloat32()) return var(a.getFloat32());
	else if (a.isInt32()) return var(a.getInt32());
	else if (a.isString()) return var(a.getString());
	return var("error");
}

var OSCModule::getJSONData()
{
	var data = Module::getJSONData();
	if (receiveCC != nullptr)
	{
		var inputData = receiveCC->getJSONData();
		if (!inputData.isVoid()) data.getDynamicObject()->setProperty("input", inputData);
	}

	if (outputManager != nullptr)
	{
		var outputsData = outputManager->getJSONData();
		if (!outputsData.isVoid()) data.getDynamicObject()->setProperty("outputs", outputsData);
	}

	return data;
}

void OSCModule::loadJSONDataInternal(var data)
{
	Module::loadJSONDataInternal(data);
	if (receiveCC != nullptr) receiveCC->loadJSONData(data.getProperty("input", var()));
	if (outputManager != nullptr)
	{
		outputManager->loadJSONData(data.getProperty("outputs", var()));
		setupSenders();
	}

	if(!isThreadRunning()) startThread();
}


void OSCModule::handleRoutedModuleValue(Controllable * c, RouteParams * p)
{
	OSCRouteParams * op = dynamic_cast<OSCRouteParams *>(p);
	OSCMessage m(op->address->stringValue());
	if (c->type != Controllable::TRIGGER)
	{
		var v = dynamic_cast<Parameter *>(c)->getValue();
		if (!v.isArray()) m.addArgument(varToArgument(v));
		else
		{
			for (int i = 0; i < v.size(); i++) m.addArgument(varToArgument(v[i]));
		}
	}
	sendOSC(m);
}

void OSCModule::onContainerParameterChangedInternal(Parameter * p)
{
	Module::onContainerParameterChangedInternal(p);	
}

void OSCModule::onContainerNiceNameChanged()
{
	Module::onContainerNiceNameChanged();
	if (Engine::mainEngine->isLoadingFile || Engine::mainEngine->isClearing) return;
	if(!isThreadRunning()) startThread();
}

void OSCModule::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	Module::onControllableFeedbackUpdateInternal(cc, c);

	if (outputManager != nullptr && c == outputManager->enabled)
	{
		bool rv = receiveCC->enabled->boolValue();
		bool sv = outputManager->enabled->boolValue();
		for(auto &o : outputManager->items) o->setForceDisabled(!sv);
		setupIOConfiguration(rv, sv);

	}else if (receiveCC != nullptr && c == receiveCC->enabled)
	{
		bool rv = receiveCC->enabled->boolValue();
		bool sv = outputManager->enabled->boolValue();
		setupIOConfiguration(rv,sv);
		localPort->setEnabled(rv);
		setupReceiver();

	} else if (c == localPort) setupReceiver();
}

void OSCModule::oscMessageReceived(const OSCMessage & message)
{
	if (!enabled->boolValue()) return;
	processMessage(message);
}

void OSCModule::oscBundleReceived(const OSCBundle & bundle)
{
	if (!enabled->boolValue()) return;
	for (auto &m : bundle)
	{
		processMessage(m.getMessage());
	}
}

void OSCModule::run()
{
	setupZeroConf();
}

OSCModule::OSCRouteParams::OSCRouteParams(Module * sourceModule, Controllable * c) 
{
	bool sourceIsGenericOSC = sourceModule->getTypeString() == "OSC";

	String tAddress;

	if (!sourceIsGenericOSC)
	{
		tAddress = c->shortName;

		ControllableContainer * cc = c->parentContainer;
		while (cc != nullptr)
		{
			if (cc->shortName != "values")
			{
				tAddress = cc->shortName + "/" + tAddress;
			}
			Module * m = dynamic_cast<Module *>(cc);
			if (m != nullptr) break;

			cc = cc->parentContainer;
		}
	} else
	{
		tAddress = c->niceName; //on CustomOSCModule, niceName is the actual address
	}

	if (!tAddress.startsWithChar('/')) tAddress = "/" + tAddress;
	
	address = addStringParameter("Address", "Route Address", tAddress);
}



///// OSC OUTPUT

OSCOutput::OSCOutput() :
	 BaseItem("OSC Output"),
	forceDisabled(false)
{
	isSelectable = false;

	useLocal = addBoolParameter("Local", "Send to Local IP (127.0.0.1). Allow to quickly switch between local and remote IP.", true);
	remoteHost = addStringParameter("Remote Host", "Remote Host to send to.", "127.0.0.1");
	remoteHost->setEnabled(!useLocal->boolValue());
	remotePort = addIntParameter("Remote port", "Port on which the remote host is listening to", 9000, 1024, 65535);

	if (!Engine::mainEngine->isLoadingFile) setupSender();
}

OSCOutput::~OSCOutput()
{
}

void OSCOutput::setForceDisabled(bool value)
{
	if (forceDisabled == value) return;
	forceDisabled = value;
	setupSender();
}

void OSCOutput::onContainerParameterChangedInternal(Parameter * p)
{

	if (p == remoteHost || p == remotePort || p == useLocal)
	{
		if(!Engine::mainEngine->isLoadingFile) setupSender();
		if (p == useLocal) remoteHost->setEnabled(!useLocal->boolValue());
	}
}

void OSCOutput::setupSender()
{
	sender.disconnect();
	if (!enabled->boolValue() || forceDisabled || Engine::mainEngine->isClearing) return;

	String targetHost = useLocal->boolValue() ? "127.0.0.1" : remoteHost->stringValue();
	bool result = sender.connect(targetHost, remotePort->intValue());
	if (result)
	{ 
		NLOG(niceName, "Now sending to " + remoteHost->stringValue() + ":" + remotePort->stringValue());
	} else
	{
		NLOGWARNING(niceName, "Could not connect to " + remoteHost->stringValue() + ":" + remotePort->stringValue());
	}
	
}

void OSCOutput::sendOSC(const OSCMessage & m)
{
	if (!enabled->boolValue() || forceDisabled) return;
	sender.send(m);
}
