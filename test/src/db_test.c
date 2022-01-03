#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <simplecrc.h>

#define xstr(a) str(a)
#define str(a) #a

#define min(a, b)               \
	({                          \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a < _b ? _a : _b;      \
	})

struct test_definition {
	char test_name[32];
	struct crc_def params;
};

int run_crc_check(struct test_definition def)
{
	uint64_t result = 0;
	unsigned char buf[] = "123456789";

	result = compute_crc(def.params, buf, 9);

	printf("%s %s-%li %s 0x%.8lx\n",
		   (result == def.params.check) ? "PASSED" : "FAILED", def.test_name,
		   def.params.width, "\t\t", result);
	if (result != def.params.check)
		return 1;
	return 0;
}

struct test_definition get_crc_def(char *line, int *errors)
{
	char *token;
	char *strptr = line;
	struct test_definition def = { 0 };
	int iteration = 0;
	while ((token = strsep(&strptr, " ")) != NULL) {
		//printf("%s\n", token);
		switch (iteration) {
		case 0: // Algorithm name
		{
			memcpy(def.test_name, token, min(32, strlen(token)));
			break;
		}
		case 1: // CRC width
		{
			unsigned long long width = strtoul(token, NULL, 10);
			if (width >= 8 && width <= 64 && !errno) {
				def.params.width = width;
			} else {
				fprintf(stderr, "Unsupported CRC width\n");
				*errors = EINVAL;
				return def;
			}
			break;
		}
		case 2: // CRC Poly
		{
			unsigned long long poly = strtoul(token, NULL, 16);
			if (poly != 0 && !errno) {
				def.params.poly = poly;
			} else {
				fprintf(stderr, "Invalid CRC poly\n");
				*errors = EINVAL;
				return def;
			}
			break;
		}
		case 3: // Reflect Input
		{
			int cmp = strcmp("true", token);
			if (cmp == 0) {
				def.params.ref_in = true;
			} else {
				def.params.ref_in = false;
			}
			break;
		}
		case 4: // Reflect Output
		{
			int cmp = strcmp("true", token);
			if (cmp == 0) {
				def.params.ref_out = true;
			} else {
				def.params.ref_out = false;
			}
			break;
		}
		case 5: // Init Value
		{
			unsigned long long init = strtoul(token, NULL, 16);
			if (!errno) {
				def.params.init = init;
			} else {
				fprintf(stderr, "Invalid CRC init\n");
				*errors = EINVAL;
				return def;
			}
			break;
		}
		case 6: // XOR Mask
		{
			unsigned long long xor_out = strtoul(token, NULL, 16);
			if (!errno) {
				def.params.xor_out = xor_out;
			} else {
				fprintf(stderr, "Invalid XOR out\n");
				*errors = EINVAL;
				return def;
			}
			break;
		}
		case 7: // Check
		{
			unsigned long long check = strtoul(token, NULL, 16);
			if (!errno) {
				def.params.check = check;
			} else {
				fprintf(stderr, "Invalid check value\n");
				*errors = EINVAL;
				return def;
			}
			break;
		}
		case 8: // Residue
		{
			unsigned long long residue = strtoul(token, NULL, 16);
			if (!errno) {
				def.params.residue = residue;
			} else {
				fprintf(stderr, "Invalid CRC residue\n");
				*errors = EINVAL;
				return def;
			}
			break;
		}
		default:
			fprintf(stderr, "Unknown parameters in CRC definition\n");
			*errors = EINVAL;
			return def;
		}
		iteration += 1;
	}

	return def;
}

int main(int argc, char **argv)
{
	char *db_filename = NULL;
	struct stat file_buf;
	FILE *fp;

	size_t line_counter = 1;
	char line[1024];

	if (argc <= 1) {
		printf("test <filename>\n");
		return 1;
	}
	db_filename = argv[1];
	if (db_filename == NULL) {
		fprintf(stderr, "Filename not found\n");
		return 1;
	}

	if (stat(db_filename, &file_buf) != 0) {
		fprintf(stderr, "File %s not found\n", db_filename);
		return 1;
	}
	fp = fopen(db_filename, "r");
	if (!fp) {
		fprintf(stderr, "Could not open %s\n", db_filename);
		return 1;
	}

	while (fgets(line, sizeof(line), fp)) {
		struct test_definition def;
		int err_flag = 0;
		char *eol = strchr(line, ';');
		if (eol == NULL) {
			fprintf(stderr, "Could not find terminator on line %zu\n%zu: %s\n",
					line_counter, line_counter, line);
			line_counter += 1;
			continue;
		}
		*eol = '\0';

		def = get_crc_def(line, &err_flag);
		if (err_flag != 0) {
			fprintf(stderr, "Skipping CRC %s on line %zu\n", def.test_name,
					line_counter);
			err_flag = 0;
			line_counter += 1;
			continue;
		}

		run_crc_check(def);

		line_counter += 1;
	}
	fclose(fp);

	return 0;
}