/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Fm_noise_reducerAudioProcessorEditor::Fm_noise_reducerAudioProcessorEditor(Fm_noise_reducerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setSize(536, 430);
	addAndMakeVisible(gui_component);
	startTimerHz(10);
	gui_component.setSize(this->getWidth(), this->getHeight());
	gui_component.setProcesser(p);
}

Fm_noise_reducerAudioProcessorEditor::~Fm_noise_reducerAudioProcessorEditor()
{
}

//==============================================================================
void Fm_noise_reducerAudioProcessorEditor::paint(juce::Graphics& g)
{
}

void Fm_noise_reducerAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
}

void Fm_noise_reducerAudioProcessorEditor::timerCallback()
{
	repaint();
}
