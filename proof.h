#include "input.h"
#include <cstdint>
#include <vector>
#include <cstdio>

#ifndef EQUIHASH_PROOF_H
#define EQUIHASH_PROOF_H
namespace _POW {
    // визначення класу розв'язку
    struct Proof{
        Input input;
        const Nonce nonce;
        const std::vector<Integer> inputs;

        Proof(Input in, Nonce V_v, std::vector<Integer> inputs_v):
                input(in), nonce(V_v), inputs(std::move(inputs_v)){};
        Proof():input(Input()),nonce(0),inputs(std::vector<Integer>()) {};

        std::vector<Integer> calculateIntegerXORBlocks();
        void printSuccessfulProof();
        bool ValidateProof();
    };
}
#endif
