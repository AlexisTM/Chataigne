/*
  ==============================================================================

    MappingOutput.h
    Created: 28 Oct 2016 8:12:01pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MAPPINGOUTPUT_H_INCLUDED
#define MAPPINGOUTPUT_H_INCLUDED

#include "BaseItem.h"

class MappingOutput :
	public BaseItem
{
public:
	MappingOutput();
	virtual ~MappingOutput();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingOutput)
};

#endif  // MAPPINGOUTPUT_H_INCLUDED
