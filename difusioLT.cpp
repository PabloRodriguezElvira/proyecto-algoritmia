#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
using namespace std;

struct Node {
    double llindar;
    int influencia;
};

using VI = vector<int>;
using Grafo = vector<VI>;
using VNode = vector<Node>;
using VB = vector<bool>;
using QueueInt = queue<int>;

//Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
Grafo leerGrafo(double* ratio, QueueInt& Q, VB& activados, VNode& nodos) {
    //Fichero que contiene el grafo:
    string nombreFichero = "grafo01LT.txt";
    ifstream file(nombreFichero.c_str());
    string line;

    //n -> vértices, m -> aristas
    int n, m;
    getline(file, line); n = atoi(line.c_str());
    getline(file, line); m = atoi(line.c_str());
    
    Grafo G(n);
    nodos.resize(n);

    //Probabilidad:
    getline(file, line); *ratio = atof(line.c_str());

    //Leer aristas: 	
    int u, v;
    while (m--) {
        getline(file, line); 
        stringstream ss(line);
        ss >> u; ss >> v;
        nodos[u].llindar += *ratio;
        nodos[v].llindar += *ratio; 
        G[u].push_back(v);
        G[v].push_back(u);
    }

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

//Función que visita cada nodo adyacente a u y los activa según si cumplen la condición del modelo de difusión LT. 
//Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(const Grafo& G, VB& activados, VI& C, double ratio, int u, int* t, VNode& nodos, QueueInt& Q) {
    bool influenciado = false;
    for (int v : G[u]) {
        //Incrementamos en 1 la influencia de cada nodo vecino de u.
        ++nodos[v].influencia;
        if (!activados[v] && nodos[v].influencia >= nodos[v].llindar) {
            influenciado = true;
            activados[v] = true; 
            Q.push(v);
        }
    }
    if (influenciado) *t = *t + 1;
}

//La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según si cumplen la condición
//del modelo LT.
VI difusionLT(const Grafo& G, VB& activados, double ratio, QueueInt& Q, int* t, VNode& nodos) {
    VI C;
    while (not Q.empty()) {
        int w = Q.front(); Q.pop();
        C.push_back(w);
        influenciarNodos(G, activados, C, ratio, w, t, nodos, Q);
	//Sacar el conjunto de nodos activados a cada iteracion. Si alguno de estos nodos activa algun otro nodo, ya 
	//cuenta como t+1.
    } 
    return C;
}

int main () {
    double ratio;
    QueueInt Q;
    VB activados;
    VNode nodos;
    Grafo G = leerGrafo(&ratio, Q, activados, nodos);
	
    int t = 0;
    VI C = difusionLT(G, activados, ratio, Q, &t, nodos);

    //Mostrar respuesta:
    int size = C.size();
    cout << "Tamaño del conjunto C: " << size << endl;
    cout << "Número de pasos: " << t << endl;
    cout << "Nodos activados: {"; 
    for (int i = 0; i <= size-2; ++i) cout << C[i] << ", ";
    cout << C[size-1] << "}" << endl;
} 
