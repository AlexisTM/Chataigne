/*
  ==============================================================================

    ProcessorManagerUI.cpp
    Created: 15 Oct 2017 1:26:01pm
    Author:  Ben

  ==============================================================================
*/

#include "ProcessorManagerUI.h"

ProcessorManagerUI::ProcessorManagerUI(ProcessorManager * _manager) :
	BaseManagerUI("Processors", _manager)
{
	addExistingItems();
}

ProcessorManagerUI::~ProcessorManagerUI()
{
}

ProcessorUI * ProcessorManagerUI::createUIForItem(Processor *p)
{
	return p->getUI();
}
