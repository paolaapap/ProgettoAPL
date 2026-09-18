#pragma once
#include <functional>
#include <list>
#include <string>
#include <vector>

// -----------------------------------------------------------------------
// BenchmarkResult — risultato di N run di un algoritmo
// -----------------------------------------------------------------------

struct BenchmarkResult {
    std::string algorithm;
    int         n;          // dimensione dell'array/lista
    int         runs;       // numero di esecuzioni
    double      mean_ns;
    double      median_ns;
    double      q1_ns;
    double      q3_ns;
    double      std_dev_ns;
    std::vector<double> run_times_ns; // tempi raw di ogni run
};

// -----------------------------------------------------------------------
// Benchmarker — misura i tempi di esecuzione di un algoritmo
// Usa std::chrono::high_resolution_clock per massima precisione.
//
// Due modalità di benchmark:
//   run()     → algoritmo su array heap (int*, new/delete espliciti)
//   runList() → algoritmo su std::list<int> (doubly linked list)
//
// Entrambi usano internamente il template runGeneric<AlgoFn, DataFactory>
// per condividere la logica di timing e calcolo statistiche.
// -----------------------------------------------------------------------

/* La classe Benchmarker separa:
    - logica di timinig (uguale per tutti)
    - costruzione dei dati (array o lista)
    - esecuzione dell'algoritmo (diversa per ogni algoritmo)

*/
class Benchmarker {
public:
    Benchmarker() = default;
    ~Benchmarker() = default;

    // Non copiabile
    Benchmarker(const Benchmarker&) = delete;
    Benchmarker& operator=(const Benchmarker&) = delete;

    /// Esegue algoFn su array heap (int*) per `runs` volte.
    /// algoFn riceve (int* arr, int sz) — array allocato con new, liberato dopo la misura.
    BenchmarkResult run(const std::string& algoName,
                        std::function<void(int*, int)> algoFn,
                        int n,
                        int runs = 30,
                        const std::string& dataDistribution = "random");

    /// Esegue algoFn su std::list<int> per `runs` volte.
    /// algoFn riceve std::list<int>& — lista costruita da generateData() ad ogni run.
    /// Non compatibile con binary_search e dijkstra.
    BenchmarkResult runList(const std::string& algoName,
                            std::function<void(std::list<int>&)> algoFn,
                            int n,
                            int runs = 30,
                            const std::string& dataDistribution = "random");

private:
    /// Template interno condiviso: gestisce il loop dei run, il timing e le statistiche.
    /// AlgoFn:      callable che riceve una struttura dati già pronta e la ordina/cerca
    /// DataFactory: callable che produce la struttura dati per un dato run (seed = indice run)
    template<typename AlgoFn, typename DataFactory>
    BenchmarkResult runGeneric(const std::string& algoName,
                               AlgoFn algoFn,
                               DataFactory makeData,
                               int n,
                               int runs);

    /// Genera un vettore di n interi con il seed e la distribuzione dati
    static std::vector<int> generateData(int n, unsigned int seed, const std::string& distType);

    /// Calcola mediana da un vettore di valori (modifica una copia interna)
    static double calcMedian(std::vector<double> vals);

    /// Calcola quartile (0.25 o 0.75) da un vettore già ordinato
    static double calcQuantile(const std::vector<double>& sorted, double q);

    /// Calcola deviazione standard della popolazione
    static double calcStdDev(const std::vector<double>& vals, double mean);
};

