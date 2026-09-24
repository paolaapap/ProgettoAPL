# Progetto Advanced Programming Languages (APL)

Piattaforma per la **visualizzazione step-by-step** e il **benchmarking** di algoritmi di ordinamento, ricerca e cammini minimi su grafi, con tracciamento visivo della memoria tra **Stack** ed **Heap** e supporto a diverse strutture dati.

---

## Architettura

| Livello | Linguaggio / Tecnologie | Ruolo |
| :--- | :--- | :--- |
| **Frontend** | Python 3.11+, PyQt6, Matplotlib, Requests | Interfaccia grafica, visualizzatore step-by-step, pannello memoria Stack/Heap, benchmark con boxplot e curva O(N). |
| **Middleware** | Go 1.21+, `net/http`, Goroutines, Channels | Server HTTP REST locale, Worker Pool concorrente per l'esecuzione isolata dei task, supervisione dei sottoprocessi C++. |
| **Backend** | C++20, STL, nlohmann/json | Esecuzione algoritmi, campionamento statistico multi-run, memory tracking di stack frame e blocchi heap, I/O JSON su stream standard. |

---

## Gestione delle Dipendenze

### Python — `pip` + `requirements.txt`
```bash
cd frontend
pip install -r requirements.txt
```

### Go — modulo standard `go.mod`
Non è necessario installare nulla manualmente. Il modulo Go (`apl_middleware`) usa esclusivamente la standard library. Le dipendenze vengono risolte automaticamente da `go run` o `go build`.

### C++ — CMake + header-only incluse
Le librerie di terze parti (es. `nlohmann/json`) sono già incluse in `backend/include/` e non richiedono installazione separata. È sufficiente avere CMake e un compilatore C++20 (es. MinGW-w64 su Windows).

---

## Avvio della Piattaforma

> ⚠️ L'applicazione richiede **tre terminali aperti in parallelo**, uno per ogni componente. Seguire l'ordine indicato.

---

### Terminale 1 — Compilare il Backend (C++)

Aprire un primo terminale ed eseguire:

```bash
cd backend
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

L'eseguibile viene generato in `backend/build/backend.exe`.  
**Questo terminale può essere chiuso al termine della compilazione.**

---

### Terminale 2 — Avviare il Middleware (Go)

Aprire un **secondo terminale** ed eseguire:

```bash
cd middleware
go run main.go -backend "..\backend\build\backend.exe"
```

Il server si avvierà e rimarrà in ascolto su `http://localhost:8080`. **Non chiudere questo terminale.**

---

### Terminale 3 — Avviare il Frontend (Python)

Aprire un **terzo terminale** ed eseguire:

```bash
cd frontend
python main.py
```

Si aprirà la finestra dell'applicazione. **Non chiudere questo terminale.**

---

## Funzionalità dell'Applicazione

### Tab "Visualizzatore"

Questa schermata permette di eseguire un algoritmo e osservarne l'andamento passo dopo passo in modo animato.

**Algoritmi disponibili:**
- *Ordinamento*: Bubble Sort, Insertion Sort, Selection Sort, Merge Sort, Quick Sort.
- *Ricerca*: Linear Search, Binary Search (con ordinamento automatico preventivo dell'array).
- *Grafi*: Algoritmo di Dijkstra per cammini minimi.

**Cosa si trova l'utente davanti:**
- Un **menu a tendina** per selezionare l'algoritmo tra quelli elencati sopra.
- Un campo per impostare la **dimensione N** dell'array e un pulsante per generarlo casualmente, oppure un campo per **inserire manualmente** i valori separati da virgola.
- Per gli algoritmi di ricerca, un campo aggiuntivo per specificare il **valore target** da cercare.

**Flusso di utilizzo:**
1. Scegliere l'algoritmo dal menu.
2. Inserire o generare l'array di input.
3. Cliccare **"Avvia Visualizzazione"**.
4. Usare i pulsanti **Play / Pausa** e lo **slider della velocità** per controllare l'animazione.
5. Osservare il grafico a barre aggiornarsi ad ogni step, con gli elementi attivi evidenziati e i contatori di **Confronti** e **Swap** aggiornati in tempo reale.
6. In basso, le sezioni **Stack** e **Heap** mostrano come la memoria viene occupata e liberata durante l'esecuzione (particolarmente visibile in algoritmi ricorsivi come MergeSort e QuickSort).

---

### Tab "Benchmark"

Questa schermata permette di misurare e confrontare le prestazioni degli algoritmi in modo statisticamente rigoroso.

**Strutture Dati a Confronto:**
- **Heap Array**: Array allocato esull'Heap.
- **Linked List**: Doubly linked list standard C++.
- *Regola di compatibilità*: per algoritmi come Binary Search (che richiede accesso indicizzato O(1)) e Dijkstra (grafo), l'opzione Linked List viene automaticamente disabilitata.

**Distribuzioni dell'Input:**
- `random` — dati casuali non correlati.
- `sorted` — caso migliore/peggiore in base all'algoritmo.
- `reversed` — ordinamento inverso.
- `nearly_sorted` — ordinato con il 5% di scambi casuali.

**Modalità di Analisi:**
- **Boxplot (Singolo N)**: Esegue M run indipendenti (default 30) a parità di dimensione N. Calcola Min, Max, Media, Mediana, Q1 (25° percentile), Q3 (75° percentile), Deviazione Standard e genera un Boxplot comparativo con storico cumulabile.
- **Curva O(N)**: Esegue misurazioni incrementali al variare di N tracciando empiricamente la curva di complessità temporale asintotica dell'algoritmo.

**Cosa si trova l'utente davanti:**
- Un **menu a tendina** per selezionare l'algoritmo da analizzare.
- Controlli per impostare il **numero di run** (default: 30), la **distribuzione dell'input** e la **struttura dati** su cui operare.
- Due modalità di analisi selezionabili: **Boxplot** (singolo N) e **Curva O(N)**.

**Flusso di utilizzo — Boxplot:**
1. Selezionare l'algoritmo e impostare N e il numero di run.
2. Scegliere la distribuzione e la struttura dati.
3. Cliccare **"Avvia Benchmark"**.
4. Al termine, viene mostrato un **boxplot** con la distribuzione dei tempi di esecuzione sui run effettuati, affiancato da una tabella con Min, Max, Media, Mediana, Q1, Q3 e Deviazione Standard.

**Flusso di utilizzo — Curva O(N):**
1. Selezionare l'algoritmo e impostare l'intervallo di N (da, a, passo).
2. Cliccare **"Avvia Curva"**.
3. Al termine, viene tracciato un grafico che mostra empiricamente come il tempo di esecuzione cresce al crescere di N, permettendo di verificare visivamente la complessità asintotica dell'algoritmo.

