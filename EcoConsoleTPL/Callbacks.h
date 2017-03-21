#pragma once
#include <iostream>

namespace fn {
    void sum(const int& a, const int& b) {
        std::cout << a << " + " << b << " = " << a + b << '\n';
    }
    void div(const double& a, const double& b) {
        std::cout << a << " / " << b << " = " << a / b << '\n';
    }
    void square(const unsigned long& a) {
        std::cout << a << " ^ 2 = " << a * a << '\n';
    }
    void reverse(const std::string& str) {
        std::cout << str << " -> " << std::string{ str.rbegin(), str.rend() } << '\n';
    }
    void con_exit() {
        exit(0);
    }
}
