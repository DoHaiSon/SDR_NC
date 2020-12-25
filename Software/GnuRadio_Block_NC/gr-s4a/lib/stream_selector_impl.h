/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_STREAM_SELECTOR_IMPL_H
#define INCLUDED_S4A_STREAM_SELECTOR_IMPL_H

#include <s4a/stream_selector.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

class stream_selector_impl: public stream_selector
{
private:
	enum state_t
	{
		ST_CHECK, ST_SYMS, ST_ZEROS
	};
	state_t d_state;

	int d_magnitude_min;
	int d_magnitude_max;

	int d_input_stream_length;
	int d_output_stream_length;

	int d_stream_selector_sz;

	int d_syms_idx;
	int d_zero_idx;

public:
	stream_selector_impl(int magnitude_min, int magnitude_max,
			int input_stream_length, int output_stream_length);
	~stream_selector_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_STREAM_SELECTOR_IMPL_H */

