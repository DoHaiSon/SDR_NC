/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CSI_CONVERTER_IMPL_H
#define INCLUDED_S4A_CSI_CONVERTER_IMPL_H

#include <s4a/csi_converter.h>
#include <gnuradio/fft/fft.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

class csi_converter_impl: public csi_converter
{
private:
	int d_data_syms_length;
	int d_channel_size;

	fft::fft_complex *d_ifft;

public:
	csi_converter_impl(int data_syms_length, int channel_size);
	~csi_converter_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CSI_CONVERTER_IMPL_H */

