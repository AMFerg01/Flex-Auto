#include <iostream>
#include <vector>

#ifndef _DEBUG
#define LOG_MSG(x) std::cout << x << std::endl;
#else
#define LOG_MSG(x) std::cout << x << std::endl;
#endif

template <typename T>
void print_vector(std::vector<T> &v)
{

    for (size_t i = 0; i < v.size(); i++)
    {
        std::cout << "v[" << i << "]: " << v.at(i) << "\n";
    }
}