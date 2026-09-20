#include "algorithms/sorting.hpp"
#include <string>
#include <algorithm>



// Bubble Sort  
void bubbleSort(std::vector<int>& arr, MemoryTracer& mem, StepCallback cb) {
    int n = static_cast<int>(arr.size());
    int step = 0, cmp = 0, swp = 0;

    mem.pushFrame("bubbleSort", {{"n", std::to_string(n)},
                                  {"i", "0"}, {"j", "0"}});
    emitStep(step, cmp, swp, arr, {}, mem, cb); 

    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            mem.updateFrame({{"i", std::to_string(i)},
                              {"j", std::to_string(j)}});
            ++cmp;
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                ++swp;
            }
            emitStep(step, cmp, swp, arr, {j, j + 1}, mem, cb); 
            // highlight {j, j+1}
            //evidenzia sempre la coppia attualmente confrontata, il frontend colora queste due barre in modo diverso
        }
    }
    emitStep(step, cmp, swp, arr, {}, mem, cb); 
    mem.popFrame();
}

// Insertion Sort 
void insertionSort(std::vector<int>& arr, MemoryTracer& mem, StepCallback cb) {
    int n = static_cast<int>(arr.size());
    int step = 0, cmp = 0, swp = 0;

    mem.pushFrame("insertionSort", {{"n", std::to_string(n)}, {"i", "1"}});
    emitStep(step, cmp, swp, arr, {}, mem, cb);

    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j   = i - 1;
        mem.updateFrame({{"i", std::to_string(i)},
                          {"key", std::to_string(key)},
                          {"j", std::to_string(j)}});

        while (j >= 0 && arr[j] > key) {
            ++cmp;
            arr[j + 1] = arr[j];
            ++swp;
            --j;
            mem.updateFrame({{"j", std::to_string(j)}});
            emitStep(step, cmp, swp, arr, {j + 1, j + 2}, mem, cb);
        }
        ++cmp;  
        arr[j + 1] = key;
        emitStep(step, cmp, swp, arr, {j + 1}, mem, cb);
    }
    emitStep(step, cmp, swp, arr, {}, mem, cb);
    mem.popFrame();
}


// Selection Sort
void selectionSort(std::vector<int>& arr, MemoryTracer& mem, StepCallback cb) {
    int n = static_cast<int>(arr.size());
    int step = 0, cmp = 0, swp = 0;

    mem.pushFrame("selectionSort", {{"n", std::to_string(n)}, {"i", "0"}});
    emitStep(step, cmp, swp, arr, {}, mem, cb);

    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        mem.updateFrame({{"i", std::to_string(i)},
                          {"min_idx", std::to_string(min_idx)}});

        for (int j = i + 1; j < n; ++j) {
            ++cmp;
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
                mem.updateFrame({{"min_idx", std::to_string(min_idx)},
                                  {"j", std::to_string(j)}});
            }
            emitStep(step, cmp, swp, arr, {j, min_idx}, mem, cb);
        }
        if (min_idx != i) {
            std::swap(arr[i], arr[min_idx]);
            ++swp;
        }
        emitStep(step, cmp, swp, arr, {i, min_idx}, mem, cb);
    }
    emitStep(step, cmp, swp, arr, {}, mem, cb);
    mem.popFrame();
}


// Merge Sort
// STRUTTURATO IN TRE FUNZIONI
// mergeSort() entry point pubblico, chiamata da main.cpp
// mergeSortRec() helper ricorsivo (static)
// mergeParts() merge di due sottarray ordinati (static)

static int  ms_step = 0, ms_cmp = 0, ms_swp = 0; 
// static perchè a livello di file permettono all'helper ricorsivo
// di accedere a stemp, cmp, swap ecc 
//senza passarli come param ad ogni chiamata ricorsiva 
static std::vector<int>* ms_arr_ptr = nullptr;
static MemoryTracer* ms_mem_ptr = nullptr;
static StepCallback ms_cb;

static void mergeParts(std::vector<int>& arr, int l, int m, int r,
                        MemoryTracer& mem, const StepCallback& cb) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // allocazione del buffer temporaneo sull'heap (tracciato dal MemoryTracer)
    std::vector<int> L(arr.begin() + l, arr.begin() + l + n1);
    std::vector<int> R(arr.begin() + m + 1, arr.begin() + m + 1 + n2);
    std::string L_str = "[";
    for(size_t idx=0; idx<L.size(); ++idx) L_str += std::to_string(L[idx]) + (idx+1==L.size() ? "" : ", ");
    L_str += "]";
    std::string R_str = "[";
    for(size_t idx=0; idx<R.size(); ++idx) R_str += std::to_string(R[idx]) + (idx+1==R.size() ? "" : ", ");
    R_str += "]";
    mem.heapAlloc("merge_buffer_L", n1, L_str);
    mem.heapAlloc("merge_buffer_R", n2, R_str);

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        ++ms_cmp;
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
        emitStep(ms_step, ms_cmp, ms_swp, arr, {k - 1}, mem, cb);
    }
    while (i < n1) { arr[k++] = L[i++]; emitStep(ms_step, ms_cmp, ms_swp, arr, {k-1}, mem, cb); }
    while (j < n2) { arr[k++] = R[j++]; emitStep(ms_step, ms_cmp, ms_swp, arr, {k-1}, mem, cb); }

    // free del buffer temporaneo
    mem.heapFree("merge_buffer_L");
    mem.heapFree("merge_buffer_R");
}

static void mergeSortRec(std::vector<int>& arr, int l, int r,
                          MemoryTracer& mem, const StepCallback& cb) {
    if (l >= r) return;
    int m = l + (r - l) / 2;

    mem.pushFrame("mergeSort", {{"l", std::to_string(l)},
                                 {"r", std::to_string(r)},
                                 {"m", std::to_string(m)}});
    emitStep(ms_step, ms_cmp, ms_swp, arr, {l, m, r}, mem, cb);

    mergeSortRec(arr, l, m, mem, cb);
    mergeSortRec(arr, m + 1, r, mem, cb);
    mergeParts(arr, l, m, r, mem, cb);

    mem.popFrame();
}

void mergeSort(std::vector<int>& arr, MemoryTracer& mem, StepCallback cb) {
    ms_step = 0; ms_cmp = 0; ms_swp = 0; // reset var statiche
    if (!arr.empty()) {
        mem.pushFrame("mergeSort_main");
        emitStep(ms_step, ms_cmp, ms_swp, arr, {}, mem, cb); // stato iniziale
        mergeSortRec(arr, 0, static_cast<int>(arr.size()) - 1, mem, cb);
        emitStep(ms_step, ms_cmp, ms_swp, arr, {}, mem, cb); // finale
        mem.popFrame();
    }
}


// Quick Sort 
// STRUTTURATO IN TRE FUNZIONI
// quickSort() entry point pubblico, chiamata da main.cpp
// quickSortRec() helper ricorsivo (static)
// partition() sceglie pivot e partiziona (static)

static int qs_step = 0, qs_cmp = 0, qs_swp = 0;

static int partition(std::vector<int>& arr, int low, int high,
                      MemoryTracer& mem, const StepCallback& cb) {
    int pivot = arr[high];
    int i     = low - 1;
    mem.updateFrame({{"pivot", std::to_string(pivot)},
                      {"i", std::to_string(i)}});

    for (int j = low; j < high; ++j) {
        ++qs_cmp;
        mem.updateFrame({{"j", std::to_string(j)}});
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
            ++qs_swp;
        }
        emitStep(qs_step, qs_cmp, qs_swp, arr, {i, j, high}, mem, cb);
    }
    std::swap(arr[i + 1], arr[high]);
    ++qs_swp;
    emitStep(qs_step, qs_cmp, qs_swp, arr, {i + 1}, mem, cb);
    return i + 1;
}

static void quickSortRec(std::vector<int>& arr, int low, int high,
                          MemoryTracer& mem, const StepCallback& cb) {
    if (low >= high) return;

    mem.pushFrame("quickSort", {{"low", std::to_string(low)},
                                 {"high", std::to_string(high)},
                                 {"pivot", std::to_string(arr[high])}});
    emitStep(qs_step, qs_cmp, qs_swp, arr, {low, high}, mem, cb);

    int pi = partition(arr, low, high, mem, cb);
    mem.popFrame();

    quickSortRec(arr, low, pi - 1, mem, cb);
    quickSortRec(arr, pi + 1, high, mem, cb);
}

void quickSort(std::vector<int>& arr, MemoryTracer& mem, StepCallback cb) {
    qs_step = 0; qs_cmp = 0; qs_swp = 0;
    if (!arr.empty()) {
        mem.pushFrame("quickSort_main");
        emitStep(qs_step, qs_cmp, qs_swp, arr, {}, mem, cb);
        quickSortRec(arr, 0, static_cast<int>(arr.size()) - 1, mem, cb);
        emitStep(qs_step, qs_cmp, qs_swp, arr, {}, mem, cb);
        mem.popFrame();
    }
}


// VERSIONI PURE PER BENCHMARK SU ARRAY 

void bubbleSortBench(int* arr, int n) {
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void insertionSortBench(int* arr, int n) {
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

void selectionSortBench(int* arr, int n) {
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            std::swap(arr[i], arr[min_idx]);
        }
    }
}

static void mergeBench(int* arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int* L = new int[n1];
    int* R = new int[n2];
    for(int i = 0; i < n1; ++i) L[i] = arr[l + i];
    for(int j = 0; j < n2; ++j) R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    while(i < n1 && j < n2) {
        if(L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while(i < n1) arr[k++] = L[i++];
    while(j < n2) arr[k++] = R[j++];
    delete[] L;
    delete[] R;
}

static void mergeSortRecBench(int* arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSortRecBench(arr, l, m);
    mergeSortRecBench(arr, m + 1, r);
    mergeBench(arr, l, m, r);
}

void mergeSortBench(int* arr, int n) {
    if (n > 1) mergeSortRecBench(arr, 0, n - 1);
}

static int partitionBench(int* arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

static void quickSortRecBench(int* arr, int low, int high) {
    if (low >= high) return;
    int pi = partitionBench(arr, low, high);
    quickSortRecBench(arr, low, pi - 1);
    quickSortRecBench(arr, pi + 1, high);
}

void quickSortBench(int* arr, int n) {
    if (n > 1) quickSortRecBench(arr, 0, n - 1);
}


// VERSIONI PURE PER BENCHMARK SU LISTA

void bubbleSortBenchList(std::list<int>& lst) {
    if (lst.size() < 2) return;
    bool swapped;
    do {
        swapped = false;
        auto it   = lst.begin();
        auto next = std::next(it);
        while (next != lst.end()) {
            if (*it > *next) {
                std::swap(*it, *next);  // swap dei valori, non dei puntatori
                swapped = true;
            }
            ++it;
            ++next;
        }
    } while (swapped);
}


void insertionSortBenchList(std::list<int>& lst) {
    if (lst.size() < 2) return;
    auto it = std::next(lst.begin());   
    while (it != lst.end()) {
        auto next_it = std::next(it);   
        int key = *it;
       
        auto pos = lst.begin();
        while (pos != it && *pos <= key) {
            ++pos;
        }

        if (pos != it) {
            lst.splice(pos, lst, it);
        }
        it = next_it;
    }
}


void selectionSortBenchList(std::list<int>& lst) {
    if (lst.size() < 2) return;
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        auto min_it = it;
        auto scan   = std::next(it);

        while (scan != lst.end()) {
            if (*scan < *min_it) {
                min_it = scan;
            }
            ++scan;
        }
 
        if (min_it != it) {
            std::swap(*it, *min_it);
        }
    }
}


static void mergeListsInPlace(std::list<int>& lst1, std::list<int>& lst2) {
    auto it1 = lst1.begin();
    while (!lst2.empty()) {
        while (it1 != lst1.end() && *it1 <= lst2.front()) {
            ++it1;
        }
        lst1.splice(it1, lst2, lst2.begin());
    }
}


static void mergeSortListRec(std::list<int>& lst) {
    if (lst.size() <= 1) return;

    std::list<int> right;
    auto mid = lst.begin();
    std::advance(mid, lst.size() / 2); 
    right.splice(right.begin(), lst, mid, lst.end()); 

    mergeSortListRec(lst);
    mergeSortListRec(right);

    mergeListsInPlace(lst, right);
}

void mergeSortBenchList(std::list<int>& lst) {
    mergeSortListRec(lst);
}


static void quickSortListRec(std::list<int>& lst) {
    if (lst.size() <= 1) return;

    int pivot = lst.front();
    lst.pop_front();

    std::list<int> less, greater;
    for (int val : lst) {
        if (val <= pivot) less.push_back(val);
        else              greater.push_back(val);
    }

    quickSortListRec(less);
    quickSortListRec(greater);

    less.push_back(pivot);
    less.splice(less.end(), greater);
    lst = std::move(less); 
}

void quickSortBenchList(std::list<int>& lst) {
    quickSortListRec(lst);
}


