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
                            const std::vector<int> gen2,
                            int image_size,
                            int packet_size)
{
    return gnuradio::get_initial_sptr(new convolutional_encoder_impl(gen1, gen2, image_size, packet_size));
}

/*
     * The private constructor
     */

convolutional_encoder_impl::convolutional_encoder_impl(const std::vector<int> gen1,
                                                       const std::vector<int> gen2,
                                                       int image_size,
                                                       int packet_size)
    : gr::block("convolutional_encoder",
                gr::io_signature::make(1, 1, sizeof(char)),
                gr::io_signature::make(1, 1, sizeof(char))),
      d_gen1(gen1),
      d_gen2(gen2),
      d_image_size(image_size), d_packet_size(packet_size), d_offset(0),
      d_packet_index(0), d_ctrl(0), d_packet_no(0),
      d_state(ST_PACKETIZE),
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
    const char *in = (const char *)input_items[0];
    char *out = (char *)output_items[0];
    int ninput_items = ninput_items_v[0];
    int ni = 0;
    int no = 0;
    int i = 0;
    char msg;

    while (no < noutput_items)
    {
        switch (d_state)
        {
        case ST_ENCODING:
        {
            if (ni < ninput_items)
            {
                if (d_ctrl == 0)
                {
                    msg = (in[ni] & 0x0F);
                    out[no++] = encode(msg);
                    d_packet_index++;
                    d_offset++;
                    d_ctrl = 1;
                    if (d_packet_index == d_packet_size)
                    {
                        d_packet_size = 0;
                        d_state = ST_PACKETIZE;
                    }
                }
                else
                {
                    msg = ((in[ni] >> 4) & 0x0F);
                    out[no++] = encode(msg);
                    d_packet_index++;
                    d_offset++;
                    d_ctrl = 0;
                    ni++;
                    if (d_packet_index == d_packet_size)
                    {
                        d_packet_index = 0;
                        d_state = ST_PACKETIZE;
                    }
                }
            }
            else
            {
                consume_each(ni);
                return no;
            }
            break;
        }
        case ST_PACKETIZE:
        {
            if (d_offset < (2 * d_image_size))
            {
                d_packet_no++;
                out[no++] = 0x00;
                d_state = ST_ENCODING;
            }
            else
            {
                out[no++] = 0x01;
                d_state = ST_FREE;
            }
            break;
        }
        case ST_FREE:
        {
            out[no++] = 0x01;
            consume_each(ni);
            return no;
        }
        }
    }

    consume_each(ni);
    return no;
}
char convolutional_encoder_impl::encode(char msg)
{
    char reg[2] = {0, 0};
    char codeword = 0;
    for (int i = 0; i < 4; i++)
    {
        int current_bit = (msg >> (3 - i)) & 1;
        char code1 = (current_bit * d_gen1[2] + reg[0] * d_gen1[1] + reg[1] * d_gen1[0]) % 2;
        char code2 = (current_bit * d_gen2[2] + reg[0] * d_gen2[1] + reg[1] * d_gen2[0]) % 2;
        reg[1] = reg[0];
        reg[0] = current_bit;

        codeword <<= 1;
        codeword |= code1;
        codeword <<= 1;
        codeword |= code2;
    }
    return codeword;
}

} /* namespace DNC2 */
} // namespace gr
