#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include <simplecrc.h>


DECLARE_CRC(IBM_3740, 0x1021, 0xFFFF, false, false, 0x0, 0x0, 0x29b1, 16);
DECLARE_CRC(IBM_3740_FAKE, 0x1021, 0x0000, false, false, 0x0, 0x0, 0x29b1, 16);

uint64_t x2n_table[32] = {0};

/* Compute a(x) * b(x) mod G(x) */
uint16_t multmodp(uint16_t a, uint16_t b, uint16_t  g) {
    uint16_t m, p;
    printf("multmodp 0x%.4X 0x%.4X 0x%.4X\n", a, b, g);
    m = (uint16_t)1 << 15;
    p = 0;
    for(;;) {
        if(a & m) {
            p ^= b;
            if  ((a & (m - 1)) == 0)
                break;
        }

        m >>= 1;

        b = (b & 1) ? (b >> 1) ^ g : (b >> 1);
    }
    printf("multmodp return 0x%.4X\n", p);
    return p;
}

uint16_t multmodp2(uint16_t a, uint16_t b) {
    uint16_t m = (uint16_t)1 << 15;
    uint16_t p = 0;
    for (;;) {
        if (a & m) {
            p ^= b;
            if ((a & (m - 1)) == 0)
                break;
        }
        m >>= 1;
        b = b & 1 ? (b >> 1) ^ 0x1021 : b >> 1;
    }
    return p;
}


uint64_t multmodm(uint64_t a, uint64_t b, uint64_t m) {
    uint64_t res = 0;
    printf("multmodm 0x%.4lX 0x%.4lX 0x%.4lX\n", a, b, m);
    while (a != 0) {
        if (a & 1) res = (res + b) % m;
        a >>= 1;
        b = (b << 1) % m;
    }
    printf("multmodp return 0x%.4lX\n", res);
    return res;
}
uint64_t x2nmodp(uint64_t n, unsigned k)
{
    uint64_t p;

    p = (uint64_t)1 << 15;           /* x^0 == 1 */
    while (n) {
        if (n & 1)
            p = multmodp2(x2n_table[k & 31], p);
        n >>= 1;
        k++;
    }
    return p;
}

void init_crc_tables(void) {

    uint64_t p;
    unsigned n;

    /* initialize the x^2^n mod p(x) table */
    p = (uint16_t)1 << 14;         /* x^1 */
    x2n_table[0] = p;
    for (n = 1; n < 32; n++)
        x2n_table[n] = p = multmodp2(p, p);
}

uint64_t combine_crcs_test(uint64_t crc1, uint64_t crc2, size_t len2) {
    return multmodm(x2nmodp(len2, 3), crc1, 0x1021) ^ crc2;
}

void math_test(void)
{
    init_crc_tables();
    char str1[] = {'1', '2', '3', '\0', '\0', '\0'};

    uint64_t crc1 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str1, 3);
    uint64_t crc2 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str1, 6);
    uint64_t len2 = 3;

    printf("Math Test\n");
    printf("\tCRC(2) = 0x%.4lX\n", crc2);
    printf("\tCRC(1) = 0x%.4lX\n", multmodm(x2nmodp(len2, 3), crc1, CRC_IBM_3740_FAKE.poly));
    printf("\tCRC(1) = 0x%.4X\n", multmodp(x2nmodp(len2, 3), crc1, CRC_IBM_3740_FAKE.poly));
    printf("\tCRC(1) = 0x%.4X\n", multmodp2(x2nmodp(len2, 3), crc1));
    printf("------------------\n");

}


void extension_test(void)
{
    char str1[] = {'1', '2', '3', '\0', '\0', '\0'};

    uint64_t crc1 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str1, 3);
    uint64_t crc2 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str1, 4);
    uint64_t crc3 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str1, 5);
    uint64_t crc4 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str1, 6);

    printf("Extension Test\n");
    printf("\tCRC(1) = 0x%.4lX\n", crc1);
    printf("\tCRC(2) = 0x%.4lX\n", crc2);
    printf("\tCRC(3) = 0x%.4lX\n", crc3);
    printf("\tCRC(4) = 0x%.4lX\n", crc4);

    printf("------------------\n");

}

void combine_test(void)
{
    char str1[] = {'1', '2', '3'};
    char str2[] = {'4', '5', '6'};
    char str3[] = {'1', '2', '3', '\0', '\0', '\0'};
    char str4[] = {'1', '2', '3', '4', '5', '6'};

    uint64_t crc1 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str1, sizeof(str1));
    uint64_t crc2 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str2, sizeof(str2));
    uint64_t crc3 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str3, sizeof(str3));
    uint64_t crc4 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str4, sizeof(str4));

    printf("Combine Test\n");
    printf("\tCRC(msg1) = 0x%.4lX\n", crc1);
    printf("\tCRC(msg2) = 0x%.4lX\n", crc2);

    printf("------------------\n");
    printf("\tCRC(full) = 0x%.4lX\n", crc4);
    printf("\tCRC(msg1 + msg2) = 0x%.4lX\n", crc3 ^ crc2);
    printf("\tCombined CRC(msg1 + msg2) = 0x%.4lX\n", (uint64_t)combine_crcs_test(crc1, crc2, 3));

}

void remove_init(void)
{
    char str1[] = {'1', '2', '3'};
    char str2[] = {'\0', '\0', '\0'};

    uint64_t crc1 = compute_crc(CRC_IBM_3740, (const unsigned char *)&str1, sizeof(str1));
    uint64_t crc2 = compute_crc(CRC_IBM_3740, (const unsigned char *)&str2, sizeof(str2));
    uint64_t crc3 = compute_crc(CRC_IBM_3740_FAKE, (const unsigned char *)&str1, sizeof(str1));

    /* In order to split the init value out of the CRC, we treat it as a
     * combination problem
     * CRC([ init ] + [ msg ])
     * However, we know CRC([ init ]) already, it's our init value!
     * Therefore we can find CRC([ init ] + Zeros(msg len)) = crc2
     * Since we also have CRC([ init ] + [ msg ]) and the
     *   CRC function is linear, we can "subtract out" crc2 from crc1 to get the
     *   value we wanted, which is equal to crc3. Since CRC math is taking place
     *   over GF(2) where addition and subtraction are both equivalent to xor,
     *   we can do this with a single ^
     */
    printf("Init Test\n");
    printf("\tCRC(msg1) = 0x%.4lX\n", crc1);
    printf("\tCRC(msg2) = 0x%.4lX\n", crc2);
    printf("\tCRC(msg1 fake) = 0x%.4lX\n", crc3);
    printf("\tCRC(msg1 no init) = 0x%.4lX\n", crc1 ^ crc2);
}
int main(int argc, char **argv)
{
    math_test();
    extension_test();
    combine_test();
    remove_init();
    return 0;
}
