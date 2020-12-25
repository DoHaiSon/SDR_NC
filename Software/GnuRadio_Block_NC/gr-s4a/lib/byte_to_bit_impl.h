/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_BYTE_TO_BIT_IMPL_H
#define INCLUDED_S4A_BYTE_TO_BIT_IMPL_H

#include <s4a/byte_to_bit.h>

namespace gr
{
namespace s4a
{

class byte_to_bit_impl: public byte_to_bit
{
private:
	// Nothing to declare in this block.

public:
	byte_to_bit_impl();
	~byte_to_bit_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_BYTE_TO_BIT_IMPL_H */

