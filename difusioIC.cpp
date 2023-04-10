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
using queueInt = queue<int>;

struct Solucion {
    VI C; //Conjunto de Nodos activados
    int t; //Pasos de tiempo realizados por la difusión.
};

//Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
Grafo leerGrafo(double* prob, queueInt& S, const char* nombreFichero) {
    //Fichero que contiene el grafo:
    ifstream file(nombreFichero);
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
        
        //Adyacencias.
        G[u].push_back(v);
        G[v].push_back(u);
    }

    //Probabilidad:
    getline(file, line); *prob = atof(line.c_str());

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

//Función que visita cada nodo adyacente a u y los activa con una probabilidad prob (condición del modelo IC). 
//Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(const Grafo& G, VB& activados, Solucion& sol, const double& prob, const int& u, queueInt& S) {
    int pp = prob*10;
    for (int v : G[u]) {
        int random = rand()%9;
        if (!activados[v] && random < pp) {
            activados[v] = true;
            S.push(v);
        }
    }
    //Aumentamos en 1 los pasos de difusión.
    sol.t = sol.t + 1;
}

//La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según la probabilidad prob.
Solucion difusionIC(const Grafo& G, const queueInt& S, const double& prob) {

    //Copia de la cola:
    queueInt Saux = S;

    Solucion sol; 
    sol.t = 0;
    int n = G.size();
    VB activados(n, false);

    //Mientras la cola no esté vacía, hacemos un paso de difusión desde el nodo que esté en el front de la cola.
    while (not Saux.empty()) {
        int w = Saux.front(); Saux.pop();

        activados[w] = true; //Marcamos nodo como activado.
        sol.C.push_back(w); //Añadimos el nodo al conjunto solución de nodos activados.

        //Realizamos la difusión:
        influenciarNodos(G, activados, sol, prob, w, Saux);
    } 

    return sol;
}

int main (int argc, char** argv) {
    if (argc != 2) {
        cout << "El uso del programa es: ./difusioIC <fichero_grafo>" << endl;
        exit(1);
    }
    else {
        //Semilla de los números aleatorios.
        srand(time(NULL)); 

        double prob;
        queueInt S;
        Grafo G = leerGrafo(&prob, S, argv[1]);

        //A difusionIC le pasamos la entrada -> el grafo G, el subconjunto de vertices S y la prob p. (o ratio r).
        Solucion sol = difusionIC(G, S, prob);

        //Mostrar respuesta:
        int size = sol.C.size();
        cout << "Tamaño del conjunto C: " << size << endl;
        cout << "Número de pasos: " << sol.t << endl;
        cout << "Nodos activados: {"; 
        for (int i = 0; i <= size-2; ++i) cout << sol.C[i] << ", ";
        cout << sol.C[size-1] << "}" << endl;
    }
} 
