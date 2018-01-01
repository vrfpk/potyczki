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
#include <stdint.h>
#include <iomanip>
#include <string>
#include <vector>
#include <deque>
#include <numeric>
#include <cmath>
#include <complex>

template<typename TH>
void log(const char* data, TH head){
    std::cout << data << "=" << head << std::endl;
}

template<typename TH, typename... TA>
void log(const char* data, TH head, TA... tail){
    while(*data != ',') std::cerr << *data++;
    std::cout << "=" << head << ",";
    log(data+1, tail...);
}

#define LOCAL

#ifdef LOCAL
#define LOG(...) log(#__VA_ARGS__, __VA_ARGS__)
#else
#define LOG(...) (__VA_ARGS__)
#endif

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

inline void fibomul_mod(uint64_t* f, uint64_t* g, uint64_t m) {
    uint64_t tmp = (f[0] * g[0] + f[1] * g[1]) % m;
    f[1] = (f[0] * g[1] + f[1]*(g[0] + g[1])) % m;
    f[0] = tmp;
}

uint64_t fibo_mod(uint64_t n, uint64_t m) {
    uint64_t f[] = {1, 0};
    uint64_t g[] = {0, 1};
    int cnt = 0;
    while (n > 0) {
        ++cnt;
        if (n & 1) {
            fibomul_mod(f, g, m);
            --n;
        } else {
            fibomul_mod(g, g, m);
            n >>= 1;
        }
    }
    return f[1];
}

uint64_t fibo_digits(uint64_t n) {
    if (n == 1)
        return 1;
    long double phi = (1 + std::sqrt(5.0L)) / 2.0L;
    long double d = (n * std::log10(phi)) - ((std::log10(5)) / 2);
    return std::ceil(d);
}

uint64_t get_power(int d) {
    uint64_t ret = 1;
    while (--d >= 0) {
        ret *= 10;
    }
    return ret;
}

uint64_t get_period(int d) {
    switch (d) {
    case 1: return 60;
    case 2: return 300;
    default: return 15 * get_power(d-1);
    }
}

void solve(const std::string& suffix_str) {
    uint64_t suffix_size = suffix_str.size();
    uint64_t suffix = std::stoll(suffix_str);
    
    uint64_t m = 10;
    uint64_t period = get_period(1);
    
    std::deque<uint64_t> deq;
    
    if ((suffix % m) == 0) deq.push_back(0);
    if ((suffix % m) == 1) { deq.push_back(1); deq.push_back(2); }
    for (uint64_t i = 3, a = 1, b = 1; i < period; ++i) {
        uint64_t f = (a + b) % m;
        if (f == (suffix % m)) deq.push_back(i);
        a = b;
        b = f;
    }
    
    for (uint64_t i = 2; i <= suffix_size; ++i) {
        uint64_t prev_d = i - 1;
        uint64_t prev_period = get_period(prev_d);
        m *= 10;
        period = get_period(i);
        
        std::size_t deq_size = deq.size();
        for (uint64_t j = 0; j < deq_size; ++j) {
            uint64_t val = deq.front();
            deq.pop_front();
            for (uint64_t k = 0; k < (period / prev_period); ++k) if (fibo_mod(val + k * prev_period, m) == (suffix % m)) {
                deq.push_back(val + k * prev_period);
            }
        }
        
        LOG(m, deq.size());
        if (deq.empty()) {
            std::cout << "NIE" << std::endl;
            return;
        }
    }
    uint64_t val = deq.front();
    while (fibo_digits(val) < suffix_size) val += period;
    LOG(suffix_str, val);
    LOG(gmp_fibonacci(mpz_class(val)));
//    std::cout
//        << "suffix:   " << suffix_str << '\n'
//        << "solution: " << val << std::endl;
//    std::cout
//        << "F(" << val << "): " << std::flush << gmp_fibonacci(mpz_class(val)) << std::endl;
}

void solve() {
    std::string suffix_str;
    std::cin >> suffix_str;
    solve(suffix_str);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [suffix]" << std::endl;
        return 1;
    }
    
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    
    std::string in_file(argv[1]);
//    std::string out_file(in_file.substr(0, in_file.find_last_of('.')) + ".out");

    std::ifstream in(in_file);
    std::streambuf *cinbuf = std::cin.rdbuf();
    std::cin.rdbuf(in.rdbuf());
   
//    std::ofstream out(out_file);
//    std::streambuf *coutbuf = std::cout.rdbuf();
//    std::cout.rdbuf(out.rdbuf());

    solve();
    
    std::cin.rdbuf(cinbuf); //reset to standard input again
//    std::cout.rdbuf(coutbuf); //reset to standard output again

    return 0;
}

