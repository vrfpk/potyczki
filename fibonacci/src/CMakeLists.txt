add_executable (fibo main.cpp)
message("GMP_LIBRARIES: ${GMP_LIBRARIES}")
target_link_libraries(fibo gmp gmpxx)
