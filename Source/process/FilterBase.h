#pragma once
#include <memory>
#include <mutex>


class FilterBase
{
public:
	FilterBase(int segment_length, int buffer_length);
	virtual ~FilterBase();

	/* --- method*/
	void process(float* buffer_1, float* buffer_2, int size);
	int output(float* buffer_1, float* buffer_2, int size);
	void setProcessFlag(bool flag);
	bool getProcessFlag();
	/* --- member */
protected:
	/* --- method*/
	/* --- member */
	std::mutex flag_lock;
	std::mutex finish_lock;
	bool process_done;
	int segment_length;
	int buffer_length;
private:
	/* --- method*/
	virtual void filter_process(float* in_data, float* out_data, int size, int side);
	/* --- member */
	float* input_buffer[3];
	float* output_buffer[2];
	float* filter_buffer[2];
	float* mix_table;
	int input_pointer;
	int output_buffer_pointer;
	int output_pointer;
};

