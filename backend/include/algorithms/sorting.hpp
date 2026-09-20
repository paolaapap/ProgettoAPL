#pragma once
#include "memory_tracer.hpp"
#include <functional>
#include <list>
#include <string>
#include <vector>

// StepEvent: struttra che rappresenta un singolo passo dell'algoritmo da inviare al frontend

struct StepEvent {
  int step;                   // indice progressivo dello step
  std::vector<int> array;     // stato attuale dell'array (copia, non rif)
  std::vector<int> highlight; // indici da evidenziare nel grafico pys
  int comparisons;            // comparazioni totali fino a questo step
  int swaps;                  // swap totali fino a questo step
  MemorySnapshot memory;      // snapshot Stack/Heap in questo step
};

// Tipo della callback invocata ad ogni step
using StepCallback = std::function<void(const StepEvent &)>;

// emitStep: costruisce uno StepEvent e lo passa alla cb
// Inline  perché usato da più TU (sorting, search)
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



// Algoritmi di ordinamento
void bubbleSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);
void insertionSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);
void selectionSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);
void mergeSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);
void quickSort(std::vector<int> &arr, MemoryTracer &mem, StepCallback cb);


// Versioni Pure per i Benchmark su array heap
void bubbleSortBench(int *arr, int n);
void insertionSortBench(int *arr, int n);
void selectionSortBench(int *arr, int n);
void mergeSortBench(int *arr, int n);
void quickSortBench(int *arr, int n);


// Versioni Pure per i Benchmark su liste 
void bubbleSortBenchList(std::list<int> &lst);
void insertionSortBenchList(std::list<int> &lst);
void selectionSortBenchList(std::list<int> &lst);
void mergeSortBenchList(std::list<int> &lst);
void quickSortBenchList(std::list<int> &lst);
