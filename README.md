# Progetto Advanced Programming Languages (APL)

Piattaforma per la **visualizzazione step-by-step** e l'**benchmarking** di algoritmi di ordinamento, ricerca e cammini minimi su grafi, con tracciamento visivo della memoria tra **Stack** ed **Heap** e supporto a diverse strutture dati (**Array contiguo su Heap** e **Doubly Linked List** `std::list`).

---

## Architettura 

| Livello | Linguaggio / Tecnologie | Ruolo e Responsabilità |
| :--- | :--- | :--- |
| **Frontend (GUI)** | **Python 3.11+**<br>PyQt6, Matplotlib, NetworkX, Requests | Interfaccia grafica utente, visualizzatore dinamico di step e grafi, pannello memoria Stack/Heap, configuratore ed esecutore di benchmark con boxplot e curve $O(N)$. |
| **Middleware** | **Go 1.21+**<br>`net/http`, Goroutines, Channels, `sync` | Server HTTP REST locale, gestione del **Worker Pool concorrente** per accodamento ed esecuzione isolata dei task, monitoraggio e supervisione dei sottoprocessi C++. |
| **Backend (Core)** | **C++20**<br>STL (`std::vector`, `std::list`, `std::chrono`), nlohmann/json | Esecuzione degli algoritmi, campionamento statistico multi-run con seed indipendenti, memory tracking di stack frame e blocchi heap, I/O JSON su standard stream. |

---

## Gestione Dipendenze e Prerequisiti

Ciascun componente possiede il proprio sistema standard di gestione e installazione delle dipendenze:

### 1. Python 
Disponibili tre formati standard:
- `frontend/requirements.txt`: installabile direttamente tramite `pip`.
- `frontend/pyproject.toml`: compatibile con installer come ad esmepio `uv`o `pip`, `flit` o `build`.
- `frontend/environment.yml`: per chi utilizza ambienti virtuali basati su `conda` / `mamba`.

### 2. Go 
- `middleware/go.mod`: definisce il modulo Go standard (`apl_middleware`). Utilizza esclusivamente la standard library (`net/http`, `os/exec`, `sync`, `encoding/json`).

### 3. C++
- Librerie di terze parti header-only incluse direttamente in `backend/include/` (`nlohmann/json.hpp`).
- Sistema di build doppio:
  - **CMake** (`backend/CMakeLists.txt`)
  - **Makefile standalone cross-platform** (`backend/Makefile`) per compilazione diretta con `make` o `mingw32-make`.

---

## Guida all'Avvio della piattaforma

Per avviare la piattaforma, aprire tre terminali dedicati ai tre componenti.

### 1. Compilare il Backend (C++)

È possibile compilare sia tramite **Makefile** che tramite **CMake**.

#### Opzione A: Tramite Makefile 
```bash
cd backend
mingw32-make clean
mingw32-make -j4
```
L'eseguibile compilato verrà salvato in `backend/build/backend.exe`.

#### Opzione B: Tramite CMake
```bash
cd backend
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

### 2. Avviare il Middleware (Go)

Il middleware coordina le richieste ed esegue `backend.exe`:

```bash
cd middleware
go run main.go -backend "..\backend\build\backend.exe"
```
*In alternativa, è possibile compilare ed eseguire l'eseguibile binario:*
```bash
go build -o middleware.exe main.go
.\middleware.exe -backend "..\backend\build\backend.exe"
```
Il server avvierà il Worker Pool e si metterà in ascolto su `http://localhost:8080`.

---

### 3. Avviare il Frontend (Python)

#### Creazione e attivazione virtual environment 
```bash
cd frontend
python -m venv .venv

# Su Windows (PowerShell):
.venv\Scripts\Activate.ps1
# Su Linux/macOS:
# source .venv/bin/activate
```

#### Installazione dipendenze
È possibile usare uno dei metodi supportati:

- **Tramite pip:**
  ```bash
  pip install -r requirements.txt
  ```
- **Oppure tramite uv (se installato):**
  ```bash
  uv pip install -r requirements.txt
  ```
- **Oppure tramite Conda:**
  ```bash
  conda env create -f environment.yml
  conda activate apl-frontend
  ```

#### Esecuzione dell'applicazione
```bash
python main.py
```

---

## Funzionalità dell'Applicazione

### 1. Tab "Visualizzatore"
- **Selezione da parte dell'utente dell'algoritmo**:
  - *Ordinamento*: Bubble Sort, Insertion Sort, Selection Sort, Merge Sort, Quick Sort.
  - *Ricerca*: Linear Search, Binary Search (con ordinamento automatico preventivo dell'array).
  - *Grafi*: Algoritmo di Dijkstra per cammini minimi.
- **Controllo da parte dell'utente sui dati**:
  - Generazione array casuale con dimensione $N$.
  - Inserimento manuale di valori separati da virgola.
  - Generazione casuale con preview per impostare il target di ricerca (utile per gli algoritmi di ricerca).
- **Player**:
  - Pulsanti Play / Pausa, Step Precedente, Step Successivo.
  - Slider di regolazione velocità di playback (millisecondi per frame).
  - Conteggio e visualizzazione delle metriche intrinseche indipendenti dall'hardware (**Confronti** e **Swap**).
- **Rappresentazione della Memoria (Stack & Heap)**:
  - **Stack View**: Traccia i frame attivi di funzione, evidenziando i parametri correnti e la profondità di ricorsione (es. chiamate ricorsive di MergeSort e QuickSort).
  - **Heap View**: Visualizza le allocazioni dinamiche temporanee (es. i buffer `merge_buffer_L` e `merge_buffer_R` istanziati durante il merge).

### 2. Tab "Benchmark"
- **Strutture Dati a Confronto**:
  - **Heap Array**: Array contiguo allocato esplicitamente sull'Heap tramite raw pointer. 
  - **Linked List**: Doubly linked list standard C++. 
  - *Regola di compatibilità*: per algoritmi come Binary Search (che richiede accesso indicizzato $O(1)$) e Dijkstra (grafo), l'opzione Linked List viene automaticamente disabilitata.
- **Distribuzioni dell'Input**:
  - `random` (dati casuali non correlati)
  - `sorted` (caso migliore/peggiore in base all'algoritmo)
  - `reversed` (ordinamento inverso)
  - `nearly_sorted` (ordinato con il 5% di scambi casuali)
- **Modalità di Analisi**:
  - **Boxplot (Singolo N)**: Esegue $M$ run indipendenti (default 30) a parità di dimensione $N$. Calcola Min, Max, Media, Mediana, Q1 (25° percentile), Q3 (75° percentile), Deviazione Standard e genera un Boxplot comparativo con storico cumulabile.
  - **Curva O(N)**: Esegue misurazioni incrementali al variare di $N$ tracciando empiricamente la curva di complessità temporale asintotica dell'algoritmo.
