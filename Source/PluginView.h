/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "PluginProcessor.h"
//[/Headers]



//==============================================================================
/**
																	//[Comments]
	An auto-generated component, created by the Projucer.

	Describe your class and how it works here!
																	//[/Comments]
*/
class PluginView : public juce::Component,
	public juce::Slider::Listener,
	public juce::Button::Listener
{
public:
	//==============================================================================
	PluginView();
	~PluginView() override;

	//==============================================================================
	//[UserMethods]     -- You can add your own custom methods in this section.
	void setProcesser(Fm_noise_reducerAudioProcessor& p);
	//[/UserMethods]

	void paint(juce::Graphics& g) override;
	void resized() override;
	void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;
	void buttonClicked(juce::Button* buttonThatWasClicked) override;



private:
	//[UserVariables]   -- You can add your own custom variables in this section.
	Fm_noise_reducerAudioProcessor* processer;
	//[/UserVariables]

	//==============================================================================
	std::unique_ptr<juce::Slider> NoiseThreshold;
	std::unique_ptr<juce::TextButton> NoiseLearnButton;


	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

