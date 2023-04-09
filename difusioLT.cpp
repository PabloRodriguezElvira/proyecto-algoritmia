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
using VNode = vector<Node>;
using AdjList = vector<VI>;
using Grafo = pair<AdjList, VNode>;
using VB = vector<bool>;
using QueueInt = queue<int>;

struct Solucion {
    VI C; //Conjunto de Nodos activados
    int t; //Pasos de tiempo realizados por la difusión.
};

//Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
Grafo leerGrafo(double* ratio, QueueInt& S, const char* nombreFichero) {
    //Fichero que contiene el grafo:
    ifstream file(nombreFichero);
    string line;

    //n -> vértices, m -> aristas
    int n, m;
    getline(file, line); n = atoi(line.c_str());
    getline(file, line); m = atoi(line.c_str());

    //Ratio:
    getline(file, line); *ratio = atof(line.c_str());
    
    AdjList adj(n);
    VNode nodos(n);
    Grafo G = make_pair(adj, nodos);

    //Leer aristas: 	
    int u, v;
    while (m--) {
        getline(file, line); 
        stringstream ss(line);
        ss >> u; ss >> v;

        //Adyacencias:
        G.first[u].push_back(v);
        G.first[v].push_back(u);

        //Como el peso de todas las aristas es 1, incrementamos cada umbral en ratio.
        G.second[u].llindar += *ratio;
        G.second[v].llindar += *ratio;

        //La influencia recibida inicial de todos los nodos es 0.
        G.second[u].influencia = 0;
        G.second[v].influencia = 0;
    }

    //Subconjunto S:
    int w;
    getline(file, line); w = atoi(line.c_str());
    S.push(w);
    while (w != -1) {
        getline(file, line); w = atoi(line.c_str());
        if (w != -1) S.push(w);
    }

    file.close();
    return G;
}

//Función que visita cada nodo adyacente a u y los activa según si cumplen la condición del modelo de difusión LT. 
//Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(Grafo& G, VB& activados, Solucion& sol, const double& ratio, const int& u, QueueInt& S) {
    bool influenciado = false;
    for (int v : G.first[u]) {
        //Incrementamos en 1 la influencia de cada nodo vecino de u.
        ++G.second[v].influencia;
        if (!activados[v] && G.second[v].influencia >= G.second[v].llindar) {
            influenciado = true;
            activados[v] = true; 
            S.push(v);
        }
    }
    //Aumentamos en 1 los pasos de difusión.
    if (influenciado) sol.t = sol.t + 1;
}

//La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según si cumplen la condición
//del modelo LT.
Solucion difusionLT(Grafo& G, QueueInt& S, const double& ratio) {
    Solucion sol; 
    sol.t = 0;
    int n = G.second.size();
    VB activados(n, false);

    while (not S.empty()) {
        int w = S.front(); S.pop();

        activados[w] = true; //Marcamos nodo como activado.
        sol.C.push_back(w); //Añadimos el nodo al conjunto solución de nodos activados.

        //Realizamos la difusión:
        influenciarNodos(G, activados, sol, ratio, w, S);
    } 

    return sol;
}

int main (int argc, char** argv) {
    if (argc != 2) {
        cout << "El uso del programa es: ./difusioLT <fichero_grafo>" << endl;
        exit(1);
    }
    else {
        double ratio;
        QueueInt S;
        Grafo G = leerGrafo(&ratio, S, argv[1]);
        
        //A difusionIC le pasamos la entrada -> el grafo G, el subconjunto de vertices S y la prob p. (o ratio r).
        Solucion sol = difusionLT(G, S, ratio);

        //Mostrar respuesta:
        int size = sol.C.size();
        cout << "Tamaño del conjunto C: " << size << endl;
        cout << "Número de pasos: " << sol.t << endl;
        cout << "Nodos activados: {"; 
        for (int i = 0; i <= size-2; ++i) cout << sol.C[i] << ", ";
        cout << sol.C[size-1] << "}" << endl;
    }
} 
