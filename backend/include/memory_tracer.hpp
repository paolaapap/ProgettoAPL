#pragma once
#include <map>
#include <string>
#include <vector>

// ---------------------------------------------------------
// Strutture dati per gli snapshot di memoria Stack/Heap
// ---------------------------------------------------------

// Rappresenta uno stack frame (record di attivazione di una funzione):
struct MemFrame {
  std::string func_name; // nome della funzione in esecuzione 
  std::map<std::string, std::string> vars;  // mappa che associa nomeVariabile - valore
                                            // il valore è una stringa perchè le variabili tracciate hanno tipi eterogenei
};

// Rappresenta un blocco di memoria allocato dinamicamente sull'heap
struct HeapBlock {
  std::string label;   // es. "merge_buffer"
  int size;            // numero di elementi nel blocco 
  std::string content; // contenuto dell'array (es. "[1, 2, 3]")
};

// Snapshot completo di memoria in un dato istante 
// sequenza di stack frame + allocazioni heap attive 
struct MemorySnapshot {
  std::vector<MemFrame> stack;
  std::vector<HeapBlock> heap;
};

/**
 * ============================================================================
 *  MEMORY TRACER (SIMULATORE LOGICO DI MEMORIA)
 * ============================================================================
 * 
 * - Gestisce un modello concettuale e pedagogico dell'uso della memoria
 * - Viene pilotato esplicitamente dagli algoritmi durante l'esecuzione
 * - Non traccia lo stato reale a basso livello della RAM fisica o dell'OS
 * 
 * Principi di funzionamento:
 * 
 *    Gli algoritmi invocano direttamente i metodi del tracer per dichiarare
 *    ogni evento saliente come ad esempio l' ingresso in una funzione con 
 *    le relative variabili.
 * 
 *    Il tracer ignora l'effettiva allocazione fisica di memoria; registra
 *    soltanto la semantica logica definita nel codice.
 * 
 * ============================================================================
 */

class MemoryTracer {
public:
  MemoryTracer() = default;
  ~MemoryTracer() = default;

  // Disabilito costruttore di copia e operatore di assegnazione di copia
  // al fine di impedire la duplicazione dell'oggetto
  MemoryTracer(const MemoryTracer &) = delete;
  MemoryTracer &operator=(const MemoryTracer &) = delete;

  // Aggiunge un frame allo stack 
  void pushFrame(const std::string &funcName,
                 const std::map<std::string, std::string> &vars = {});

  // Rimuove il frame più recente dallo stack 
  void popFrame();

  // Aggiorna le variabili del frame corrente 
  void updateFrame(const std::map<std::string, std::string> &vars);

  // Registra un'allocazione heap
  void heapAlloc(const std::string &label, int size,
                 const std::string &content = "");

  // Rimuove un'allocazione heap
  void heapFree(const std::string &label);

  // Ritorna uno snapshot dello stato corrente
  MemorySnapshot snapshot() const;

  // Reset per ogni run
  void reset();

private:
  std::vector<MemFrame> stack_;
  std::vector<HeapBlock> heap_;
};
