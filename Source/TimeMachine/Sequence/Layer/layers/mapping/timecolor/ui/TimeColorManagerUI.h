/*
  ==============================================================================

    TimeColorManagerUI.h
    Created: 11 Apr 2017 11:40:47am
    Author:  Ben

  ==============================================================================
*/

#ifndef TIMECOLORMANAGERUI_H_INCLUDED
#define TIMECOLORMANAGERUI_H_INCLUDED

#include "../TimeColorManager.h"
#include "TimeColorUI.h"

class TimeColorManagerUI :
	public BaseManagerUI<TimeColorManager, TimeColor, TimeColorUI>,
	public ContainerAsyncListener,
	public TimeColorManager::TimeColorManagerListener

{
public:
	TimeColorManagerUI(TimeColorManager * manager);
	~TimeColorManagerUI();

	float viewStartPos;
	float viewEndPos;

	void setViewRange(float start, float end);

	void paint(Graphics &g) override;

	void resized() override;

    void addItemUIInternal(TimeColorUI * item) override;
    void removeItemUIInternal(TimeColorUI * item) override;

	void mouseDoubleClick(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;

	void placeItemUI(TimeColorUI * tui);

	int getXForPos(float time);
	float getPosForX(int tx, bool offsetStart = true);

	bool isInView(TimeColorUI * tui);

	void newMessage(const ContainerAsyncEvent &e) override;

	void gradientUpdated() override;
	
};



#endif  // TIMECOLORMANAGERUI_H_INCLUDED
