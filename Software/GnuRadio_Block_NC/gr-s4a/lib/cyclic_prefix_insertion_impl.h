/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CYCLIC_PREFIX_INSERTION_IMPL_H
#define INCLUDED_S4A_CYCLIC_PREFIX_INSERTION_IMPL_H

#include <s4a/cyclic_prefix_insertion.h>

namespace gr
{
namespace s4a
{

class cyclic_prefix_insertion_impl: public cyclic_prefix_insertion
{
private:
	int d_fft_length;
	int d_cp_length;
	int d_out_length;

public:
	cyclic_prefix_insertion_impl(int fft_length, int cp_length);
	~cyclic_prefix_insertion_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CYCLIC_PREFIX_INSERTION_IMPL_H */

