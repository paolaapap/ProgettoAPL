#pragma once
#include <vector>
#include <list>
#include <functional>
#include "memory_tracer.hpp"
#include "sorting.hpp" 

// -----------------------------------------------------------------------
// Algoritmi di ricerca
// -----------------------------------------------------------------------

/// Ricerca lineare: cerca target in arr, invoca cb ad ogni confronto
/// Ritorna l'indice trovato, oppure -1 se non presente
int linearSearch(const std::vector<int>& arr, int target,
                 MemoryTracer& mem, StepCallback cb);

/// Ricerca binaria: arr deve essere ordinato
/// Ritorna l'indice trovato, oppure -1 se non presente
int binarySearch(const std::vector<int>& arr, int target,
                 MemoryTracer& mem, StepCallback cb);

// Versioni Pure su array (int*)
int linearSearchBench(const int* arr, int n, int target);
int binarySearchBench(const int* arr, int n, int target);

// Versione Pure su linked list
int linearSearchBenchList(const std::list<int>& lst, int target);
