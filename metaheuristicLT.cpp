#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
using namespace std;

struct Node
{
    double llindar;
    int influencia;
};

using VI = vector<int>;
using Grafo = vector<VI>;
using VNode = vector<Node>;
using VB = vector<bool>;
using QueueInt = queue<int>;
using VPair = vector<pair<double, int>>;

// Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
Grafo leerGrafo(double *ratio, QueueInt &Q, VB &activados, VNode &nodos)
{
    // Fichero que contiene el grafo:
    string nombreFichero = "grafo01LT.txt";
    ifstream file(nombreFichero.c_str());
    string line;

    // n -> vértices, m -> aristas
    int n, m;
    getline(file, line);
    n = atoi(line.c_str());
    getline(file, line);
    m = atoi(line.c_str());

    Grafo G(n);
    nodos.resize(n);

    // Probabilidad:
    getline(file, line);
    *ratio = atof(line.c_str());

    // Leer aristas:
    int u, v;
    while (m--)
    {
        getline(file, line);
        stringstream ss(line);
        ss >> u;
        ss >> v;
        nodos[u].llindar += *ratio;
        nodos[v].llindar += *ratio;
        G[u].push_back(v);
        G[v].push_back(u);
    }

    // Subconjunto Q:
    int w;
    activados.resize(n);
    getline(file, line);
    w = atoi(line.c_str());
    Q.push(w);
    activados[w] = true;
    while (w != -1)
    {
        getline(file, line);
        w = atoi(line.c_str());
        if (w != -1)
        {
            Q.push(w);
            activados[w] = true;
        }
    }

    file.close();
    return G;
}

// Función que visita cada nodo adyacente a u y los activa según si cumplen la condición del modelo de difusión LT.
// Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(const Grafo &G, VB &activados, VI &C, double ratio, int u, int *t, VNode &nodos, QueueInt &Q)
{
    bool influenciado = false;
    for (int v : G[u])
    {
        // Incrementamos en 1 la influencia de cada nodo vecino de u.
        ++nodos[v].influencia;
        if (!activados[v] && nodos[v].influencia >= nodos[v].llindar)
        {
            influenciado = true;
            activados[v] = true;
            Q.push(v);
        }
    }
    if (influenciado)
        *t = *t + 1;
}

// La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según si cumplen la condición
// del modelo LT.
VI difusionLT(const Grafo &G, VB &activados, double ratio, QueueInt &Q, int *t, VNode &nodos)
{
    VI C;
    while (not Q.empty())
    {
        int w = Q.front();
        Q.pop();
        C.push_back(w);
        influenciarNodos(G, activados, C, ratio, w, t, nodos, Q);
        // Sacar el conjunto de nodos activados a cada iteracion. Si alguno de estos nodos activa algun otro nodo, ya
        // cuenta como t+1.
    }
    return C;
}

// Ordena según el lindar del nodo
vector<pair<double, int>> Ordenar(VNode &nodos)
{
    vector<pair<double, int>> pares;
    for (int i = 0; i < nodos.size(); ++i)
    {
        pares.push_back({nodos[i].llindar, i});
    }
    sort(pares.rbegin(), pares.rend());
    return pares;
}

// comprobar que todos los nodos estan activados
bool check_solution(const Grafo &G, VB &activados, double ratio, VNode &nodos)
{
    QueueInt Q;
    Q.push(activados[0]);
    VB activados_aux = activados;
    int t = 0;
    VI C = difusionLT(G, activados_aux, ratio, Q, &t, nodos);
    int nodes_activados = 0;
    for (auto i : activados_aux)
    {
        if (i)
            ++nodes_activados;
    }
    return (nodes_activados == G.size());
}

VI greedy(const Grafo &G, double ratio, const VNode &nodos, const VPair &pares, int& t_optima, QueueInt& Q_inicial)
{
    VI C;
    int n = G.size();
    int k_min = 1;
    int k_max = n;
    int k_optimo = -1;
    int t;
    while (k_min <= k_max) {
        int k_mid = (k_min + k_max) / 2;
        VB activados(n, false);
        QueueInt Q;
        for (int i = 0; i < k_mid; ++i) {
            activados[pares[i].second] = true;
            Q.push(pares[i].second);
        }
        Q_inicial = Q; 
        VNode aux = nodos;
        t = 0;
        C = difusionLT(G, activados, ratio, Q, &t, aux);
        int tamano_C = C.size();
        cout << k_mid << " " << tamano_C << endl; //Aqui esta el problema!!!!

        if (tamano_C == n) { //Si el tamaño de C es todo el grafo, busca si hay un valor menor de k. Sino aumenta el valor de K.
            k_optimo = k_mid;
            k_max = k_mid - 1;
        }
        else k_min = k_mid + 1;
    }
    t_optima = t;
    return C;
}

int heuristic_value(VI &sol_actual)
{
    return sol_actual.size();
}

// Función para calcular el conjunto objetivo óptimo utilizando el algoritmo de greedy+Hill Climbing(metaheuristica)
VI greedy_and_hillClimbing(Grafo &G, VB &activados, double ratio, QueueInt &Q, VNode &nodos, const VPair &pares)
{
    int num_nodes = G.size();
    VI sol_actual;
    VI best_sol;
    VB best_activados;
    VB activados_actual = activados;

    int t_optima;
    sol_actual = greedy(G, ratio, nodos, pares, t_optima, Q);
    int best_score = heuristic_value(sol_actual);
    best_sol = sol_actual;
    best_activados = activados_actual;

    cout << "Solution_ini: { ";
    for (int i : sol_actual)
    {
        cout << i << " ";
    }
    cout << "}\n";

    // Iteramos hasta que no podamos encontrar una solución mejor
    bool improved = true;
    while (improved)
    {
        improved = false;

        // Evaluamos todos los vecinos del conjunto actual
        for (int i = 0; i < sol_actual.size() && sol_actual.size() > 1; i++)
        {

            VI neighbor_sol = sol_actual;
            VB neighbor_activate = best_activados;
            neighbor_activate[neighbor_sol[i]] = false;
            neighbor_sol.erase(neighbor_sol.begin() + i);
            if (check_solution(G, neighbor_activate, ratio, nodos))
            {
                int neighbor_score = heuristic_value(neighbor_sol);
                if (neighbor_score < best_score)
                {
                    best_score = neighbor_score;
                    best_sol = neighbor_sol;
                    best_activados = neighbor_activate;
                    improved = true;
                }
            }

            // Si encontramos un vecino mejor, actualizamos la solución actual
        }

        // Actualizamos el conjunto actual al mejor vecino encontrado
        if (improved)
        {
            cout << "Solution_improve: { ";
            for (int i : best_sol)
            {
                cout << i << " ";
            }
            cout << "}\n";
            sol_actual = best_sol;
            best_activados = best_activados;
        }
    }
    return best_sol;
}

int main()
{
    double ratio;
    QueueInt Q;
    VB activados;
    VNode nodos;
    Grafo G = leerGrafo(&ratio, Q, activados, nodos);
    VPair pares = Ordenar(nodos);

    VI Solution = greedy_and_hillClimbing(G, activados, ratio, Q, nodos, pares);
    cout << "Solution: { ";
    for (int i : Solution)
    {
        cout << i << " ";
    }
    cout << "}\n";
}