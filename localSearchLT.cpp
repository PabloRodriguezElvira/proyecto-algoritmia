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

void general_sol(Grafo &G, VB &activados, VI &sol_actual, double ratio, VNode &nodos)
{
    bool ok = false;
    VB activados_aux;
    VI sol_actual_aux;
    while (not ok) // generar solucin randoment que sea valido
    {
        activados_aux = activados;
        sol_actual_aux = sol_actual;
        int num_nodes = G.size();
        for (int i = 0; i < num_nodes; i++)
        {
            if (rand() % 2 == 1)
            {
                activados_aux[i] = true;
                sol_actual_aux.push_back(i);
            }
        }
        cout << "Solution_random: { ";
        for (int i : sol_actual_aux)
        {
            cout << i << " ";
        }
        cout << "}\n";
        if (check_solution(G, activados_aux, ratio, nodos))
            ok = true;
    }
    activados = activados_aux;
    sol_actual = sol_actual_aux;
}

int heuristic_value(VI &sol_actual)
{
    return sol_actual.size();
}

// Función para calcular el conjunto objetivo óptimo utilizando el algoritmo de Hill Climbing
VI hillClimbing(Grafo &G, VB &activados, double ratio, QueueInt &Q, VNode &nodos)
{
    int num_nodes = G.size();
    VI sol_actual;
    VI best_sol;
    VB best_activados;
    VB activados_actual = activados;
    general_sol(G, activados_actual, sol_actual, ratio, nodos);
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
                    cout << "}\n";
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

    VI Solution = hillClimbing(G, activados, ratio, Q, nodos);
    cout << "Solution: { ";
    for (int i : Solution)
    {
        cout << i << " ";
    }
    cout << "}\n";
}