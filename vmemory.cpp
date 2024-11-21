#include <iostream>
#include <fstream>
#include <unordered_map>
#include <list>
#include <vector>
#include <string>

using namespace std;

// Función para leer el archivo con las referencias de páginas
vector<int> leerArchivoPaginas(const string &nombreArchivo) {
    ifstream archivo(nombreArchivo);
    vector<int> paginas;
    int pagina;

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        exit(1);
    }

    while (archivo >> pagina) {
        paginas.push_back(pagina);
    }

    archivo.close();
    return paginas;
}

// Simulación FIFO
int simularFIFO(int numMarcos, const vector<int> &paginas) {
    unordered_map<int, bool> tablaPaginas;  // Mapa para verificar presencia de páginas en memoria
    list<int> cola;                        // Cola de páginas 
    int fallosPagina = 0;

    for (int pagina : paginas) {
        if (tablaPaginas.find(pagina) == tablaPaginas.end()) { // Página no está en memoria
            fallosPagina++;
            if (cola.size() == numMarcos) { // Memoria llena
                int paginaAntigua = cola.front();
                cola.pop_front();
                tablaPaginas.erase(paginaAntigua);
            }
            cola.push_back(pagina);
            tablaPaginas[pagina] = true;
        }
    }
    return fallosPagina;
}

// Simulación LRU
int simularLRU(int numMarcos, const vector<int> &paginas) {
    unordered_map<int, list<int>::iterator> tablaPaginas; // Mapa: página -> posición en la lista
    list<int> listaLRU;                                  // Lista para controlar el orden de uso
    int fallosPagina = 0;

    for (int pagina : paginas) {
        if (tablaPaginas.find(pagina) == tablaPaginas.end()) { // Página no está en memoria
            fallosPagina++;
            if (listaLRU.size() == numMarcos) { // Memoria llena
                int paginaAntigua = listaLRU.back();
                listaLRU.pop_back();
                tablaPaginas.erase(paginaAntigua);
            }
        } else { // Página ya está en memoria, se mueve al frente (más recientemente usada)
            listaLRU.erase(tablaPaginas[pagina]);
        }
        listaLRU.push_front(pagina);
        tablaPaginas[pagina] = listaLRU.begin();
    }

    return fallosPagina;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " <num_marcos> <algoritmo> <archivo_paginas>" << endl;
        cerr << "Algoritmos soportados: FIFO, LRU" << endl;
        return 1;
    }

    int numMarcos = stoi(argv[1]);
    string algoritmo = argv[2];
    string nombreArchivo = argv[3];

    vector<int> paginas = leerArchivoPaginas(nombreArchivo);

    int fallosPagina = 0;
    if (algoritmo == "FIFO") {
        fallosPagina = simularFIFO(numMarcos, paginas);
    } else if (algoritmo == "LRU") {
        fallosPagina = simularLRU(numMarcos, paginas);
    } else {
        cerr << "Algoritmo no reconocido: " << algoritmo << endl;
        cerr << "Algoritmos soportados: FIFO, LRU" << endl;
        return 1;
    }

    cout << "Número de fallos de página: " << fallosPagina << endl;

    return 0;
}
