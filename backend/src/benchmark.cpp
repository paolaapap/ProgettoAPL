#include "benchmark.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <numeric>
#include <random>
#include <stdexcept>

// -----------------------------------------------------------------------
// generateData — produce un vettore di n interi con seed e distribuzione dati
// Separato da runGeneric così può essere usato da run() e runList()
// -----------------------------------------------------------------------

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
        int swaps = std::max(1, (int)(n * 0.05));
        for (int i = 0; i < swaps; ++i)
            std::swap(data[dist(rng)], data[dist(rng)]);
    } else {
        // random (default)
        std::uniform_int_distribution<int> dist(0, n * 10);
        for (auto& x : data) x = dist(rng);
    }
    return data;
}

// -----------------------------------------------------------------------
// runGeneric — template interno condiviso
//
// AlgoFn:      callable invocato con la struttura dati già pronta
// DataFactory: callable(int seed) che costruisce la struttura dati
//
// Il timer wrappa SOLO la chiamata ad algoFn — la costruzione della
// struttura dati avviene fuori dal timing, così le misure sono pulite.
// -----------------------------------------------------------------------

template<typename AlgoFn, typename DataFactory>
BenchmarkResult Benchmarker::runGeneric(const std::string& algoName,
                                         AlgoFn algoFn,
                                         DataFactory makeData,
                                         int n,
                                         int runs) {
    if (n <= 0 || runs <= 0)
        throw std::invalid_argument("n e runs devono essere > 0");

    std::vector<double> times;
    times.reserve(runs);

    for (int r = 0; r < runs; ++r) {
        auto data = makeData(r);  // costruisce la struttura per questo run (seed = r)

        auto t0 = std::chrono::high_resolution_clock::now();
        algoFn(data);             // esegue l'algoritmo — UNICA cosa misurata
        auto t1 = std::chrono::high_resolution_clock::now();

        double elapsed_ns = static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
        times.push_back(elapsed_ns);
    }

    double mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    auto sorted = times;
    std::sort(sorted.begin(), sorted.end());

    BenchmarkResult result;
    result.algorithm   = algoName;
    result.n           = n;
    result.runs        = runs;
    result.mean_ns     = mean;
    result.median_ns   = calcMedian(times);
    result.q1_ns       = calcQuantile(sorted, 0.25);
    result.q3_ns       = calcQuantile(sorted, 0.75);
    result.std_dev_ns  = calcStdDev(times, mean);
    result.run_times_ns = times;
    return result;
}

// -----------------------------------------------------------------------
// run() — benchmark su array heap (int*, allocazione new/delete esplicita)
//
// Per ogni run:
//   1. genera i dati come std::vector<int> con generateData()
//   2. alloca un array heap con new int[n]
//   3. copia i dati con std::copy
//   4. misura algoFn(arr, n)
//   5. dealloca con delete[]
//
// La DataFactory passata a runGeneric wrappa i passi 2-3 e restituisce
// un oggetto RAII che chiama delete[] al distruttore, così il codice
// di timing in runGeneric non deve sapere nulla dell'allocazione.
// -----------------------------------------------------------------------

// Wrapper RAII per un array heap: alloca in costruzione, libera in distruzione
struct HeapArray {
    int* ptr;
    int  sz;
    HeapArray(const std::vector<int>& src)
        : ptr(new int[src.size()]), sz(static_cast<int>(src.size())) {
        std::copy(src.begin(), src.end(), ptr);
    }
    ~HeapArray() { delete[] ptr; }
    // Non copiabile
    HeapArray(const HeapArray&) = delete;
    HeapArray& operator=(const HeapArray&) = delete;
};

BenchmarkResult Benchmarker::run(const std::string& algoName,
                                  std::function<void(int*, int)> algoFn,
                                  int n,
                                  int runs,
                                  const std::string& dataDistribution) {
    // DataFactory: cattura n e dataDistribution, produce un HeapArray per ogni seed
    auto makeData = [&](int seed) -> HeapArray {
        auto vec = generateData(n, static_cast<unsigned int>(seed), dataDistribution);
        return HeapArray(vec);
    };

    // AlgoFn wrapper: runGeneric passa HeapArray&, noi vogliamo chiamare algoFn(int*, int)
    auto wrappedAlgo = [&](HeapArray& ha) {
        algoFn(ha.ptr, ha.sz);
    };

    return runGeneric(algoName, wrappedAlgo, makeData, n, runs);
}

// -----------------------------------------------------------------------
// runList() — benchmark su std::list<int> (doubly linked list)
//
// Per ogni run:
//   1. genera i dati come std::vector<int> con generateData()
//   2. costruisce std::list<int> dal vettore (O(n))
//   3. misura algoFn(lst)
//   4. la lista è distrutta automaticamente (RAII)
//
// Differenza rispetto a run(): i nodi della lista sono sparsi sull'heap
// (un nodo allocato per volta), non contigui. Questo impatta la cache.
// -----------------------------------------------------------------------

BenchmarkResult Benchmarker::runList(const std::string& algoName,
                                      std::function<void(std::list<int>&)> algoFn,
                                      int n,
                                      int runs,
                                      const std::string& dataDistribution) {
    auto makeData = [&](int seed) -> std::list<int> {
        auto vec = generateData(n, static_cast<unsigned int>(seed), dataDistribution);
        return std::list<int>(vec.begin(), vec.end());
    };

    return runGeneric(algoName, algoFn, makeData, n, runs);
}

// -----------------------------------------------------------------------
// Helpers statistici — invariati
// -----------------------------------------------------------------------

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
    size_t lo = static_cast<size_t>(std::floor(pos));
    size_t hi = static_cast<size_t>(std::ceil(pos));
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


