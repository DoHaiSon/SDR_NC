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
#include "random_bit_generator_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

random_bit_generator::sptr random_bit_generator::make(int sequence_size,
		int seed, bool repeat)
{
	return gnuradio::get_initial_sptr(
			new random_bit_generator_impl(sequence_size, seed, repeat));
}

/*
 * The private constructor
 */
random_bit_generator_impl::random_bit_generator_impl(int sequence_size,
		int seed, bool repeat) :
		gr::block("random_bit_generator", gr::io_signature::make(0, 0, 0),
				gr::io_signature::make(1, 1, sizeof(char))), d_sequence_size(
				sequence_size), d_step_size(0), d_repeat(repeat), d_seed(seed)
{
	printf("init::random_bit_generator\n");

	if (sequence_size % 8 != 0)
	{
		throw std::runtime_error("Sequence size should be multiple of 8");
	}

	d_step_size = sequence_size / 8;

	d_number_sequence.resize(d_step_size);
	std::fill(d_number_sequence.begin(), d_number_sequence.end(), 0);

	int i = 0;
	int j = 0;
	srand(seed);

	for (i = 0; i < d_step_size; i++)
	{
		d_number_sequence[i] = rand() % 255;
	}

	// Print Results
	printf("number_sequence=[");
	for (i = 0; i < d_step_size; i++)
	{
		printf("%d, ", d_number_sequence[i]);
	}
	printf("]\n");

	unsigned char bit = 0;
	printf("bit_sequence=[");
	for (i = 0; i < d_step_size; i++)
	{
		for (j = 0; j < 8; j++)
		{
			bit = (d_number_sequence[i] >> j) & 0x1;
			printf("%d, ", bit);
		}
	}
	printf("]\n");
}

/*
 * Our virtual destructor.
 */
random_bit_generator_impl::~random_bit_generator_impl()
{
}

void random_bit_generator_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
}

int random_bit_generator_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
	char *out = (char *) output_items[0];

	memcpy(&out[0], &d_number_sequence[0], d_step_size * sizeof(unsigned char));

	return d_step_size;
}

} /* namespace s4a */
} /* namespace gr */

