#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

#include <simplecrc.h>

#define xstr(a) str(a)
#define str(a) #a

#define DECLARE_TEST(NAME, BUF, VAR)                               \
	VAR = compute_crc(BUF, 9, CRC_##NAME);                     \
	printf("%s %-16s 0x%.8lx\n",                               \
	       (result == CRC_##NAME.check) ? "PASSED" : "FAILED", \
	       xstr(CRC_##NAME), VAR);   \
    if(result != CRC_##NAME.check) return 1


/* DECLARE_CRC(NAME, POLY, INIT, REF_IN, REF_OUT, XOR_OUT, RESIDUE, CHECK, WIDTH */
int main(int argc, char **argv)
{
	uint64_t result = 0;
	unsigned char buf[] = "123456789";
    char *test_name;
    struct crc_def params = {0};
    int c;

    while(1) {
        static struct option long_options[] = {
            {"name", required_argument, 0, 'n'},
            {"poly", required_argument, 0, 'p'},
            {"refin", required_argument, 0, 'i'},
            {"refout", required_argument, 0, 'o'},
            {"xor", required_argument, 0, 'x'},
            {"residue", required_argument, 0, 'r'},
            {"check", required_argument, 0, 'c'},
            {"width", required_argument, 0, 'w'},
            {0, 0, 0, 0}
        };

        int opt_index = 0;
        c = getopt_long(argc, argv, "n:p:i:o:x:r:c:w:", long_options, 
        &opt_index);

        if(c == -1)
            break;
        switch(c) {
            case 'n': {
                test_name = optarg;
                break;
            }
            case 'p': {
                unsigned long long poly = strtoul(optarg, NULL, 16);
                if(poly != 0 && !errno) {
                    params.poly = poly;
                }
                break;
            }
            case 'i': {
                int cmp =  strcmp("true", optarg);
                if(cmp == 0) {
                    params.ref_in = true;
                } else {
                    params.ref_in = false; 
                }
                break;
            }
            case 'o': {
                int cmp =  strcmp("true", optarg);
                if(cmp == 0) {
                    params.ref_out = true;
                } else {
                    params.ref_out = false; 
                }
                break;
            }
            case 'x': {
                unsigned long long xor_out = strtoul(optarg, NULL, 16);
                if(xor_out != 0 && !errno) {
                    params.xor_out = xor_out;
                }
                break;
            }
            case 'r': {
                unsigned long long residue = strtoul(optarg, NULL, 16);
                if(residue != 0 && !errno) {
                    params.residue = residue;
                }
                break;
            }
            case 'c': {
                unsigned long long check = strtoul(optarg, NULL, 16);
                if(check != 0 && !errno) {
                    params.check = check;
                }
                break;
            }
            case 'w': {
                unsigned long long width = strtoul(optarg, NULL, 16);
                if(width > 0 && width <= 64 && !errno) {
                    params.width = width;
                }
                break;
            }
            default:
                abort();
        }
    }

    printf("name: %s\npoly: 0x%.8lx\n"
           "xor_out: 0x%.8lx\nresidue: 0x%.8lx\n"
           "check: 0x%.8lx\nwidth: 0x%lu\n"
           "ref_in: %i\nref_out: %i\n",
           test_name, params.poly, params.xor_out, params.residue, params.check, params.width, params.ref_in, params.ref_out);
	return 0;
}