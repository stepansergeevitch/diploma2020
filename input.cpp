#include "input.h"
#include "string.h"
#include <inttypes.h>
#include "stdio.h"
#include "solver.h"
#include <cstdlib>


using namespace _POW;

int32_t Input::calculateMemoryKB() {
    return ((((uint64_t)1) << (n / (k + 1))) * LIST_LENGTH * k * tupleStructMultiplyer * sizeof(Integer)) / (1UL << 10);
}

void Input::printToConsole() {
    printf("Input arguments:\n");
    printf("\tn = %" PRIu32 " \n", n);
    printf("\tk = %" PRIu32 " \n", k);
    printf("\tseed = [");
    for (unsigned i = 0; i < SEED_LENGTH; ++i) {
        printf("%" PRIu32 , seed[i]);
        if (i != SEED_LENGTH - 1) {
            printf(", ");
        }
    }
    printf("]\n");
    printf("\tRequired memory: %.2fMB\n", calculateMemoryKB() / 1024.);
}

void Input::makeInputArray(Integer *input) {
    for (unsigned i = 0; i < SEED_LENGTH; ++i)
        input[i] = seed[i];
    input[SEED_LENGTH + 1] = 0;
}



Input _POW::inputFromArgs(int argc, char *argv[], int *errlen, char *err) {
    Integer n = 0, k = 1;
    Seed seed;
    bool verbose = false;
    *errlen = 0;
    if (argc < 2) {
        strcpy(err, "not enough arguments, should be at least 2");
        return Input();
    }
    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];
        unsigned long input = 0;
        if (!strcmp(a, "-n")) {
            if (i < argc - 1) {
                i++;
                input = strtoul(argv[i], NULL, 10);
                if (input == 0 || input >= 1UL << MAX_N_BYTES) {
                    const char errMsg[] = "bad numeric input for -n";
                    strcpy(err, errMsg);
                    *errlen = strlen(errMsg);
                    return Input();
                }
                n = input;
                continue;
            }
            else {
                const char errMsg[] = "missing -n argument";
                strcpy(err, errMsg);
                *errlen = strlen(errMsg);
                return Input();
            }
        }
        else if (!strcmp(a, "-k")) {
            if (i < argc - 1) {
                i++;
                input = strtoul(argv[i], NULL, 10);
                if (input == 0 || input > 1UL << MAX_K_BYTES) {
                    const char errMsg[] = "bad numeric input for -n";
                    strcpy(err, errMsg);
                    *errlen = strlen(errMsg);
                    return Input();
                }
                k = input;
                continue;
            }
            else {
                const char errMsg[] = "missing -k argument";
                strcpy(err, errMsg);
                *errlen = strlen(errMsg);
                return Input();
            }
        }
        else if (!strcmp(a, "-s")) {
            if (i < argc - 1) {
                i++;
                input = strtoul(argv[i], NULL, 16);
                if (input == 0 || input > 1ULL << MAX_SEED_BYTES) {
                    const char errMsg[] = "bad numeric input for -s";
                    strcpy(err, errMsg);
                    *errlen = strlen(errMsg);
                    return Input();
                }
                seed = Seed(input);
                continue;
            }
            else {
                const char errMsg[] = "missing -s argument";
                strcpy(err, errMsg);
                *errlen = strlen(errMsg);
                return Input();
            }
        } else if (!strcmp(a, "-v")) {
            verbose = true;
        } else {
            const char errMsg[] = "invalid argument";
            strcpy(err, errMsg);
            *errlen = strlen(errMsg);
            return Input();
        }
    }

    if (n / (k + 1) >= 64) {
        const char errMsg[] = "n / (k + 1) should be lower than 64";
        strcpy(err, errMsg);
        *errlen = strlen(errMsg);
        return Input();
    }

    if (n / (k + 1) >= 64) {
        const char errMsg[] = "n / (k + 1) should be lower than 64";
        strcpy(err, errMsg);
        *errlen = strlen(errMsg);
        return Input();
    }

    Input i(n, k, seed, verbose);
    int32_t memoryKB = i.calculateMemoryKB();
    if(memoryKB > 4 << 20) {
        sprintf(err, "max memory limit exceeded, max allowed if 4GB, got %f", memoryKB / (1024. * 1024.));
        *errlen = 1;
        return Input();
    }
    return i;
}

