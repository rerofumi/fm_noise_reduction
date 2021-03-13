#include "FilterBase.h"

FilterBase::FilterBase(int segment_length, int buffer_length) :
	segment_length(segment_length),
	buffer_length(buffer_length),
	input_pointer(0),
	output_buffer_pointer(0),
	output_pointer(0),
	process_done(false)
{
	input_buffer[0] = new float[segment_length];
	input_buffer[1] = new float[segment_length];
	input_buffer[2] = new float[segment_length];
	output_buffer[0] = new float[buffer_length];
	output_buffer[1] = new float[buffer_length];
	filter_buffer[0] = new float[segment_length];
	filter_buffer[1] = new float[segment_length];
	mix_table = new float[segment_length / 4];
	{
		int size = segment_length / 4;
		for (auto i = 0; i < size; i++) {
			float p = (float)i / (float)size;
			mix_table[i] = std::sinf(3.1415926f / 2.0f * p);
		}
	}
}

FilterBase::~FilterBase()
{
	std::lock_guard<std::mutex> lock(finish_lock);
	delete[] input_buffer[0];
	delete[] input_buffer[1];
	delete[] input_buffer[2];
	delete[] output_buffer[0];
	delete[] output_buffer[1];
	delete[] filter_buffer[0];
	delete[] filter_buffer[1];
	delete[] mix_table;
}

void FilterBase::setProcessFlag(bool flag)
{
	std::lock_guard<std::mutex> lock(flag_lock);
	process_done = flag;
}

bool FilterBase::getProcessFlag()
{
	std::lock_guard<std::mutex> lock(flag_lock);
	return process_done;
}

void FilterBase::process(float* buffer_1, float* buffer_2, int size)
{
	std::lock_guard<std::mutex> lock(finish_lock);
	for (size_t copy_counter = 0; copy_counter < size; copy_counter++) {
		input_buffer[0][input_pointer] = buffer_1[copy_counter];
		input_buffer[1][input_pointer] = buffer_2[copy_counter];
		input_pointer = (input_pointer + 1) % segment_length;
		if (input_pointer == 0) {
			filter_process(input_buffer[0], filter_buffer[0], segment_length, 0);
			filter_process(input_buffer[1], filter_buffer[1], segment_length, 1);
			{
				int offset = segment_length / 4;
#if 0
				int outptr_overrap = (output_buffer_pointer + buffer_length - offset) % buffer_length;
				for (size_t i = 0; i < offset; i++) {
					output_buffer[0][outptr_overrap] = output_buffer[0][outptr_overrap] * (1.0f - mix_table[i]) + filter_buffer[0][i] * mix_table[i];
					output_buffer[1][outptr_overrap] = output_buffer[1][outptr_overrap] * (1.0f - mix_table[i]) + filter_buffer[1][i] * mix_table[i];
					outptr_overrap = (outptr_overrap + 1) % buffer_length;
				}
#endif
				for (size_t i = 0; i < segment_length / 2; i++) {
					output_buffer[0][output_buffer_pointer] = filter_buffer[0][i + offset];
					output_buffer[1][output_buffer_pointer] = filter_buffer[1][i + offset];
					output_buffer_pointer = (output_buffer_pointer + 1) % buffer_length;
				}
			}
			setProcessFlag(true);
		}
		else if (input_pointer == segment_length / 2) {
			int shift = segment_length / 2;
			for (size_t i = 0; i < segment_length; i++) {
				input_buffer[2][i] = input_buffer[0][shift];
				shift = (shift + 1) % segment_length;
			}
			filter_process(input_buffer[2], filter_buffer[0], segment_length, 0);
			for (size_t i = 0; i < segment_length; i++) {
				input_buffer[2][i] = input_buffer[1][shift];
				shift = (shift + 1) % segment_length;
			}
			filter_process(input_buffer[2], filter_buffer[1], segment_length, 1);
			{
				int offset = segment_length / 4;
#if 0
				int outptr_overrap = (output_buffer_pointer + buffer_length - offset) % buffer_length;
				for (size_t i = 0; i < offset; i++) {
					output_buffer[0][outptr_overrap] = output_buffer[0][outptr_overrap] * (1.0f - mix_table[i]) + filter_buffer[0][i] * mix_table[i];
					output_buffer[1][outptr_overrap] = output_buffer[1][outptr_overrap] * (1.0f - mix_table[i]) + filter_buffer[1][i] * mix_table[i];
					outptr_overrap = (outptr_overrap + 1) % buffer_length;
				}
#endif
				for (size_t i = 0; i < segment_length / 2; i++) {
					output_buffer[0][output_buffer_pointer] = filter_buffer[0][i + offset];
					output_buffer[1][output_buffer_pointer] = filter_buffer[1][i + offset];
					output_buffer_pointer = (output_buffer_pointer + 1) % buffer_length;
				}
			}
			setProcessFlag(true);
		}
	}
}

int FilterBase::output(float* buffer_1, float* buffer_2, int size)
{
	for (size_t counter = 0; counter < size; counter++) {
		buffer_1[counter] = 0;
		buffer_2[counter] = 0;
	}
	int filtered_size = (output_buffer_pointer + buffer_length - output_pointer) % buffer_length;
	if (filtered_size < size) {
		return 0;
	}
	for (size_t counter = 0; counter < size; counter++) {
		buffer_1[counter] = output_buffer[0][output_pointer];
		buffer_2[counter] = output_buffer[1][output_pointer];
		output_pointer = (output_pointer + 1) % buffer_length;
	}
	return size;
}

void FilterBase::filter_process(float* in_data, float* out_data, int size, int side)
{
	// sample: copy only
	for (int i = 0; i < size; i++) {
		out_data[i] = in_data[i];
	}
}

