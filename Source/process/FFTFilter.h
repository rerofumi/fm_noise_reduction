#pragma once
#include "FilterBase.h"
#include <JuceHeader.h>

class FFTFilter : public FilterBase
{
public:
	FFTFilter(int segment_length, int buffer_length);
	virtual ~FFTFilter();

	/* --- method*/
	std::shared_ptr<float> getSpectrumInput(void);
	std::shared_ptr<float> getSpectrumOutput(void);
	/* --- member */
	enum
	{
		fftOrder = 10,
		fftSize = 1 << fftOrder,  //1024
		spectrumNum = 512
	};
protected:
	/* --- member */
	std::shared_ptr<float> fftbuf_in, fftbuf_out, fft_window;
private:
	/* --- method*/
	void filter_process(float* in_data, float* out_data, int size, int side) override;
	virtual void effect(const juce::HeapBlock<juce::dsp::Complex<float>>& in_data, juce::HeapBlock<juce::dsp::Complex<float>>& out_data, int size, int side);
	/* --- member */
	std::shared_ptr<float> level;
	juce::dsp::FFT forwardFFT;
	juce::dsp::WindowingFunction<float> window;
	//
	juce::HeapBlock<juce::dsp::Complex<float>> fftIn, fftOut;
};
