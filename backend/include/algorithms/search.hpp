#pragma once
#include <vector>
#include <list>
#include <functional>
#include "memory_tracer.hpp"
#include "sorting.hpp" // importata per avere StepEvent e StepCallback
/**
 * ============================================================================
 *  ALGORITMI DI RICERCA — Linear Search e Binary Search
 * ============================================================================
 *
 *  Ogni algoritmo esiste in tre varianti:
 *
 *  1. VERSIONE STEP-BY-STEP  —  linearSearch / binarySearch
 *     Usata dal Visualizzatore della piattaforma
 *     Ad ogni confronto significativo aggiorna il MemoryTracer 
 *     e invoca la StepCallback con uno StepEvent
 *
 *  2. VERSIONE BENCHMARK SU HEAP ARRAY  —  linearSearchBench / binarySearchBench
 *     Usata dal Benchmark della piattafroma su struttura "Heap Array"
 *     Versioni pure: strutturalmente identiche alle precedenti ma senza
 *     MemoryTracer, StepCallback né StepEvent cioè zero overhead.
 *     Ricevono int* (raw pointer) per coerenza con le altre funzioni bench
 *     e per eliminare il costo dei wrapper di std::vector.
 *     I tempi misurati riflettono esclusivamente l'algoritmo puro
 *
 *  3. VERSIONE BENCHMARK SU LINKED LIST  —  linearSearchBenchList
 *     Usata dal Benchmark della piattaforma su struttura "Linked List"
 *     Solo la ricerca lineare è implementata su lista perchè la ricerca binaria
 *     richiede accesso per indice O(1), impossibile su std::list
 *     che espone solo iteratori con avanzamento O(n)
 *     Implementarla su lista degraderebbe la complessità da O(log n) a O(n),
 *     snaturando l'algoritmo
 *
 * ============================================================================
 */


// Ricerca lineare: cerca target in arr, invoca cb ad ogni confronto
// Ritorna l'indice trovato, oppure -1 se non presente
int linearSearch(const std::vector<int>& arr, int target,
                 MemoryTracer& mem, StepCallback cb);

// Ricerca binaria: arr deve essere ordinato
// Ritorna l'indice trovato, oppure -1 se non presente
int binarySearch(const std::vector<int>& arr, int target,
                 MemoryTracer& mem, StepCallback cb);

// Versioni Pure su array heap (int*)
int linearSearchBench(const int* arr, int n, int target);
int binarySearchBench(const int* arr, int n, int target);

// Versione Pure su linked list
int linearSearchBenchList(const std::list<int>& lst, int target);
