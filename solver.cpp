/*Code by Dmitry Khovratovich, 2016
CC0 license
*/

#include "blake/blake2.h"
#include "solver.h"
#include "utils.cpp"
#include <ctime>
#include <random>
#include <algorithm>


using namespace _POW;
using namespace std;

void Solver::AllocateMemory()
{
    Integer tuple_n = ((int64_t)1) << (input.n / (input.k + 1));

    Tuple default_tuple(input.k);
    std::vector<Tuple> def_tuples(LIST_LENGTH, default_tuple);

    tupleList = std::vector<std::vector<Tuple>>(tuple_n, def_tuples);
    filledList= std::vector<unsigned>(tuple_n, 0);

    solutions.resize(0);
    forks.resize(0);
}

void Solver::FillMemory(Integer length)
{
    Integer inArr[SEED_LENGTH + 2];
    input.makeInputArray(inArr);
    inArr[SEED_LENGTH] = (nonce + shift) % MAX_NONCE;

    Integer buf[MAX_N_BYTES / 4];
    Integer blocksCount = input.n / (input.k + 1);
    for (unsigned i = 0; i < length; ++i, ++inArr[SEED_LENGTH + 1]) {
        blake2b((uint8_t*)buf, &inArr, NULL, sizeof(buf), sizeof(inArr), 0);
        Integer index = buf[0] >> (32 - blocksCount);

        unsigned count = filledList[index];
        if (count < LIST_LENGTH) {
            for (unsigned j = 1; j < (input.k + 1); ++j) {
                tupleList[index][count].blocks[j - 1] = buf[j] >> (32 - blocksCount);
            }
            tupleList[index][count].reference = i;
            filledList[index]++;
        }
    }
}

void Solver::InitMemory(Integer length) {
    auto fill_start = rdtsc();
    AllocateMemory();
    FillMemory(length);

    auto fill_end = rdtsc();
    if(input.verbose) {
        printf("\tMemory initialization took: %2.2f Mcycles \n", (double) (fill_end - fill_start) / (1UL << 20));
    }
}

std::vector<Integer> Solver::ResolveTreeByLevel(Fork fork, unsigned level) {
    if (level == 0)
        return std::vector<Integer>{fork.ref1, fork.ref2};
    auto v1 = ResolveTreeByLevel(forks[level - 1][fork.ref1], level - 1);
    auto v2 = ResolveTreeByLevel(forks[level - 1][fork.ref2], level - 1);
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}

std::vector<Integer> Solver::ResolveTree(Fork fork) {
    return ResolveTreeByLevel(fork, forks.size());
}


void Solver::ResolveCollisions(bool lastStep) {
    const unsigned tableLength = tupleList.size();  // кількість рядків хеш-таблиці
    const unsigned maxNewCollisions = tupleList.size() * 3;  // максимально можлива кількість колізій
    const unsigned newBlocks = tupleList[0][0].blocks.size() - 1;// кількість блоків у майбутніх колізіях

    std::vector<Fork> newForks(maxNewCollisions); // массив форків, які будуть створені на цьому кроці
    auto tableRow = vector<Tuple>(LIST_LENGTH, Tuple(newBlocks)); // рядок хеш таблиці
    vector<vector<Tuple>> collisionList(tableLength,tableRow); // список знайдених колізій
    std::vector<unsigned> newFilledList(tableLength,0);  // кількість елементів кожного рядка
    Integer newColls = 0; // кількість знайдених колізій

    for (unsigned i = 0; i < tableLength; ++i) {        
        for (unsigned j = 0; j < filledList[i]; ++j)        {
            for (unsigned m = j + 1; m < filledList[i]; ++m) {
                Integer newIndex = tupleList[i][j].blocks[0] ^ tupleList[i][m].blocks[0];
                Fork newFork = Fork(tupleList[i][j].reference, tupleList[i][m].reference);

                if (lastStep) {
                    if (newIndex == 0) { // знайдено розв'язок
                        std::vector<Integer> solution_inputs = ResolveTree(newFork);
                        solutions.emplace_back(input, (nonce + shift) % MAX_NONCE, solution_inputs);
                    }
                }
                else {
                    if (newFilledList[newIndex] < LIST_LENGTH && newColls < maxNewCollisions) {
                        for (unsigned l = 0; l < newBlocks; ++l) {
                            collisionList[newIndex][newFilledList[newIndex]].blocks[l] 
                                = tupleList[i][j].blocks[l+1] ^ tupleList[i][m].blocks[l+1];
                        }
                        newForks[newColls] = newFork;
                        collisionList[newIndex][newFilledList[newIndex]].reference = newColls;
                        newFilledList[newIndex]++;
                        newColls++;
                    }
                }
            }
        }
    }

    forks.push_back(newForks);
    std::swap(tupleList, collisionList);
    std::swap(filledList, newFilledList);
}

unsigned getRandomShift() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<unsigned> dist(0, MAX_NONCE - 1);
    return dist(mt);
}

Proof Solver::FindProof(){
    nonce = 0;
    shift = getRandomShift();
    unsigned blocksCount = input.n / (input.k + 1);
    printf("Random shift: %d\n", shift);
    while (nonce < MAX_NONCE) {
        if(input.verbose) {
            printf("Testing nonce %d\n", (nonce + shift) % MAX_NONCE);
        }
        InitMemory(4UL << (blocksCount - 1));
        auto start_collisions = rdtsc();

        for (unsigned i = 1; i <= input.k; ++i) {
            auto resolve_start = rdtsc();
            ResolveCollisions(i == input.k);
            auto resolve_end = rdtsc();
            if(input.verbose) {
                printf("\tResolving collisions for block %d: %2.2f Mcycles\n",
                       i, (double) (resolve_end - resolve_start) / (1UL << 20));
            }
        }

        auto stop_cycles = rdtsc();

        double  mcycles_d = (double)(stop_cycles - start_collisions) / (1UL << 20);

        if(input.verbose) {
            printf("\tChecked nonce %d:\n\t\tspent %2.2f Mcycles\n",
                   (nonce + shift) % MAX_NONCE, mcycles_d);
        }

        // Перевірка на дублі
        for (auto & solution : solutions) {
            auto vec = solution.inputs;
            std::sort(vec.begin(), vec.end());
            bool dup = false;
            for (unsigned k = 0; k < vec.size() - 1; ++k) {
                if (vec[k] == vec[k + 1])
                    dup = true;
            }
            if (!dup)
                return solution;
        }
        nonce++;
    }
    return Proof(input, nonce - 1, std::vector<uint32_t>());
}
