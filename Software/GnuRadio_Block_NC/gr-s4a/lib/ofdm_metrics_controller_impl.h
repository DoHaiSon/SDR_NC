/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_METRICS_CONTROLLER_IMPL_H
#define INCLUDED_S4A_OFDM_METRICS_CONTROLLER_IMPL_H

#include <s4a/ofdm_metrics_controller.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

class ofdm_metrics_controller_impl: public ofdm_metrics_controller
{
private:
	int d_data_syms_length;
	int d_channel_size;
	int d_metrics_size;

	gr_complex* d_nulled_syms;
	gr_complex* d_nulled_h_t;

public:
	ofdm_metrics_controller_impl(int data_syms_length, int channel_size);
	~ofdm_metrics_controller_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_METRICS_CONTROLLER_IMPL_H */

