/* 
 * File:   main.cpp
 * Author: user1
 *
 * Created on December 20, 2017, 11:31 PM
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <utility>
#include <stdint.h>
#include <deque>

void solve_naive() {
    uint32_t n, m;
    std::cin >> n >> m;

    std::vector<uint32_t> a(n);
    std::vector<uint64_t> d(m), b(m);
    for (uint32_t i = 0; i < n; ++i) std::cin >> a[i];
    for (uint32_t i = 0; i < m; ++i) std::cin >> d[i] >> b[i];

    std::sort(a.begin(), a.end());

    struct field {
        uint32_t a_;
        uint64_t d_last_;
        uint64_t b_last_;
    };

    std::vector<field> fields;
    for (auto&& elem : a) {
        fields.push_back(field{elem, 0, 0});
    }
    
   
    auto height = [](uint32_t d, const field& f) {
        return f.b_last_ + f.a_ * (d - f.d_last_);
    };
   
    struct {
        uint64_t d_;
        bool operator()(const uint64_t& b, const field & f) {
            return b < f.b_last_ + f.a_ * (d_ - f.d_last_);
        }
    } field_compare;
   
    struct {
        uint64_t d_;
        uint64_t b_;
        uint64_t operator()(const uint64_t& x, field& f) {
            uint64_t cut = (uint64_t) f.b_last_ + f.a_ * (d_ - f.d_last_) - b_;
            f.d_last_ = d_;
            f.b_last_ = b_;
            return x + cut;
        }
    } field_accumulator;
   
    for (int i = 0; i < m; ++i) {
        field_compare.d_ = d[i];
        std::vector<field>::iterator ub = std::upper_bound(fields.begin(), fields.end(), b[i], field_compare);
        int cut = 0;
        if (ub != fields.end()) {
            field_accumulator.d_ = d[i];
            field_accumulator.b_ = b[i];
            cut = std::accumulate(ub, fields.end(), uint64_t(0), field_accumulator);
        }
    }
   
    std::cout << std::flush;
}

void solve_stacked() {
    uint32_t n, m;
    std::cin >> n >> m;
    
    std::vector<uint64_t> a(n);
    for (uint32_t i = 0; i < n; ++i) std::cin >> a[i];

    std::vector<uint64_t> d(m), b(m);
    for (uint32_t i = 0; i < m; ++i) std::cin >> d[i] >> b[i];

    std::sort(a.begin(), a.end());
    
    struct range {
        std::vector<uint64_t>::const_iterator a_begin_;
        std::vector<uint64_t>::const_iterator a_end_;
        uint64_t d_last_;
        uint64_t b_last_;
        uint64_t a_sum_;
    };

    std::vector<range> ranges;
   
    ranges.push_back({ a.begin(), a.end(), 0, 0, std::accumulate(a.begin(), a.end(), uint64_t(0)) });
   
    auto height = [](uint64_t a, uint64_t d_last, uint64_t d_current, uint64_t b_last) {
        return b_last + a * (d_current - d_last);
    };
    
    auto range_need_any_cut = [&](uint64_t d_current, uint64_t b_current, const range& r) {
        auto back_iter = r.a_end_ - 1;
        return b_current < height(*back_iter, r.d_last_, d_current, r.b_last_);
    };
   
    auto range_need_full_cut = [&](uint64_t d_current, uint64_t b_current, const range& r) {
       return b_current < height(*r.a_begin_, r.d_last_, d_current, r.b_last_);
    };
   
    auto find_split = [&](uint64_t d_current, uint64_t b_current, const range& r) {
        return std::upper_bound(r.a_begin_, r.a_end_, b_current, [&](uint64_t x, uint64_t y) {
            uint64_t h = height(y, r.d_last_, d_current, r.b_last_);
            return x < h;
        });
    };
   
    auto range_cut = [&](uint64_t d_current, uint64_t b_current, const range& r) {
        uint64_t span = (uint64_t) (r.a_end_ - r.a_begin_);
        uint64_t sum = ((d_current - r.d_last_) * r.a_sum_) + ((r.b_last_ - b_current) * span);
        return sum;
    };
    
    for (int j = 0; j < m; ++j) {
        uint64_t d_current = d[j];
        uint64_t b_current = b[j];
        uint64_t a_sum_to_push = 0;
        uint64_t cut = 0;
        for (;;) {
            range r_current = ranges.back();
            if (!range_need_any_cut(d_current, b_current, r_current)) {
                if (a_sum_to_push != 0) {
                    ranges.push_back({ r_current.a_end_, a.end(), d_current, b_current, a_sum_to_push });
                }
                break;
            } else if (range_need_full_cut(d_current, b_current, r_current)) {
                ranges.pop_back();
                cut += range_cut(d_current, b_current, r_current);
                a_sum_to_push += r_current.a_sum_;
                if (ranges.empty()) {
                    ranges.push_back({r_current.a_begin_, a.end(), d_current, b_current, a_sum_to_push });
                    break;
                }
            } else { // if (range_need_split(d_current, b_current, r_current)) {
                ranges.pop_back();
                auto a_split_end = find_split(d_current, b_current, r_current);
                auto a_split_sum = std::accumulate(r_current.a_begin_, a_split_end, uint64_t(0));
                auto a_rest_sum = r_current.a_sum_ - a_split_sum;
                a_sum_to_push += a_rest_sum;
                
                cut += range_cut(d_current, b_current, { a_split_end, r_current.a_end_, r_current.d_last_, r_current.b_last_, a_rest_sum });
                ranges.push_back({ r_current.a_begin_, a_split_end, r_current.d_last_, r_current.b_last_, a_split_sum });
                ranges.push_back({ a_split_end, a.end(), d_current, b_current, a_sum_to_push });
                break;
            }
        }
                
        std::cout << cut << '\n';
    }
}

void solve_memoized() {
    uint32_t n, m;
    std::cin >> n >> m;
    
    std::vector<uint64_t> a(n);
    for (uint32_t i = 0; i < n; ++i) std::cin >> a[i];

    std::vector<uint64_t> d(m), b(m);
    for (uint32_t i = 0; i < m; ++i) std::cin >> d[i] >> b[i];

    std::sort(a.begin(), a.end());
    
    std::vector<uint64_t> sums = a;
    uint64_t sum_acc = sums[n-1];
    std::for_each(sums.rbegin() + 1, sums.rend(), [&](uint64_t& sum) {
        sum += sum_acc;
        sum_acc = sum;
    });
    
    struct range {
        std::vector<uint64_t>::iterator a_begin_;
        std::vector<uint64_t>::iterator a_end_;
        uint64_t d_last_;
        uint64_t b_last_;
        uint64_t a_sum_;
    };

    std::vector<range> ranges;
   
    ranges.push_back({ a.begin(), a.end(), 0, 0, sums[0] });
   
    auto height = [](uint64_t a, uint64_t d_last, uint64_t d_current, uint64_t b_last) {
        return b_last + a * (d_current - d_last);
    };
    
    auto range_need_any_cut = [&](uint64_t d_current, uint64_t b_current, const range& r) {
        auto back_iter = r.a_end_ - 1;
        return b_current < height(*back_iter, r.d_last_, d_current, r.b_last_);
    };
   
    auto range_need_full_cut = [&](uint64_t d_current, uint64_t b_current, const range& r) {
       return b_current < height(*r.a_begin_, r.d_last_, d_current, r.b_last_);
    };
   
    auto find_split = [&](uint64_t d_current, uint64_t b_current, const range& r) {
        return std::upper_bound(r.a_begin_, r.a_end_, b_current, [&](uint64_t x, uint64_t y) {
            return x < height(y, r.d_last_, d_current, r.b_last_);
        });
    };
   
    auto range_cut = [&](uint64_t d_current, uint64_t b_current, const range& r) {
        auto span = (uint64_t) (r.a_end_ - r.a_begin_);
        return ((d_current - r.d_last_) * r.a_sum_) + ((r.b_last_ - b_current) * span);
    };
    
    for (int j = 0; j < m; ++j) {
        auto d_current = d[j];
        auto b_current = b[j];
        auto cut = (uint64_t) 0;
        for (;;) {
            range r_current = ranges.back();
            if (!range_need_any_cut(d_current, b_current, r_current)) {
                if (r_current.a_end_ != a.end()) {
                    auto sum_to_push = sums[r_current.a_end_ - a.begin()];
                    ranges.push_back({ r_current.a_end_, a.end(), d_current, b_current, sum_to_push });
                }
                break;
            } else if (range_need_full_cut(d_current, b_current, r_current)) {
                ranges.pop_back();
                cut += range_cut(d_current, b_current, r_current);
                if (ranges.empty()) {
                    auto sum_to_push = sums[r_current.a_begin_ - a.begin()];
                    ranges.push_back({r_current.a_begin_, a.end(), d_current, b_current, sum_to_push });
                    break;
                }
            } else {
                ranges.pop_back();
                auto split_point = find_split(d_current, b_current, r_current);
                auto sum_to_push = sums[split_point - a.begin()];
                auto ante_split_sum = sums[r_current.a_begin_ - a.begin()] - sum_to_push;
                auto post_split_sum = r_current.a_sum_ - ante_split_sum;
                cut += range_cut(d_current, b_current, { split_point, r_current.a_end_, r_current.d_last_, r_current.b_last_, post_split_sum });
                ranges.push_back({ r_current.a_begin_, split_point, r_current.d_last_, r_current.b_last_, ante_split_sum });
                ranges.push_back({ split_point, a.end(), d_current, b_current, sum_to_push });
                break;
            }
        }
                
        std::cout << cut << '\n';
    }
}

int main(int argc, char**argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl;
        return 1;
    }
    std::string in_file(argv[1]);
    std::string out_file(in_file.substr(0, in_file.find_last_of('.')) + ".out");

    std::ifstream in(in_file);
    std::streambuf *cinbuf = std::cin.rdbuf();
    std::cin.rdbuf(in.rdbuf());
   
    std::ofstream out(out_file);
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());
   
//    solve_naive();
//    solve_stacked();
    solve_memoized();
    
    std::cin.rdbuf(cinbuf); //reset to standard input again
    std::cout.rdbuf(coutbuf); //reset to standard output again

    return 0;
}


