/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_QAM4_MAPPER_IMPL_H
#define INCLUDED_S4A_QAM4_MAPPER_IMPL_H

#include <s4a/qam4_mapper.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

class qam4_mapper_impl: public qam4_mapper
{
private:
	std::vector<gr_complex> d_symbols;

public:
	qam4_mapper_impl();
	~qam4_mapper_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_QAM4_MAPPER_IMPL_H */

