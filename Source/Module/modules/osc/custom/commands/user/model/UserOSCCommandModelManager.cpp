/*
  ==============================================================================

    UserOSCCommandModelManager.cpp
    Created: 15 Jan 2017 4:52:31pm
    Author:  Ben

  ==============================================================================
*/

#include "UserOSCCommandModelManager.h"

UserOSCCommandModelManager::UserOSCCommandModelManager() :
	BaseManager("Models")
{
	itemDataType = "OSCModel";
}

UserOSCCommandModelManager::~UserOSCCommandModelManager()
{
}

void UserOSCCommandModelManager::addItemInternal(UserOSCCommandModel *, var data)
{
	DBG("Add item internal model");
}
