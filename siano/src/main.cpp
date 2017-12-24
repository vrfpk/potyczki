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
//        uint32_t d_current_ = d[i];
//        uint32_t b_current_ = b[i];
//        uint32_t cut = 0;
//        std::cout << "day: " << d[i] << ", b: " << b[i] << " | before: { ";
//        for (auto&& f : fields) {
//            std::cout << f.b_last_ + f.a_ * (d[i] - f.d_last_) << " ";
//        }
//        std::cout << "} | after: {";
        field_compare.d_ = d[i];
        std::vector<field>::iterator ub = std::upper_bound(fields.begin(), fields.end(), b[i], field_compare);
        int cut = 0;
        if (ub != fields.end()) {
//            std::cout << "b: " << b_current_ << ", ub: " << ub->a_ << std::endl;
            field_accumulator.d_ = d[i];
            field_accumulator.b_ = b[i];
            cut = std::accumulate(ub, fields.end(), uint64_t(0), field_accumulator);
        }
//        for (auto&& f : fields) {
//            std::cout << f.b_last_ + f.a_ * (d[i] - f.d_last_) << " ";
//        }
//        std::cout << "} cut: " << cut << std::endl;
//        std::cout << cut << '\n';
    }
   
    std::cout << std::flush;
}

void solve_complicated() {
    uint32_t n, m;
    std::cin >> n >> m;
    
    std::vector<uint64_t> a(n);
    for (uint32_t i = 0; i < n; ++i) std::cin >> a[i];

    std::vector<uint64_t> d(m), b(m);
    for (uint32_t i = 0; i < m; ++i) std::cin >> d[i] >> b[i];

    std::sort(a.begin(), a.end());
    
//    auto sums = std::accumulate(a.rbegin() + 1, a.rend(), std::deque<uint64_t>{ a.back() }, [](std::deque<uint64_t>& deq, uint64_t x) {
//        deq.push_front(x + deq.front());
//        return deq;
//    });

//    std::vector<int> sums(n);
//    sums[n-1] = a[n-1];
//    for (int i = n-1; i >= 0; --i) {
//        sums[i] = a[i] + sums[i+1];
//    }

    struct range {
        std::vector<uint64_t>::const_iterator a_begin_;
        std::vector<uint64_t>::const_iterator a_end_;
        uint64_t d_last_;
        uint64_t b_last_;
        uint64_t a_sum_;
    };

    std::vector<range> ranges;
   
    ranges.push_back({ a.begin(), a.end(), 0, 0, std::accumulate(a.begin(), a.end(), uint64_t(0)) });
   
//    auto range_print = [](const range& r) {
//        std::stringstream ss;
//        ss << "range { ";
//        for (auto iter = r.a_begin_; iter != r.a_end_; ++iter) {
//            ss << *iter << " ";
//        }
//        ss << "} : " << r.a_sum_;
//        return ss.str();
//    };
   
    auto height = [](uint64_t a, uint64_t d_last, uint64_t d_current, uint64_t b_last) {
        return b_last + a * (d_current - d_last);
    };
    
    auto range_need_any_cut = [&](uint64_t d_current, uint64_t b_current, const range& r) {
        auto back_iter = r.a_end_ - 1;
        return b_current < height(*back_iter, r.d_last_, d_current, r.b_last_);
    };
   
//    auto range_need_split = [&](uint64_t d_current, uint64_t b_current, const range& r) {
//        auto back_iter = r.a_end_ - 1;
//        return height(*r.a_begin_, r.d_last_, d_current, r.b_last_) <= b_current && b_current < height(*back_iter, r.d_last_, d_current, r.b_last_);
//    };
    
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
                
//                auto a_split_end_index = a_split_end - a.begin();
//                auto a_split_begin_index = r_current.a_begin_ - a.begin();
//                a_sum_to_push = sums[a_split_end_index];
//                auto a_split_sum = sums[a_split_begin_index] - a_sum_to_push;
//                auto a_rest_sum = r_current.a_sum_ - a_split_sum;
                
                cut += range_cut(d_current, b_current, { a_split_end, r_current.a_end_, r_current.d_last_, r_current.b_last_, a_rest_sum });
                ranges.push_back({ r_current.a_begin_, a_split_end, r_current.d_last_, r_current.b_last_, a_split_sum });
                ranges.push_back({ a_split_end, a.end(), d_current, b_current, a_sum_to_push });
                break;
            } /*else {
                std::cout << "Error!!!" << std::endl;
            }*/
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
    solve_complicated();

    std::cin.rdbuf(cinbuf); //reset to standard input again
    std::cout.rdbuf(coutbuf); //reset to standard output again

//    for (auto&& elem : fields) {
//        std::cout << "> a_:" << elem.a_ << ", d_last_:" << elem.d_last_ << ", b_last_:" << elem.b_last_ << std::endl;
//    }

    return 0;
}


