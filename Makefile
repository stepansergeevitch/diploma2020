all:
	g++ -m64 -maes -mavx -O3 -std=c++11 "equihash.cpp" "main.cpp" "./blake/blake2b.cpp" "proof.cpp"\
	 "utils.cpp" "proof.h" -o equihash


