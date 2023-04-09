#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <chrono>
using namespace std;

using VI = vector<int>;
using Grafo = vector<VI>;
using VB = vector<bool>;
using QueueInt = queue<int>;

struct Solucion {
    VI C; //Conjunto de Nodos activados
    int t; //Pasos de tiempo realizados por la difusión.
};

//Función que se encarga de leer el grafo, la probabilidad
Grafo leerGrafo(double* prob, const char* nombreFichero) {
    //Fichero que contiene el grafo:
    ifstream file(nombreFichero);
    string line;
    stringstream ss;

    //n -> vértices, m -> aristas
    int n, m;
    string str;
    getline(file, line);
    ss = stringstream(line);
    ss >> str; ss >> str;
    ss >> n; ss >> m;

    Grafo G(n);

    //Probabilidad:
    getline(file, line);
    ss = stringstream(line);
    ss >> str; ss >> (*prob);


    int u, v;
    //Leer aristas:
    while (m--) {
        getline(file, line);
        ss = stringstream(line);
        ss >> str;
        ss >> u; ss >> v;
        --u; --v;
        G[u].push_back(v);
        G[v].push_back(u);
    }

    file.close();
    return G;
}

//Función que visita cada nodo adyacente a u y los activa con una probabilidad prob (condición del modelo IC).
//Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(const Grafo& G, VB& activados, Solucion& sol, const double& prob, const int& u, QueueInt& Q) {
    int pp = prob*10;
    for (int v : G[u]) {
        int random = rand()%9;
        if (!activados[v] && random < pp) {
            activados[v] = true;
            Q.push(v);
        }
    }
    //Aumentamos en 1 los pasos de difusión.
    sol.t = sol.t + 1;
}

//La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según la probabilidad prob.
Solucion difusionIC(const Grafo& G, const VI& S, const double& prob) {
    Solucion sol;
    sol.t = 0;
    int n = G.size();
    QueueInt Q;
    VB activados(n, false);

    //Activamos los vértices de S y los volcamos a la cola Q.
    for (int v : S) {
        activados[v] = true;
        Q.push(v);
    }

    //Mientras la cola no esté vacía, hacemos un paso de difusión desde el nodo que esté en el front de la cola.
    while (not Q.empty()) {
        //Añadimos el nodo al conjunto solución de nodos activados.
        int w = Q.front(); Q.pop();
        sol.C.push_back(w);

        //Realizamos la difusión:
        influenciarNodos(G, activados, sol, prob, w, Q);
    }

    return sol;
}

//Función que ordena los nodos según su grado en orden decreciente y los devuelve en un vector.
VI ordenarPorGrado(const Grafo& G) {
    int n = G.size();
    vector<pair<int,int>> nodosGrado(n);
    for (int u = 1; u <= n; ++u) {
        int grado = G[u].size();
        nodosGrado[u] = make_pair(grado, u);
    }
    sort(nodosGrado.rbegin(), nodosGrado.rend());
    VI nodosOrdenados(n);
    for (int i = 0; i < n; ++i) nodosOrdenados[i] = nodosGrado[i].second;
    return nodosOrdenados;
}

//Función que encontra una semilla de tamaño k en un grafo usando el modelo de difusión de la cascada (IC)
VI greedy(const Grafo& G, const VI& nodosOrdenados, const double& prob, Solucion& sol) {
    VI S;
    S.push_back(nodosOrdenados[0]);

    Solucion res = difusionIC(G, S, prob);
    int max = res.C.size();

    VI aux;
    aux = S;
    int n = G.size();
    for (int i = 1; i < n; ++i) {
        aux.push_back(nodosOrdenados[i]);
        Solucion ans = difusionIC(G, aux, prob);
        int tam = sol.C.size();

        if (max >= tam) break;
        else res = ans;
    }

    S = aux;
    sol = res;
    return S;
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
        VB activados;
        VI S;
        Grafo G = leerGrafo(&prob, argv[1]);
        VI nodosOrdenados = ordenarPorGrado(G);

        Solucion sol;
        auto start = std::chrono::steady_clock::now();
        S = greedy(G, nodosOrdenados, prob, sol);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        cout << "Tamaño del subconjunto S: " << S.size() << endl;
        cout << "Tamaño del conjunto C: " << sol.C.size() << endl;
        cout << "Número de pasos: " << sol.t << endl;
        std::cout << "Tiempo de ejecución: " << elapsed_seconds.count() << " segundos" << std::endl;
    }
}
