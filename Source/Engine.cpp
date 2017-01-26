/*
 ==============================================================================

 Engine.cpp
 Created: 2 Apr 2016 11:03:21am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "Engine.h"
#include "Inspector.h"
#include "ModuleManager.h"
#include "StateManager.h"
#include "SequenceManager.h"
#include "CustomLogger.h"
#include "PresetManager.h"
#include "StringUtil.h"
#include "Outliner.h"
#include "ControllableFactory.h"
#include "MIDIManager.h"
#include "GamepadManager.h"
#include "WiimoteManager.h"

juce_ImplementSingleton(Engine) 

const char* const filenameSuffix = ".noisette";
const char* const filenameWildcard = "*.noisette";

Engine::Engine():
	FileBasedDocument (filenameSuffix,
                                    filenameWildcard,
                                    "Load a Noisette",
                                    "Save a Noisette"),
	ControllableContainer("Root")
{
  
  skipControllableNameInAddress = true;

  //to move into engine
  Logger::setCurrentLogger(CustomLogger::getInstance());

  addChildControllableContainer(ModuleManager::getInstance());
  addChildControllableContainer(StateManager::getInstance());
  addChildControllableContainer(SequenceManager::getInstance());

  MIDIManager::getInstance(); //Trigger MIDIManager singleton constructor
}


Engine::~Engine(){

//delete managers
	
  Outliner::deleteInstance();

  ModuleManager::deleteInstance();
  StateManager::deleteInstance();
  SequenceManager::deleteInstance();

  PresetManager::deleteInstance();
  CustomLogger::deleteInstance();
  Logger::setCurrentLogger(nullptr);

  ControllableFactory::deleteInstance();
  
  MIDIManager::deleteInstance();
  GamepadManager::deleteInstance();
  WiimoteManager::deleteInstance();

}

void Engine::parseCommandline(const String & commandLine){

  for (auto & c:StringUtil::parseCommandLine(commandLine)){

    if(c.command== "f"|| c.command==""){
      if(c.args.size()==0){
        LOG("no file provided for command : "+c.command);
        jassertfalse;
        continue;
      }
      String fileArg = c.args[0];
      if (File::isAbsolutePath(fileArg)) {
        File f(fileArg);
        if (f.existsAsFile()) loadDocument(f);
      }
      else {
        NLOG("Engine","File : " << fileArg << " not found.");
      }
    }

  }

}

void Engine::clear() {


	if (Outliner::getInstanceWithoutCreating())
	{
		Outliner::getInstanceWithoutCreating()->clear();
		Outliner::getInstanceWithoutCreating()->enabled = false;
	}

	if (Inspector::getInstanceWithoutCreating())
	{
		Inspector::getInstance()->clear();
		Inspector::getInstance()->setEnabled(false);
	}

	ModuleManager::getInstance()->clear();
	StateManager::getInstance()->clear();
	SequenceManager::getInstance()->clear();

	PresetManager::getInstance()->clear();

	if (Outliner::getInstanceWithoutCreating()) Outliner::getInstanceWithoutCreating()->enabled = true;

	if (Inspector::getInstanceWithoutCreating()) Inspector::getInstance()->setEnabled(true);

	changed();    //fileDocument
}