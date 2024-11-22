#include <iostream>
#include <fstream>
#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

// Estructura para cada entrada de la tabla de páginas (usada en Reloj)
struct EntradaTabla {
    int marcoFisico;   // Número del marco físico
    bool valido;       // Bit de validez
    bool referencia;   // Bit de referencia
};

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

// Simulación Reloj
int simularReloj(int numMarcos, const vector<int> &paginas) {
    // Tabla de páginas y memoria física para el algoritmo de Reloj
    unordered_map<int, EntradaTabla> tablaPaginasReloj;
    vector<int> memoriaFisicaReloj;
    int punteroReloj = 0; // Puntero circular para el algoritmo de Reloj
    int fallosPagina = 0;

    for (int pagina : paginas) {
        if (tablaPaginasReloj.find(pagina) != tablaPaginasReloj.end() && tablaPaginasReloj[pagina].valido) {
            // Página ya está en memoria
            tablaPaginasReloj[pagina].referencia = true;
        } else {
            // Fallo de página
            fallosPagina++;
            if (memoriaFisicaReloj.size() >= numMarcos) {
                while (true) {
                    int paginaARemover = memoriaFisicaReloj[punteroReloj];
                    if (tablaPaginasReloj[paginaARemover].referencia) {
                        tablaPaginasReloj[paginaARemover].referencia = false;
                        punteroReloj = (punteroReloj + 1) % numMarcos;
                    } else {
                        tablaPaginasReloj[paginaARemover].valido = false;
                        memoriaFisicaReloj[punteroReloj] = pagina;
                        tablaPaginasReloj[pagina] = {punteroReloj, true, true};
                        punteroReloj = (punteroReloj + 1) % numMarcos;
                        break;
                    }
                }
            } else {
                int marcoFisico = memoriaFisicaReloj.size();
                memoriaFisicaReloj.push_back(pagina);
                tablaPaginasReloj[pagina] = {marcoFisico, true, true};
            }
        }
    }
    return fallosPagina;
}
//Simulador Óptimo
int simularOPT(int numMarcos, const vector<int> &paginas) {
    // Memoria física
    vector<int> memoria;
    int fallosPagina = 0;

    for (size_t i = 0; i < paginas.size(); ++i) {
        int pagina = paginas[i];
        // Si la página ya está en memoria, no ocurre un fallo
        if (find(memoria.begin(), memoria.end(), pagina) != memoria.end()) {
            continue;
        }

        fallosPagina++;
        if (memoria.size() < numMarcos) {
            // Hay espacio libre en memoria, cargamos la página
            memoria.push_back(pagina);
        } else {
            // Memoria llena, necesitamos reemplazar una página
            int indiceReemplazo = -1;
            int usoMasLejano = -1;
            // Encontrar la página cuyo próximo uso está más lejos en el futuro
            for (size_t j = 0; j < memoria.size(); ++j) {
                int proximoUso = -1;
                for (size_t k = i + 1; k < paginas.size(); ++k) {
                    if (paginas[k] == memoria[j]) {
                        proximoUso = k;
                        break;
                    }
                }
                // Si la página no se usará más, reemplazarla
                if (proximoUso == -1) {
                    indiceReemplazo = j;
                    break;
                }
                // Actualizar el índice si esta página está más lejos en el futuro
                if (proximoUso > usoMasLejano) {
                    usoMasLejano = proximoUso;
                    indiceReemplazo = j;
                }
            }
            // Reemplazar la página más óptima
            memoria[indiceReemplazo] = pagina;
        }
    }
    return fallosPagina;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " <num_marcos> <algoritmo> <archivo_paginas>" << endl;
        cerr << "Algoritmos soportados: FIFO, LRU, RELOJ" << endl;
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
    } else if (algoritmo == "RELOJ") {
        fallosPagina = simularReloj(numMarcos, paginas);
    } else {
        cerr << "Algoritmo no reconocido: " << algoritmo << endl;
        cerr << "Algoritmos soportados: FIFO, LRU, RELOJ" << endl;
        return 1;
    }

    cout << "Número de fallos de página: " << fallosPagina << endl;

    return 0;
}
