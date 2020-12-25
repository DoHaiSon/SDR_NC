/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_PRECODER_IMPL_H
#define INCLUDED_S4A_VFDM_PRECODER_IMPL_H

#include <s4a/vfdm_precoder.h>
#include <vector>
#include <gnuradio/fft/fft.h>

// Use SVD decomposer from LAPACK
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

class vfdm_precoder_impl: public vfdm_precoder
{
private:
	enum state_t
	{
		ST_IDLE,
		ST_BUILD_PRECODER,
		ST_PREAMBLE_OUT,
		ST_PILOT_PRECODE,
		ST_PILOT_OUT,
		ST_DATA_STORE,
		ST_DATA_PRECODE,
		ST_DATA_OUT,
		ST_SILENCE_OUT,
		ST_TIMEOUT
	};
	// SVD private variables;
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

	fft::fft_complex *d_fft;

	int d_fft_length;
	int d_cp_length;
	int d_left_padding;
	int d_precoder_length;
	int d_channel_size;
	int d_out_length;

	int d_preamble_block_rep;
	int d_preamble_block_idx;

	int d_pilot_block_t_sz;
	int d_pilot_block_rep;
	int d_pilot_block_idx;
	int d_pilot_block_total;

	int d_data_block_sz;
	int d_data_block_rep;
	int d_data_block_idx;
	int d_data_block_total;

	int d_silence_block_rep;
	int d_silence_block_idx;

	int d_block_sz;

	state_t d_state;

	/* timeout syms */
	int d_timeout_syms_total;
	int d_timeout_syms_idx;
	gr_complex d_timeout_ref_sym;

	gr_complex* d_pilot_f; // Pilots (FFT matrix)
	std::vector<gr_complex> d_preamble_f; // Preamble in frequency
	std::vector<gr_complex> d_preamble_t; // Preamble in time

	gr_complex* d_data;
	gr_complexd* d_h21; // Latest channel vector
	gr_complexd* d_V1; // Result vector (Temporal)
	gr_complexd* d_V2; // Result vector (Temporal)
	gr_complexd* d_T; // Latest Toeplitz matrix
	gr_complex* d_E; // Latest precoder matrix
	gr_complex* d_R; // Result matrix (Temporal)

	struct timeval t_begin,t_end;

	unsigned long mtime, seconds, useconds;
	int d_time_idx;


	bool isOK;

	/**************************************************************
	 Linear Algebra
	 **************************************************************/
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

public:
	vfdm_precoder_impl(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int channel_size, int preamble_block_rep,
			int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep);
	~vfdm_precoder_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);

	static std::vector<gr_complex>
	generate_complex_sequence(int seq_size, int zeros_period, int seed);

	std::vector<gr_complex>
	generate_complex_sequence(int seq_size, int left_padding, int zeros_period,
			int seed, bool verbose);
	std::vector<gr_complex>
	generate_complex_sequence_in_time(int seq_size, int left_padding,
			int zeros_period, int seed, bool verbose);

	// SVD Decomposer public variables
	void
	svd_init(size_t M, size_t N, bool calculate_vt);

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
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_PRECODER_IMPL_H */

