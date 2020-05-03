#include <cstdint>
#include <vector>

#ifndef EQUIHASH_INPUT_H
#define EQUIHASH_INPUT_H

using namespace std;

namespace _POW {
    const int LIST_LENGTH = 5;
    const int MAX_N_BYTES = 8; // Максимальна довжина n в байтах
    const int MAX_K_BYTES = 5; // Максимальна довжина k в байтах
    const int MAX_SEED_BYTES = 63; // Максимальна довжина seed в байтах
    const int SEED_LENGTH=4; // Довжина seed (кількість елементів);
    const int tupleStructMultiplyer = 8;

    // визначення типів Nonce та In teger
    typedef uint32_t Nonce;
    typedef uint32_t Integer;

    // визначення класу Seed, який представляє вхідні значення алгоритму
    class Seed{
        std::vector<Integer> v;
    public:
        Seed(){
            v.resize(SEED_LENGTH,0);
        }
        explicit Seed(Integer x){
            v.resize(SEED_LENGTH, x);
        }
        Seed(const Seed&r){
            v= r.v;
        }
        Seed& operator=(const Seed&r){
            v = r.v;
            return *this;
        }
        const Integer& operator[](unsigned i) const{ return v[i]; }
    };

    // визначення класу Input, який зберігає в собі вхідні дані програми
    struct Input {
        const unsigned n;
        const unsigned k;
        const Seed seed;
        const bool verbose;

        Input(unsigned n_v, unsigned k_v, Seed I_v, bool v): n(n_v), k(k_v), seed(I_v), verbose(v){};
        Input():n(0),k(1),seed(0), verbose(false) {};

        void printToConsole();
        int32_t calculateMemoryKB();
        void makeInputArray(Integer *input);
    };


    Input inputFromArgs(int argc, char *argv[], int *errlen, char *err);
}

#endif //EQUIHASH_INPUT_H
