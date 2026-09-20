#pragma once
#include <functional>
#include <list>
#include <string>
#include <vector>

/**
 * ============================================================================
 *  STRUTTURA DATI BENCHMARK RESULT
 *  Memorizza il risultato di N run indipendenti di un algoritmo
 * ============================================================================
 */
struct BenchmarkResult {
    std::string algorithm;             // nome dell'algoritmo
    int         n;                     // dimensione dell'input
    int         runs;                  // numero di run effettuati
    double      mean_ns;               // media aritmetica dei tempi
    double      median_ns;             // mediana
    double      q1_ns;                 // 1° quartile (25° percentile)
    double      q3_ns;                 // 3° quartile (75° percentile)
    double      std_dev_ns;            // deviazione standard
    std::vector<double> run_times_ns;  // tempi raw di ogni run

    // tutti i tempi sono in nanosecondi
};


/**
 * ============================================================================
 *  CLASSE BENCHMARKER
 * ============================================================================
 *  Misura i tempi di esecuzione di un algoritmo su array heap o linked list
 *
 *  Due modalità di benchmark:
 *    run()     -> algoritmo su array heap 
 *    runList() -> algoritmo su std::list<int> 
 * 
 * ============================================================================
 */
class Benchmarker {
public:
    Benchmarker()  = default;
    ~Benchmarker() = default;

    // Non copiabile , non ha stato interno
    Benchmarker(const Benchmarker&)            = delete;
    Benchmarker& operator=(const Benchmarker&) = delete;

    // Esegue algoFn su array heap per runs volte
    // algoFn riceve (int* arr, int sz) 
    BenchmarkResult run(const std::string& algoName,
                        std::function<void(int*, int)> algoFn,
                        int n,
                        int runs = 30,
                        const std::string& dataDistribution = "random");

    // Esegue algoFn su std::list<int> per runs volte
    // algoFn riceve std::list<int>&
    // Non compatibile con binary_search e dijkstra
    BenchmarkResult runList(const std::string& algoName,
                            std::function<void(std::list<int>&)> algoFn,
                            int n,
                            int runs = 30,
                            const std::string& dataDistribution = "random");

private:
    // Genera un vettore di n interi con seed e distribuzione dati
    static std::vector<int> generateData(int n, unsigned int seed,
                                          const std::string& distType);

    // Helpers per il calcolo statistico
    static double calcMedian(std::vector<double> vals);
    static double calcQuantile(const std::vector<double>& sorted, double q);
    static double calcStdDev(const std::vector<double>& vals, double mean);

    // Costruisce la struct BenchmarkResult dai tempi misurati
    static BenchmarkResult buildResult(const std::string& algoName, int n, int runs,
                                        std::vector<double>& times);
};
