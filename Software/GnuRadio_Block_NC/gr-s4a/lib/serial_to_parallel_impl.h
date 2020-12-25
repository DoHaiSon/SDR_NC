/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_SERIAL_TO_PARALLEL_IMPL_H
#define INCLUDED_S4A_SERIAL_TO_PARALLEL_IMPL_H

#include <s4a/serial_to_parallel.h>

namespace gr
{
namespace s4a
{

class serial_to_parallel_impl: public serial_to_parallel
{
private:
	int d_parallel_size;

public:
	serial_to_parallel_impl(int parallel_size);
	~serial_to_parallel_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_SERIAL_TO_PARALLEL_IMPL_H */

