/* -*- c++ -*- */
/* 

 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "convolutional_encoder_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <time.h>
#include <sys/time.h>

namespace gr {
  namespace DNC2 {

    convolutional_encoder::sptr
    convolutional_encoder::make(const std::vector<int> gen1,
			const std::vector<int> gen2)
    {
      return gnuradio::get_initial_sptr
        (new convolutional_encoder_impl(gen1, gen2));
    }

    /*
     * The private constructor
     */
    convolutional_encoder_impl::convolutional_encoder_impl(const std::vector<int> gen1,
			const std::vector<int> gen2)
      : gr::block("convolutional_encoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
	      d_gen1(gen1),
	      d_gen2(gen2),
	      generator1(0x7),
	      generator2(0x5)
    {
	char generator1 = 0x7;//(d_gen1[1] | (d_gen1[2]<<1)) | (d_gen1[3]<<2); 
	//std::cout<<gen1[1]<<"_"<<gen1[2]<<"_"<<gen1[3]<<"_"<<(int)generator1<<std::endl;
	char generator2 = 0x5;//(d_gen2[1] | (d_gen2[2]<<1)) | (d_gen2[3]<<2); 
	//std::cout<<d_gen2[1]<<"_"<<d_gen2[2]<<"_"<<d_gen2[3]<<"_"<<(int)generator2<<std::endl;
    }

    /*
     * Our virtual destructor.
     */
    convolutional_encoder_impl::~convolutional_encoder_impl()
    {
    }

    void
    convolutional_encoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items/2;
    }

    int
    convolutional_encoder_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const char *in = (const char *) input_items[0];
        char *out = (char *) output_items[0];
	int ninput_items = ninput_items_v[0];

        int ni=0; int no=0;
	char code1, code2;
	while (ni<ninput_items && no<noutput_items) {
		char code1[1024], code2[1024], conv1, conv2, out1, out2;
		conv1 = convolution(in[ni],generator1);
		conv2 = convolution(in[ni],generator2); //printf("%d_%d\n",conv1, conv2);
		for (int i=0; i<8; i++)  code1[i] = ((conv1 >> (7-i)) & 0x1); //for (int i = 0;i<8;i++) {printf("%d",code1[i]);} printf("\n");
		for (int i=0; i<8; i++)  code2[i] = ((conv2 >> (7-i)) & 0x1); //for (int i = 0;i<8;i++) {printf("%d",code2[i]);} printf("\n\n");
		for (int i = 0;i<4;i++) {
			out[no] |= (((code1[i]) & 0x1)<<(7-2*i)); //printf("%d_",(((code1[i]) & 0x1)<<(2*i)));
			out[no] |= (((code2[i]) & 0x1)<<(6-2*i)); //printf("%d__",(((code2[i]) & 0x1)<<(2*i+1))); 
			out[no+1] |= (((code1[i+4]) & 0x1)<<(7-2*i)); //printf("%d_",(((code1[i+4]) & 0x1)<<(2*i)));
			out[no+1] |= (((code2[i+4]) & 0x1)<<(6-2*i)); //printf("%d\n",(((code2[i+4]) & 0x1)<<(2*i+1)));
		}
		//out[no]	= out1; out[no+1] = out2;
		//for (int i = 0;i<8;i++) {printf("%d",(out[no]>> (7-i)) & 0x1);} 
 		//for (int i = 0;i<8;i++) {printf("%d",(out[no+1]>> (7-i)) & 0x1);} printf("\n");	
		ni ++; no += 2; 
	}
        consume_each (ni);      
        return no;
    }

    char convolutional_encoder_impl::convolution(char msg, char gen){
	char codeword = 0x0;
	char tmp;
	for (int i = 0;i<8;i++) {
		tmp = 0x0;
		for (int j = 0;j<3;j++) {
			tmp ^= (((msg)>>(i+j))&(gen>>j)); //printf("tmp = %d\n",((msg<<2)>>(i+j) & 0x1));
		}
		codeword |= ((tmp & 0x1)<<i);   	
	}
	return codeword;
    }

  } /* namespace DNC2 */
} /* namespace gr */

