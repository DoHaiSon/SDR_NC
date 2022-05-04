"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr
import numpy as np

class blk(gr.sync_block):  # other base classes are basic_block, decim_block, interp_block
    """Embedded Python Block example - a simple multiply const"""

    def __init__(self, total_bits=1000):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
        gr.sync_block.__init__(
            self,
            name='BER',   # will show up in GRC
            in_sig=[np.byte],
            out_sig=[]
        )
        # if an attribute with the same name as a parameter is found,
        # a callback is registered (properties work, too).
        self.Total_Bits = total_bits
	self.count = 0
	self.bits  = []

    def work(self, input_items, output_items):
        """example: multiply with constant"""
	self.count = self.count + len(input_items[0])

	# Concatenate 
	in0 = np.array(input_items[0], dtype=np.ubyte)

	if (self.count < self.Total_Bits):
		self.bits = np.append(self.bits, in0)
	else:
		err = self.bits == 0
		err = sum(err)
		total_bit = float(len(self.bits))
		ser = 1 - (err / total_bit)
		print("--------------------------------------\nBER: {0}\n--------------------------------------".format(ser))
		self.bits  = []
		self.count = 0
	self.consume_each(len(input_items[0]))
        return 0
