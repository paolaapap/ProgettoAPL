#include "algorithms/search.hpp"
#include "algorithms/sorting.hpp"  // inclusa per stepEvent, stepCallback, emitStep
#include <string>


// LINEAR SEARCH
int linearSearch(const std::vector<int>& arr, int target,
                 MemoryTracer& mem, StepCallback cb) {
    int n = static_cast<int>(arr.size());
    int step = 0, cmp = 0, swaps = 0;  // swaps sempre 0 perchè le ricerche non fanno swap

    mem.pushFrame("linearSearch", {{"n",      std::to_string(n)},
                                   {"target", std::to_string(target)},
                                   {"i",      "0"}});
    emitStep(step, cmp, swaps, arr, {}, mem, cb);  // stato iniziale

    for (int i = 0; i < n; ++i) {
        ++cmp;
        mem.updateFrame({{"i",      std::to_string(i)},
                         {"arr[i]", std::to_string(arr[i])}});
        emitStep(step, cmp, swaps, arr, {i}, mem, cb);  // elemento esaminato

        if (arr[i] == target) {
            // step duplicato quando si trova l'elemento 
            // serve al frontend per mantenere l'highlight dell'elemento trovato 
            // per almeno un frame prima di terminare l'animazione
            emitStep(step, cmp, swaps, arr, {i}, mem, cb);  // trovato
            mem.popFrame();
            return i;
        }
    }
    emitStep(step, cmp, swaps, arr, {}, mem, cb);  // non trovato, highlight vuoto
    mem.popFrame();
    return -1;
}


// BINARY SEARCH SU ARRAY ORDINATO (ordinato dal main.cpp)
int binarySearch(const std::vector<int>& arr, int target,
                 MemoryTracer& mem, StepCallback cb) {
    int n   = static_cast<int>(arr.size());
    int lo  = 0;
    int hi  = n - 1;
    int step = 0, cmp = 0, swaps = 0; // swaps sempre 0 perchè le ricerche non fanno swap

    mem.pushFrame("binarySearch", {{"n",      std::to_string(n)},
                                   {"target", std::to_string(target)},
                                   {"lo",     "0"},
                                   {"hi",     std::to_string(hi)}});
    emitStep(step, cmp, swaps, arr, {}, mem, cb);  // stato iniziale

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;  
        ++cmp;
        mem.updateFrame({{"lo",      std::to_string(lo)},
                         {"hi",      std::to_string(hi)},
                         {"mid",     std::to_string(mid)},
                         {"arr[mid]",std::to_string(arr[mid])}});
        emitStep(step, cmp, swaps, arr, {lo, mid, hi}, mem, cb);  // intervallo corrente

        if (arr[mid] == target) {
            emitStep(step, cmp, swaps, arr, {mid}, mem, cb);  // trovato
            mem.popFrame();
            return mid;
        } else if (arr[mid] < target) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    emitStep(step, cmp, swaps, arr, {}, mem, cb);  // non trovato
    mem.popFrame();
    return -1;
}



// VERSIONI PURE PER BENCHMARK 
// su heap array
int linearSearchBench(const int* arr, int n, int target) {
    for (int i = 0; i < n; ++i) {
        if (arr[i] == target) return i;
    }
    return -1;
}

int binarySearchBench(const int* arr, int n, int target) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

// su lista
int linearSearchBenchList(const std::list<int>& lst, int target) {
    int idx = 0;
    // range based for loop per rif costante
    for (const int& val : lst) {
        if (val == target) return idx;
        ++idx;
    }
    return -1;
}

