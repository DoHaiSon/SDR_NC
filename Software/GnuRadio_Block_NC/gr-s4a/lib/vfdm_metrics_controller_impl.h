/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_METRICS_CONTROLLER_IMPL_H
#define INCLUDED_S4A_VFDM_METRICS_CONTROLLER_IMPL_H

#include <s4a/vfdm_metrics_controller.h>
#include <gnuradio/fft/fft.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

class vfdm_metrics_controller_impl: public vfdm_metrics_controller
{
private:
	int d_data_syms_length;
	int d_channel_size;
	int d_metrics_size;
	int d_normalize_h;

	fft::fft_complex *d_ifft;

	gr_complex* d_nulled_syms;
	gr_complex* d_nulled_h_t;

public:
	vfdm_metrics_controller_impl(int data_syms_length, int channel_size,
			int normalize_h);
	~vfdm_metrics_controller_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_METRICS_CONTROLLER_IMPL_H */

