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

#ifndef INCLUDED_DNC2_CONVOLUTIONAL_ENCODER_IMPL_H
#define INCLUDED_DNC2_CONVOLUTIONAL_ENCODER_IMPL_H

#include <DNC2/convolutional_encoder.h>

namespace gr
{
namespace DNC2
{

class convolutional_encoder_impl : public convolutional_encoder
{
private:
  std::vector<int> d_gen1;
  std::vector<int> d_gen2;
  char generator1;
  char generator2;
  enum state
  {
    ST_ENCODING,
    ST_PACKETIZE,
    ST_FREE
  };
  state d_state;
  int d_image_size;
  int d_packet_size;
  int d_offset;
  int d_packet_index;
  int d_packet_no;
  int d_ctrl;

public:
  convolutional_encoder_impl(const std::vector<int> gen1,
                             const std::vector<int> gen2,
                             int image_size,
                             int packet_size);
  ~convolutional_encoder_impl();

  void forecast(int noutput_items, gr_vector_int &ninput_items_required);

  int general_work(int noutput_items,
                   gr_vector_int &ninput_items,
                   gr_vector_const_void_star &input_items,
                   gr_vector_void_star &output_items);
  char encode(char msg);
};

} // namespace DNC2
} // namespace gr

#endif /* INCLUDED_DNC2_CONVOLUTIONAL_ENCODER_IMPL_H */
