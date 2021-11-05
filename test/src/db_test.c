#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
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
    char *db_filename = NULL;
    struct stat file_buf;
    struct crc_def params = {0};
    FILE* fp;
    size_t line_counter = 0;
    char line[1024];

    if(argc <= 1) {
        printf("test <filename>\n");
        return 1;
    }
    db_filename = argv[1];
    if(db_filename == NULL) {
        fprintf(stderr, "Filename not found\n");
        return 1;
    }
    
    if(stat(db_filename, &file_buf) != 0) {
        fprintf(stderr, "File %s not found\n", db_filename);
        return 1;
    }
    fp = fopen(db_filename, "r");
    if(!fp) {
        fprintf(stderr, "Could not open %s\n", db_filename);
        return 1;
    }

    while (fgets(line, sizeof(line), fp)) {
        //printf("%s", line); 
        char *token;
        char *strptr = line;
        char *eol = strchr(line, ';');
        if(eol == NULL) {
            fprintf(stderr, "Could not find terminator on line %zu\n%zu: %s\n", line_counter, line_counter, line);
            return 1;
        }
        *eol = '\0';
        while( (token = strsep(&strptr, " ")) != NULL ) {
            printf("%s\n",token);
        }
        line_counter += 1;
        printf("\n");
    }
    fclose(fp);

    
        /*
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
                break;*/
    printf("name: %s\npoly: 0x%.8lx\n"
           "xor_out: 0x%.8lx\nresidue: 0x%.8lx\n"
           "check: 0x%.8lx\nwidth: 0x%lu\n"
           "ref_in: %i\nref_out: %i\n",
           "test", params.poly, params.xor_out, params.residue, params.check, params.width, params.ref_in, params.ref_out);
	return 0;
}