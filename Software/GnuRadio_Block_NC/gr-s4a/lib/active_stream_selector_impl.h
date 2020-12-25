/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_ACTIVE_STREAM_SELECTOR_IMPL_H
#define INCLUDED_S4A_ACTIVE_STREAM_SELECTOR_IMPL_H

#include <s4a/active_stream_selector.h>

namespace gr
{
namespace s4a
{

class active_stream_selector_impl: public active_stream_selector
{
private:
	enum state_t
	{
		ST_FIRST_CHECK, ST_FIRST_OUT, ST_SECOND_CHECK, ST_SECOND_OUT
	};
	state_t d_state;

	int d_stream_idx;
	int d_first_stream_length;
	int d_second_stream_length;
	int d_total_length;

public:
	active_stream_selector_impl(int first_stream_length,
			int second_stream_length);
	~active_stream_selector_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_ACTIVE_STREAM_SELECTOR_IMPL_H */

