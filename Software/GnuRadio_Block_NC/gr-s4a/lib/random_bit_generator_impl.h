/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_RANDOM_BIT_GENERATOR_IMPL_H
#define INCLUDED_S4A_RANDOM_BIT_GENERATOR_IMPL_H

#include <s4a/random_bit_generator.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

class random_bit_generator_impl: public random_bit_generator
{
private:
	unsigned int d_sequence_size;
	unsigned int d_step_size;
	unsigned int d_seed;

	bool d_repeat;

	std::vector<unsigned char> d_number_sequence;

public:
	random_bit_generator_impl(int sequence_size, int seed, bool repeat);
	~random_bit_generator_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_RANDOM_BIT_GENERATOR_IMPL_H */

