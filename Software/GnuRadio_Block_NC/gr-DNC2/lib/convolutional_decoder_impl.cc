/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "convolutional_decoder_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <time.h>
#include <sys/time.h>

namespace gr {
  namespace DNC2 {

    convolutional_decoder::sptr
    convolutional_decoder::make(const std::vector<int> gen1,
			const std::vector<int> gen2)
    {
      return gnuradio::get_initial_sptr
        (new convolutional_decoder_impl(gen1, gen2));
    }

    /*
     * The private constructor
     */
    convolutional_decoder_impl::convolutional_decoder_impl(const std::vector<int> gen1,
			const std::vector<int> gen2)
      : gr::block("convolutional_decoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
	      d_gen1(gen1),
	      d_gen2(gen2),
	      generator1(0x7),
	      generator2(0x5)
    {
	aa.resize(8); std::fill(aa.begin(), aa.end(), 0);
	ac.resize(8); std::fill(ac.begin(), ac.end(), 0);
	ba.resize(8); std::fill(ba.begin(), ba.end(), 0);
	bc.resize(8); std::fill(bc.begin(), bc.end(), 0);
	cb.resize(8); std::fill(cb.begin(), cb.end(), 0);
	cd.resize(8); std::fill(cd.begin(), cd.end(), 0);
	db.resize(8); std::fill(db.begin(), db.end(), 0);
	dd.resize(8); std::fill(dd.begin(), dd.end(), 0);
	
	Ma.resize(8); std::fill(Ma.begin(), Ma.end(), 0);
	Mb.resize(8); std::fill(Mb.begin(), Mb.end(), 0);
	Mc.resize(8); std::fill(Mc.begin(), Mc.end(), 0);
	Md.resize(8); std::fill(Md.begin(), Md.end(), 0);

	Aa.resize(8); std::fill(Aa.begin(), Aa.end(), 0);
	Ab.resize(8); std::fill(Ab.begin(), Ab.end(), 0);
	Ac.resize(8); std::fill(Ac.begin(), Ac.end(), 0);
	Ad.resize(8); std::fill(Ad.begin(), Ad.end(), 0);
    }

    /*
     * Our virtual destructor.
     */
    convolutional_decoder_impl::~convolutional_decoder_impl()
    {
    }

    void
    convolutional_decoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items*2;
    }

    int
    convolutional_decoder_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const char *in = (const char *) input_items[0];
        char *out = (char *) output_items[0];
	int ninput_items = ninput_items_v[0];
	
	int ni = 0, no = 0;
	while (ni<ninput_items && no<noutput_items) {
		out[no] = decode(in[ni],in[ni+1],generator1,generator2); //printf("%d\n",out[no]);
		ni += 2;no ++;
	}

        consume_each (ni);
        return no;
    }

char convolutional_decoder_impl::decode(char code1,char code2, char gen1, char gen2) {
	char rcode[1024];
	for (int i=0; i<8; i++)  {
		rcode[i] = ((code1 >> (7-i)) & 0x1);
		rcode[i+8] = ((code2 >> (7-i)) & 0x1);
	}
	//printf("rcode: ");for (int i=0; i<16; i++)  {printf("%d",rcode[i]);} printf("\n");
	/*calculate all branch metric*/
	aa = branch_metric_calculation(rcode, 0x0); 
	ac = branch_metric_calculation(rcode, 0x3);
	ba = ac;
	bc = aa;
	cb = branch_metric_calculation(rcode, 0x2);
	cd = branch_metric_calculation(rcode, 0x1);
	db = cd;
	dd = cb;
	ba[0] = 0; ba[1] = 0;
    	bc[0] = 0; bc[1] = 0;
    	db[0] = 0; db[1] = 0;
    	dd[0] = 0; dd[1] = 0;
    	cb[0] = 0;
    	cd[0] = 0;
	/*print_vector(aa,8);
	print_vector(ac,8);
	print_vector(ba,8);
	print_vector(bc,8);
	print_vector(cb,8);
	print_vector(cd,8);
	print_vector(db,8);
	print_vector(dd,8);
	/*t = 0. Suviving metric*/
	Ma[0] = aa[0];
    	Mb[0] = 0;
    	Mc[0] = ac[0];
    	Md[0] = 0;
	/*initial trace*/
	Aa[0] = 1;  
    	Ab[0] = 0;  
    	Ac[0] = 1; 
    	Ad[0] = 0; 
	/*t = 1. Surviving metric*/
	Ma[1] = Ma[0] + aa[1];
    	Mb[1] = Mc[0] + cb[1];
    	Mc[1] = Ma[0] + ac[1];
    	Md[1] = Mc[0] + cd[1];
	/*trace*/
	Aa[1] = 1;
    	Ab[1] = 3;
    	Ac[1] = 1;
    	Ad[1] = 3;
	/*Continous surviving metric and trace*/
		for (int i=2;i<8;i++) {  
       			//Metric at a
        		if (  Ma[i-1]+aa[i] > Mb[i-1]+ba[i]  ) {
            			Aa[i] = 2;
            			Ma[i] = Mb[i-1]+ba[i];
			}
        		else {
            			Aa[i] = 1;
            			Ma[i] = Ma[i-1]+aa[i];
			}
        		//Metric at b
        		if ( Mc[i-1] + cb[i] > Md[i-1] +db[i] ) {
            			Ab[i] = 4;
            			Mb[i] = Md[i-1] + db[i];
			}
       	 		else {
            			Ab[i] = 3;
            			Mb[i] = Mc[i-1] + cb[i];
			}
        		//Metric at c
        		if (Ma[i-1] + ac[i] > Mb[i-1] + bc[i] ) {
            			Ac[i] = 2;
            			Mc[i] = Mb[i-1] + bc[i];
			}
        		else {
            			Ac[i] = 1;
            			Mc[i] = Ma[i-1] + ac[i];
			}
        		//Metric at d
        		if (Mc[i-1] + cd[i] > Md[i-1]+dd[i] ) {
            			Ad[i] = 4;
            			Md[i] = Md[i-1] + dd[i];
			}
        		else {
            			Ad[i] = 3;
            			Md[i] = Mc[i-1] + cd[i];
			}
		}
	/*printf("Aa: "); for (int i = 1;i<8;i++) printf ("%i ",Aa[i]); printf("\n");
	printf("Ab: "); for (int i = 1;i<8;i++) printf ("%i ",Ab[i]); printf("\n");
	printf("Ac: "); for (int i = 1;i<8;i++) printf ("%i ",Ac[i]); printf("\n");
	printf("Ad: "); for (int i = 1;i<8;i++) printf ("%i ",Ad[i]); printf("\n");
	//printf("Ma: %i, Mb: %i, Mc: %i, Md: %i\n", Ma[7], Mb[7], Mc[7], Md[7]);
	/*Decide shortest path and corresponding trace*/
	int Mmin = Ma[7]; int index = 1;
	if (Mb[7]<Mmin) { index = 2; Mmin = Mb[7];}
	if (Mc[7]<Mmin) { index = 3; Mmin = Mc[7];}
	if (Md[7]<Mmin) { index = 4; Mmin = Md[7];};
	//printf("Ma: %i, Mb: %i, Mc: %i, Md: %i, Mmin: %i\n", Ma[7], Mb[7], Mc[7], Md[7], Mmin);
	/*Trace back to estimate message*/
	char emsg=0x0;
	//printf("path: ");printf("%i ",index);
	for (int i = 7;i>=0;i--) {
        	int tmpIndex = index; 
        	if (index == 1) index = Aa[i];
		else if (index == 2)  index = Ab[i]; 
		else if (index == 3)  index = Ac[i]; 
		else if (index == 4)  index = Ad[i]; 
		//printf("%i ",index); //change index; 
        	if ( ( (tmpIndex == 1) && (index == 1) )||((tmpIndex == 1) && (index == 2)) )
            		emsg |= (0x0 <<(7-i));
        	else if ( (tmpIndex == 2) && (index == 3)||(tmpIndex == 2) && (index == 4) )
            		emsg |= (0x0 <<(7-i));
        	else if ( (tmpIndex == 3) && (index == 1) ||(tmpIndex == 3) && (index == 2)) 
            		emsg |= (0x1 <<(7-i));
        	else if ( (tmpIndex == 4) && (index == 3)||(tmpIndex == 4) && (index == 4) ) 
            		emsg |= (0x1 <<(7-i));
        }
	//printf("\n"); printf("emsg: "); for (int i = 0;i<8;i++) {printf("%d",(emsg>> (7-i)) & 0x1);}; printf("\n");
        return emsg;
}

std::vector<int> convolutional_decoder_impl::branch_metric_calculation(char code[], char value) {
	std::vector<int> metric; metric.resize(8); std::fill(metric.begin(), metric.end(), 0);
	char d_value[1024]; 
	d_value[0] = (value >> 1) & 0x1; d_value[1] = value & 0x1;
	//printf("rcode in function: ");for (int i=0; i<16; i++)  {printf("%d",code[i]);} printf("\n");	
	for (int i=0;i<8;i++) {
		for (int j=0;j<2;j++) { 		
			if ((code[2*i+j] & 0x1) != (d_value[j]&0x1)) metric[i] ++; 
			//printf("%d__%d ",(code[2*i+j] & 0x1),(d_value[j]&0x1));
		}
	}
	//printf("\n");	
	//printf("Metric: ");for (int i=0; i<8; i++)  {printf("%d ",metric[i]);} printf("\n");	
	return metric;
}
 
void convolutional_decoder_impl::print_vector(std::vector<int> x, int length) {
	for (int i=0; i<length; i++)  {printf("%d ",x[i]);}; 
	printf("\n");	
}
  } /* namespace DNC2 */
} /* namespace gr */
