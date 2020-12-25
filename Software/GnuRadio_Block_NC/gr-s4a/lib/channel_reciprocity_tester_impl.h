/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CHANNEL_RECIPROCITY_TESTER_IMPL_H
#define INCLUDED_S4A_CHANNEL_RECIPROCITY_TESTER_IMPL_H

#include <s4a/channel_reciprocity_tester.h>
#include <vector>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

namespace gr
{
namespace s4a
{

class channel_reciprocity_tester_impl: public channel_reciprocity_tester
{
private:
	enum state_t
	{
		ST_RECEIVE = 0, ST_TRANSMIT = 1
	};

	enum working_mode_t
	{
		MASTER = 0, SLAVE = 1, SPY = 3
	};

	int d_channel_size;
	int d_rbytes;
	float d_power_current;
	float d_power_step;
	float d_power_max;
	float d_power_min;
	int d_node_id;
	int d_remote_node_id;
	int d_response_timeout_ms;

	int d_wrong_packet_idx;
	int d_wrong_packet_discard_limit;

	int d_tx_packet_idx;
	int d_tx_packet_repetition;

	float d_snr_limit;
	int d_snr_console_print_rate;

	state_t d_state;
	working_mode_t d_working_mode;

	int d_count;

	int d_tx_idx;
	int d_tx_total;

	int d_rbytes_idx;

	long int d_clock;
	long int d_clock_diff;

	clock_t d_initial_clock;
	double d_elapsed_time;

	// Output buffers
	float d_out_time;
	float d_out_snr;
	float d_out_node_id;
	float d_out_remote_node_id;

	std::vector<gr_complex> d_out_h;

	/* Time */
	struct timeval t_start, t_tx, t_end;
	long mtime, seconds, useconds;

public:
	channel_reciprocity_tester_impl(int channel_size,
			int required_data_bytes_on_each_burst, float power_current,
			float power_step, float power_max, float power_min,
			int working_mode, int node_id, int remote_node_id,
			int response_timeout_ms, int wrong_packet_discard_limit,
			int tx_packet_repetition, float snr_limit,
			int snr_console_print_rate);

	~channel_reciprocity_tester_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CHANNEL_RECIPROCITY_TESTER_IMPL_H */

