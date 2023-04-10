#include "difusioIC.cpp"
#include <algorithm>
#include <chrono>
using namespace std;

using VPair = vector<pair<int, int>>;

const bool cmp (pair<int,int> a, pair<int,int> b) {
    return a.second > b.second;
}

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

//Función que ordena los nodos según su grado en orden decreciente y los devuelve en un vector.
VPair ordenarPorGrado(const Grafo& grafo) {
    int n = grafo.size();
    VPair pares(n);

    for (int i = 0; i < n; ++i) {
        int grado_i = grafo[i].size(); //Grado del vértice.
        pares.push_back({i, grado_i});
    }

    //Ordenamos de forma decreciente por grado del vértice.
    sort(pares.begin(), pares.end(), cmp);

    return pares;
}

//Encuentra el subconjunto mínimo inicial (queueInt) y los nodos activados (solucion) usando la estrategia greedy.
queueInt greedy(const Grafo& G, const VPair& nodosOrdenados, const double& prob, Solucion& sol) { 
    int max = -1;

    queueInt S, SS;
    // Solucion res;

    int n = G.size();
    for (int i = 0; i < n; ++i) {
        SS.push(nodosOrdenados[i].first);
        Solucion ans = difusionIC(G, SS, prob);
        int tam = ans.C.size();
        if (max >= tam) break;
        else { //Se guarda el resultado de la ejecución i-1;
           max = tam;
           sol = ans;
           S = SS;
        }
    }

    // sol = res; 
    return S;
}

int main (int argc, char** argv) {
    if (argc != 2) {
        cout << "El uso del programa es: ./greedyIC <fichero_grafo>" << endl;
        exit(1);
    }
    else {
        //Semilla de los números aleatorios.
        srand(time(NULL));

        double prob;
        Grafo G = leerGrafo(&prob, argv[1]);

        auto start = std::chrono::steady_clock::now();

        VPair ordenados = ordenarPorGrado(G);
        Solucion sol;

        queueInt S = greedy(G, ordenados, prob, sol);

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        cout << "Tamaño del subconjunto S: " << S.size() << endl;
        cout << "Tamaño del conjunto C: " << sol.C.size() << endl;
        cout << "Número de pasos: " << sol.t << endl;
        std::cout << "Tiempo de ejecución: " << elapsed_seconds.count() << " segundos" << std::endl;
    }
}
