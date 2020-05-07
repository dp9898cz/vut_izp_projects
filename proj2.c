/*
 * proj2 "Iteracni vypocty"
 * Program na vypocet prirozeneho logaritmu a exponencialni funkce
 * autor: Daniel Patek, xpatek08@fit.vutbr.cz
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <math.h>

#define CUSTOM_LP 0 // zmenit na 1 pro zobrazeni premioveho reseni
#define EPS 0.00000001 // presnost premioveho reseni

/*
 * Prototypy funkci:
 */
double taylor_log(double x, unsigned int n);
double taylor_pow(double x, double y, unsigned int n);
double cfrac_log(double x, unsigned int n);
double taylorcf_pow(double x, double y, unsigned int n);
double argv_2_double(const char *argv, int *error);
unsigned int argv_2_int(const char *argv, int *error);
int check_argvs(char *argv[], int *error);
int check_argc(int argc, char *argv);
void print_log(double x, unsigned int ite);
void print_pow(double x, double y, unsigned int ite);
void print_help(void);
double mylog(double x);
double mypow(double x, double y);

/*
 * funkce main:
 *      návratová hodnota zavisí na návratové hodnotě funkce check_argvs, check_argc
 */
int main(int argc, char *argv[]){
    //deklarace promennych
    int error = 0, res_a, res_b;

    // prvni kontrola argc
    if ((res_a = check_argc(argc, argv[1])) == -1){
        print_help();
        return EXIT_FAILURE;
    }

    //druha kontrola argvs
    if ((res_b = check_argvs(argv, &error)) == -1){
        print_help();
        return EXIT_FAILURE;
    }

    //pokud uzivatel neco zadal spatne
    if (error){
        print_help();
        return EXIT_FAILURE;

    // vytiskne hodnoty log
    } else if (res_a == 0 && res_b == 0){
        print_log(argv_2_double(argv[2], &error), argv_2_int(argv[3], &error));
        return EXIT_SUCCESS;

    // vytiskne hodnoty pow
    } else {
        print_pow(argv_2_double(argv[2], &error), argv_2_double(argv[3], &error), argv_2_int(argv[4], &error));
        return EXIT_SUCCESS;
    }
}

/*
 * funkce check_argc
 *      parametr int argc - pocet argumentu
 *      parametr char *argv - prvni argument (pro rozliseni --log a --pow
 *      navratova hodnota - 0 v pripade spravneho chodu log, 1 v pripade spravneho chodu pow nebo -1 v pripade chyby
 */
int check_argc(int argc, char *argv){
    if (argc <= 0 || argc == 1){
        fprintf(stderr, "Nezadali jste zadny argument. Postupujte podle napovedy.\n");
        return -1;
    } else if (argc < 4){
        fprintf(stderr, "Zadali jste prilis malo argumentu. Postupujte podle napovedy.\n");
        return -1;
    } else if (argc == 4 && strcmp(argv, "--log") == 0){
        return 0;
    } else if (argc == 5 && strcmp(argv, "--pow") == 0){
        return 1;
    } else if (argc > 5) {
        fprintf(stderr, "Zadali jste prilis mnoho argumentu. Postupujte podle napovedy.\n");
        return -1;
    } else {
        fprintf(stderr, "Zadali jste prilis malo argumentu. Postupujte podle napovedy.\n");
        return -1;
    }
}

/*
 * funkce check_argvs
 *      parametr char *argv[] - jednotlive argumenty (pro rozliseni --log a --pow) a kontrolu ostatnich argumentu
 *      paramentr int *error - pouziva se v pripade neocekavaneho vstupu pro indikaci napovedy
 *      navratova hodnota - 0 v pripade spravneho chodu log, 1 v pripade spravneho chodu pow nebo -1 v pripade chyby
 */
int check_argvs(char *argv[], int *error){
    if (strcmp(argv[1], "--log") == 0) {
        argv_2_double(argv[2], error);
        argv_2_int(argv[3], error);
        return 0;
    }
    else if (strcmp(argv[1], "--pow") == 0) {
        argv_2_double(argv[2], error);
        argv_2_double(argv[3], error);
        argv_2_int(argv[4], error);
        return 1;
    }
    else {
        fprintf(stderr, "Nespravny argument: %s\nPostupujte podle napovedy.\n", argv[1]);
        return -1;
    }
}

/*
 * funkce argv_2_double
 *      parametr const char *argv - string (jeden z argumentu programu), ktery chceme prevest na typ double
 *      paramentr int *error - pouziva se v pripade neocekavaneho vstupu pro indikaci napovedy
 *      navratova hodnota - prevedene cislo typu double
 */
double argv_2_double(const char *argv, int *error) {
    if (strcmp(argv, "inf") == 0 || strcmp(argv, "INF") == 0)
        return INFINITY;
    if (strcmp(argv, "-inf") == 0 || strcmp(argv, "-INF") == 0)
        return -INFINITY;
    if (strcmp(argv, "nan") == 0 || strcmp(argv, "NAN") == 0)
        return NAN;
    char *end = NULL;
    double x = strtod(argv, &end);
    if (*end) {
        fprintf(stderr, "Nespravny argument: %s\nPostupujte podle napovedy.\n", argv);
        *error = 1;
    }
    return x;
}

/*
 * funkce argv_2_int
 *      parametr const char *argv - string (jeden z argumentu programu), ktery chceme prevest na typ int
 *      paramentr int *error - pouziva se v pripade neocekavaneho vstupu pro indikaci napovedy
 *      navratova hodnota - prevedene cislo typu int (vetsi nez nula)
 */
unsigned int argv_2_int(const char *argv, int *error) {
    unsigned int x;
    char *end = NULL;
    x = strtoul(argv, &end, 10);
    if ((argv[0] == '-') || (x <= 0) || *end) {
        fprintf(stderr, "Nespravny argument: %s\nPostupujte podle napovedy.\n", argv);
        *error = 1;
    }
    return x;
}

/*
 * funkce cfrac_log
 *      parametr double x - cislo, z ktereho pocitame logaritmus
 *      parametr unsigned int n - pocet iteraci
 *      navratova hodnota - vypocitany logaritmus typu double
 */
double cfrac_log(double x, unsigned int n) {
    if (x == 0)
        return -INFINITY;
    if (x < 0)
        return NAN;
    if (x == INFINITY)
        return INFINITY;
    if (x == -INFINITY)
        return NAN;
    if (isnan(x))
        return NAN;
    double down;
    double num = ((2 * n) - 1), result = num;
    unsigned int i = n;
    x = (x - 1) / (x + 1);
    while (i > 1){
        i--;
        num -= 2;
        down = (i * i * x * x) / result;
        result = num - down;
    }
    return (2 * x) / result;
}

/*
 * funkce taylor_log
 *      parametr double x - cislo, z ktereho pocitame logaritmus
 *      parametr unsigned int n - pocet iteraci
 *      navratova hodnota - vypocitany logaritmus typu double
 */
double taylor_log(double x, unsigned int n) {
    if (x == 0)
        return -INFINITY;
    if (x < 0)
        return NAN;
    if (x == INFINITY)
        return INFINITY;
    if (x == -INFINITY)
        return NAN;
    if (isnan(x))
        return NAN;
    double result = 0, up = 1;
    if (x < 1) {
        x = 1 - x;
        result = 0;
        for (unsigned int i = 1; i <= n; i++) {
            up = up * x;
            result -= up / i;
        }
    }
    else if (x >= 1){
        for (unsigned int i = 1; i <= n; i++) {
            up = up * ((x - 1) / x);
            result += (up / i);
        }
    }
    return result;
}

/*
 * funkce taylor_pow
 *      parametr double x - cislo, z ktereho pocitame exponencialni funkci
 *      parametr double y - exponent
 *      parametr unsigned int n - pocet iteraci
 *      navratova hodnota - vypocitana exponencialni funkce typu double
 */
double taylor_pow(double x, double y, unsigned int n){
    if (x == 0)
        return 0;
    if (x == 1) {
        if (y == -INFINITY)
            return NAN;
        return 1;
    }
    if (y == 0)
        return 1;
    if (isnan(x) || isnan(y))
        return NAN;
    if (y == INFINITY){
        if (fabs(x) < 1)
            return 0;
        return INFINITY;
    }
    if (y == -INFINITY) {
        if (fabs(x) < 1)
            return INFINITY;
        return 0;
    }
    if (x == INFINITY) {
        if (y < 0)
            return 0;
        return INFINITY;
    }
    if (x == -INFINITY && y < 0)
        return 0;
    if (x == -INFINITY && y >= 1)
        return -INFINITY;
    double result = 1;
    int y_int = (int)y;
    double up = 1;
    double down = 1, taylor = taylor_log(fabs(x), n);
    for (unsigned int i = 1; i <= n - 1; i++) {
        up *= y * taylor;
        down *= i;
        if (isnan(up / down))
            break;
        result += (up / down);
    }
    if (fabs(x) != x && (y_int % 2) != 0 && y_int == y)
        return -result;
    if (fabs(x) != x && y_int != y)
        return -NAN;
    return result;
}

/*
 * funkce taylorcf_pow
 *      parametr double x - cislo, z ktereho pocitame exponencialni funkci
 *      parametr double y - exponent
 *      parametr unsigned int n - pocet iteraci
 *      navratova hodnota - vypocitana exponencialni funkce typu double
 */
double taylorcf_pow(double x, double y, unsigned int n){
    if (x == 0)
        return 0;
    if (x == 1) {
        if (y == -INFINITY)
            return NAN;
        return 1;
    }
    if (y == 0)
        return 1;
    if (isnan(x) || isnan(y))
        return NAN;
    if (y == INFINITY){
        if (fabs(x) < 1)
            return 0;
        return INFINITY;
    }
    if (y == -INFINITY) {
        if (fabs(x) < 1)
            return INFINITY;
        return 0;
    }
    if (x == INFINITY) {
        if (y < 0)
            return 0;
        return INFINITY;
    }
    if (x == -INFINITY && y < 0)
        return 0;
    if (x == -INFINITY && y >= 1)
        return -INFINITY;
    double result = 1, cfrac = cfrac_log(fabs(x), n);
    long double up = 1;
    long double down = 1;
    int y_int = (int)y;
    for (unsigned int i = 1; i <= n - 1; i++) {
        up *= y * cfrac;
        down *= i;
        if (isnan(up / down))
            break;
        result += (up / down);
    }
    if (fabs(x) != x && (y_int % 2) != 0 && y_int == y)
        return -result;
    if (fabs(x) != x && y_int != y)
        return -NAN;
    return result;
}

/*
 * funkce print_log
 *      -vypisuje vysledky log
 */
void print_log(double x, unsigned int ite){
    printf("       log(%g) = %.12g\n", x, log(x));
    printf(" cfrac_log(%g) = %.12g\n", x, cfrac_log(x, ite));
    printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, ite));

    if (CUSTOM_LP){
        printf("     mylog(%g) = %.7e\n", x, mylog(x));
    }
}

/*
 * funkce print_pow
 *      -vypisuje vysledky pow
 */
void print_pow(double x, double y, unsigned int ite){
    printf("         pow(%g,%g) = %.12g\n", x, y, pow(x, y));
    printf("  taylor_pow(%g,%g) = %.12g\n", x, y, taylor_pow(x, y, ite));
    printf("taylorcf_pow(%g,%g) = %.12g\n", x, y, taylorcf_pow(x, y, ite));

    if (CUSTOM_LP){
        printf("     mypow(%g) = %.7e\n", x, mypow(x, y));
    }
}

/*
 * funkce print_help
 *      -vypisuje pomocnou zpravu
 */
void print_help(void) {
    printf("Pouziti programu:\n");
    printf("     proj2 --log X N (vypocita logaritmus s prirozenym zakladem z cisla X v N iteracich)\n");
    printf("          plati: X > 0; N > 0 z mnoziny prirozenych cisel\n");
    printf("     proj2 --pow X Y N (vypocita exponencialni funkci cisla Y se zakladem X v N iteracich)\n");
    printf("          plati: X > 0; Y > 0; N > 0 z mnoziny prirozenych cisel\n");
}

double mylog(double x) {
    double taylor = 0, taylor_last, cfrac = 0, cfrac_last, t_abs, c_abs;
    unsigned int n = 1;
    do {
        taylor_last = taylor;
        cfrac_last = cfrac;
        taylor = taylor_log(x, n);
        cfrac = cfrac_log(x, n);
        t_abs = fabs(taylor - taylor_last);
        c_abs = fabs(cfrac - cfrac_last);
        n++;
    } while (t_abs > EPS && c_abs > EPS);

    if (t_abs <= EPS) {
        printf("    (taylor__%d. iterace)\n", n);
        return taylor;
    } else {
        printf("    (cfrac__%d. iterace)\n", n);
        return cfrac;
    }
}

double mypow(double x, double y) {
    double taylor = 0, taylor_last, cfrac = 0, cfrac_last, t_abs, c_abs;
    unsigned int n = 1;
    do {
        taylor_last = taylor;
        cfrac_last = cfrac;
        taylor = taylor_pow(x, y, n);
        cfrac = taylorcf_pow(x, y, n);
        t_abs = fabs(taylor - taylor_last);
        c_abs = fabs(cfrac - cfrac_last);
        n++;
    } while (t_abs > EPS && c_abs > EPS);

    if (t_abs <= EPS) {
        printf("    (taylor__%d. iterace)\n", n);
        return taylor;
    } else {
        printf("    (cfrac__%d. iterace)\n", n);
        return cfrac;
    }
}
