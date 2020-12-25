/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_COHERENCE_TIME_COUNTER_GENERATOR_IMPL_H
#define INCLUDED_S4A_COHERENCE_TIME_COUNTER_GENERATOR_IMPL_H

#include <s4a/coherence_time_counter_generator.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

class coherence_time_counter_generator_impl: public coherence_time_counter_generator
{
private:
	int d_packet_header_sz;
	int d_packet_header_val;
	int d_packet_counter_sz;
	int d_packet_rep;
	int d_packet_zero_padding;

	int d_packet_total_bits;
	int d_packet_rep_idx;

	int d_counter;
	unsigned int d_i;

	enum state_t
	{
		ST_HEADER = 0, ST_COUNTER = 1, ST_REPEAT = 2, ST_ZEROS = 3
	};

	state_t d_state;

public:
	coherence_time_counter_generator_impl(int packet_header_sz,
			int packet_header_val, int packet_counter_sz, int packet_rep,
			int packet_zero_padding);
	~coherence_time_counter_generator_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_COHERENCE_TIME_COUNTER_GENERATOR_IMPL_H */

