#include "benchmark.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <numeric>
#include <random>
#include <stdexcept>


/**
 * ============================================================================
 *  METODO PRIVATO DELLA CLASSE - generateData()
 * ============================================================================
 *  Genera n interi con seed deterministico e distribuzione scelta
 * ============================================================================
 */
std::vector<int> Benchmarker::generateData(int n, unsigned int seed,
                                            const std::string& distType) {
    std::mt19937 rng(seed);
    std::vector<int> data(n);

    if (distType == "sorted") {
        for (int i = 0; i < n; ++i) data[i] = i;
    } else if (distType == "reversed") {
        for (int i = 0; i < n; ++i) data[i] = n - i;
    } else if (distType == "nearly_sorted") {
        for (int i = 0; i < n; ++i) data[i] = i;
        std::uniform_int_distribution<int> dist(0, n - 1);
        int swaps = std::max(1, (int)(n * 0.05));  // 5% di elementi scambiati
        for (int i = 0; i < swaps; ++i)
            std::swap(data[dist(rng)], data[dist(rng)]);
    } else {
        // random (default)
        std::uniform_int_distribution<int> dist(0, n * 10);
        for (auto& x : data) x = dist(rng);
    }
    return data;
}


/**
 * ============================================================================
 *  METODO PRIVATO DELLA CLASSE - buildResult()
 * ============================================================================
 *  Calcola le statistiche e costruisce il BenchmarkResult
 * ============================================================================
 */
BenchmarkResult Benchmarker::buildResult(const std::string& algoName,
                                          int n, int runs,
                                          std::vector<double>& times) {
    double mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    auto sorted = times;
    std::sort(sorted.begin(), sorted.end());

    BenchmarkResult result;
    result.algorithm    = algoName;
    result.n            = n;
    result.runs         = runs;
    result.mean_ns      = mean;
    result.median_ns    = calcMedian(times);
    result.q1_ns        = calcQuantile(sorted, 0.25);
    result.q3_ns        = calcQuantile(sorted, 0.75);
    result.std_dev_ns   = calcStdDev(times, mean);
    result.run_times_ns = times;
    return result;
}


/**
 * ============================================================================
 *  METODO PUBBLICO DELLA CLASSE - run()
 * ============================================================================
 *  Esegue algoFn su array heap per runs volte
 *
 *  Per ogni run:
 *    - genera i dati fuori dal timing (con generateData)
 *    - misura il tempo con std::chrono::high_resolution_clock
 *    - al termine calcola le statistiche (buildResult)
 * 
 * ============================================================================
 */
BenchmarkResult Benchmarker::run(const std::string& algoName,
                                  std::function<void(int*, int)> algoFn,
                                  int n, int runs,
                                  const std::string& dataDistribution) {
    if (n <= 0 || runs <= 0)
        throw std::invalid_argument("n e runs devono essere > 0");

    std::vector<double> times;
    times.reserve(runs);

    for (int r = 0; r < runs; ++r) {
        auto data = generateData(n, static_cast<unsigned int>(r), dataDistribution);

        auto t0 = std::chrono::high_resolution_clock::now();
        algoFn(data.data(), static_cast<int>(data.size()));
        auto t1 = std::chrono::high_resolution_clock::now();

        times.push_back(static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()));
    }   

    return buildResult(algoName, n, runs, times);
}


/**
 * ============================================================================
 *  METODO PUBBLICO DELLA CLASSE - runList()
 * ============================================================================
 *  Esegue algoFn su linked list per runs volte
 *  Stesso funzionamento di run()
 * 
 *  Unica differenza:
 *    - costruisce la lista dal vector generato da generateData()
 * 
 * ============================================================================
 */
BenchmarkResult Benchmarker::runList(const std::string& algoName,
                                      std::function<void(std::list<int>&)> algoFn,
                                      int n, int runs,
                                      const std::string& dataDistribution) {
    if (n <= 0 || runs <= 0)
        throw std::invalid_argument("n e runs devono essere > 0");

    std::vector<double> times;
    times.reserve(runs);

    for (int r = 0; r < runs; ++r) {
        auto vec = generateData(n, static_cast<unsigned int>(r), dataDistribution);
        std::list<int> data(vec.begin(), vec.end());

        auto t0 = std::chrono::high_resolution_clock::now();
        algoFn(data);
        auto t1 = std::chrono::high_resolution_clock::now();

        times.push_back(static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()));
    }  

    return buildResult(algoName, n, runs, times);
}

/**
 * ============================================================================
 *  CALCOLO DELLE STATISTICHE
 * ============================================================================
 *    1. Mediana: calcMedian()
 *    2. Quantile: calcQuantile(), invocato poi con 0.25 e 0.75
 *    3. Deviazione Standard: calcStdDev()
 * 
 * ============================================================================
 */
double Benchmarker::calcMedian(std::vector<double> vals) {
    std::sort(vals.begin(), vals.end());
    size_t n = vals.size();
    if (n % 2 == 0)
        return (vals[n / 2 - 1] + vals[n / 2]) / 2.0;
    return vals[n / 2];
}

double Benchmarker::calcQuantile(const std::vector<double>& sorted, double q) {
    if (sorted.empty()) return 0.0;
    double pos = q * (static_cast<double>(sorted.size()) - 1.0);
    size_t lo  = static_cast<size_t>(std::floor(pos));
    size_t hi  = static_cast<size_t>(std::ceil(pos));
    if (lo == hi) return sorted[lo];
    double frac = pos - static_cast<double>(lo);
    return sorted[lo] * (1.0 - frac) + sorted[hi] * frac;
}

double Benchmarker::calcStdDev(const std::vector<double>& vals, double mean) {
    double sq_sum = 0.0;
    for (double v : vals)
        sq_sum += (v - mean) * (v - mean);
    return std::sqrt(sq_sum / static_cast<double>(vals.size()));
}
