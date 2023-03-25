#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
using namespace std;

using VI = vector<int>;
using Grafo = vector<VI>;
using VB = vector<bool>;
using QueueInt = queue<int>;

//Función que se encarga de leer el grafo y la probabilidad 
Grafo leerGrafo(double* prob, QueueInt& Q, VB& activados) {
    //Fichero que contiene el grafo:
    string nombreFichero = "grafo01IC.txt";
    ifstream file(nombreFichero.c_str());
    string line;

    //n -> vértices, m -> aristas
    int n, m;
    getline(file, line); n = atoi(line.c_str());
    getline(file, line); m = atoi(line.c_str());
    
    Grafo G(n);
    int u, v;
    //Leer aristas: 	
    while (m--) {
        getline(file, line); 
        stringstream ss(line);
        ss >> u; ss >> v;
        G[u].push_back(v);
        G[v].push_back(u);
    }

    //Probabilidad:
    getline(file, line); *prob = atof(line.c_str());
    
    //Subconjunto Q:
    Q = seleccionarSubconjunto(G, activados, prob);

    file.close();
    return G;
}

// Función para seleccionar el mínimo subconjunto Q que puede activar la mayor cantidad de nodos
QueueInt seleccionarSubconjunto(const Grafo& G, VB& activados, double prob) {
    int n = G.size();
    vector<pair<int, int>> nodos(n);

    for (int i = 0; i < n; i++) {
        nodos[i].first = G[i].size();
        nodos[i].second = i;
    }

    // Ordena los nodos según su grado (en orden descendente)
    sort(nodos.begin(), nodos.end(), greater<pair<int, int>>());

    // Realiza la influencia desde cada nodo ordenado y elige el que activa la mayor cantidad de nodos
    int maxActivados = -1;
    QueueInt subconjunto;
    for (int i = 0; i < n; i++) {
        int w = nodos[i].second;

        // Activa el nodo y realizar la difusión
        VB subactivados(n, false);
        subactivados[w] = true;
        QueueInt Q;
        Q.push(w);
        int t = 0;
        VI C = difusionIC(G, subactivados, prob, Q, &t);

        // Verifica si este subconjunto activa más nodos que el anterior máximo
        if (C.size() > maxActivados) {
            maxActivados = C.size();
            activados = subactivados;  // Actualizar el vector de nodos activados
            subconjunto = Q;  // Actualizar el subconjunto Q
        }
    }

    return subconjunto;
}

//Función que visita cada nodo adyacente a u y los activa con una probabilidad prob (condición del modelo IC). 
//Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influirNodos(const Grafo& G, VB& activados, VI& C, double prob, int u, int* t, QueueInt& Q) {
    bool influenciado = false;
    int pp = prob*10;
    for (int v : G[u]) {
        int random = rand()%9;
        if (!activados[v] && random < pp) {
            influenciado = true;
            activados[v] = true; 
            Q.push(v);
        }
    }
    if (influenciado) *t = *t + 1;
}

//La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según la probabilidad prob.
VI difusionIC(const Grafo& G, VB& activados, double ratio, QueueInt& Q, int* t) {
    VI C;
    while (not Q.empty()) {
        int w = Q.front(); Q.pop();
        C.push_back(w);
        influirNodos(G, activados, C, ratio, w, t, Q);
    } 
    return C;
}

int main () {
    //Semilla de los números aleatorios.
    srand(time(NULL)); 

    double prob;
    VI S;	
    VB activados;
    QueueInt Q;
    Grafo G = leerGrafo(&prob, Q, activados);
	
    int t = 0;
    VI C = difusionIC(G, activados, prob, Q, &t);

    //Mostrar respuesta:
    int size = C.size();
    cout << "Tamaño del conjunto C: " << size << endl;
    cout << "Número de pasos: " << t << endl;
    cout << "Nodos activados: {"; 
    for (int i = 0; i <= size-2; ++i) cout << C[i] << ", ";
    cout << C[size-1] << "}" << endl;
} 
