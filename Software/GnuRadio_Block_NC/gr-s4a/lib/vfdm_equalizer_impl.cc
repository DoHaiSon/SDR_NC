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
#include "vfdm_equalizer_impl.h"
#include <math.h>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>

#define VERBOSE 1
#define M_TWOPI (2*M_PI)
#define TINY 1.0e-20
#define MAX_NUM_SYMBOLS 1000

namespace gr
{
namespace s4a
{

vfdm_equalizer::sptr vfdm_equalizer::make(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int preamble_block_rep,
		int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
		int data_block_rep, int silence_block_rep, int verbose)
{
	return gnuradio::get_initial_sptr(
			new vfdm_equalizer_impl(fft_length, occupied_tones, cp_length,
					precoder_length, preamble_block_rep, pilot_block_t_sz,
					pilot_block_rep, data_block_sz, data_block_rep,
					silence_block_rep, verbose));
}

/*
 * The private constructor
 */
vfdm_equalizer_impl::vfdm_equalizer_impl(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int preamble_block_rep,
		int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
		int data_block_rep, int silence_block_rep, int verbose) :
		gr::block("vfdm_equalizer",
				gr::io_signature::make(1, 1, fft_length * sizeof(gr_complex)),
				gr::io_signature::make3(3, 3,
						(data_block_sz * precoder_length) * sizeof(gr_complex),
						(fft_length * precoder_length) * sizeof(gr_complex),
						sizeof(float))), d_fft_length(fft_length), d_occupied_tones(
				occupied_tones), d_cp_length(cp_length), d_precoder_length(
				precoder_length), d_preamble_block_rep(preamble_block_rep), d_preamble_block_idx(
				0), d_pilot_block_idx(0), d_pilot_block_t_sz(pilot_block_t_sz), d_pilot_block_rep(
				pilot_block_rep), d_pilot_block_rdx(0), d_data_block_sz(
				data_block_sz), d_data_block_rep(data_block_rep), d_data_block_idx(
				0), d_silence_block_rep(silence_block_rep), d_silence_block_idx(
				0), d_state(ST_IDLE), d_est_SNR(0.0f), d_signal_power(0.0f), d_noise_power(
				0.0f), d_preamble_power(0.0f), d_pilot_power(0.0f), d_data_power(
				0.0f), d_average_SNR(0.0f), d_SNR_idx(0), d_verbose(verbose)
{
	int i = 0;
	int j = 0;
	int k = 0;

	d_fft = new fft::fft_complex(d_fft_length, true); // Load FFTW
	
	d_left_padding = floor((fft_length - occupied_tones) / 2);

	d_block_sz = preamble_block_rep + pilot_block_t_sz * pilot_block_rep
			+ data_block_sz * data_block_rep + silence_block_rep;
	
	d_fft_size = fft_length - precoder_length;

	printf("init::vfdm_equalizer::data_block_rep %d\n", data_block_rep);

	/*/ Init time and freq domain seq
	d_preamble_f.resize(d_fft_size);
	std::fill(d_preamble_f.begin(), d_preamble_f.end(), 0);

	d_pilot_f.resize(d_fft_size);
	std::fill(d_pilot_f.begin(), d_pilot_f.end(), 0);

	d_data_f.resize(d_fft_size);
	std::fill(d_data_f.begin(), d_data_f.end(), 0);

	d_silence_f.resize(d_fft_size);
	std::fill(d_silence_f.begin(), d_silence_f.end(), 0);

	d_preamble_t.resize(d_fft_size);
	std::fill(d_preamble_f.begin(), d_preamble_f.end(), 0);

	d_pilot_t.resize(d_fft_size);
	std::fill(d_pilot_f.begin(), d_pilot_f.end(), 0);

	d_data_t.resize(d_fft_size);
	std::fill(d_data_f.begin(), d_data_f.end(), 0);

	d_silence_t.resize(d_fft_size);
	std::fill(d_silence_f.begin(), d_silence_f.end(), 0);*/

	// Init syms sequence (to store data syms)
	d_data = (gr_complex *) malloc(precoder_length * data_block_rep * data_block_sz
					* sizeof(gr_complex));
	memset(d_data, 0,precoder_length * data_block_rep * data_block_sz
					* sizeof(gr_complex));
	d_data_block_total = data_block_rep * data_block_sz;

	// Init pilot sequence (FFT matrix)
	d_pilot_tx_f = (gr_complex *) malloc(precoder_length * pilot_block_t_sz * sizeof(gr_complex));
	k = 0;
	double deg = 0.0f;
	for (i = 0; i < pilot_block_t_sz; i++)
	{
		for (j = 0; j < precoder_length; j++)
		{
			deg = (-1 * M_TWOPI * j * i) / ((double) pilot_block_t_sz);
			d_pilot_tx_f[k] = gr_complex(cos(deg), sin(deg));
			k++;
		}
	}

	// Keep hermitian(pilot_tx_f) for further usage
	d_pilot_tx_f_hermitian = (gr_complex *) malloc(
		pilot_block_t_sz * precoder_length * sizeof(gr_complex));
	hermitian(d_pilot_tx_f_hermitian, d_pilot_tx_f, precoder_length,pilot_block_t_sz);

	// Init h_est_sum
	d_h_est_sum = (gr_complex *) malloc(fft_length * pilot_block_t_sz * sizeof(gr_complex));
	memset(d_h_est_sum, 0, fft_length * pilot_block_t_sz * sizeof(gr_complex));

	// Init h_est
	d_h_est = (gr_complex *) malloc(fft_length * precoder_length * sizeof(gr_complex));
	memset(d_h_est, 0, fft_length * precoder_length * sizeof(gr_complex));

	// Init h_est_pinv ( pinv(h_est))
	d_h_est_pinv = (gr_complexd *) malloc(precoder_length * fft_length * sizeof(gr_complexd));
	memset(d_h_est_pinv, 0, precoder_length * fft_length * sizeof(gr_complexd));

	/*
	 Init counter packet for coherence time estimation
	 */
	if (occupied_tones < 48)
	{
		throw std::runtime_error("init::vfdm_equalizer::Occupied tones should be greater than 48\n");
	}

	if (occupied_tones % 8 != 0)
	{
		throw std::runtime_error("init::vfdm_equalizer::Occupied tones should be multiple of 8\n");
	}

	if (data_block_rep % 2 != 1)
	{
		throw std::runtime_error("init::vfdm_equalizer::data_block_rep should be an odd number [1 3 5 ... ]\n");
	}

	if (preamble_block_rep % 2 != 0 && preamble_block_rep != 1)
	{
		throw std::runtime_error(
			"init::vfdm_equalizer::preamble_block_rep should be an even number or 1 [1 2 4 ... ]\n");
	}

	printf("fft_length %d, precoder_length %d", fft_length, precoder_length);
	d_mat_A = (gr_complexd *) malloc(fft_length * precoder_length * sizeof(gr_complexd));
	memset(d_mat_A, 0, fft_length * precoder_length * sizeof(gr_complexd));

	pinv_init(fft_length, precoder_length);

	set_output_multiple(data_block_rep);
}

/*
 * Our virtual destructor.
 */
vfdm_equalizer_impl::~vfdm_equalizer_impl()
{
}

/*****************************************************************
  FFT
 *****************************************************************/

/*std::vector<gr_complex> vfdm_equalizer_impl::fast_fourier_transform(
		std::vector<gr_complex> &seq_t, int fft_size, bool verbose = false)
{
	std::vector<gr_complex> seq_f(fft_size, gr_complex(0, 0));

	memcpy(d_fft->get_inbuf(), &seq_t[0], fft_size * sizeof(gr_complex));

	/* Copy symbols to input buffer (with FFT shift)
	gr_complex *dst = d_fft->get_inbuf();
	// half length of complex array
	unsigned int len = (unsigned int) (floor(d_fft_length / 2.0));
	memcpy(&dst[0], &result[len], sizeof(gr_complex) * (d_fft_length - len));
	memcpy(&dst[d_fft_length - len], &result[0], sizeof(gr_complex) * len);
	//memcpy(dst, &result[0], sizeof(gr_complex)*d_fft_length);

	// Execute FFT
	d_fft->execute();

	// Get results
	memcpy(&seq_f[0], d_fft->get_outbuf(),
			sizeof(gr_complex) * fft_size);
	// Print results
	if (verbose)
	{
		int i = 0;
		printf("vfdm_seq_in_f = [");
		for (i = 0; i < fft_size; i++)
		{
			gr_complex sym = seq_f[i];
			printf("%.4f+1i*(%.4f), ", sym.real(), sym.imag());
		}
		printf("]\n\n");
	}
	return seq_f;
}*/

/*****************************************************************
 Linear Algebra
 *****************************************************************/
// Multiply
void vfdm_equalizer_impl::multiply(std::vector<gr_complex> &matR,
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

void vfdm_equalizer_impl::multiply(gr_complexd* matR, gr_complexd* matA,
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

void vfdm_equalizer_impl::multiply(gr_complex* matR, gr_complex* matA,
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

// Hermitian

void vfdm_equalizer_impl::hermitian(std::vector<gr_complex> &matR,
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

void vfdm_equalizer_impl::hermitian(gr_complexd* matR, gr_complexd* matA,
		int am, int an)
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

void vfdm_equalizer_impl::hermitian(gr_complex* matR, gr_complex* matA, int am,
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

void vfdm_equalizer_impl::transpose(std::vector<gr_complex> &matR,
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

void vfdm_equalizer_impl::transpose(gr_complexd* matR, gr_complexd* matA,
		int am, int an)
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
void vfdm_equalizer_impl::toeplitz(std::vector<gr_complex> &matR,
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

void vfdm_equalizer_impl::toeplitz(gr_complexd* matR, gr_complexd* vecA,
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
void vfdm_equalizer_impl::print_matrix(const char* name,
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

void vfdm_equalizer_impl::print_matrix(const char* name, gr_complexd* matA,
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

void vfdm_equalizer_impl::print_matrix(const char* name, gr_complex* matA,
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

void vfdm_equalizer_impl::print_matrix(const char* name, double* matA, int am,
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

void vfdm_equalizer_impl::print_array(const char* name, gr_complexd* matA,
		int asz)
{
	int i = 0;
	int j = 0;

	for (i = 0; i < asz; i++)
	{
		gr_complexd res = matA[i];
		//printf("%s[%d]=gr_complex(%.4f,%.4f);  ", i, res.real(), res.imag());
	}
}

void vfdm_equalizer_impl::print_matrix_sum(const char* name, gr_complexd* matA,
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

void vfdm_equalizer_impl::print_matrix_sum(const char* name, gr_complex* matA,
		int am, int an)
{
	int i = 0;
	int j = 0;
	gr_complex sum = 0;

	for (i = 0; i < am; i++)
	{
		for (j = 0; j < an; j++)
		{
			sum += matA[j * am + i];
		}
	}
	printf("%s = %.8f\n", name, abs(sum));
}
/****************************************************************
 Pseudo-inverse of Matrix (using SVD)
 ****************************************************************/
void vfdm_equalizer_impl::pinv_init(int am, int an)
{
	d_mat_Uh = (gr_complexd *) malloc(am * am * sizeof(gr_complexd));

	d_mat_V = (gr_complexd *) malloc(an * an * sizeof(gr_complexd));

	d_mat_SM = (gr_complexd *) malloc(an * am * sizeof(gr_complexd));

	d_mat_T = (gr_complexd *) malloc(am * an * sizeof(gr_complexd));

	int rdim = std::max(am, an);
	d_mat_R = (gr_complexd *) malloc(rdim * rdim * sizeof(gr_complexd));

	svd_init(am, an, true);
}

void vfdm_equalizer_impl::pinv()
{
	int i = 0;
	int a_sz = d_M * d_N;

	// Copy input symbols to d_mat_A matrix
	for (i = 0; i < a_sz; i++)
	{
		d_mat_A[i] = (gr_complex) d_h_est[i];
	}

	svd_decompose();

	TOL = d_S[0] * std::max(d_M, d_N) * std::numeric_limits<double>::epsilon();

	// Compute matrix SM	
	int SM_sz = std::min(d_M, d_N);

	memset(d_mat_SM, 0, d_N * d_M * sizeof(gr_complexd));

	for (int i = 0; i < SM_sz; i++)
	{
		if (abs(d_S[i]) > TOL)
		{
			d_mat_SM[i * d_N + i] = gr_complex(1 / d_S[i], 0.0f);
		}
		else
		{
			d_mat_SM[i * d_N + i] = gr_complex(0, 0);
		}

	}

	// Hermitian(U)
	hermitian(d_mat_Uh, d_mat_U, d_M, d_M);
	//print_matrix("Uh",d_mat_Uh, d_M, d_M);

	// Hermitian(V)
	hermitian(d_mat_V, d_mat_VT, d_N, d_N);
	//print_matrix("V",d_mat_V, d_N, d_N);

	// pinv(A) = V*SM*U' = d_mat_VT * d_mat_SM * d_mat_U
	multiply(d_mat_T, d_mat_SM, d_mat_Uh, d_N, d_M, d_M, d_M);
	multiply(d_mat_R, d_mat_V, d_mat_T, d_N, d_N, d_N, d_M);

	// Copy result to d_h_est_pinv matrix
	for (i = 0; i < a_sz; i++)
	{
		d_h_est_pinv[i] = d_mat_R[i];
	}
}

/***************************************************************
 Singular Value Decomposition
 ***************************************************************/
// Initialize SVD
void vfdm_equalizer_impl::svd_init(size_t M, size_t N, bool calculate_vt)
{
	d_M = M;
	d_N = N;

	d_LDA = M;

	/* setup the array S */
	d_S = new double[std::min(M, N)];

	/* setup the array U */
	d_LDU = M;
	d_LDVT = N;

	d_mat_U = new gr_complexd[M * M];
	d_mat_VT = new gr_complexd[N * N];

	/* setup internal stuff */
	d_RWORK = new double[5 * std::min(M, N) * std::min(M, N)
			+ 7 * std::min(M, N)];
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

void vfdm_equalizer_impl::svd_decompose()
{
	char jobu = 'A';
	char jobvt = 'A';
	zgesvd_(&jobu, &jobvt, &d_M, &d_N, d_mat_A, &d_LDA, d_S, d_mat_U, &d_LDU,
			d_mat_VT, &d_LDVT, d_WORK, &d_LWORK, d_RWORK, &d_INFO);
}

/***************************************************************
 Forecast and Work
 ***************************************************************/
void vfdm_equalizer_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % d_data_block_rep))
			/ d_data_block_rep;
	//printf("coeff %d block_sz %d\n",coeff,d_block_sz);
	ninput_items_required[0] = d_block_sz * coeff;
}

int vfdm_equalizer_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const gr_complex *in_syms = (const gr_complex *) input_items[0];

	gr_complex *out_syms = (gr_complex *) output_items[0];
	gr_complex *out_h = (gr_complex *) output_items[1];
	float *out_snr = (float *) output_items[2];

	int i = 0;

	int no = 0; // number items output
	int ni = 0; // number items read from input
	int nidx = 0;
	int nodx = 0;
	int no_hdx = 0;
	int p_idx = 0;
	int d_idx = 0;
	int p = 0;	
	
	double temp_power = 0.0f;
	
	while (ni < ninput_items && no < noutput_items)
	{
		//if (p == 0) printf("ni1: %i, no: %i \n",ninput_items,noutput_items);
		//p ++;
		//printf("nin1: %i, nout: %i \n",ninput_items,noutput_items);
		switch (d_state)
		{
		case ST_IDLE:	//printf("IDLE_");
			d_state = ST_PREAMBLE;
			break;
		case ST_PREAMBLE:	//printf("PREAMBLE_");
			if (d_preamble_block_idx < d_preamble_block_rep)
			{
				//memcpy(&d_preamble_t[0], &in_syms[nidx+d_cp_length], d_fft_size * sizeof(gr_complex));
				//d_preamble_f = fast_fourier_transform(d_preamble_t,d_fft_size, 								false);				
				/*printf("vfdm_preamble_f_rx = {");
				for (int j = 0; j < d_fft_size; j++)
				{
					//gr_complex sym = d_preamble_f[j];
					gr_complex sym = in_syms[nidx + d_cp_length + j];
					printf("%i = (%.4f, %.4f), ", j, sym.real(), sym.imag());
				}
				printf("}\n\n");*/

				for (i = 0; i < /*d_occupied_tones*/d_fft_length - d_cp_length; i++)
				{
					temp_power = pow(abs(in_syms[nidx + d_cp_length + /*d_left_padding*/ + i]),2);
					d_signal_power += temp_power;
					d_preamble_power += temp_power;
				}
				nidx += d_fft_length;
				d_preamble_block_idx++;
				d_state = ST_PREAMBLE;
			}
			else
			{
				memset(d_h_est_sum, 0,d_fft_length * d_pilot_block_t_sz * sizeof(gr_complex));
				d_preamble_block_idx = 0;
				d_state = ST_PILOT;
			}
			break;
		case ST_PILOT:	//printf("PILOT_");
		{
			if (d_pilot_block_rdx < d_pilot_block_rep)
			{ // Pilot repetition?
				if (d_pilot_block_idx < d_pilot_block_t_sz)
				{ // Pilot T size?
					p_idx = d_pilot_block_idx * d_fft_length;
					for (i = 0; i < d_fft_length; i++)
					{
						d_h_est_sum[p_idx + i] += in_syms[nidx + i];
						temp_power = pow(abs(in_syms[nidx + i]), 2); // SNR purpose
						d_pilot_power += temp_power;
						d_signal_power += temp_power;
					}
					d_pilot_block_idx++;
					nidx += d_fft_length;
				}
				else
				{
					d_pilot_block_idx = 0;
					d_pilot_block_rdx++;
				}
				d_state = ST_PILOT;
			}
			else
			{
				d_pilot_block_idx = 0;
				d_pilot_block_rdx = 0;
				d_state = ST_PILOT_POST;
			}
			break;
		}
		case ST_PILOT_POST:	//printf("PILOT-POST_");
			// It's time to process after we receive all the pilot blocks!
		{
			if (d_pilot_block_rep > 0)
			{
				multiply(d_h_est, d_h_est_sum, d_pilot_tx_f_hermitian,
						d_fft_length, d_pilot_block_t_sz, d_pilot_block_t_sz,
						d_precoder_length);
				pinv();
			};
			d_state = ST_DATA;
			break;
		}
		case ST_DATA:	//printf("DATA_");
		{
			d_idx = d_data_block_idx * d_precoder_length;

			// Is data block available?
			if (d_data_block_idx < d_data_block_total)
			{
				// Copy input symbols to temporary matrix
				for (i = 0; i < d_fft_length; i++)
				{
					gr_complex s2 = in_syms[nidx + i];
					d_mat_T[i] = in_syms[nidx + i];

					temp_power += pow(abs(s2), 2); // SNR purpose
					d_data_power += temp_power;
					d_signal_power += temp_power;
				}

				// Equalize
				//print_matrix("H_est", d_h_est_pinv, d_precoder_length, d_fft_length);
				//printf("\n\n");
				multiply(d_mat_R, d_h_est_pinv, d_mat_T, d_precoder_length,
						d_fft_length, d_fft_length, 1);

				// Copy result to data matrix
				for (i = 0; i < d_precoder_length; i++)
				{
					d_data[d_idx + i] = d_mat_R[i];
				}
				nidx += d_fft_length;
				d_data_block_idx++;
				d_state = ST_DATA;
			}
			else
			{
				d_data_block_idx = 0;
				d_state = ST_SILENCE;
			}
			break;
		}
		case ST_SILENCE:	//printf("SILENCE_");
		{
			if (d_silence_block_idx < d_silence_block_rep)
			{
				for (i = 0; i < d_fft_length; i++)
				{
					d_noise_power += pow(abs(in_syms[nidx + i]), 2); // SNR purpose
				}
				nidx += d_fft_length;
				d_silence_block_idx++;
				d_state = ST_SILENCE;
			}
			else
			{
				if (d_silence_block_rep > 0)
				{
					d_signal_power = d_signal_power/ ((d_fft_length - d_cp_length)*d_preamble_block_rep 								+ d_fft_length*d_pilot_block_rep* d_pilot_block_t_sz  
							+ d_fft_length*d_data_block_sz*d_data_block_rep);
					d_preamble_power = d_preamble_power
								/ (/*d_occupied_tones*/(d_fft_length - d_cp_length) * 									d_preamble_block_rep);
					d_pilot_power = d_pilot_power
								/ (d_fft_length * d_pilot_block_rep
										* d_pilot_block_t_sz);
					d_data_power = d_data_power
								/ (d_fft_length * d_data_block_sz
										* d_data_block_rep);
					d_noise_power = (d_noise_power + TINY)
								/ (d_fft_length * d_silence_block_rep);
					d_est_SNR = 10* log10(d_signal_power/ d_noise_power);
				
					d_average_SNR += 10 * log10(d_preamble_power / d_noise_power);
					
					d_SNR_idx ++;
					/*if (d_SNR_idx % 100 == 0) 
					{
						printf("VFDM Eq > Iter %i, Pre-power = %4.4f, Noise power = %4.4f, Average SNR = %4.2f dB \n", d_SNR_idx/100,d_preamble_power,d_noise_power, d_average_SNR/d_SNR_idx);
					}*/

					if (d_verbose == 1)
					{
						printf("\nVFDM Eq > ");
						printf("Pre: %4.2f dB, \t",
								10 * log10(d_preamble_power / d_noise_power));
						printf("Pil: %4.2f dB, \t",
								10 * log10(d_pilot_power / d_noise_power));
						printf("Dat: %4.2f dB, \t",
								10 * log10(d_data_power / d_noise_power));
						printf("Sil: %4.2f dB, \t",
								10 * log10(d_noise_power / d_noise_power));
						printf("Aver: %4.2f dB, \t",
								d_est_SNR);
						//printf("Pre/Data = %4.2f dB \t",
						//		10 * log10(d_preamble_power / d_data_power));
					}
					d_signal_power = 0.0f;
					d_preamble_power = 0.0f;
					d_pilot_power = 0.0f;
					d_data_power = 0.0f;
					d_noise_power = 0.0f;
				}
				d_silence_block_idx = 0;
				d_state = ST_OUT;
			}
			break;
		}
		case ST_OUT:	//printf("OUT_");
		{
			d_idx = d_data_block_idx * (d_precoder_length * d_data_block_sz);
			if (d_data_block_idx < d_data_block_rep)
			{
				// Send data symbols to output
				//printf("Data out: ");
				for (i = 0; i < (d_precoder_length * d_data_block_sz); i++)
				{
					//printf("Equalized Data Size: %i", d_data->size());
					gr_complex s1 = d_data[d_idx + i];
					out_syms[nodx + i] = s1;
					//int bit;
					//if (s1.real() > 0) bit = 1; else bit = 0;
					//printf("%i",bit); if ((i+1) % 8 == 0) printf("  ");
				}
				//printf("\n");

				no_hdx = no * (d_fft_length * d_precoder_length);

				// Send estimated channel to output
				//printf("Channel out\n");
				for (i = 0; i < d_fft_length * d_precoder_length; i++)
				{
					out_h[no_hdx + i] = d_h_est[i];
				}

				// Send estimated SNR to output
				//printf("SNR out\n");
				out_snr[no] = (float) d_est_SNR;

				nodx += d_precoder_length * d_data_block_sz;
				d_data_block_idx++;
				d_state = ST_OUT;
			}
			else
			{
				ni += d_block_sz;
				no += d_data_block_rep;
				d_data_block_idx = 0;
				d_state = ST_IDLE;
			}
			break;
		}
		default:
			throw std::invalid_argument("vfdm_equalizer::invalid state");
			break;
		}
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

