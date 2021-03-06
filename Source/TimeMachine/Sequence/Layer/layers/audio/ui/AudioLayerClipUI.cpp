/*
  ==============================================================================

	AudioLayerClipUI.cpp
	Created: 8 Feb 2017 12:20:09pm
	Author:  Ben

  ==============================================================================
*/

#include "AudioLayerClipUI.h"
#include "UI/ChataigneAssetManager.h"

AudioLayerClipUI::AudioLayerClipUI(AudioLayerClip * _clip) :
	BaseItemUI(_clip),
	thumbnailCache(100000),
	thumbnail(50, _clip->formatManager, thumbnailCache),
	clip(_clip)
{
	browseBT = AssetManager::getInstance()->getFileBT();
	addAndMakeVisible(browseBT);
	browseBT->addListener(this);
	clip->addAsyncClipListener(this);

	bgColor = clip->isCurrent ? AUDIO_COLOR.brighter() : BG_COLOR.brighter(.1f);

#if JUCE_WINDOWS
	if (clip->filePath->stringValue().startsWithChar('/')) return;
#endif

	thumbnail.setSource(new FileInputSource(clip->filePath->getFile()));

	lockUI = item->isLocked->createImageToggle(ChataigneAssetManager::getInstance()->getToggleBTImage(ChataigneAssetManager::getInstance()->getLockImage()));
	addAndMakeVisible(lockUI);

	repaint();
}

AudioLayerClipUI::~AudioLayerClipUI()
{
	if (!inspectable.wasObjectDeleted()) clip->removeAsyncClipListener(this);
}

void AudioLayerClipUI::paint(Graphics & g)
{
	BaseItemUI::paint(g);

	if (item->isLocked->boolValue())
	{
		g.setTiledImageFill(ChataigneAssetManager::getInstance()->stripeImage, 0, 0, .1f);
		g.fillRoundedRectangle(getMainBounds().toFloat(), 2);
	}

	if (clip->filePath->stringValue().isEmpty()) return;
	g.setColour(Colours::white.withAlpha(.5f));


	if (clip->isLoading)
	{
		g.setFont(20);
		g.drawText("Loading...", getLocalBounds(), Justification::centred);
		
	} else
	{
		thumbnail.drawChannels(g, getLocalBounds(), 0, item->clipDuration, item->volume->floatValue());
	}
}

void AudioLayerClipUI::resizedInternalHeader(Rectangle<int>& r)
{
	browseBT->setBounds(r.removeFromRight(r.getHeight()));
	lockUI->setBounds(r.removeFromRight(r.getHeight()));
	r.removeFromRight(2);

}

void AudioLayerClipUI::mouseDown(const MouseEvent & e)
{
	BaseItemUI::mouseDown(e);
	if (item->isLocked->boolValue()) return;
	timeAtMouseDown = clip->time->floatValue();
	posAtMouseDown = getX();
}

void AudioLayerClipUI::mouseDrag(const MouseEvent & e)
{
	BaseItemUI::mouseDrag(e);
	if (item->isLocked->boolValue()) return;
	clipUIListeners.call(&ClipUIListener::clipUIDragged, this, e);
}

void AudioLayerClipUI::mouseUp(const MouseEvent & e)
{
	BaseItemUI::mouseUp(e);
	if (item->isLocked->boolValue()) return;
	item->time->setUndoableValue(timeAtMouseDown, item->time->floatValue());
}

void AudioLayerClipUI::buttonClicked(Button * b)
{
	BaseItemUI::buttonClicked(b);
	if (b == browseBT)
	{
		FileChooser chooser("Load a carrot", File(), "*.wav;*.mp3;*.aiff");
		bool result = chooser.browseForFileToOpen();
		if (result) clip->filePath->setUndoableValue(clip->filePath->stringValue(),chooser.getResult().getFullPathName());
	}
}

void AudioLayerClipUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	if (c == item->time || c == item->clipLength)
	{
		clipUIListeners.call(&ClipUIListener::clipUITimeChanged, this);
		repaint();
	} else if (c == item->volume)
	{
		repaint();
	} else if (c == item->isLocked)
	{
		repaint();
	}
}

void AudioLayerClipUI::newMessage(const AudioLayerClip::ClipEvent & e)
{
	switch (e.type)
	{
	case AudioLayerClip::ClipEvent::CLIP_IS_CURRENT_CHANGED:
		bgColor = clip->isCurrent ? AUDIO_COLOR.brighter() : BG_COLOR.brighter(.1f);
		repaint();
		break;

	case AudioLayerClip::ClipEvent::SOURCE_LOAD_START:
		thumbnail.setSource(nullptr);
		repaint();
		break;

	case AudioLayerClip::ClipEvent::SOURCE_LOAD_END:
		thumbnail.setSource(new FileInputSource(clip->filePath->getFile()));
		repaint();
		break;
	
	}
	
}