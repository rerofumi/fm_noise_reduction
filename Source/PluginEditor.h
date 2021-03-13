/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginView.h"

//==============================================================================
/**
*/
class Fm_noise_reducerAudioProcessorEditor :
	public juce::AudioProcessorEditor,
	private juce::Timer
{
public:
	Fm_noise_reducerAudioProcessorEditor(Fm_noise_reducerAudioProcessor&);
	~Fm_noise_reducerAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	Fm_noise_reducerAudioProcessor& audioProcessor;

	PluginView gui_component;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fm_noise_reducerAudioProcessorEditor)
};
