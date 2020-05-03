/*Code by Dmitry Khovratovich, 2016
CC0 license
*/
#include "solver.h"

#include <inttypes.h>
#include <ctime>
#include "string.h"
#include <cstdlib>

using namespace _POW;

static void promptUsage() {
    printf("Usage: [-h] [-n N] [-k K] [-s S]\n");
    printf("Parameters:\n");
    printf("\t-h  \t\tPrint this help message\n");
    printf("\t-n N\t\tTuple length of iterations to N\n");
    printf("\t-k K\t\tNumber of steps to K perform\n");
    printf("\t-s S\t\tSeed, can be interpreted as input\n");
    printf("\t-v  \t\tExtended output\n");
}

int main(int argc, char *argv[]) {
    char *err = new char[256];
    int* errlen = new int(0);
    if (argc > 1 && !strcmp(argv[1], "-h")) {
        promptUsage();
        exit(1);
    }

    Input in = inputFromArgs(argc, argv, errlen, err);


    if (*errlen > 0) {
        printf("%s\n", err);
        promptUsage();
        exit(1);
    }
    in.printToConsole();

    Solver solver(in);
    Proof p = solver.FindProof();
    p.ValidateProof();

    return 0;
}
