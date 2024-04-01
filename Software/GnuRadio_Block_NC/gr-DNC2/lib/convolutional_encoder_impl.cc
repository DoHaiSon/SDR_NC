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

namespace gr
{
namespace DNC2
{

convolutional_encoder::sptr
convolutional_encoder::make(const std::vector<int> gen1,
                            const std::vector<int> gen2)
{
    return gnuradio::get_initial_sptr(new convolutional_encoder_impl(gen1, gen2));
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
    char generator1 = 0x7;
    char generator2 = 0x5;
}

convolutional_encoder_impl::~convolutional_encoder_impl()
{
}

void convolutional_encoder_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
    ninput_items_required[0] = noutput_items / 2;
}

int convolutional_encoder_impl::general_work(int noutput_items,
                                             gr_vector_int &ninput_items_v,
                                             gr_vector_const_void_star &input_items,
                                             gr_vector_void_star &output_items)
{
    static bool done = false;
    if (done)
    {
        return -1;
    }
    const char *in = (const char *)input_items[0];
    char *out = (char *)output_items[0];
    int ninput_items = ninput_items_v[0];
    int ni = 0;
    int no = 0;
    int resgister[2] = {0, 0};
    char code1, code2;
    printf("Loan test in: \n");
    while (ni < ninput_items)
    {
        printf("%d ", in[ni]);
        out[no] = (in[ni] + resgister[0] + resgister[1]) % 2;
        out[no + 1] = (in[ni] + resgister[0] * 0 + resgister[1]) % 2;
        resgister[1] = resgister[0];
        resgister[0] = in[ni];
        ni++;
        no += 2;
    }

    printf("\nLoan test out\n");
    for (int i = 0; i < no; i++)
    {
        printf(" %d ", out[i]);
    }
    consume_each(ni);
    done = true;
    return no;
}

char convolutional_encoder_impl::convolution(char msg, char gen)
{
    char codeword = 0x0;
    char tmp;
    for (int i = 0; i < 8; i++)
    {
        tmp = 0x0;
        for (int j = 0; j < 3; j++)
        {
            tmp ^= (((msg) >> (i + j)) & (gen >> j));
        }
        codeword |= ((tmp & 0x1) << i);
    }
    return codeword;
}

} /* namespace DNC2 */
} /* namespace gr */
