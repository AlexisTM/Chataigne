/*
  ==============================================================================

    BaseManagerUI.h
    Created: 28 Oct 2016 8:03:45pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef BASEMANAGERUI_H_INCLUDED
#define BASEMANAGERUI_H_INCLUDED

#pragma warning(disable:4244)

#include "BaseManager.h"
#include "BaseItemUI.h"
#include "Style.h"

template<class M, class T, class U>
class BaseManagerUI :
	public InspectableComponent,
	public BaseManager<T>::Listener
{
public:
	BaseManagerUI<M, T, U>(const String &contentName, M * _manager);
	virtual ~BaseManagerUI();

	M * manager;
	OwnedArray<U> itemsUI;
	
	
	//ui
	Colour bgColor;
	int labelHeight = 10;
	String managerUIName;

	String addItemText;
	bool drawContour;
	bool drawHighlightWhenSelected;

	int itemHeight = 20;
	int gap = 2;

	virtual void mouseDown(const MouseEvent &e) override;
	virtual void paint(Graphics &g) override;

	virtual void resized() override;
	
	virtual void addItemFromMenu();

	virtual U * addItemUI(T * item);
	virtual void addItemUIInternal(U *) {}
	virtual void removeItemUI(T * item);
	virtual void removeItemUIInternal(U *) {}

	U * getUIForItem(T * item);

	void itemAdded(BaseItem * item) override;
	void itemRemoved(BaseItem * item) override;


};

template<class M, class T, class U>
BaseManagerUI<M, T, U>::BaseManagerUI(const String & contentName, M * _manager) :
	manager(_manager),
	drawContour(false),
	bgColor(BG_COLOR),
	managerUIName(contentName)
{
	relatedControllableContainer = static_cast<ControllableContainer *>(manager);
	static_cast<BaseManager<T>*>(manager)->addBaseManagerListener(this);
	addItemText = "Add Item";
}


template<class M, class T, class U>
BaseManagerUI<M, T, U>::~BaseManagerUI()
{
	static_cast<BaseManager<T>*>(manager)->removeBaseManagerListener(this);
}

template<class M, class T, class U>
void BaseManagerUI<M, T, U>::mouseDown(const MouseEvent & e)
{
	InspectableComponent::mouseDown(e);
	if (e.mods.isLeftButtonDown())
	{
	} else if (e.mods.isRightButtonDown())
	{
		PopupMenu p;
		p.addItem(1, addItemText);
		int result = p.show();
		if (result)
		{
			switch (result)
			{
			case 1:
				addItemFromMenu();
				break;
			}
		}
	} 
}

template<class M, class T, class U>
void BaseManagerUI<M, T, U>::paint(Graphics & g)
{
	if (drawContour)
	{
		Rectangle<int> r = getLocalBounds();
		g.setColour(bgColor);
		g.fillRoundedRectangle(r.toFloat(),4);
		Colour contourColor = bgColor.brighter(.3f);
		g.setColour(contourColor);
		g.drawRoundedRectangle(r.toFloat(), 4, 2);
		
		g.setFont(g.getCurrentFont().withHeight(labelHeight));
		float textWidth = g.getCurrentFont().getStringWidth(managerUIName);
		Rectangle<int> tr = r.removeFromTop(labelHeight+2).reduced((r.getWidth() - textWidth) / 2, 0).expanded(4,0);
		g.fillRect(tr);
		Colour textColor = contourColor.withBrightness(contourColor.getBrightness() > .5f ? .1f : .9f).withAlpha(1.f);
		g.setColour(textColor);

		g.drawText(manager->niceName, tr, Justification::centred, 1);
	}else
	{
		g.fillAll(bgColor);
	}
}

template<class M, class T, class U>
void BaseManagerUI<M, T, U>::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	if (drawContour) r.removeFromTop(15);
	for (auto &ui : itemsUI)
	{
		BaseItemUI<T> * bui = static_cast<BaseItemUI<T>*>(ui);
		bui->setBounds(r.removeFromTop(itemHeight));
		r.removeFromTop(gap);
	}
}

template<class M, class T, class U>
inline void BaseManagerUI<M, T, U>::addItemFromMenu()
{
	manager->BaseManager<T>::addItem();
}

template<class M, class T, class U>
U * BaseManagerUI<M, T, U>::addItemUI(T * item)
{
	U * tui = new U(item);
	itemsUI.add(tui);
	addAndMakeVisible(static_cast<BaseItemUI<T>*>(tui));
	addItemUIInternal(tui);
	resized();

	return tui;
}

template<class M, class T, class U>
void BaseManagerUI<M, T, U>::removeItemUI(T * item)
{
	U * tui = getUIForItem(item);
	if (tui == nullptr) return;

	removeChildComponent(static_cast<BaseItemUI<T>*>(tui));
	removeItemUIInternal(tui);
	itemsUI.removeObject(tui);
	resized();
}

template<class M, class T, class U>
U * BaseManagerUI<M, T, U>::getUIForItem(T * item)
{
	for (auto &ui : itemsUI) if (static_cast<BaseItemUI<T>*>(ui)->item == item) return ui;
	return nullptr;
}

template<class M, class T, class U>
void BaseManagerUI<M, T, U>::itemAdded(BaseItem * item)
{
	addItemUI(static_cast<T*>(item));
}

template<class M, class T, class U>
void BaseManagerUI<M, T, U>::itemRemoved(BaseItem * item)
{
	removeItemUI(static_cast<T*>(item));
}


#endif  // BASEMANAGERUI_H_INCLUDED
