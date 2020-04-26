#include "solver.h"
#include "blake/blake2.h"

using namespace _POW;

std::vector<Integer> Proof::calculateIntegerXORBlocks() {
    Integer inArr[SEED_LENGTH + 2];
    input.makeInputArray(inArr);
    inArr[SEED_LENGTH] = nonce;

    Integer buf[MAX_N_BYTES / 4];
    std::vector<Integer> blocks(input.k+1,0);

    for (unsigned int i : inputs) {
        inArr[SEED_LENGTH + 1] = i;
        blake2b((uint8_t*)buf, &inArr, NULL, sizeof(buf), sizeof(inArr), 0);
        for (unsigned j = 0; j < ( + 1); ++j) {
            //обирається j-й блок розміру n/(k+1) біт
            blocks[j] ^= buf[j] >> (32 - input.n / (input.k + 1));
        }
    }
    return blocks;
}

void Proof::printSuccessfulProof() {
    printf("Proof found:\n");
    printf("\tProof nonce: %x\n", nonce);
    printf("\tProof inputs:\n\t\t");
    for (unsigned int i = 0; i < inputs.size();++i) {
        printf("%x ", inputs[i]);
        if((i + 1) % 5 == 0) {
            // виводимо рядками по 5 елементів
            printf("\n\t\t");
        }
    }
    printf("\n");
}

bool Proof::ValidateProof()
{
    bool b = true;
    auto blocks = calculateIntegerXORBlocks();

    for (unsigned int j: blocks) {
        b &= (j == 0);
    }

    if (b && !inputs.empty()) {
        printSuccessfulProof();
    }

    return b;
}

