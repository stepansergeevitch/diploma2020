/*Code by Dmitry Khovratovich, 2016
CC0 license
*/

#ifndef EQUIHASH_SOLVER_H
#define EQUIHASH_SOLVER_H

#include <cstdint>

#include <utility>
#include <vector>
#include <cstdio>
#include "proof.h"


namespace _POW{
    const int MAX_NONCE = 0xFFFFF; // максимальне значення nonce

	// визначення службового класу Tuple
    class Tuple {
    public:
        std::vector<uint32_t> blocks;
        Integer reference;
        Tuple(unsigned i){ blocks.resize(i); reference = 0;}
        Tuple& operator=(const Tuple &r) {
            blocks = r.blocks;
            reference = r.reference;
            return *this;
        }
    };

    // визначення службового класу Fork
    class Fork {
    public:
        Integer ref1, ref2;
        Fork(): ref1(0), ref2(0){};
        Fork(Integer r1, Integer r2) : ref1(r1), ref2(r2) {};
    };

	// визначення класу алгоритму, який знаходить Proof
    // припускається, що n/(k+1) <=32
	class Solver{
	    Input input;

        std::vector<std::vector<Tuple>> tupleList;
        std::vector<unsigned> filledList;
        std::vector<Proof> solutions;
        std::vector<std::vector<Fork>> forks;

        Nonce nonce;
        Nonce shift;
	public:
	    Solver(Input in): input(std::move(in)), nonce(0) {};
        ~Solver() {};
        void AllocateMemory(); // Виділяє пам'ять для подальшого використання
        void FillMemory(Integer length); // Заповнює пам'ять значеннями хеш функції
        void InitMemory(Integer length); // Виконує повну ініціалізацію пам'яті та вимірює час виконання
        void ResolveCollisions(bool store);
        std::vector<Integer> ResolveTree(Fork fork);
        std::vector<Integer> ResolveTreeByLevel(Fork fork, unsigned level);
        Proof FindProof();
	};
}

#endif //define EQUIHASH_SOLVER_H
