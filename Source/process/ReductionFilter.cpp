#pragma once
#include "ReductionFilter.h"


ReductionFilter::ReductionFilter(int segment_length, int buffer_length) :
	FFTFilter(segment_length, buffer_length)
	, learn_table(new float[segment_length])
	, learn_table_db(new float[segment_length])
	, display_level(new float[segment_length])
	, analyze_buffer(new float[segment_length])
	, post_filter(new float[segment_length])
	, reduction_threshold(0.0)
	, sample_rate(48000.0)
	, capture_trigger(0)
	, gate_level(1.0)
{
	for (int i = 0; i < segment_length; i++) {
		learn_table.get()[i] = 0.0f;
		learn_table_db.get()[i] = 0.0f;
		display_level.get()[i] = 0.0f;
		analyze_buffer.get()[i] = 0.0f;
		post_filter.get()[i] = 0.0f;
	}
}

ReductionFilter::~ReductionFilter()
{
}

void ReductionFilter::setNoiseThreshold(float threshold, float if_samplerate)
{
	reduction_threshold = threshold;
	sample_rate = if_samplerate;
}

void ReductionFilter::setLearnTable(float* table, int size)
{
	auto loopcount = juce::jmin(size, segment_length);
	for (auto i = 0; i < loopcount; i++) {
		learn_table.get()[i] = table[i];
		learn_table_db.get()[i] = juce::Decibels::gainToDecibels(table[i]);
	}
}

void ReductionFilter::startCapture()
{
	capture_trigger = 100;
}

float ReductionFilter::getNoiseThreshold()
{
	return reduction_threshold;
}

std::shared_ptr<float>& ReductionFilter::getLearnTable()
{
	return learn_table;
}

std::shared_ptr<float>& ReductionFilter::getReductionLevel(void)
{
	auto LevelRange = juce::FloatVectorOperations::findMinAndMax(learn_table.get(), segment_length / 2);
	auto LevelMax = juce::jmax(LevelRange.getEnd(), 200.0f);
	auto LevelMin = juce::jmin(LevelRange.getStart(), 0.0f);
	auto mindB = -60.0f;
	auto maxdB = 0.0f;
	for (int i = 0; i < spectrumNum; i++) {
		auto pos = (int)((segment_length / 2) * i / spectrumNum);
		auto level = learn_table_db.get()[pos] + (reduction_threshold * 24.0f);
		auto data = juce::jmap(juce::Decibels::decibelsToGain(level), LevelMin, LevelMax, 0.0f, 1.0f);
		auto power = juce::jmap(
			juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(data)),
			mindB, maxdB, 0.0f, 1.0f
		);
		display_level.get()[i] = power;
	}
	return display_level;
}


void ReductionFilter::do_capture()
{
	float  pow;
	learn_table.get()[0] = analyze_buffer.get()[0];
	learn_table.get()[1] = analyze_buffer.get()[1];
	learn_table.get()[segment_length - 1] = analyze_buffer.get()[segment_length - 1];
	learn_table.get()[segment_length - 2] = analyze_buffer.get()[segment_length - 2];
	float* inbuf = analyze_buffer.get();
	for (auto i = 2; i < segment_length - 2; i++) {
		pow = inbuf[i - 2] + inbuf[i - 1] + inbuf[i] * 4 + inbuf[i + 1] + inbuf[i + 2];
		learn_table.get()[i] = (learn_table.get()[i] * 29.0f + (pow / 9.0f)) / 30.0f;
	}
	for (int i = 0; i < segment_length; i++) {
		learn_table_db.get()[i] = juce::Decibels::gainToDecibels(learn_table.get()[i]);
	}
}


void ReductionFilter::update_level()
{
	int fft_size = FFTFilter::fftSize;
	int fq_size = fft_size / 2;
	float level, data, gain_level;
	// noise gate
	for (auto i = 0; i < segment_length / 2; i++) {
		level = juce::Decibels::gainToDecibels(analyze_buffer.get()[i]);
		data = learn_table_db.get()[i] + (reduction_threshold * 24.0f);
		if ((level - data) > 8.0f) {
			post_filter.get()[i] = 1.0f;
			analyze_buffer.get()[i] = 1.0f;
		}
		else if ((level - data) < -8.0f) {
			post_filter.get()[i] *= 0.9f;
			analyze_buffer.get()[i] = post_filter.get()[i];
		}
		else {
			gain_level = juce::jmin((level - data + 8.0f) / 16.0f, 1.0f);
			post_filter.get()[i] = (post_filter.get()[i] * 19.0f + gain_level) / 20.0f;
			analyze_buffer.get()[i] = post_filter.get()[i] * post_filter.get()[i];
		}
	}
	// -----------------------------
	// complement table to complex conjugate
	// -----------------------------
	for (auto i = 0; i < fq_size - 1; i++) {
		auto attenuator = analyze_buffer.get()[i + 1];
		analyze_buffer.get()[fft_size - 1 - i] = attenuator;
	}
	//
	analyze_buffer.get()[0] = 0.0f;  //DC
	analyze_buffer.get()[segment_length / 2] = 1.0f;
}

void ReductionFilter::effect(const juce::HeapBlock<juce::dsp::Complex<float>>& in_data, juce::HeapBlock<juce::dsp::Complex<float>>& out_data, int size, int side)
{
	{
		// level check
		for (auto i = 0; i < segment_length; i++) {
			analyze_buffer.get()[i] = std::sqrtf(in_data.get()[i].real() * in_data.get()[i].real() + in_data.get()[i].imag() * in_data.get()[i].imag());
		}
		if (capture_trigger > 0) {
			do_capture();
			capture_trigger -= 1;
		}
		update_level();
		// level control
		for (int i = 0; i < size; i++) {
			out_data.get()[i].real(in_data.get()[i].real() * analyze_buffer.get()[i] * gate_level);
			out_data.get()[i].imag(in_data.get()[i].imag() * analyze_buffer.get()[i] * gate_level);
		}
	}
}