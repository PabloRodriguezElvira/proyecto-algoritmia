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

// Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
Grafo leerGrafo(double *prob, QueueInt &Q, VB &activados)
{
    // Fichero que contiene el grafo:
    string nombreFichero = "grafo03IC.txt";
    ifstream file(nombreFichero.c_str());
    string line;

    // n -> vértices, m -> aristas
    int n, m;
    getline(file, line);
    n = atoi(line.c_str());
    getline(file, line);
    m = atoi(line.c_str());

    Grafo G(n);
    int u, v;
    // Leer aristas:
    while (m--)
    {
        getline(file, line);
        stringstream ss(line);
        ss >> u;
        ss >> v;
        G[u].push_back(v);
        G[v].push_back(u);
    }

    // Probabilidad:
    getline(file, line);
    *prob = atof(line.c_str());

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
// Función que visita cada nodo adyacente a u y los activa con una probabilidad prob (condición del modelo IC).
// Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(const Grafo &G, VB &activados, VI &C, double prob, int u, int *t, QueueInt &Q)
{
    int pp = prob * 10;
    for (int v : G[u])
    {
        int random = rand() % 9;
        if (!activados[v] && random < pp)
        {
            activados[v] = true;
            Q.push(v);
        }
    }
    *t = *t + 1;
}

// La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según la probabilidad prob.
VI difusionIC(const Grafo &G, VB &activados, double ratio, QueueInt &Q, int *t)
{
    VI C;
    while (not Q.empty())
    {
        int w = Q.front();
        Q.pop();
        C.push_back(w);
        influenciarNodos(G, activados, C, ratio, w, t, Q);
    }
    return C;
}

// comprobar que todos los nodos estan activados
bool check_solution(const Grafo &G, VB &activados, double ratio, QueueInt &Q, int *t)
{
    VB activados_aux = activados;
    VI C = difusionIC(G, activados_aux, ratio, Q, t);
    int nodes_activados = 0;
    for (auto i : activados_aux)
    {
        if (i)
            ++nodes_activados;
    }
    return (nodes_activados == G.size());
}

void general_sol(Grafo &G, VB &activados, VI &sol_actual, double ratio)
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
        int t = 0;
        QueueInt punt_ini;
        punt_ini.push(sol_actual_aux[0]);
        if (check_solution(G, activados_aux, ratio, punt_ini, &t))
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
VI hillClimbing(Grafo &G, VB &activados, double ratio, QueueInt &Q)
{
    int num_nodes = G.size();
    VI sol_actual;
    VI best_sol;
    VB best_activados;
    VB activados_actual = activados;
    general_sol(G, activados_actual, sol_actual, ratio);
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
            int t = 0;
            QueueInt punt_ini;
            punt_ini.push(neighbor_sol[0]);
            if (check_solution(G, neighbor_activate, ratio, punt_ini, &t))
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
            sol_actual = best_sol;
            best_activados = best_activados;
        }
    }
    return best_sol;
}

int main()
{
    // Semilla de los números aleatorios.
    srand(time(NULL));
    double prob;
    VB activados;
    QueueInt Q;
    Grafo G = leerGrafo(&prob, Q, activados);
    cout << G.size() << endl;
    auto start = std::chrono::steady_clock::now();
    VI Solution = hillClimbing(G, activados, prob, Q);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << "Solution: { ";
    for (int i : Solution)
    {
        cout << i << " ";
    }
    cout << "}\n";
    cout << endl;
    std::cout << "El tiempo de ejecución fue de " << elapsed_seconds.count() << " segundos." << std::endl;
}