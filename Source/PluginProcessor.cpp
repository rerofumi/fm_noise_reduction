/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Fm_noise_reducerAudioProcessor::Fm_noise_reducerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
	, spectrum(juce::Image::RGB, 512, 256, true)
	, myFilter(ReductionFilter::fftSize, ReductionFilter::fftSize * 4)
	, if_samplerate(48000.0)
	, if_samplesPerBlock(512)
	, update_parameter_flag(false)
{
	// plugin parameter
	addParameter(prm_reduction_threshold = new juce::AudioParameterFloat("PrmReductionThreshold", "ReductionThreshold", -1.0f, 1.0f, 0.0f));
	post_reduction_threshold = *prm_reduction_threshold;
}

Fm_noise_reducerAudioProcessor::~Fm_noise_reducerAudioProcessor()
{
}

//==============================================================================
const juce::String Fm_noise_reducerAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool Fm_noise_reducerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool Fm_noise_reducerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool Fm_noise_reducerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double Fm_noise_reducerAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int Fm_noise_reducerAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int Fm_noise_reducerAudioProcessor::getCurrentProgram()
{
	return 0;
}

void Fm_noise_reducerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String Fm_noise_reducerAudioProcessor::getProgramName(int index)
{
	return {};
}

void Fm_noise_reducerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void Fm_noise_reducerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	if_samplerate = sampleRate;
	if_samplesPerBlock = samplesPerBlock;
}

void Fm_noise_reducerAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Fm_noise_reducerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void Fm_noise_reducerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	/* --- my processing code*/
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	auto data_size = buffer.getNumSamples();
	float* channel_1 = buffer.getWritePointer(0);
	float* channel_2 = buffer.getWritePointer(1);
	// do work -----
	myFilter.process(channel_1, channel_2, data_size);
	myFilter.output(channel_1, channel_2, data_size);
	// -----
}

//==============================================================================
bool Fm_noise_reducerAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Fm_noise_reducerAudioProcessor::createEditor()
{
	return new Fm_noise_reducerAudioProcessorEditor(*this);
}

//==============================================================================
void Fm_noise_reducerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// store parameter
	std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("ParamFmNoiceReducer"));
	xml->setAttribute("reduction_threshold", (double)*prm_reduction_threshold);
	{
		juce::String store;
		float* table = getFreqBand();
		for (auto i = 0; i < ReductionFilter::fftSize; i++) {
			if (i != 0) {
				store += ",";
			}
			store += juce::String(table[i]);
		}
		xml->setAttribute("frequency_table", store);
	}
	copyXmlToBinary(*xml, destData);
}

void Fm_noise_reducerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// read parameter
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState.get() != nullptr) {
		if (xmlState->hasTagName("ParamFmNoiceReducer")) {
			*prm_reduction_threshold = (float)xmlState->getDoubleAttribute("reduction_threshold", 0.0);
			juce::String table = (juce::String)xmlState->getStringAttribute("frequency_table", "");
			{
				std::shared_ptr<float> freq;
				juce::String chunk = "";
				freq.reset(new float[ReductionFilter::fftSize]);
				int count = 0;
				for (auto i = 0; i < table.length(); i++) {
					if (table.substring(i, i + 1) == ",") {
						float dat = chunk.getFloatValue();
						freq.get()[count] = dat;
						chunk = "";
						count += 1;
						if (count >= ReductionFilter::fftSize) break;
					}
					else {
						chunk += table.substring(i, i + 1);
					}
				}
				myFilter.setLearnTable(freq.get(), ReductionFilter::fftSize);
			}
		}
	}
	post_reduction_threshold = 0.0;
	update_parameter_flag = true;
	sync_plugin_parameter();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new Fm_noise_reducerAudioProcessor();
}


//==============================================================================
// add my methods
//==============================================================================
/*---
* change parameter
*/
void Fm_noise_reducerAudioProcessor::setNoiseThreshold(float threshold)
{
	myFilter.setNoiseThreshold(threshold, (float)if_samplerate);
	*prm_reduction_threshold = threshold;
	post_reduction_threshold = threshold;
}


float Fm_noise_reducerAudioProcessor::getNoiseThreshold()
{
	return *prm_reduction_threshold;
}


void Fm_noise_reducerAudioProcessor::startNoiseCapture()
{
	myFilter.startCapture();
}


void Fm_noise_reducerAudioProcessor::setFreqBand(float* freq, int size)
{
	myFilter.setLearnTable(freq, size);
}

float* Fm_noise_reducerAudioProcessor::getFreqBand()
{
	return myFilter.getLearnTable().get();
}

bool  Fm_noise_reducerAudioProcessor::isUpdateParameter()
{
	bool result = update_parameter_flag;
	return result;
}


void Fm_noise_reducerAudioProcessor::sync_plugin_parameter()
{
	if (*prm_reduction_threshold != post_reduction_threshold) {
		myFilter.setNoiseThreshold(*prm_reduction_threshold, (float)if_samplerate);
		post_reduction_threshold = *prm_reduction_threshold;
		update_parameter_flag = true;
	}
}


/* ---
* rendering spectrum view
*/
juce::Image& Fm_noise_reducerAudioProcessor::getSpectrumView()
{
	if (myFilter.getProcessFlag()) {
		spectrum.clear(juce::Rectangle<int>(512, 256), juce::Colour(0, 0, 0));
		// noise reduction threshold
		auto level_reduction = myFilter.getReductionLevel();
		drawSpectrumGraph(spectrum, level_reduction, juce::Colour(200, 200, 0), true);
		// input level
		auto level = myFilter.getSpectrumInput();
		drawSpectrumGraph(spectrum, level, juce::Colour(0, 200, 0), true);
		// output level
		auto level_out = myFilter.getSpectrumOutput();
		drawSpectrumGraph(spectrum, level_out, juce::Colour(200, 0, 80), true);
		//
		myFilter.setProcessFlag(false);
	}
	return spectrum;
}

void Fm_noise_reducerAudioProcessor::drawLevelGate(juce::Image view, float freq, float level)
{
	float pos = (float)juce::jlimit(0, 512, (int)(512.0f * freq / ((float)if_samplerate / 2.0f)));
	float skewedProportionX = std::exp(std::log((float)pos / 512.0f) * 0.2f);
	float line_x = (float)juce::jlimit(0, (int)ReductionFilter::spectrumNum, (int)(skewedProportionX * (float)ReductionFilter::spectrumNum));
	juce::Graphics g(view);
	g.setColour(juce::Colour(0, 120, 120));
	g.setOpacity(1.0);
	g.drawLine(line_x, 0.0f, line_x, 255.0f * level);
}

/* --- draw graph */
void Fm_noise_reducerAudioProcessor::drawSpectrumGraph(juce::Image view, std::shared_ptr<float> power, juce::Colour color, bool isLog)
{
	int post_point = 0;
	float post_level = 0.0f;
	juce::Graphics g(view);
	for (int x = 1; x < 512; x++) {
		float skewedProportionX = 0.0f;
		if (isLog) {
			skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)x / 512.0f) * 0.2f);
		}
		else {
			skewedProportionX = (float)x / 512.0f;
		}
		auto fftDataIndex = juce::jlimit(0, (int)ReductionFilter::spectrumNum, (int)(skewedProportionX * (float)ReductionFilter::spectrumNum));
		auto lv = power.get()[fftDataIndex];
		if ((std::fabs(post_level - lv) > 0.0001) || (x == 511) || (!isLog)) {
			g.setColour(color);
			g.setOpacity(1.0);
			g.drawLine(
				(float)post_point,
				juce::jmap(post_level, 0.0f, 1.0f, 256.0f, 0.0f),
				(float)x,
				juce::jmap(lv, 0.0f, 1.0f, 256.0f, 0.0f)
			);
			{
				g.setOpacity(0.5);
				juce::Path pen;
				pen.startNewSubPath(juce::Point<float>((float)post_point, juce::jmap(post_level, 0.0f, 1.0f, 256.0f, 0.0f)));
				pen.lineTo(juce::Point<float>((float)x, juce::jmap(lv, 0.0f, 1.0f, 256.0f, 0.0f)));
				pen.lineTo(juce::Point<float>((float)x, 256.0f));
				pen.lineTo(juce::Point<float>((float)post_point, 256.0f));
				pen.closeSubPath();
				g.fillPath(pen);
			}
			post_point = x;
			post_level = lv;
		}
	}
	g.setOpacity(1.0);
	//
	// update plugin parameter
	sync_plugin_parameter();
}
