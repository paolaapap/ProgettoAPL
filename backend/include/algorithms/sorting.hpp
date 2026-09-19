#pragma once
#include "memory_tracer.hpp"
#include <functional>
#include <list>
#include <string>
#include <vector>

// -----------------------------------------------------------------------
// StepEvent — un singolo "passo" dell'algoritmo da inviare al frontend
// -----------------------------------------------------------------------

struct StepEvent {
  int step;
  std::vector<int> array;     // stato attuale dell'array
  std::vector<int> highlight; // indici degli elementi "attivi" in questo step
  int comparisons;            // comparazioni totali fino a questo step
  int swaps;                  // swap totali fino a questo step
  MemorySnapshot memory;      // snapshot Stack/Heap in questo step
};

// Tipo della callback invocata ad ogni step
using StepCallback = std::function<void(const StepEvent &)>;

// -----------------------------------------------------------------------
// emitStep — helper condiviso: costruisce uno StepEvent e lo passa alla cb
// Inline nell'header perché usato da più translation unit (sorting, search)
// -----------------------------------------------------------------------
inline void emitStep(int &stepCount, int &comparisons, int &swaps,
                     const std::vector<int> &arr,
                     const std::vector<int> &highlight,
                     MemoryTracer &mem,
                     const StepCallback &cb) {
    StepEvent ev;
    ev.step        = stepCount++;
    ev.array       = arr;
    ev.highlight   = highlight;
    ev.comparisons = comparisons;
    ev.swaps       = swaps;
    ev.memory      = mem.snapshot();
    cb(ev);
}


// -----------------------------------------------------------------------
// Dichiarazioni degli algoritmi di ordinamento
// Ogni funzione modifica arr sul posto e invoca cb ad ogni step significativo
// -----------------------------------------------------------------------

void bubbleSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);
void insertionSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);
void selectionSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);
void mergeSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);
void quickSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);

// -----------------------------------------------------------------------
// Versioni Pure per i Benchmark su array (int* — allocazione heap esplicita)
// -----------------------------------------------------------------------
void bubbleSortBench(int *arr, int n);
void insertionSortBench(int *arr, int n);
void selectionSortBench(int *arr, int n);
void mergeSortBench(int *arr, int n);
void quickSortBench(int *arr, int n);

// -----------------------------------------------------------------------
// Versioni Pure per i Benchmark su std::list<int> (doubly linked list)
// Compatibili con: bubble, insertion, selection, merge, quick, linear_search
// NON compatibili con: binary_search (richiede accesso O(1)), dijkstra (grafo)
// -----------------------------------------------------------------------
void bubbleSortBenchList(std::list<int> &lst);
void insertionSortBenchList(std::list<int> &lst);
void selectionSortBenchList(std::list<int> &lst);
void mergeSortBenchList(std::list<int> &lst);
void quickSortBenchList(std::list<int> &lst);
