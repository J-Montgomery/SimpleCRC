#ifndef SIMPLECRC_H_
#define SIMPLECRC_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct crc_def {
	uint64_t poly;
	uint64_t init;
	uint64_t xor_out;
	uint64_t residue;
	uint64_t check;
	uint64_t width;
	bool ref_in;
	bool ref_out;
};

uint64_t crc_one_byte(int init, int width, uint64_t poly);
void precompute_table_bit(struct crc_def params);
uint64_t compute_crc(const unsigned char *buf, size_t len, struct crc_def params);

void print_mem(uint64_t *buf, unsigned len);
void precompute_table_bit(struct crc_def params);
unsigned char crc_calc_test(unsigned char *buf, int size, uint64_t *table,
			    unsigned init, unsigned width);
uint64_t compute_crc_bit(const char *buf, size_t len, struct crc_def params);

#endif /* SIMPLECRC_H_ */
