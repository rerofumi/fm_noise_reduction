#pragma once
#pragma once
#include "FFTFilter.h"
#include <JuceHeader.h>

class ReductionFilter :
	public FFTFilter
{
public:
	ReductionFilter(int segment_length, int buffer_length);
	virtual ~ReductionFilter();

	/* --- method*/
	void setNoiseThreshold(float threshold, float if_samplerate);
	void setLearnTable(float* table, int size);
	void startCapture();
	std::shared_ptr<float>& getReductionLevel(void);
	float getNoiseThreshold(void);
	std::shared_ptr<float>& getLearnTable(void);

	/* --- member */
private:
	/* --- method*/
	void do_capture();
	void update_level();
	void effect(const juce::HeapBlock<juce::dsp::Complex<float>>& in_data, juce::HeapBlock<juce::dsp::Complex<float>>& out_data, int size, int side) override;

	/* --- member */
	std::shared_ptr<float> learn_table;
	std::shared_ptr<float> learn_table_db;
	std::shared_ptr<float> display_level;
	std::shared_ptr<float> analyze_buffer;
	std::shared_ptr<float> post_filter;
	float reduction_threshold;
	float sample_rate;
	float gate_level;
	int capture_trigger;
};

