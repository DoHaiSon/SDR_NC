/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "parallel_to_serial_impl.h"
#include <cstdio>

namespace gr {
  namespace s4a {

    parallel_to_serial::sptr
    parallel_to_serial::make(int parallel_size)
    {
      return gnuradio::get_initial_sptr
        (new parallel_to_serial_impl(parallel_size));
    }

    /*
     * The private constructor
     */
    parallel_to_serial_impl::parallel_to_serial_impl(int parallel_size)
      : gr::block("parallel_to_serial",
              gr::io_signature::make(1, 1,
                             sizeof(gr_complex) * parallel_size),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
     d_parallel_size(parallel_size)
    {
	  printf("init::parallel_to_serial\n");

	  set_output_multiple(d_parallel_size);
    }

    /*
     * Our virtual destructor.
     */
    parallel_to_serial_impl::~parallel_to_serial_impl()
    {
    }

    void
    parallel_to_serial_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
	  unsigned ninputs = ninput_items_required.size();
	  for (unsigned i = 0; i < ninputs; i++)
	    ninput_items_required[i] = 1;
    }

    int
    parallel_to_serial_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
	  int ninput_items = ninput_items_v[0];
	  const gr_complex *in = (const gr_complex *) input_items[0];
	  gr_complex *out = (gr_complex *) output_items[0];

	  int ni = 0;
	  int no = 0;

	  while (ni < ninput_items && no < noutput_items)
	    {
	      memcpy(&out[ni * d_parallel_size], &in[ni * d_parallel_size],
		     d_parallel_size * sizeof(gr_complex));
	      ni++;
	      no += d_parallel_size;
	    }

	  consume_each(ni);
	  return no;
    }

  } /* namespace s4a */
} /* namespace gr */

