#include <benchmark/benchmark.h>
#include <simplecrc.h>
#include <cstring>

DECLARE_CRC(KERMIT, 0x1021, 0x0000, true, true, 0x0, 0x0, 0x2189, 16);

#define TABLE_LEN (256)

static uint64_t internal_table[TABLE_LEN] = { 0 };

uint64_t reflect(uint64_t x, uint8_t width)
{
	x = (x & 0x5555555555555555) << 1 | (x & 0xAAAAAAAAAAAAAAAA) >> 1;
	x = (x & 0x3333333333333333) << 2 | (x & 0xCCCCCCCCCCCCCCCC) >> 2;
	x = (x & 0x0F0F0F0F0F0F0F0F) << 4 | (x & 0xF0F0F0F0F0F0F0F0) >> 4;
	x = (x & 0x00FF00FF00FF00FF) << 8 | (x & 0xFF00FF00FF00FF00) >> 8;
	x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
	x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;

	x >>= (64 - width);
	return x;
}

uint64_t gen_mask(uint8_t width)
{
	if (width >= 64)
		return 0xffffffffffffffff;
	else
		return ((uint64_t)1 << width) - 1;
}

static uint64_t crc_one_byte(int init, int width, uint64_t poly)
{
	uint64_t topbit = (uint64_t)1 << (width - 1);
	uint64_t crc = init;

	for (int bit = width; bit > 0; --bit) {
		if (crc & topbit)
			crc = (crc << 1) ^ poly;
		else
			crc <<= 1;
	}

	return crc & gen_mask(width);
}

static void precompute_table_bitshift(struct crc_def params, uint64_t **buf)
{
	uint64_t *tbl = *buf;

	for (int byte = 0; byte < TABLE_LEN; ++byte) {
		uint64_t crc = crc_one_byte(byte, params.width, params.poly);
		tbl[byte] = crc;
	}
}

bool precompute_crc_table(struct crc_def params, uint64_t **table, size_t len)
{
	if (len != (TABLE_LEN * sizeof(uint64_t)) || !table || !(*table))
		return false;

	precompute_table_bitshift(params, table);
	return true;
}

uint64_t compute_crc_fast(struct crc_def params, const unsigned char *buf,
						  size_t len, const uint64_t *table)
{
	uint64_t crc;
	const unsigned char *ptr;
	size_t a;

	crc = params.init;
	ptr = buf;

	for (a = 0; a < len; a++) {
		uint8_t u_char = (uint8_t)*ptr++;
		if (params.ref_in)
			u_char = reflect(u_char, 8);
		if (params.width >= 8)
			crc = table[((crc >> (params.width - 8)) ^ (uint64_t)u_char) &
						0xFFull] ^
				  (crc << 8);
	}

	if (params.ref_out)
		crc = reflect(crc, params.width) ^ params.xor_out;
	else
		crc = crc ^ params.xor_out;

	return crc & gen_mask(params.width);
}

uint64_t compute_crc_old(struct crc_def params, const unsigned char *buf,
					 size_t len)
{
	uint64_t *tbl_ptr = internal_table;

	precompute_crc_table(params, &tbl_ptr, TABLE_LEN * sizeof(uint64_t));
	return compute_crc_fast(params, buf, len, internal_table);
}

// Define another benchmark
static void BM_CrcFastOld(benchmark::State& state) {
    unsigned char* msg = new unsigned char[state.range(0)];
    uint64_t* tbl = new uint64_t[TABLE_LEN];

    memset(msg, 'x', state.range(0));
    precompute_crc_table(CRC_KERMIT, &tbl, TABLE_LEN * sizeof(uint64_t));

    for (auto _ : state) {
        compute_crc_fast(CRC_KERMIT, msg, state.range(0), tbl);
    }

    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
    delete[] msg;
    delete[] tbl;
}
BENCHMARK(BM_CrcFastOld)->Arg(8)->Arg(64)->Arg(512)->Arg(1<<10)->Arg(8<<10);

// Define another benchmark
static void BM_CrcSlowOld(benchmark::State& state) {
    unsigned char* msg = new unsigned char[state.range(0)];

    memset(msg, 'x', state.range(0));

    for (auto _ : state) {
        compute_crc_old(CRC_KERMIT, msg, state.range(0));
    }

    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
    delete[] msg;
}
BENCHMARK(BM_CrcSlowOld)->Arg(8)->Arg(64)->Arg(512)->Arg(1<<10)->Arg(8<<10);

// Define another benchmark
static void BM_CrcNew(benchmark::State& state) {
    unsigned char* msg = new unsigned char[state.range(0)];
    memset(msg, 'x', state.range(0));

    for (auto _ : state) {
        compute_crc(CRC_KERMIT, msg, state.range(0));
    }

    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
    delete[] msg;
}
BENCHMARK(BM_CrcNew)->Arg(8)->Arg(64)->Arg(512)->Arg(1<<10)->Arg(8<<10);

BENCHMARK_MAIN();
