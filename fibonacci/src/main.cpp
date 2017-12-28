/* 
 * File:   newmain.cpp
 * Author: user1
 *
 * Created on December 27, 2017, 11:29 PM
 */

#include <fstream>
#include <iostream>
#include <string>
#include <gmpxx.h>

inline void fibmul(uint64_t* f, uint64_t* g) {
    uint64_t tmp = f[0] * g[0] + f[1] * g[1];
    f[1] = f[0] * g[1] + f[1]*(g[0] + g[1]);
    f[0] = tmp;
}

uint64_t fibonacci(uint64_t n) {
    uint64_t f[] = {1, 0};
    uint64_t g[] = {0, 1};
    while (n > 0) {
        if (n & 1) {
            fibmul(f, g);
            --n;
        } else {
            fibmul(g, g);
            n >>= 1;
        }
    }
    return f[1];
}

inline void gmp_fibmul(mpz_class* f, mpz_class* g) {
    mpz_class tmp = f[0] * g[0] + f[1] * g[1];
    f[1] = f[0] * g[1] + f[1]*(g[0] + g[1]);
    f[0] = tmp;
}

mpz_class gmp_fibonacci(const mpz_class& n) {
    mpz_class f[] = {1, 0};
    mpz_class g[] = {0, 1};
    mpz_class nn(n);
    while (nn > 0) {
        if (nn % 2 == 1) {
            gmp_fibmul(f, g);
            --nn;
        } else {
            gmp_fibmul(g, g);
            nn >>= 1;
        }
    }
    return f[1];
}

void solve(const std::string& str) {
    mpz_class num(str);
    std::cout << gmp_fibonacci(num) % 1000000007 << std::endl;
//    mpz_class num;
//    mpz_fib_ui(num.get_mpz_t(), std::stoul(str));
//    std::cout << num % 1000000007 << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [NUM]" << std::endl;
        return 1;
    }
//    std::string in_file(argv[1]);
//    std::string out_file(in_file.substr(0, in_file.find_last_of('.')) + ".out");
//
//    std::ifstream in(in_file);
//    std::streambuf *cinbuf = std::cin.rdbuf();
//    std::cin.rdbuf(in.rdbuf());
//
//    std::ofstream out(out_file);
//    std::streambuf *coutbuf = std::cout.rdbuf();
//    std::cout.rdbuf(out.rdbuf());

    std::string str(argv[1]);
    solve(str);

//    std::cin.rdbuf(cinbuf); //reset to standard input again
//    std::cout.rdbuf(coutbuf); //reset to standard output again

    return 0;
}

