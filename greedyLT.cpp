#include <iostream>
#include <algorithm>
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

//Función que se encarga de leer el grafo y la probabilidad.
Grafo leerGrafo(double* ratio, VNode& nodos) {
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

//Ordena según el lindar del nodo
vector<pair<double,int>> Ordenar(VNode& nodos) {
    vector<pair<double, int>> pares;
    for (int i = 0; i < nodos.size(); ++i) {
        pares.push_back({nodos[i].llindar, i});
    }
    sort(pares.rbegin(), pares.rend());
    return pares;
}

//Escoge el valor mínimo de K, que activa todo el grafo
int escogerK(const Grafo& G, double ratio, const VNode& nodos) {
    int n = G.size();
    int k_min = 1;
    int k_max = n;
    int k_optimo = -1;
    while (k_min < k_max) {
        int k_mid = (k_min + k_max) / 2;
        VB activados(n, false);
        QueueInt Q;
        for (int i = 0; i < k_mid; ++i) {
            activados[i] = true;
            Q.push(i);
        }
        VNode aux = nodos;
        int t = 0;
        VI C = difusionLT(G, activados, ratio, Q, &t, aux);
        int tamano_C = C.size();
        cout << k_mid << " " << tamano_C << endl; //Aqui esta el problema!!!!

        if (tamano_C == n) { //Si el tamaño de C es todo el grafo, busca si hay un valor menor de k. Sino aumenta el valor de K.
            k_optimo = k_mid;
            k_max = k_mid - 1;
        }
        else k_min = k_mid + 1;
    }

    return k_optimo;
}

//Escoge los nodos activados en el instante t = 0
VI greedy(const vector<pair<double,int>>& pares, int k) {
    VI S;
    for (int i = 0; i < k; ++i) {
        S.push_back(pares[i].second);
    }
    return S;
}

int main () {
    double ratio;
    QueueInt Q;
    VB activados;
    VNode nodos;
    Grafo G = leerGrafo(&ratio, nodos);
    vector<pair<double, int>> pares = Ordenar(nodos);

    int k = escogerK(G,ratio, nodos);
    VI S = greedy(pares, k);

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
    VI C = difusionLT(G, activados, ratio, Q, &t, nodos);

    //Mostrar respuesta:
    int size = C.size();
    cout << "Tamaño del conjunto C: " << size << endl;
    cout << "Número de pasos: " << t << endl;
    cout << "Nodos activados: {";
    for (int i = 0; i <= size-2; ++i) cout << C[i] << ", ";
    cout << C[size-1] << "}" << endl;
}
