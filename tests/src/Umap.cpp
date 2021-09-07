#include <gtest/gtest.h>

#include "umappp/Umap.hpp"
#include "knncolle/knncolle.hpp"

#include <map>
#include <random>
#include <cmath>

class UmapTest : public ::testing::TestWithParam<std::tuple<int, int> > {
protected:
    template<class Param>
    void assemble(Param p) {
        nobs = std::get<0>(p);
        k = std::get<1>(p);

        std::mt19937_64 rng(nobs * k); // for some variety
        std::normal_distribution<> dist(0, 1);

        data.resize(nobs * ndim);
        for (int r = 0; r < data.size(); ++r) {
            data[r] = dist(rng);
        }

        knncolle::VpTreeEuclidean<> searcher(ndim, nobs, data.data());
        stored.reserve(nobs);
        for (size_t i = 0; i < searcher.nobs(); ++i) {
            stored.push_back(searcher.find_nearest_neighbors(i, k));
        }
        return;
    }

    int nobs, k;
    int ndim = 5;
    std::vector<double> data;
    umappp::NeighborList stored;
};

TEST_P(UmapTest, Basic) {
    assemble(GetParam());

    umappp::Umap runner;
    std::vector<double> output(nobs * ndim);
    auto status = runner.initialize(std::move(stored), ndim, output.data());
    runner.run(status, ndim, output.data());
}

INSTANTIATE_TEST_SUITE_P(
    Umap,
    UmapTest,
    ::testing::Combine(
        ::testing::Values(50, 100, 200), // number of observations
        ::testing::Values(5, 10, 15) // number of neighbors
    )
);
