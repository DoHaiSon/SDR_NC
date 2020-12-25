/******************************************************************************
 *
 * 			SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 			http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_EQUALIZER_IMPL_H
#define INCLUDED_S4A_VFDM_EQUALIZER_IMPL_H

#include <s4a/vfdm_equalizer.h>
#include <vector>
#include <sys/time.h>
#include <gnuradio/fft/fft.h>

#include <boost/thread.hpp>
#include <cstdio>

// SVD from LAPACK
extern "C"
{
void
zgesdd_(char* JOBZ, size_t* M, size_t* N, std::complex<double>* A, size_t* LDA,
		double* S, std::complex<double>* U, size_t* LDU,
		std::complex<double>* VT, size_t* LDVT, std::complex<double>* WORK,
		size_t* LWORK, double* RWORK, int* IWORK, int* INFO);
void
zgesvd_(char* JOBU, char* JOBVT, size_t* M, size_t* N, std::complex<double>* A,
		size_t* LDA, double* S, std::complex<double>* U, size_t* LDU,
		std::complex<double>* VT, size_t* LDVT, std::complex<double>* WORK,
		size_t* LWORK, double* RWORK, int* INFO);
}

namespace gr
{
namespace s4a
{

class vfdm_equalizer_impl: public vfdm_equalizer
{
private:
	enum state_t
	{
		ST_IDLE,
		ST_PREAMBLE,
		ST_PILOT,
		ST_PILOT_POST,
		ST_DATA,
		ST_SILENCE,
		ST_OUT
	};
	state_t d_state;

	fft::fft_complex *d_fft;
	
	int d_fft_size;
	int d_fft_length;
	int d_occupied_tones;
	int d_left_padding;
	int d_cp_length;
	int d_precoder_length;

	int d_preamble_block_rep;
	int d_preamble_block_idx;

	int d_pilot_block_idx;
	int d_pilot_block_t_sz;
	int d_pilot_block_rep;
	int d_pilot_block_rdx;

	int d_data_block_sz;
	int d_data_block_rep;
	int d_data_block_idx;
	int d_data_block_total;

	int d_silence_block_rep;
	int d_silence_block_idx;

	int d_verbose;

	int d_block_idx;
	int d_block_sz;

	/* SNR Calculation */
	float d_snr_threshold;
	double d_est_SNR;
	double d_signal_power;
	double d_preamble_power;
	double d_pilot_power;
	double d_data_power;
	double d_noise_power;
	double d_average_SNR;
	int d_SNR_idx;

	/* Vectors for channel estimation */
	gr_complex* d_data;

	gr_complex* d_pilot_tx_f;
	gr_complex* d_pilot_tx_f_hermitian;

	gr_complex* d_h_est_sum;
	gr_complex* d_h_est;
	gr_complexd* d_h_est_pinv;

	/* SVD variables & functions */
	size_t d_M, d_N, d_LWORK, d_LDVT, d_LDU, d_LDA;
	gr_complexd* d_mat_A;
	gr_complexd* d_mat_U;
	gr_complexd* d_mat_VT;
	gr_complexd* d_WORK;
	double* d_RWORK;
	double* d_S;
	int* d_IWORK;
	int d_INFO;
	bool d_calculate_vt;
	void
	svd_init(size_t M, size_t N, bool calculate_vt); // Initialize SVD Decomposer
	void
	svd_decompose();
	void
	svd_set_matrix(gr_complexd* mat_A);
	double*
	svd_get_S();
	gr_complexd*
	svd_get_U();
	gr_complexd*
	svd_get_VT();

	/* Pseudo-inverse variables */
	double TOL;
	gr_complexd* d_mat_Uh;
	gr_complexd* d_mat_V;
	gr_complexd* d_mat_SM;
	gr_complexd* d_mat_T;
	gr_complexd* d_mat_R;
	
	/* Time and Frequency domain variables
	std::vector<gr_complex> d_preamble_f;
	std::vector<gr_complex> d_pilot_f;
	std::vector<gr_complex>	d_data_f;
	std::vector<gr_complex> d_silence_f;
	std::vector<gr_complex> d_preamble_t;
	std::vector<gr_complex> d_pilot_t;
	std::vector<gr_complex>	d_data_t;
	std::vector<gr_complex> d_silence_t;*/

	/*****************************************************************
	 Linear Algebra
	 ****************************************************************/
	void
	multiply(std::vector<gr_complex> &matR, std::vector<gr_complex> &matA,
			std::vector<gr_complex> &matB, int am, int an, int bm, int bn);
	void
	multiply(gr_complexd* matR, gr_complexd* matA, gr_complexd* matB, int am,
			int an, int bm, int bn);
	void
	multiply(gr_complex* matR, gr_complex* matA, gr_complex* matB, int am,
			int an, int bm, int bn);

	void
	hermitian(std::vector<gr_complex> &matR, std::vector<gr_complex> &matA,
			int am, int an);
	void
	hermitian(gr_complexd* matR, gr_complexd* matA, int am, int an);

	void
	hermitian(gr_complex* matR, gr_complex* matA, int am, int an);

	void
	transpose(std::vector<gr_complex> &matR, std::vector<gr_complex> &matA,
			int am, int an);
	void
	transpose(gr_complexd* matR, gr_complexd* matA, int am, int an);

	void
	toeplitz(std::vector<gr_complex> &matR, std::vector<gr_complex> &vecA,
			std::vector<gr_complex> &vecB, int am, int bm);
	void
	toeplitz(gr_complexd* matR, gr_complexd* vecA, gr_complexd* vecB, int am,
			int bm);

	void
	pinv_init(int am, int an);
	void
	pinv();

	void
	print_matrix(const char* name, std::vector<gr_complex> &matA, int am,
			int an);
	void
	print_matrix(const char* name, gr_complexd* matA, int am, int an);
	void
	print_matrix(const char* name, gr_complex* matA, int am, int an);
	void
	print_matrix(const char* name, double* matA, int am, int an);

	void
	print_matrix_sum(const char* name, gr_complexd* matA, int am, int an);
	void
	print_matrix_sum(const char* name, gr_complex* matA, int am, int an);

	void
	print_array(const char* name, gr_complexd* matA, int asz);

public:
	vfdm_equalizer_impl(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int preamble_block_rep, int pilot_block_t_sz,
			int pilot_block_rep, int data_block_sz, int data_block_rep,
			int silence_block_rep, int verbose);
	~vfdm_equalizer_impl();
	std::vector<gr_complex>	fast_fourier_transform(
		std::vector<gr_complex> &seq_t, int fft_size, bool verbose);

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_EQUALIZER_IMPL_H */

