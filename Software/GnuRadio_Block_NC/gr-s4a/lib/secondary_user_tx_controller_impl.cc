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

#include <gnuradio/io_signature.h>
#include "secondary_user_tx_controller_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <time.h>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

secondary_user_tx_controller::sptr secondary_user_tx_controller::make(
		int channel_size, int required_data_bytes_on_each_burst,
		int working_mode, int symbol_flow, int node_id, int remote_node_id,
		int response_timeout_ms, int wrong_packet_discard_limit,
		int tx_packet_repetition, float snr_limit, int snr_console_print_rate)
{
	return gnuradio::get_initial_sptr(
			new secondary_user_tx_controller_impl(channel_size,
					required_data_bytes_on_each_burst, working_mode,
					symbol_flow, node_id, remote_node_id, response_timeout_ms,
					wrong_packet_discard_limit, tx_packet_repetition, snr_limit,
					snr_console_print_rate));
}

/*
 * The private constructor
 */
secondary_user_tx_controller_impl::secondary_user_tx_controller_impl(
		int channel_size, int required_data_bytes_on_each_burst,
		int working_mode, int symbol_flow, int node_id, int remote_node_id,
		int response_timeout_ms, int wrong_packet_discard_limit,
		int tx_packet_repetition, float snr_limit, int snr_console_print_rate) :
		gr::block("secondary_user_tx_controller",
				gr::io_signature::make3(3, 3,
						required_data_bytes_on_each_burst * sizeof(char),
						channel_size * sizeof(gr_complex), sizeof(float)),
				gr::io_signature::make2(2, 2,
						required_data_bytes_on_each_burst * sizeof(char),
						(int(pow(2, ceil(log2(5 + channel_size)))))
								* sizeof(gr_complex))), d_channel_size(
				channel_size), d_rbytes(required_data_bytes_on_each_burst), d_node_id(
				node_id), d_remote_node_id(remote_node_id), d_response_timeout_ms(
				response_timeout_ms), d_wrong_packet_idx(0), d_wrong_packet_discard_limit(
				wrong_packet_discard_limit), d_tx_packet_idx(0), d_symbol_flow(
				symbol_flow), d_out_node_id(0.0), d_out_remote_node_id(0.0), d_tx_packet_repetition(
				tx_packet_repetition), d_snr_limit(snr_limit), d_snr_console_print_rate(
				snr_console_print_rate), d_tx_total(10), d_tx_idx(0), d_count(
				0), d_clock(0), d_elapsed_time(0.0f), d_out_snr(0.0)
{
	printf("init::secondary_user_tx_controller\n");
	printf("init::secondary_user_tx_controller::node_id: \n", node_id);
	printf("init::secondary_user_tx_controller::metrics_length: %d \n",
			(1 + 1 + channel_size) * sizeof(gr_complex));

	if (working_mode == 0)
	{
		printf("init::secondary_user_tx_controller:: MASTER\n");
		d_state = ST_TRANSMIT_OFDM;
		d_working_mode = MASTER;
	}
	else if (working_mode == 1)
	{
		printf("init::secondary_user_tx_controller:: SLAVE\n");
		d_state = ST_RECEIVE;
		d_working_mode = SLAVE;
	}
	else
	{
		printf("init::secondary_user_tx_controller:: SPY\n");
		d_state = ST_RECEIVE;
		d_working_mode = SPY;
	}

	d_initial_clock = clock();

	d_out_h.resize(channel_size);
	std::fill(d_out_h.begin(), d_out_h.end(), gr_complex(0, 0));

	gettimeofday(&t_start, NULL);

	d_out_node_id = (float) d_node_id;

	d_metrics_size = int(pow(2, ceil(log2(5 + channel_size))));

	printf("vfdm_secondary_user_tx_controller::metrics_size %d\n",
			d_metrics_size);

	//set_output_multiple(tx_packet_repetition);
}

/*
 * Our virtual destructor.
 */
secondary_user_tx_controller_impl::~secondary_user_tx_controller_impl()
{
}

void secondary_user_tx_controller_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
}

int secondary_user_tx_controller_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items_bytes = ninput_items_v[0];
	int ninput_items_h = ninput_items_v[1];
	int ninput_items_snr = ninput_items_v[2];

	int ninput_items = std::min(ninput_items_bytes, ninput_items_h);
	ninput_items = std::min(ninput_items, ninput_items_snr);

	const char *in_bytes = (const char *) input_items[0];
	const gr_complex *in_h = (const gr_complex *) input_items[1];
	const float *in_snr = (const float *) input_items[2];

	char *out_bytes = (char *) output_items[0];
	gr_complex *out_est_metrics = (gr_complex *) output_items[1];

	int ni = 0;
	int no = 0;

	int nidx_bytes = 0;
	int nidx_h = 0;
	int nidx_snr = 0;

	int nodx_bytes = 0;
	int nodx_est_metrics = 0;

	int i = 0;
	int j = 0;

	while (ni < ninput_items && no < noutput_items)
	{
		switch (d_state)
		{
		case ST_RECEIVE:
		{
			//printf("SNR = %.2f /%.2f \n", in_snr[nidx_snr], d_snr_limit);
			if (in_snr[nidx_snr] > d_snr_limit)
			{
				unsigned char resolved_node_id;

				// Check if the data contains remote node id
				resolved_node_id = (unsigned char) in_bytes[nidx_bytes];
				//printf("[");
				for (j = 0; j < d_rbytes; j++)
				{
					resolved_node_id &=
							(unsigned char) in_bytes[nidx_bytes + j];
					//printf("%#02X, ", (unsigned char)in_bytes[nidx_bytes + j]);
				}
				//printf("]\n");

				if (resolved_node_id == d_remote_node_id)
				{
					//printf("resolved_node_id = remote_node_id \n");
					gettimeofday(&t_end, NULL);
					seconds = t_end.tv_sec - t_start.tv_sec;
					useconds = t_end.tv_usec - t_start.tv_usec;
					mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

					if (d_count % d_snr_console_print_rate == 0)
					{
						printf("\n%ld ms: ", mtime);
						printf("SNR = %f , ", in_snr[nidx_snr]);
						printf("ID = %#02X \n", resolved_node_id);
					}

					d_count++;
					d_wrong_packet_idx = 0;
					//d_clock = clock();

					// Store time
					d_out_time = float(mtime);

					// Store SNR
					d_out_snr = in_snr[nidx_snr];

					// Store the channel
					for (j = 0; j < d_channel_size; j++)
					{
						d_out_h[j] = in_h[nidx_h + j];
					}

					// printf("%d, ", d_count);
					d_tx_packet_idx = 0;
					d_state = ST_TRANSMIT_VFDM;
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
							d_state = ST_TRANSMIT_OFDM;
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
							d_state = ST_TRANSMIT_OFDM;
						}
						else
						{
							if (d_symbol_flow == CONTINIOUS)
							{
								d_state = ST_TRANSMIT_ZEROS;
							}
							else
							{
								d_state = ST_RECEIVE;
							}
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
							printf("SNR = %f , ", in_snr[nidx_snr]);
							printf("ID = %#02X \n", resolved_node_id);
						}

						d_count++;
						d_wrong_packet_idx = 0;
						//d_clock = clock();

						// Store time
						d_out_time = float(mtime);

						// Store SNR
						d_out_snr = in_snr[nidx_snr];

						// Store received node id
						d_out_remote_node_id = (float) resolved_node_id;

						// Store the channel
						for (j = 0; j < d_channel_size; j++)
						{
							d_out_h[j] = in_h[nidx_h + j];
						}

						// printf("%d, ", d_count);
						d_tx_packet_idx = 0;
						d_state = ST_TRANSMIT_OFDM;
					}
					else
					{
						if (d_symbol_flow == CONTINIOUS)
						{
							d_state = ST_TRANSMIT_ZEROS;
						}
						else
						{
							d_state = ST_RECEIVE;
						}
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
				//printf("check timout!\n");
				if (d_working_mode == MASTER)
				{
					// Check timeout
					gettimeofday(&t_end, NULL);
					seconds = t_end.tv_sec - t_tx.tv_sec;
					useconds = t_end.tv_usec - t_tx.tv_usec;
					mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

					/*
					 d_clock_diff = (clock() - d_clock)
					 / ((double) CLOCKS_PER_MSEC);
					 */
					if (mtime > d_response_timeout_ms)
					{
						d_wrong_packet_idx = 0;
						d_tx_packet_idx = 0;
						d_state = ST_TRANSMIT_OFDM;
					}
					else
					{
						d_state = ST_RECEIVE;
					}
				}
				else if (d_working_mode == SPY)
				{
					d_state = ST_RECEIVE;
				}
				else
				{
					if (d_symbol_flow == CONTINIOUS)
					{
						d_state = ST_TRANSMIT_ZEROS;
					}
					else
					{
						d_state = ST_RECEIVE;
					}
				}
			}
			nidx_bytes += d_rbytes;
			nidx_h += d_channel_size;
			nidx_snr += 1;
			ni++;
			//d_state = ST_RECEIVE;
			break;
		}
		case ST_TRANSMIT_OFDM:
		{
			while (d_tx_packet_idx < d_tx_packet_repetition)
			{
				// Push the bytes to be transmitted next
				int i = 0;
				for (i = 0; i < d_rbytes; i++)
				{
					out_bytes[nodx_bytes + i] = (unsigned char) (d_node_id);
					//printf("%#02X ",  (unsigned char) (d_node_id));
				}
				//printf("\n");

				// Push the time from the previous reception
				out_est_metrics[nodx_est_metrics + 0] = gr_complex(d_out_time,0);

				// Push the SNR from the previous reception
				out_est_metrics[nodx_est_metrics + 1] = gr_complex(d_out_snr,0);

				// Push the current node id
				out_est_metrics[nodx_est_metrics + 2] = gr_complex(d_out_node_id, 0);

				// Push the remote node id
				out_est_metrics[nodx_est_metrics + 3] = gr_complex(d_out_remote_node_id, 0);

				// Push the transmission mode
				out_est_metrics[nodx_est_metrics + 4] = gr_complex(1, 0);

				// Push the channel (h) from the previous reception
				for (i = 0; i < d_channel_size; i++)
				{
					out_est_metrics[nodx_est_metrics + 5 + i] = d_out_h[i];
				}
				d_tx_packet_idx++;
				nodx_bytes += d_rbytes;
				nodx_est_metrics += d_metrics_size;
			}
			if (d_tx_packet_idx == d_tx_packet_repetition)
			{
				d_tx_packet_idx = 0;
				gettimeofday(&t_tx, NULL);
				no += d_tx_packet_repetition;
				d_state = ST_RECEIVE;
			}
			break;
		}
		case ST_TRANSMIT_OFDM_CONT:
		{
			printf("OFDM_CONT!\n");
			//cpu_wait(100000);

			while (d_tx_packet_idx < d_tx_packet_repetition)
			{
				// Push the bytes to be transmitted next
				int i = 0;
				for (i = 0; i < d_rbytes; i++)
				{
					out_bytes[nodx_bytes + i] = (unsigned char) (d_node_id);
					//printf("%#02X ",  (unsigned char) (d_node_id));
				}
				//printf("\n");

				// Push the time from the previous reception
				out_est_metrics[nodx_est_metrics + 0] = gr_complex(d_out_time,0);

				// Push the SNR from the previous reception
				out_est_metrics[nodx_est_metrics + 1] = gr_complex(d_out_snr,0);

				// Push the current node id
				out_est_metrics[nodx_est_metrics + 2] = gr_complex(d_out_node_id, 0);

				// Push the remote node id
				out_est_metrics[nodx_est_metrics + 3] = gr_complex(d_out_remote_node_id, 0);

				// Push the transmission mode
				out_est_metrics[nodx_est_metrics + 4] = gr_complex(1, 0);

				// Push the channel (h) from the previous reception
				for (i = 0; i < d_channel_size; i++)
				{
					out_est_metrics[nodx_est_metrics + 5 + i] = d_out_h[i];
				}
				d_tx_packet_idx++;
				nodx_bytes += d_rbytes;
				nodx_est_metrics += d_metrics_size;
			}
			if (d_tx_packet_idx == d_tx_packet_repetition)
			{
				d_tx_packet_idx = 0;
				gettimeofday(&t_tx, NULL);
				no += d_tx_packet_repetition;
				d_state = ST_TRANSMIT_VFDM;

				nidx_bytes += d_rbytes;
				nidx_h += d_channel_size;
				nidx_snr += 1;
				ni++;
			}
			break;
		}
		case ST_TRANSMIT_VFDM:
		{
			printf("VFDM TX!\n");
			//cpu_wait(100000);

			while (d_tx_packet_idx < d_tx_packet_repetition)
			{
				// Push the bytes to be transmitted next
				int i = 0;
				for (i = 0; i < d_rbytes; i++)
				{
					out_bytes[nodx_bytes + i] = (unsigned char) (d_node_id);
					//printf("%#02X ",  (unsigned char) (d_node_id));
				}
				//printf("\n");

				// Push the time from the previous reception
				out_est_metrics[nodx_est_metrics + 0] = gr_complex(d_out_time,
						0);

				// Push the SNR from the previous reception
				out_est_metrics[nodx_est_metrics + 1] = gr_complex(d_out_snr,
						0);

				// Push the current node id
				out_est_metrics[nodx_est_metrics + 2] = gr_complex(
						d_out_node_id, 0);

				// Push the remote node id
				out_est_metrics[nodx_est_metrics + 3] = gr_complex(
						d_out_remote_node_id, 0);

				// Push the transmission mode
				out_est_metrics[nodx_est_metrics + 4] = gr_complex(2, 0);

				// Push the channel (h) from the previous reception
				for (i = 0; i < d_channel_size; i++)
				{
					out_est_metrics[nodx_est_metrics + 5 + i] = d_out_h[i];
				}
				d_tx_packet_idx++;
				nodx_bytes += d_rbytes;
				nodx_est_metrics += d_metrics_size;
			}
			if (d_tx_packet_idx == d_tx_packet_repetition)
			{
				d_tx_packet_idx = 0;
				gettimeofday(&t_tx, NULL);
				no += d_tx_packet_repetition;
				d_state = ST_TRANSMIT_OFDM_CONT;
			}
			break;
		}
		case ST_TRANSMIT_ZEROS:
		{
			//printf("ZEROS_TX!\n");

			// Push the bytes to be transmitted next
			int i = 0;
			for (i = 0; i < d_rbytes; i++)
			{
				out_bytes[nodx_bytes + i] = (unsigned char) (d_node_id);
			}

			// Push the time from the previous reception
			out_est_metrics[nodx_est_metrics + 0] = gr_complex(d_out_time, 0);

			// Push the SNR from the previous reception
			out_est_metrics[nodx_est_metrics + 1] = gr_complex(d_out_snr, 0);

			// Push the current node id
			out_est_metrics[nodx_est_metrics + 2] = gr_complex(d_out_node_id,0);

			// Push the remote node id
			out_est_metrics[nodx_est_metrics + 3] = gr_complex(d_out_remote_node_id, 0);

			// Push the transmission mode
			out_est_metrics[nodx_est_metrics + 4] = gr_complex(0, 0);

			// Push the channel (h) from the previous reception
			for (i = 0; i < d_channel_size; i++)
			{
				out_est_metrics[nodx_est_metrics + 5 + i] = d_out_h[i];
			}

			nodx_bytes += d_rbytes;
			nodx_est_metrics += d_metrics_size;

			no += 1;
			d_state = ST_RECEIVE;
			break;
		}
		default:
		{
			throw std::invalid_argument(
					"secondary_user_tx_controller::invalid state");
			break;
		}
		}
	}
	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

