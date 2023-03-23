#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <queue>
using namespace std;

using VI = vector<int>;
using Grafo = vector<VI>;
using VB = vector<bool>;
using QueueInt = queue<int>;

//Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
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
    int w;
    activados.resize(n);
    getline(file, line); w = atoi(line.c_str());
    Q.push(w); activados[w] = true;
    while (w != -1) {
        getline(file, line); w = atoi(line.c_str());
        if (w != -1) {Q.push(w); activados[w] = true;}
    }

    file.close();
    return G;
}

//Función que visita cada nodo adyacente a u y los activa con una probabilidad prob (condición del modelo IC). 
//Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(const Grafo& G, VB& activados, VI& C, double prob, int u, int* t, QueueInt& Q) {
    int pp = prob*10;
    for (int v : G[u]) {
        int random = rand()%9;
        if (!activados[v] && random < pp) {
            activados[v] = true; 
            Q.push(v);
        }
    }
    *t = *t + 1;
}

//La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según la probabilidad prob.
VI difusionIC(const Grafo& G, VB& activados, double ratio, QueueInt& Q, int* t) {
    VI C;
    while (not Q.empty()) {
        int w = Q.front(); Q.pop();
        C.push_back(w);
        influenciarNodos(G, activados, C, ratio, w, t, Q);
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
