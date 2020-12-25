/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

#include <gnuradio/io_signature.h>
#include "channel_reciprocity_tester_impl.h"

namespace gr
{
namespace s4a
{

channel_reciprocity_tester::sptr channel_reciprocity_tester::make(
		int channel_size, int required_data_bytes_on_each_burst,
		float power_current, float power_step, float power_max, float power_min,
		int working_mode, int node_id, int remote_node_id,
		int response_timeout_ms, int wrong_packet_discard_limit,
		int tx_packet_repetition, float snr_limit, int snr_console_print_rate)
{
	return gnuradio::get_initial_sptr(
			new channel_reciprocity_tester_impl(channel_size,
					required_data_bytes_on_each_burst, power_current,
					power_step, power_max, power_min, working_mode, node_id,
					remote_node_id, response_timeout_ms,
					wrong_packet_discard_limit, tx_packet_repetition, snr_limit,
					snr_console_print_rate));
}

/*
 * The private constructor
 */
channel_reciprocity_tester_impl::channel_reciprocity_tester_impl(
		int channel_size, int required_data_bytes_on_each_burst,
		float power_current, float power_step, float power_max, float power_min,
		int working_mode, int node_id, int remote_node_id,
		int response_timeout_ms, int wrong_packet_discard_limit,
		int tx_packet_repetition, float snr_limit, int snr_console_print_rate) :
		gr::block("channel_reciprocity_tester",
				gr::io_signature::make3(3, 3,
						required_data_bytes_on_each_burst * sizeof(char),
						channel_size * sizeof(gr_complex), sizeof(float)),
				gr::io_signature::make3(3, 3,
						required_data_bytes_on_each_burst * sizeof(char),
						sizeof(gr_complex),
						(1 + 1 + 1 + 1 + channel_size) * sizeof(gr_complex))), d_channel_size(
				channel_size), d_rbytes(required_data_bytes_on_each_burst), d_power_current(
				power_current), d_power_step(power_step), d_power_max(
				power_max), d_power_min(power_min), d_node_id(node_id), d_remote_node_id(
				remote_node_id), d_response_timeout_ms(response_timeout_ms), d_wrong_packet_idx(
				0), d_wrong_packet_discard_limit(wrong_packet_discard_limit), d_tx_packet_idx(
				0), d_out_node_id(0.0), d_out_remote_node_id(0.0), d_tx_packet_repetition(
				tx_packet_repetition), d_snr_limit(snr_limit), d_snr_console_print_rate(
				snr_console_print_rate), d_tx_total(10), d_tx_idx(0), d_count(
				0), d_clock(0), d_elapsed_time(0.0f), d_out_snr(0.0)
{
	printf("init::channel_reciprocity_tester\n");
	printf("init::channel_reciprocity_tester::node_id: \n", node_id);
	printf("init::channel_reciprocity_tester::metrics_length: %d \n",
			(1 + 1 + channel_size) * sizeof(gr_complex));

	if (working_mode == 0)
	{
		printf("init::channel_reciprocity_tester:: MASTER\n");
		d_state = ST_TRANSMIT;
		d_working_mode = MASTER;
	}
	else if (working_mode == 1)
	{
		printf("init::channel_reciprocity_tester:: SLAVE\n");
		d_state = ST_RECEIVE;
		d_working_mode = SLAVE;
	}
	else
	{
		printf("init::channel_reciprocity_tester:: SPY\n");
		d_state = ST_RECEIVE;
		d_working_mode = SPY;
	}

	d_initial_clock = clock();

	d_out_h.resize(channel_size);
	std::fill(d_out_h.begin(), d_out_h.end(), gr_complex(0, 0));

	gettimeofday(&t_start, NULL);

	d_out_node_id = (float) d_node_id;
}

/*
 * Our virtual destructor.
 */
channel_reciprocity_tester_impl::~channel_reciprocity_tester_impl()
{
}

void channel_reciprocity_tester_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

int channel_reciprocity_tester_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items_bytes = ninput_items_v[0];
	int ninput_items_h = ninput_items_v[1];
	int ninput_items_snr = ninput_items_v[2];

	const char *in_bytes = (const char *) input_items[0];
	const gr_complex *in_h = (const gr_complex *) input_items[1];
	const float *in_snr = (const float *) input_items[2];

	char *out_bytes = (char *) output_items[0];
	gr_complex *out_power = (gr_complex *) output_items[1];
	gr_complex *out_est_metrics = (gr_complex *) output_items[2];

	int ni = 0;
	int no = 0;

	int i = 0;
	int j = 0;

	while (ni < ninput_items_bytes || d_state == ST_TRANSMIT)
	{
		switch (d_state)
		{
		case ST_RECEIVE:
		{
			if (in_snr[ni] > d_snr_limit)
			{
				unsigned char resolved_node_id;

				// Check if the data contains remote node id
				resolved_node_id = (unsigned char) in_bytes[ni * d_rbytes];
				for (j = 0; j < d_rbytes; j++)
				{
					resolved_node_id &= (unsigned char) in_bytes[ni * d_rbytes
							+ j];
				}

				if (resolved_node_id == d_remote_node_id)
				{
					gettimeofday(&t_end, NULL);
					seconds = t_end.tv_sec - t_start.tv_sec;
					useconds = t_end.tv_usec - t_start.tv_usec;
					mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

					if (d_count % d_snr_console_print_rate == 0)
					{
						printf("\n%ld ms: ", mtime);
						printf("SNR = %f , ", in_snr[ni]);
						printf("ID = %#02X \n", resolved_node_id);
					}

					d_count++;
					d_wrong_packet_idx = 0;

					// Store time
					d_out_time = float(mtime);

					// Store SNR
					d_out_snr = in_snr[ni];

					// Store the channel
					for (j = 0; j < d_channel_size; j++)
					{
						d_out_h[j] = in_h[ni * d_channel_size + j];
					}

					d_tx_packet_idx = 0;
					d_state = ST_TRANSMIT;
				}
				else if (resolved_node_id == d_node_id)
				{
					d_state = ST_RECEIVE;
				}
				else if (resolved_node_id == 0x00)
				{
					d_state = ST_RECEIVE;
				}
				else
				{
					if (d_working_mode == MASTER)
					{
						d_wrong_packet_idx++;

						// Check wrong packet limit!
						if (d_wrong_packet_idx >= d_wrong_packet_discard_limit)
						{
							printf("\nMASTER PACKET TIMEOUT: %d !",
									d_wrong_packet_idx);
							d_wrong_packet_idx = 0;
							d_tx_packet_idx = 0;
							d_state = ST_TRANSMIT;
						}

						// Check timeout
						gettimeofday(&t_end, NULL);
						seconds = t_end.tv_sec - t_tx.tv_sec;
						useconds = t_end.tv_usec - t_tx.tv_usec;
						mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;
						if (mtime > d_response_timeout_ms)
						{
							printf("\nMASTER RX TIMEOUT: %ld ms !", mtime);
							d_wrong_packet_idx = 0;
							d_tx_packet_idx = 0;
							d_state = ST_TRANSMIT;
						}
						else
						{
							d_state = ST_RECEIVE;
						}
					}
					else if (d_working_mode == SPY)
					{
						gettimeofday(&t_end, NULL);
						seconds = t_end.tv_sec - t_start.tv_sec;
						useconds = t_end.tv_usec - t_start.tv_usec;
						mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

						if (d_count % d_snr_console_print_rate == 0)
						{
							printf("\nSPY MODE: time = %ld ms: ", mtime);
							printf("SNR = %f , ", in_snr[ni]);
							printf("ID = %#02X \n", resolved_node_id);
						}

						d_count++;
						d_wrong_packet_idx = 0;

						// Store time
						d_out_time = float(mtime);

						// Store SNR
						d_out_snr = in_snr[ni];

						// Store received node id
						d_out_remote_node_id = (float) resolved_node_id;

						// Store the channel
						for (j = 0; j < d_channel_size; j++)
						{
							d_out_h[j] = in_h[ni * d_channel_size + j];
						}

						d_tx_packet_idx = 0;
						d_state = ST_TRANSMIT;
					}
					else
					{
						d_wrong_packet_idx == 0;
					}
				}
			}
			else
			{
				/*
				 If we don't receive anything because of low SNR, master
				 should try to send a new request.
				 */
				if (d_working_mode == MASTER)
				{
					// Check timeout
					gettimeofday(&t_end, NULL);
					seconds = t_end.tv_sec - t_tx.tv_sec;
					useconds = t_end.tv_usec - t_tx.tv_usec;
					mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

					if (mtime > d_response_timeout_ms)
					{
						d_wrong_packet_idx = 0;
						d_tx_packet_idx = 0;
						d_state = ST_TRANSMIT;
					}
					else
					{
						d_state = ST_RECEIVE;
					}
				}
			}
			ni++;
			break;
		}
		case ST_TRANSMIT:
		{
			while (no < noutput_items
					&& d_tx_packet_idx < d_tx_packet_repetition)
			{
				// Push the bytes to be transmitted next
				int i = 0;
				for (i = 0; i < d_rbytes; i++)
				{
					out_bytes[no * d_rbytes + i] = (unsigned char) (d_node_id);
					//printf("%#02X ",  (unsigned char) (d_node_id));
				}

				// Push the power scaling coeff. for the next transmission
				out_power[no] = d_power_current;

				// Push the time from the previous reception
				out_est_metrics[no * (4 + d_channel_size) + 0] = gr_complex(
						d_out_time, 0);

				// Push the SNR from the previous reception
				out_est_metrics[no * (4 + d_channel_size) + 1] = gr_complex(
						d_out_snr, 0);

				// Push the current node id
				out_est_metrics[no * (4 + d_channel_size) + 2] = gr_complex(
						d_out_node_id, 0);

				// Push the remote node id
				out_est_metrics[no * (4 + d_channel_size) + 3] = gr_complex(
						d_out_remote_node_id, 0);

				// Push the channel (h) from the previous reception
				for (i = 0; i < d_channel_size; i++)
				{
					out_est_metrics[no * (4 + d_channel_size) + 4 + i] =
							d_out_h[i];
				}
				no++;
				d_tx_packet_idx++;
				d_state = ST_TRANSMIT;
			}

			gettimeofday(&t_tx, NULL);

			d_state = ST_RECEIVE;
			break;
		}
		default:
		{
			throw std::invalid_argument(
					"channel_reciprocity_tester::invalid state");
			break;
		}
		}
	}

	consume(0, ni);
	consume(1, ni);
	consume(2, ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

