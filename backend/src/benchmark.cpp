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
    
    // Generatore di numeri pseudocasuali basato sull'algoritmo di Mersenne Twister
    // usa il numero del run come seed
    // garantisce ogni esecuzione deterministica e riproducibile
    std::mt19937 rng(seed);
    std::vector<int> data(n);

    if (distType == "sorted") {  // vettore ordinato crescente con val da 0 a n-1
        for (int i = 0; i < n; ++i) data[i] = i;
    } else if (distType == "reversed") { // vettore ordinato decrescente con val da n a 1
        for (int i = 0; i < n; ++i) data[i] = n - i;
    } else if (distType == "nearly_sorted") { // vettore quasi ordinato, inizializza un vettore ordinato da 0 a n-1 
        for (int i = 0; i < n; ++i) data[i] = i;
        std::uniform_int_distribution<int> dist(0, n - 1);
        int swaps = std::max(1, static_cast<int>(n * 0.05));  // e poi fa un numero di swap pari al 5% della dim dell'array
        for (int i = 0; i < swaps; ++i)
            std::swap(data[dist(rng)], data[dist(rng)]);
    } else {
        // random (default)
        std::uniform_int_distribution<int> dist(0, n * 10); // una distribuzione uniforme dove ogni valore appartiene a [0, 10n]
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
    
    double mean = 0.0;
    if(!times.empty()) {                             
        // std::accumulate calcola la somma cumulativa degli elementi in un intervallo, partendo da un val specificato (0.0)
        mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    } 
    auto sorted = times;
    std::sort(sorted.begin(), sorted.end());

    BenchmarkResult result;
    result.algorithm    = algoName;
    result.n            = n;
    result.runs         = runs;
    result.mean_ns      = mean;
    result.median_ns    = calcQuantile(sorted, 0.5);
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
    times.reserve(runs); // pre alloca memoria per evitare allocazioni sull'heap durante il ciclo di misurazione

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

        auto t0 = std::chrono::high_resolution_clock::now(); // restituisce un std::chrono::time_point
        algoFn(data);
        auto t1 = std::chrono::high_resolution_clock::now();

        times.push_back(static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count())); // t1 - t0 restituisce std::chrono::duration
            // .count() restituisce int64_t
    }  

    return buildResult(algoName, n, runs, times);
}

/**
 * ============================================================================
 *  CALCOLO DELLE STATISTICHE
 * ============================================================================
 *    1. Mediana: calcolata tramite calcQuantile() chiamato con 0.5
 *    2. Quantile: calcQuantile(), invocato poi con 0.25 e 0.75
 *    3. Deviazione Standard: calcStdDev()
 * 
 * ============================================================================
 */

double Benchmarker::calcQuantile(const std::vector<double>& sorted, double q) {
    if (sorted.empty()) return 0.0;
    double pos = q * (static_cast<double>(sorted.size()) - 1.0);
    size_t lo  = static_cast<size_t>(std::floor(pos)); // arrotondo pos x difetto
    size_t hi  = static_cast<size_t>(std::ceil(pos)); // arrotondo pos x eccesso
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
