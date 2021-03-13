/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "process/ReductionFilter.h"

//==============================================================================
/**
*/
class Fm_noise_reducerAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	Fm_noise_reducerAudioProcessor();
	~Fm_noise_reducerAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//===== mycode
	juce::Image& getSpectrumView();
	void setNoiseThreshold(float threshold);
	void setFreqBand(float* freq, int size);
	float getNoiseThreshold();
	float* getFreqBand();
	void startNoiseCapture();
	bool isUpdateParameter();

private:
	//===== plugin parameter
	bool update_parameter_flag;
	juce::AudioParameterFloat* prm_reduction_threshold;
	float post_reduction_threshold;

	//===== mycode
	void drawSpectrumGraph(juce::Image view, std::shared_ptr<float> level, juce::Colour color, bool isLog);
	void drawLevelGate(juce::Image view, float freq, float level);
	void sync_plugin_parameter();
	juce::Image spectrum;
	ReductionFilter myFilter;
	double if_samplerate;
	int if_samplesPerBlock;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fm_noise_reducerAudioProcessor)
};
