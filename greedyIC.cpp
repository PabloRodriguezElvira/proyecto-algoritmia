#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>
using namespace std;

using VI = vector<int>;
using Grafo = vector<VI>;
using VB = vector<bool>;
using QueueInt = queue<int>;

//Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
Grafo leerGrafo(double* prob, QueueInt& Q) {
    //Fichero que contiene el grafo:
    string nombreFichero = "grafo02IC.txt";
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

// Función greedy para encontrar el conjunto mínimo de nodos iniciales que maximice la activación
VI greedy(const Grafo& G, double prob) {
    int n = G.size();
    VI candidatos(n);
    VI S;
    int max_activacion = 0;

    // Inicializar el conjunto de candidatos con todos los nodos del grafo
    for (int i = 0; i < n; ++i) {
        candidatos[i] = i;
    }

    bool mejora = true;
    while (!candidatos.empty() && mejora) {
        mejora = false;
        int nodo_seleccionado = -1;
        int max_contribucion = 0;

        for (int nodo : candidatos) {
            // Calcular la contribución del nodo a la activación si se agrega a Q
            VB activados(n, false);
            QueueInt Q;
            for (int s : S) {
                Q.push(s);
                activados[s] = true;
            }
            Q.push(nodo);
            activados[nodo] = true;

            int t = 0;
            VI C = difusionIC(G, activados, prob, Q, &t);

            int contribucion = C.size() - max_activacion;
            if (contribucion > max_contribucion) {
                max_contribucion = contribucion;
                nodo_seleccionado = nodo;
                mejora = true;
            }
        }

        if (mejora) {
            // Agregar el nodo seleccionado a Q y eliminarlo de los candidatos
            S.push_back(nodo_seleccionado);
            candidatos.erase(remove(candidatos.begin(), candidatos.end(), nodo_seleccionado), candidatos.end());
            max_activacion += max_contribucion;
        }
    }

    return S;
}

int main() {
    // Semilla de los números aleatorios.
    srand(time(NULL));

    double prob;
    VB activados;
    QueueInt Q;
    Grafo G = leerGrafo(&prob, Q);

    VI S = greedy(G, prob);

    int tam = S.size();
    cout << "Subconjunto inicial: {";
    for (int i = 0; i <= tam - 2; ++i) cout << S[i] << ", ";
    cout << S[tam - 1] << "}" << endl;

    // Inicializar activados y Q con los nodos seleccionados en S
    activados.assign(G.size(), false);
    for (int s : S) {
        Q.push(s);
        activados[s] = true;
    }

    int t = 0;
    VI C = difusionIC(G, activados, prob, Q, &t);

    // Mostrar respuesta:
    int size = C.size();
    cout << "Tamaño del conjunto C: " << size << endl;
    cout << "Número de pasos: " << t << endl;
    cout << "Nodos activados: {";
    for (int i = 0; i <= size - 2; ++i) cout << C[i] << ", ";
    cout << C[size - 1] << "}" << endl;
}
