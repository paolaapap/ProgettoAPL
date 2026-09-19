#include "algorithms/graphs.hpp"
#include <cmath>
#include <limits>
#include <algorithm>

const int INF = 99999999;

void dijkstra(const std::vector<int>& adj_matrix, int start_node, MemoryTracer& mem, StepCallback cb) {
    if (adj_matrix.empty()) return;
    int n = static_cast<int>(std::sqrt(adj_matrix.size()));
    if (n * n != (int)adj_matrix.size()) return; // verifica se la matrice non è quadrata
    
    if (start_node < 0 || start_node >= n) start_node = 0;

    std::vector<int> dist(n, INF); // dist[v] = distanza corrente da start_node a v
    std::vector<bool> visited(n, false); // visited[v] = true se v è un nodo gia visitato
    dist[start_node] = 0; // la distanza da sé stesso è 0

    int steps = 0, comps = 0, swaps = 0;

    mem.pushFrame("dijkstra");
    
    // Prima invocazione della funzione di cb che semplicemente aggiunge eventi nel vettore di StepEvent che ho nel main
    if (cb) {
        StepEvent ev;
        ev.step = ++steps;
        ev.array = dist; // distanza iniziale: [0, INF, INF ..., INF]
        ev.highlight = {start_node}; // evidenzia il nodo di partenza
        ev.comparisons = comps; // 0
        ev.swaps = swaps; // 0 sempre perchè questo algoritmo non fa swap
        ev.memory = mem.snapshot();
        cb(ev);
    }

    for (int count = 0; count < n - 1; ++count) {
        // Trova nodo con distanza minima
        int u = -1;
        int min_d = INF;
        for (int v = 0; v < n; ++v) {
            comps++;
            if (!visited[v] && dist[v] <= min_d) {
                min_d = dist[v];
                u = v;
            }
        }

        if (u == -1 || min_d == INF) break; // Resto dei nodi irraggiungibile

        visited[u] = true;

        if (cb) {
            StepEvent ev;
            ev.step = ++steps;
            ev.array = dist;
            ev.highlight = {u}; // Nodo in elaborazione
            ev.comparisons = comps;
            ev.swaps = swaps;
            
            // Aggiorna stack frame con le variabili chiave
            mem.popFrame(); // rimuove il vecchio frame
            mem.pushFrame("dijkstra"); // ricrea il frame dello stato corrente
            ev.memory = mem.snapshot();
            cb(ev);
        }

        // Se, per raggiungere v, passare per u costa meno allora aggiorno dist[v]
        for (int v = 0; v < n; ++v) {
            int edge_cost = adj_matrix[u * n + v]; // peso dell'arco da u a v: u -> v
            if (edge_cost > 0     &&  // arco negativo non consentito nell'algorimto
                edge_cost != INF  &&  // non è gia inf
                !visited[v]       &&  // il nodo v non è gia visitato
                dist[u] != INF) {     // il nodo u è raggiungibile 
                comps++;
                if (dist[u] + edge_cost < dist[v]) {
                    dist[v] = dist[u] + edge_cost;
                    
                    // lo step viene emesso solo quando dist[v] viene aggiornato, non ad ogni confronto
                    if (cb) {
                        StepEvent ev;
                        ev.step = ++steps;
                        ev.array = dist;
                        ev.highlight = {u, v}; // adesso evidenzio l'arco in esame
                        ev.comparisons = comps;
                        ev.swaps = swaps;
                        ev.memory = mem.snapshot();
                        cb(ev);
                    }
                }
            }
        }
    }
    
    // Step finale
    if (cb) {
        StepEvent ev;
        ev.step = ++steps;
        ev.array = dist; // adesso qui avremo le distanze finali, da start_node a tutti i nodi
        ev.highlight = {}; // nessun elemento evidenziato perchè l'algoritmo è concluso
        ev.comparisons = comps;
        ev.swaps = swaps;
        ev.memory = mem.snapshot();
        cb(ev);
    }

    mem.popFrame(); // rimuove il frame "dijkasta" dallo stack logico
}


// VERSIONE PURA, strutturalmente identica ma ZERO OVERHEAD
// cioè: senza MemoryTracer, callback, StepEvent
// con int* anizche vector<int> perchè il Benchmarker riceve int* (lavora con heap array)
// perchè questa versione viene invocata per il benchmarking
void dijkstraBench(const int* adj_matrix, int n, int start_node) {
    // in qeusto caso la verifica su n (se è un quadrato perfetto o no) viene fatta sul main
    if (start_node < 0 || start_node >= n) start_node = 0;
    std::vector<int> dist(n, INF);
    std::vector<bool> visited(n, false);
    dist[start_node] = 0;

    for (int count = 0; count < n - 1; ++count) {
        int u = -1;
        int min_d = INF;
        for (int v = 0; v < n; ++v) {
            if (!visited[v] && dist[v] <= min_d) {
                min_d = dist[v];
                u = v;
            }
        }

        if (u == -1 || min_d == INF) break;
        visited[u] = true;

        for (int v = 0; v < n; ++v) {
            int edge_cost = adj_matrix[u * n + v];
            if (edge_cost > 0 && edge_cost != INF && !visited[v] && dist[u] != INF) {
                if (dist[u] + edge_cost < dist[v]) {
                    dist[v] = dist[u] + edge_cost;
                }
            }
        }
    }
}
