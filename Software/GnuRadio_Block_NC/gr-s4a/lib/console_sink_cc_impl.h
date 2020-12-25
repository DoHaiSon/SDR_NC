/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CONSOLE_SINK_CC_IMPL_H
#define INCLUDED_S4A_CONSOLE_SINK_CC_IMPL_H

#include <s4a/console_sink_cc.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

class console_sink_cc_impl: public console_sink_cc
{
private:
	enum state_t
	{
		ST_STORE = 0, ST_PRINT = 1
	};
	state_t d_state;

	std::string d_sequence_name;

	int d_sequence_idx;
	int d_sequence_length;

	int d_print_rate_idx;
	int d_print_rate;

public:
	console_sink_cc_impl(std::string sequence_name, int sequence_length,
			int print_rate);
	~console_sink_cc_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CONSOLE_SINK_CC_IMPL_H */

