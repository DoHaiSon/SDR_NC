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
#include "active_stream_selector_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

active_stream_selector::sptr active_stream_selector::make(
		int first_stream_length, int second_stream_length)
{
	return gnuradio::get_initial_sptr(
			new active_stream_selector_impl(first_stream_length,
					second_stream_length));
}

/*
 * The private constructor
 */
active_stream_selector_impl::active_stream_selector_impl(
		int first_stream_length, int second_stream_length) :
		gr::block("active_stream_selector",
				gr::io_signature::make2(2, 2, sizeof(gr_complex),
						sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(gr_complex))), d_stream_idx(
				0), d_first_stream_length(first_stream_length), d_second_stream_length(
				second_stream_length), d_state(ST_FIRST_CHECK)
{
	printf("init::active_stream_selector");
	printf("first_stream_length = %d, ", first_stream_length);
	printf("second_stream_length = %d \n ", second_stream_length);

	d_total_length = d_first_stream_length + d_second_stream_length;
	set_output_multiple(d_total_length);
}

/*
 * Our virtual destructor.
 */
active_stream_selector_impl::~active_stream_selector_impl()
{
}

void active_stream_selector_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % d_total_length))
			/ d_total_length;
	ninput_items_required[0] = d_first_stream_length * coeff;
	ninput_items_required[1] = d_second_stream_length * coeff;
}

int active_stream_selector_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{

	int ninput_items1 = ninput_items_v[0];
	int ninput_items2 = ninput_items_v[1];

	const gr_complex *in1 = (const gr_complex *) input_items[0];
	const gr_complex *in2 = (const gr_complex *) input_items[1];

	gr_complex *out = (gr_complex *) output_items[0];

	int ni = 0;
	int ni1 = 0;
	int ni2 = 0;
	int no = 0;
	int i = 0;
	bool is_turn_completed = false;

	for (i = 0; i < 4; i++)
	{
		switch (d_state)
		{
		case ST_FIRST_CHECK:
		{
			printf("FIRST CHECK\n");
			if ((ninput_items1 - ni1) > 0)
			{
				d_stream_idx = 0;
				is_turn_completed = false;
				d_state = ST_FIRST_OUT;
			}
			else
			{
				d_state = ST_SECOND_CHECK;
			}
			break;
		}
		case ST_SECOND_CHECK:
		{
			printf("SECOND CHECK\n");
			if ((ninput_items2 - ni2) > 0)
			{
				d_stream_idx = 0;
				is_turn_completed = false;
				d_state = ST_SECOND_OUT;
			}
			else
			{
				d_state = ST_FIRST_CHECK;
			}
			break;
		}
		case ST_FIRST_OUT:
		{
			printf("FIRST OUT\n");
			while (ni1 < ninput_items1 && no < noutput_items
					&& is_turn_completed == false)
			{
				//printf("ni1 < ninput_items1 %d / %d\n",ni1, ninput_items1);
				out[no] = in1[ni1];
				d_stream_idx++;
				ni1++;
				no++;

				if (d_stream_idx == d_first_stream_length)
				{
					d_stream_idx = 0;
					d_state = ST_SECOND_CHECK;
					is_turn_completed = true;
				}
			}

			break;
		}
		case ST_SECOND_OUT:
		{
			printf("SECOND OUT\n");
			while (ni2 < ninput_items2 && no < noutput_items
					&& is_turn_completed == false)
			{
				//printf("ni2 < ninput_items2  %d / %d \n",ni2, ninput_items2);
				out[no] = in2[ni2];
				d_stream_idx++;
				ni2++;
				no++;

				if (d_stream_idx == d_second_stream_length)
				{
					d_stream_idx = 0;
					d_state = ST_FIRST_CHECK;
					is_turn_completed = true;
				}
			}
			break;
		}
		default:
			throw std::invalid_argument(
					"active_stream_selector::invalid state");
			break;
		}
	}
	//printf("active_stream_selector > ni1 = %d,  ninput_items1 = %d, ni2 = %d, ninput_items2 = %d, no = %d, noutput_items = %d\n", ni1, ninput_items1, ni2, ninput_items2, no,noutput_items);
	consume(0, ni1);
	consume(1, ni2);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

