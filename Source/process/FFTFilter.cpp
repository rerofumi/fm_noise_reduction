#include "FFTFilter.h"

FFTFilter::FFTFilter(int segment_length, int buffer_length) :
	FilterBase(segment_length, buffer_length)
	, forwardFFT(fftOrder)
	, window(fftSize, juce::dsp::WindowingFunction<float>::rectangular)
	, fftbuf_in(new float[segment_length])
	, fftbuf_out(new float[segment_length])
	, fft_window(new float[segment_length])
	, level(new float[spectrumNum])
{
	fftIn.calloc(segment_length);
	fftOut.calloc(segment_length);
	for (auto i = 0; i < segment_length; i++) {
		fft_window.get()[i] = 1.0f;
	}
	for (auto i = 0; i < segment_length / 4; i++) {
		float att = std::sinf((float)i / (float)(segment_length / 4) * (3.1415926f * 0.5));
		fft_window.get()[i] = att;
		fft_window.get()[segment_length - 1 - i] = att;
	}
}

FFTFilter::~FFTFilter()
{
	fftOut.free();
	fftIn.free();
}

std::shared_ptr<float> FFTFilter::getSpectrumInput(void)
{
	auto LevelRange = juce::FloatVectorOperations::findMinAndMax(fftbuf_in.get(), segment_length / 2);
	auto LevelMax = juce::jmax(LevelRange.getEnd(), 200.0f);
	auto LevelMin = juce::jmin(LevelRange.getStart(), 0.0f);
	auto mindB = -60.0f;
	auto maxdB = 0.0f;
	for (int i = 0; i < spectrumNum; i++) {
		auto pos = (int)((segment_length / 2) * i / spectrumNum);
		auto data = juce::jmap(fftbuf_in.get()[pos], LevelMin, LevelMax, 0.0f, 1.0f);
		auto power = juce::jmap(
			juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(data)),
			mindB, maxdB, 0.0f, 1.0f
		);
		level.get()[i] = power;
	}
	setProcessFlag(false);
	return level;
}


std::shared_ptr<float> FFTFilter::getSpectrumOutput(void)
{
	auto LevelRange = juce::FloatVectorOperations::findMinAndMax(fftbuf_in.get(), segment_length / 2);
	auto LevelMax = juce::jmax(LevelRange.getEnd(), 200.0f);
	auto LevelMin = juce::jmin(LevelRange.getStart(), 0.0f);
	auto mindB = -60.0f;
	auto maxdB = 0.0f;
	for (int i = 0; i < spectrumNum; i++) {
		auto pos = (int)((segment_length / 2) * i / spectrumNum);
		auto data = juce::jmap(fftbuf_out.get()[pos], LevelMin, LevelMax, 0.0f, 1.0f);
		auto power = juce::jmap(
			juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(data)),
			mindB, maxdB, 0.0f, 1.0f
		);
		level.get()[i] = power;
	}
	return level;
}

void FFTFilter::filter_process(float* in_data, float* out_data, int size, int side)
{
	{
		//window.multiplyWithWindowingTable(in_data, size);
		// store input for spectrum
		for (int i = 0; i < size; i++) {
			fftIn.getData()[i].real(in_data[i] * fft_window.get()[i]);
			fftIn.getData()[i].imag(0.0f);
		}
		// FFT forward
		forwardFFT.perform(fftIn.getData(), fftOut.getData(), false);
		// Spectrum Analyzer data - in
		if (side == 0) {
			for (int i = 0; i < size; i++) {
				fftbuf_in.get()[i] = std::sqrtf(fftOut.get()[i].real() * fftOut.get()[i].real() + fftOut.get()[i].imag() * fftOut.get()[i].imag());
			}
		}
		// effect work
		effect(fftOut, fftIn, size, side);
		// store output for spectrum
		if (side == 0) {
			for (int i = 0; i < size; i++) {
				fftbuf_out.get()[i] = std::sqrtf(fftIn.get()[i].real() * fftIn.get()[i].real() + fftIn.get()[i].imag() * fftIn.get()[i].imag());
			}
		}
		// inverse FFT
		forwardFFT.perform(fftIn.getData(), fftOut.getData(), true);
		// copy to output
		for (int i = 0; i < size; i++) {
			out_data[i] = (float)(fftOut.get()[i].real());
		}
	}
}

void FFTFilter::effect(const juce::HeapBlock<juce::dsp::Complex<float>>& in_data, juce::HeapBlock<juce::dsp::Complex<float>>& out_data, int size, int side)
{
	// no imprement
}
