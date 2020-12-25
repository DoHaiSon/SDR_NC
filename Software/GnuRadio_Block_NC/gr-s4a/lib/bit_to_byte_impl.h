/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_BIT_TO_BYTE_IMPL_H
#define INCLUDED_S4A_BIT_TO_BYTE_IMPL_H

#include <s4a/bit_to_byte.h>

namespace gr
{
namespace s4a
{

class bit_to_byte_impl: public bit_to_byte
{
private:
	char d_byte;
	unsigned int d_counter;

public:
	bit_to_byte_impl();
	~bit_to_byte_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_BIT_TO_BYTE_IMPL_H */

