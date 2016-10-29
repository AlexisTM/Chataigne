/*
  ==============================================================================

    Mapping.h
    Created: 28 Oct 2016 8:06:13pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MAPPING_H_INCLUDED
#define MAPPING_H_INCLUDED


#include "BaseItem.h"

class Mapping :
	public BaseItem
{
public:
	Mapping();
	virtual ~Mapping();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mapping)
};




#endif  // MAPPING_H_INCLUDED
