/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_PARALLEL_TO_SERIAL_IMPL_H
#define INCLUDED_S4A_PARALLEL_TO_SERIAL_IMPL_H

#include <s4a/parallel_to_serial.h>

namespace gr {
  namespace s4a {

    class parallel_to_serial_impl : public parallel_to_serial
    {
     private:
  	int d_parallel_size;

     public:
      parallel_to_serial_impl(int parallel_size);
      ~parallel_to_serial_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items_v,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_PARALLEL_TO_SERIAL_IMPL_H */

