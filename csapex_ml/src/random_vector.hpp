#ifndef RANDOM_VECTOR_HPP
#define RANDOM_VECTOR_HPP

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

class RandomVector
{
public:
    RandomVector(std::size_t seed) : gen_(seed)
    {
    }

    inline std::vector<std::size_t> newPermutation(const std::size_t size)
    {
        std::vector<std::size_t> random_vector(size);
        for (std::size_t i = 0; i < size; ++i) {
            random_vector[i] = i;
        }

        std::shuffle(random_vector.begin(), random_vector.end(), gen_);
        return random_vector;
    }

private:
    std::mt19937 gen_;
};
#endif  // RANDOM_VECTOR_HPP
