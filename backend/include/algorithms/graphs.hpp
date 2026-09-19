#pragma once
#include "algorithms/sorting.hpp" // inclusa per avere StepCallback e StepEvent
#include "memory_tracer.hpp"
#include <functional>
#include <vector>

// ---------------------------------------------------------------------------
// Algoritmi sui Grafi - ALGORITMO DI DIJKSTRA, ricerca dei percorsi minimi
// ---------------------------------------------------------------------------
// L'input "adj_matrix" è una matrice di adiacenza N x N (rappresenta il grafo).
// 0 o -1 indicano assenza di arco.
// ---------------------------------------------------------------------------

void dijkstra(const std::vector<int> &adj_matrix, int start_node,
              MemoryTracer &mem, StepCallback cb);

// passiamo un puntatore raw alla matrice (anizhche std::vector) per avere velocità max
void dijkstraBench(const int *adj_matrix, int n, int start_node);
