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
#include "coherence_time_counter_generator_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

coherence_time_counter_generator::sptr coherence_time_counter_generator::make(
		int packet_header_sz, int packet_header_val, int packet_counter_sz,
		int packet_rep, int packet_zero_padding)
{
	return gnuradio::get_initial_sptr(
			new coherence_time_counter_generator_impl(packet_header_sz,
					packet_header_val, packet_counter_sz, packet_rep,
					packet_zero_padding));
}

/*
 * The private constructor
 */
coherence_time_counter_generator_impl::coherence_time_counter_generator_impl(
		int packet_header_sz, int packet_header_val, int packet_counter_sz,
		int packet_rep, int packet_zero_padding) :
		gr::block("coherence_time_counter_generator",
				gr::io_signature::make(0, 0, 0),
				gr::io_signature::make(1, 1, sizeof(unsigned char))), d_packet_header_sz(
				packet_header_sz), d_packet_header_val(packet_header_val), d_packet_counter_sz(
				packet_counter_sz), d_packet_rep_idx(0), d_packet_rep(
				packet_rep), d_packet_zero_padding(packet_zero_padding), d_counter(
				0), d_packet_total_bits(0), d_state(ST_HEADER), d_i(0)
{
	if (packet_header_sz < 8)
	{
		throw std::runtime_error(
				"Packet header size should be greater than 8 bits");
	}

	if (packet_header_sz % 8 != 0)
	{
		throw std::runtime_error(
				"Packet header size should be multiple of 8 bits");
	}

	if (packet_counter_sz < 8)
	{
		throw std::runtime_error(
				"Packet counter size should be greater than 8 bits");
	}

	if (packet_counter_sz % 8 != 0)
	{
		throw std::runtime_error(
				"Packet counter size should be multiple of 8 bits");
	}

	d_packet_total_bits = packet_rep * (packet_header_sz + packet_counter_sz)
			+ packet_zero_padding;

	printf(
			"init::coherence_time_counter_generator:: packet size: %d, header size: %d, counter size: %d, packet repetition: %d, zero padding: %d\n",
			d_packet_total_bits, packet_header_sz, packet_counter_sz,
			packet_rep, packet_zero_padding);

	set_output_multiple(d_packet_total_bits);
}

/*
 * Our virtual destructor.
 */
coherence_time_counter_generator_impl::~coherence_time_counter_generator_impl()
{
}

void coherence_time_counter_generator_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

int coherence_time_counter_generator_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	unsigned char *out = (unsigned char *) output_items[0];

	int i = 0;
	int j = 0;
	int no = 0;
	int counter = 0;

	while (no < noutput_items)
	{
		switch (d_state)
		{
		case ST_HEADER:
		{
			if (d_i < d_packet_header_sz)
			{
				out[no] = ((d_packet_header_val >> d_i) & 0x1);
				d_i++;
				no++;
			}
			else
			{
				d_i = 0;
				d_state = ST_COUNTER;
			}
			break;
		}
		case ST_COUNTER:
		{
			if (d_i < d_packet_counter_sz)
			{
				out[no] = ((d_counter >> d_i) & 0x1);
				d_i++;
				no++;
				d_state = ST_COUNTER;
			}
			else
			{
				d_i = 0;
				d_packet_rep_idx++;
				d_state = ST_REPEAT;
			}
			break;
		}
		case ST_REPEAT:
		{
			if (d_packet_rep_idx < d_packet_rep)
			{
				d_packet_rep_idx++;
				d_state = ST_HEADER;
			}
			else
			{
				d_i = 0;
				d_packet_rep_idx = 0;
				d_state = ST_ZEROS;
			}
			break;
		}
		case ST_ZEROS:
		{
			if (d_i < d_packet_zero_padding)
			{
				out[no] = 0x00;
				d_i++;
				no++;
				d_state = ST_ZEROS;
			}
			else
			{
				d_counter++;
				d_i = 0;
				d_state = ST_HEADER;
			}
			break;
		}
		default:
			throw std::runtime_error(
					"coherence_time_counter_generator::invalid state\n");
		}
	}

	return noutput_items;
}

} /* namespace s4a */
} /* namespace gr */

