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
#include "vfdm_precoder_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <math.h>

#define VERBOSE 0
#define M_TWOPI (2*M_PI)

namespace gr
{
namespace s4a
{

vfdm_precoder::sptr vfdm_precoder::make(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int channel_size,
		int preamble_block_rep, int pilot_block_t_sz, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep)
{
	return gnuradio::get_initial_sptr(
			new vfdm_precoder_impl(fft_length, occupied_tones, cp_length,
					precoder_length, channel_size, preamble_block_rep,
					pilot_block_t_sz, pilot_block_rep, data_block_sz,
					data_block_rep, silence_block_rep));
}

/*
 * The private constructor
 */
vfdm_precoder_impl::vfdm_precoder_impl(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int channel_size,
		int preamble_block_rep, int pilot_block_t_sz, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep) :
		gr::block("vfdm_precoder",
				gr::io_signature::make2(2, 2,
						sizeof(gr_complex) * precoder_length,
						sizeof(gr_complex) * channel_size),
				gr::io_signature::make(1, 1,
						sizeof(gr_complex) * (fft_length + precoder_length))), d_fft_length(
				fft_length), d_cp_length(cp_length), d_precoder_length(
				precoder_length), d_channel_size(channel_size), d_preamble_block_rep(
				preamble_block_rep), d_preamble_block_idx(0), d_pilot_block_t_sz(
				pilot_block_t_sz), d_pilot_block_rep(pilot_block_rep), d_pilot_block_idx(
				0), d_data_block_sz(data_block_sz), d_data_block_rep(
				data_block_rep), d_data_block_idx(0), d_silence_block_rep(
				silence_block_rep), d_silence_block_idx(0), d_state(ST_IDLE), d_time_idx(0)
{
	printf("init::vfdm_precoder\n");

	int i = 0;
	int j = 0;
	int k = 0;

	// Load FFTW
	d_fft = new fft::fft_complex(d_fft_length, false);

	// Set out length
	d_out_length = fft_length + precoder_length;

	// Total block size
	d_block_sz = (preamble_block_rep + pilot_block_t_sz * pilot_block_rep
			+ data_block_sz * data_block_rep + silence_block_rep);
	printf("init::vfdm_precoder::block_sz %d\n", d_block_sz);

	// Total pilot block size
	d_pilot_block_total = pilot_block_t_sz * pilot_block_rep;

	// Total data block size
	d_data_block_total = data_block_sz * data_block_rep;

	// Set left padding
	d_left_padding = floor((fft_length - occupied_tones) / 2);

	// Init preamble sequence
	std::vector<gr_complex> preamble =
			vfdm_precoder_impl::generate_complex_sequence(fft_length,
					d_left_padding, 1, 0, false);
	d_preamble_f = preamble;

	std::vector<gr_complex> preamble_t = generate_complex_sequence_in_time(
			fft_length, d_left_padding, 1, 0, false);
	d_preamble_t = preamble_t;

	// Init pilot sequence (FFT matrix)
	d_pilot_f = (gr_complex *) malloc(precoder_length * pilot_block_t_sz * sizeof(gr_complex));
	k = 0;
	double deg = 0.0f;
	for (i = 0; i < pilot_block_t_sz; i++)
	{
		for (j = 0; j < precoder_length; j++)
		{
			deg = (-1 * M_TWOPI * j * i) / ((double) pilot_block_t_sz);
			d_pilot_f[k] = gr_complex(cos(deg), sin(deg));
			k++;
		}
	}

	// Init data buffer
	d_data = (gr_complex *) malloc((d_data_block_sz * d_precoder_length) * sizeof(gr_complex));
	memset(d_data, 0,(d_data_block_sz * d_precoder_length) * sizeof(gr_complex));

	// Allocate memory for d_h21, d_T, d_R, d_V1, d_V2
	d_h21 = (gr_complexd *) malloc(channel_size * sizeof(gr_complexd));
	memset(d_h21, 0, channel_size * sizeof(gr_complexd));

	d_V1 = (gr_complexd *) malloc((fft_length + 1) * sizeof(gr_complexd));
	memset(d_V1, 0, (fft_length + 1) * sizeof(gr_complexd));

	d_V2 = (gr_complexd *) malloc((fft_length + cp_length) * sizeof(gr_complexd));
	memset(d_V2, 0, (fft_length + cp_length) * sizeof(gr_complexd));

	d_T = (gr_complexd *) malloc((fft_length + 1) * (fft_length + precoder_length)* sizeof(gr_complexd));
	memset(d_T, 0,(fft_length + 1) * (fft_length + precoder_length)* sizeof(gr_complexd));

	d_E = (gr_complex *) malloc((fft_length + precoder_length) * (precoder_length)* sizeof(gr_complex));
	memset(d_E, 0,(fft_length + precoder_length) * (precoder_length)* sizeof(gr_complex));

	d_R = (gr_complex *) malloc((fft_length + cp_length) * (fft_length + cp_length)* sizeof(gr_complex));
	memset(d_R, 0,(fft_length + cp_length) * (fft_length + cp_length)* sizeof(gr_complex));

	svd_set_matrix(d_T);
	svd_init(fft_length + 1, fft_length + precoder_length, true);

	// Set timeout values
	d_timeout_syms_total = d_block_sz;
	d_timeout_syms_idx = 0;
	d_timeout_ref_sym = gr_complex(0, 0);

	/*
	 Set atomic output item size such that we can guarantee to process
	 the whole frame in one block execution.
	 */
	set_output_multiple(d_block_sz);

	isOK = false;
	printf("INIT END\n");
}

/*
 * Our virtual destructor.
 */
vfdm_precoder_impl::~vfdm_precoder_impl()
{
}

/*******************************************************************
 Linear Algebra
 *******************************************************************/
// Multiply
void vfdm_precoder_impl::multiply(std::vector<gr_complex> &matR,
		std::vector<gr_complex> &matA, std::vector<gr_complex> &matB, int am,
		int an, int bm, int bn)
{
	int i = 0;
	int j = 0;
	int k = 0;
	gr_complex localsum = 0;
	for (i = 0; i < am; i++)
	{
		for (j = 0; j < bn; j++)
		{
			localsum = 0;
			for (k = 0; k < an; k++)
			{
				gr_complex symA = matA[k * am + i];
				gr_complex symB = matB[j * bm + k];
				localsum += symA * symB;
			}
			matR[am * j + i] = localsum;
		}
	}
}

void vfdm_precoder_impl::multiply(gr_complexd* matR, gr_complexd* matA,
		gr_complexd* matB, int am, int an, int bm, int bn)
{
	int i = 0;
	int j = 0;
	int k = 0;
	gr_complexd localsum = 0;
	for (i = 0; i < am; i++)
	{
		for (j = 0; j < bn; j++)
		{
			localsum = 0;
			for (k = 0; k < an; k++)
			{
				gr_complexd symA = matA[k * am + i];
				gr_complexd symB = matB[j * bm + k];
				localsum += symA * symB;
			}
			matR[am * j + i] = localsum;
		}
	}
}

void vfdm_precoder_impl::multiply(gr_complex* matR, gr_complex* matA,
		gr_complex* matB, int am, int an, int bm, int bn)
{
	int i = 0;
	int j = 0;
	int k = 0;
	gr_complex localsum = 0;
	for (i = 0; i < am; i++)
	{
		for (j = 0; j < bn; j++)
		{
			localsum = 0;
			for (k = 0; k < an; k++)
			{
				gr_complex symA = matA[k * am + i];
				gr_complex symB = matB[j * bm + k];
				localsum += symA * symB;
			}
			matR[am * j + i] = localsum;
		}
	}
}

// Hermitian = Conjugate transpose
void vfdm_precoder_impl::hermitian(std::vector<gr_complex> &matR,
		std::vector<gr_complex> &matA, int am, int an)
{
	int i = 0;
	int j = 0;
	int idx = 0;

	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			matR[idx] = conj(matA[i + j * am]);
			idx++;
		}
	}
}

void vfdm_precoder_impl::hermitian(gr_complexd* matR, gr_complexd* matA, int am,
		int an)
{
	int i = 0;
	int j = 0;
	int idx = 0;

	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			matR[idx] = conj(matA[i + j * am]);
			idx++;
		}
	}
}

// Transpose
void vfdm_precoder_impl::transpose(std::vector<gr_complex> &matR,
		std::vector<gr_complex> &matA, int am, int an)
{
	int i = 0;
	int j = 0;
	int idx = 0;

	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			matR[idx] = matA[i + j * am];
			idx++;
		}
	}
}

void vfdm_precoder_impl::transpose(gr_complexd* matR, gr_complexd* matA, int am,
		int an)
{
	int i = 0;
	int j = 0;
	int idx = 0;

	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			matR[idx] = matA[i + j * am];
			idx++;
		}
	}
}

// Create Toeplitz matrix
void vfdm_precoder_impl::toeplitz(std::vector<gr_complex> &matR,
		std::vector<gr_complex> &vecA, std::vector<gr_complex> &vecB, int am,
		int bm)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int imin = std::min(am, bm);
	for (i = 0; i < imin; i++)
	{
		for (j = i, k = 0; j < am; j++, k++)
		{
			matR[i * am + j] = vecA[k];
		}
		for (j = i, k = 0; j < bm; j++, k++)
		{
			matR[j * am + i] = vecB[k];
		}
	}
}

void vfdm_precoder_impl::toeplitz(gr_complexd* matR, gr_complexd* vecA,
		gr_complexd* vecB, int am, int bm)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int imin = std::min(am, bm);
	for (i = 0; i < imin; i++)
	{
		for (j = i, k = 0; j < am; j++, k++)
		{
			matR[i * am + j] = vecA[k];
		}
		for (j = i, k = 0; j < bm; j++, k++)
		{
			matR[j * am + i] = vecB[k];
		}
	}
}

// Print Matrix
void vfdm_precoder_impl::print_matrix(const char* name,
		std::vector<gr_complex> &matA, int am, int an)
{
	int i = 0;
	int j = 0;

	printf("%s = [", name);
	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			gr_complex res = matA[j * am + i];
			printf("(%.4f+1i*(%.4f))  ", res.real(), res.imag());
		}
		printf(";");
	}
	printf("]\n");
}

void vfdm_precoder_impl::print_matrix(const char* name, gr_complexd* matA,
		int am, int an)
{
	int i = 0;
	int j = 0;

	printf("%s = [", name);
	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			gr_complexd res = matA[j * am + i];
			printf("(%.4f+1i*(%.4f))  ", res.real(), res.imag());
		}
		printf(";");
	}
	printf("];\n");
}

void vfdm_precoder_impl::print_matrix(const char* name, gr_complex* matA,
		int am, int an)
{
	int i = 0;
	int j = 0;

	printf("%s = [", name);
	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			gr_complex res = matA[j * am + i];
			printf("(%.4f+1i*(%.4f))  ", res.real(), res.imag());
		}
		printf(";");
	}
	printf("];\n");
}

void vfdm_precoder_impl::print_matrix(const char* name, double* matA, int am,
		int an)
{
	int i = 0;
	int j = 0;

	printf("%s = [", name);
	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			double res = matA[j * am + i];
			printf("%.6f  ", res);
		}
		printf(";");
	}
	printf("];\n");
}

void vfdm_precoder_impl::print_matrix_sum(const char* name, gr_complexd* matA,
		int am, int an)
{
	int i = 0;
	int j = 0;
	gr_complexd sum = 0;

	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			sum += matA[j * am + i];
		}
	}
	printf("%s = %.8f\n", name, abs(sum));
}
/*******************************************************************
 Singular Value Decomposition
 *******************************************************************/
// Initialize SVD
void vfdm_precoder_impl::svd_init(size_t M, size_t N, bool calculate_vt)
{
	d_M = M;
	d_N = N;

	d_LDA = M;

	/* setup the array S */
	d_S = new double[std::min(M, N)];

	/* setup the array U */
	d_LDU = M;
	d_mat_U = new gr_complexd[M * M];

	/* setup the array VT */
	d_LDVT = N;
	d_mat_VT = new gr_complexd[N * N];

	/* setup internal stuff */
	d_RWORK = new double[5 * std::min(M, N) * std::min(M, N)+ 7 * std::min(M, N)];
	d_IWORK = new int[8 * std::min(M, N)];

	/* determine optimal workspace size */
	gr_complexd tmpwork;
	size_t tmpworksize = -1;

	char jobu = 'A';
	char jobvt = 'A';

	zgesvd_(&jobu, &jobvt, &d_M, &d_N, d_mat_A, &d_LDA, d_S, d_mat_U, &d_LDU,
			d_mat_VT, &d_LDVT, &tmpwork, &tmpworksize, d_RWORK, &d_INFO);

	d_LWORK = static_cast<size_t>(std::ceil(tmpwork.real()));
	d_WORK = new gr_complexd[d_LWORK];
}

void vfdm_precoder_impl::svd_decompose()
{
	char jobu = 'A';
	char jobvt = 'A';
	zgesvd_(&jobu, &jobvt, &d_M, &d_N, d_mat_A, &d_LDA, d_S, d_mat_U, &d_LDU,
			d_mat_VT, &d_LDVT, d_WORK, &d_LWORK, d_RWORK, &d_INFO);
}

void vfdm_precoder_impl::svd_set_matrix(gr_complexd* mat_A)
{
	d_mat_A = mat_A;
}

double*
vfdm_precoder_impl::svd_get_S()
{
	return d_S;
}

gr_complexd*
vfdm_precoder_impl::svd_get_U()
{
	return d_mat_U;
}

gr_complexd*
vfdm_precoder_impl::svd_get_VT()
{
	return d_mat_VT;
}
/***********************************************************************
 Sequence Generator
 ***********************************************************************/
std::vector<gr_complex> vfdm_precoder_impl::generate_complex_sequence(
		int seq_size, int left_padding, int zeros_period, int seed = 0,
		bool verbose = false)
{
	std::vector<gr_complex> result(seq_size, gr_complex(0, 0));

	int i = 0;
	int istart = left_padding;
	int istop = seq_size - left_padding;

	// Initialize seed
	srand(seed);

	std::vector<gr_complex> syms(2, gr_complex(0, 0));
	syms[0] = gr_complex(+0.7, +0.7);
	syms[1] = gr_complex(-0.7, -0.7);

	for (i = istart; i < istop; i += zeros_period + 1)
	{
		gr_complex sym = syms[rand() % 2];
		result[i] = sym;
	}

	// Print results
	/*if (verbose)
	{
		printf("vfdm_preamble_f = [");
		for (i = 0; i < seq_size; i++)
		{
			gr_complex sym = result[i];
			printf("%.4f+1i*(%.4f), ", sym.real(), sym.imag());
		}
		printf("]\n\n");
	}*/
	return result;
}

std::vector<gr_complex> vfdm_precoder_impl::generate_complex_sequence_in_time(
		int seq_size, int left_padding, int zeros_period, int seed = 0,
		bool verbose = false)
{
	std::vector<gr_complex> result =
			vfdm_precoder_impl::generate_complex_sequence(seq_size,
					left_padding, zeros_period, seed, true);

	std::vector<gr_complex> result_t(seq_size, gr_complex(0, 0));

	memcpy(d_fft->get_inbuf(), &result[0], d_fft_length * sizeof(gr_complex));

	// Copy symbols to input buffer (with FFT shift)
	gr_complex *dst = d_fft->get_inbuf();
	// half length of complex array
	unsigned int len = (unsigned int) (floor(d_fft_length / 2.0));
	memcpy(&dst[0], &result[len], sizeof(gr_complex) * (d_fft_length - len));
	memcpy(&dst[d_fft_length - len], &result[0], sizeof(gr_complex) * len);
	//memcpy(dst, &result[0], sizeof(gr_complex)*d_fft_length);

	// Execute FFT
	d_fft->execute();

	// Get results
	memcpy(&result_t[0], d_fft->get_outbuf(),
			sizeof(gr_complex) * d_fft_length);

	// Print results
	/*if (verbose)
	{
		int i = 0;
		printf("vfdm_preamble_t = [");
		for (i = 0; i < d_fft_length; i++)
		{
			gr_complex sym = result_t[i];
			printf("%i = (%.4f, %.4f), ", i, sym.real(), sym.imag());
			//printf("%.4f+1i*(%.4f), ", sym.real(), sym.imag());
		}
		printf("]\n\n");
	}*/
	return result_t;
}

void vfdm_precoder_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % d_block_sz)) / d_block_sz;
	ninput_items_required[0] = d_data_block_sz * coeff;
	ninput_items_required[1] = 1 * coeff;
}

int vfdm_precoder_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{

	int ninput_items1 = ninput_items_v[0];
	int ninput_items2 = ninput_items_v[1];

	const gr_complex *in = (const gr_complex *) input_items[0];
	const gr_complex *in2 = (const gr_complex *) input_items[1];
	gr_complex *out = (gr_complex *) output_items[0];

	int ni1 = 0;
	int ni2 = 0;

	int no = 0;
	int nodx = 0;
	int nidx = 0;

	int i = 0;
	int j = 0;
	int k = 0;
	int p = 0;

	// General State Machine
	while (ni1 < ninput_items1 && ni2 < ninput_items2 && no < noutput_items)
	{
		//std::cout<<ninput_items1<<std::endl;
		//std::cout<<ninput_items2<<std::endl;
		//std::cout<<noutput_items<<std::endl<<std::endl;
		switch (d_state)
		{
		case ST_IDLE:
		{
			//gettimeofday(&t_begin, NULL);
			d_state = ST_BUILD_PRECODER;
			break;
		}
		case ST_BUILD_PRECODER:
		{
			//d_state = ST_PREAMBLE_OUT;
			double h21_timeout_check = 0.0;
			// Copy h21 values to the vector
			for (i = 0; i < d_channel_size; i++)
			{
				d_h21[i] = in2[ni2 * d_channel_size + i];
				h21_timeout_check += abs(in2[ni2 * d_channel_size + i]);
			}

			if (h21_timeout_check > 0)
			{
				// Prepare V1
				d_V1[0] = d_h21[d_precoder_length];
				for (i = 0; i < d_fft_length; i++)
				{
					d_V1[i+1] = 0;
				}

				// Prepare V2
				for (i = 0; i < d_precoder_length + 1; i++)
				{
					d_V2[i] = d_h21[d_precoder_length - i];
				}
				for (i = 0, j = (d_precoder_length + 1); i < d_fft_length - 1;i++, j++)
				{
					d_V2[j] = 0;
				}

				// Prepare T = toeplitz(V1,V2)
				toeplitz(d_T, d_V1, d_V2, d_fft_length + 1,d_fft_length + d_precoder_length);

				// SVD
				svd_decompose();

				// Pick E as last L columns of VT   (U,S,VT = svd(T))
				k = 0;
				for (i = 0; i < d_precoder_length; i++)
				{
					for (j = 0; j < d_fft_length + d_precoder_length; j++)
					{
						d_E[k] = conj(d_mat_VT[j * (d_fft_length + d_precoder_length)
										+ d_fft_length + i]);
						k++;
					}
				}
				d_state = ST_PREAMBLE_OUT;
			}
			else
			{
				d_state = ST_TIMEOUT;
			}
			break;
		}
		case ST_PREAMBLE_OUT:
			if (d_preamble_block_idx < d_preamble_block_rep)
			{
				// CP insertion
				memcpy(&out[nodx], &d_preamble_t[d_fft_length - d_cp_length],
						d_cp_length * sizeof(gr_complex));
				
				// Copy preamble
				memcpy(&out[nodx+d_cp_length], &d_preamble_t[0],d_fft_length * sizeof(gr_complex));

				/*// Zero padding for output
				for (i = 0; i < d_precoder_length; i++)
				{
					out[nodx + d_fft_length + i] = gr_complex(0, 0);
				}*/

				nodx += d_out_length;
				d_preamble_block_idx++;
				d_state = ST_PREAMBLE_OUT;
			}
			else
			{
				d_preamble_block_idx = 0;
				d_state = ST_PILOT_PRECODE;
			}
			break;
		case ST_PILOT_PRECODE:
		{
			multiply(d_R, d_E, d_pilot_f, d_out_length, d_precoder_length,
					d_precoder_length, d_pilot_block_t_sz);
			
			d_state = ST_PILOT_OUT;
			break;
		}
		case ST_PILOT_OUT:
			if (d_pilot_block_idx < d_pilot_block_rep)
			{
				memcpy(&out[nodx], &d_R[0],
						d_pilot_block_t_sz * d_out_length * sizeof(gr_complex));
				nodx += d_out_length * d_pilot_block_t_sz;
				d_pilot_block_idx++;
				d_state = ST_PILOT_OUT;
			}
			else
			{
				d_pilot_block_idx = 0;
				d_state = ST_DATA_STORE;
			}
			break;
		case ST_DATA_STORE:
		{
			if (d_data_block_idx < d_data_block_sz)
			{
				for (i = 0; i < d_precoder_length; i++)
				{
					d_data[d_data_block_idx * d_precoder_length + i] = in[nidx+ i];
				}
				nidx += d_precoder_length;
				d_data_block_idx++;
				d_state = ST_DATA_STORE;
			}
			else
			{
				d_data_block_idx = 0;
				d_state = ST_DATA_PRECODE;
			}
			break;
		}
		case ST_DATA_PRECODE:
		{
			multiply(d_R, d_E, &d_data[0], d_out_length, d_precoder_length,
					d_precoder_length, d_data_block_sz);

			d_state = ST_DATA_OUT;
			break;
		}
		case ST_DATA_OUT:
			if (d_data_block_idx < d_data_block_rep)
			{
				memcpy(&out[nodx], &d_R[0],d_out_length * d_data_block_sz * sizeof(gr_complex));
				nodx += d_out_length * d_data_block_sz;
				d_data_block_idx++;
				d_state = ST_DATA_OUT;
			}
			else
			{
				d_data_block_idx = 0;
				d_state = ST_SILENCE_OUT;
			}
			break;
		case ST_SILENCE_OUT:
			if (d_silence_block_idx < d_silence_block_rep)
			{
				for (i = 0; i < d_out_length; i++)
				{
					out[nodx + i] = gr_complex(0, 0); // Fill with zeros
				}
				d_silence_block_idx++;
				nodx += d_out_length;
			}
			else
			{
				ni1 += d_data_block_sz;
				ni2 += 1;
				no += d_block_sz;
				d_silence_block_idx = 0;
				d_state = ST_IDLE;
				//std::cout<<"VFDM Frame Generated!\n";
			}
			/*d_time_idx ++;			
			if (d_time_idx == 1)
			{
				gettimeofday(&t_end, NULL);
				mtime = (t_end.tv_sec*(uint64_t)1000000 + t_end.tv_usec) - 
					(t_begin.tv_sec*(uint64_t)1000000 + t_begin.tv_usec);

				printf("Time diference %lu ms\n",mtime/1000);
				d_time_idx = 0;
			}*/
			break;
		case ST_TIMEOUT:
		{
			if (d_timeout_syms_idx < d_timeout_syms_total)
			{
				for (i = 0; i < d_out_length; i++)
				{
					out[nodx + i] = d_timeout_ref_sym;
				}
				nodx += d_out_length;
				d_timeout_syms_idx++;
			}
			else
			{
				ni1 += d_data_block_sz;
				ni2 += 1;
				no += d_block_sz;
				nidx += d_data_block_sz * d_precoder_length;
				d_timeout_syms_idx = 0;
				d_state = ST_IDLE;
			}
			break;
		}
		default:
			throw std::invalid_argument("frame generation::invalid state");
			break;

		}
	}
	consume(0, ni1);
	consume(1, ni2);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

