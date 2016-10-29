/*
  ==============================================================================

    MappingManager.cpp
    Created: 28 Oct 2016 8:06:00pm
    Author:  bkupe

  ==============================================================================
*/

#include "MappingManager.h"

juce_ImplementSingleton(MappingManager)

MappingManager::MappingManager() :
	BaseManager<Mapping>("Mappings")
{
}

MappingManager::~MappingManager()
{
}
