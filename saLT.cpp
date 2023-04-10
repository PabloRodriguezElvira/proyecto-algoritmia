#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

struct Node
{
    double llindar;
    int influencia;
};

using VI = vector<int>;
using VNode = vector<Node>;
using AdjList = vector<VI>;
using Grafo = pair<AdjList, VNode>;
using VB = vector<bool>;
using QueueInt = queue<int>;

// Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
Grafo leerGrafo(double *ratio, const char *nombreFichero)
{
    // Fichero que contiene el grafo:
    ifstream file(nombreFichero);
    string line;
    stringstream ss;

    // n -> vértices, m -> aristas
    int n, m;
    string str;
    getline(file, line);
    ss = stringstream(line);
    ss >> str;
    ss >> str; // Leer p y edge.
    ss >> n;
    ss >> m;

    AdjList adj(n);
    VNode nodos(n);
    Grafo G = make_pair(adj, nodos);

    // Leer ratio.
    getline(file, line);
    ss = stringstream(line);
    ss >> str;
    ss >> (*ratio);
    // Leer aristas:
    int u, v;
    while (m--)
    {
        getline(file, line);
        stringstream ss(line);
        ss >> str;
        ss >> u;
        ss >> v;
        --u;
        --v;
        G.first[u].push_back(v);
        G.first[v].push_back(u);

        // Como el peso de todas las aristas es 1, incrementamos cada umbral en ratio.
        G.second[u].llindar += *ratio;
        G.second[v].llindar += *ratio;

        // La influencia recibida inicial de todos los nodos es 0.
        G.second[u].influencia = 0;
        G.second[v].influencia = 0;
    }
    file.close();
    return G;
}

// Función que visita cada nodo adyacente a u y los activa según si cumplen la condición del modelo de difusión LT.
// Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(Grafo &G, VB &activados, VI &C, double ratio, int u, int *t, QueueInt &Q)
{
    bool influenciado = false;
    for (int v : G.first[u])
    {
        // Incrementamos en 1 la influencia de cada nodo vecino de u.
        ++G.second[v].influencia;
        if (!activados[v] && G.second[v].influencia >= G.second[v].llindar)
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
VI difusionLT(Grafo &G, VB &activados, double ratio, QueueInt &Q, int *t)
{
    VI C;
    while (not Q.empty())
    {
        int w = Q.front();
        Q.pop();
        C.push_back(w);
        influenciarNodos(G, activados, C, ratio, w, t, Q);
        // Sacar el conjunto de nodos activados a cada iteracion. Si alguno de estos nodos activa algun otro nodo, ya
        // cuenta como t+1.
    }
    return C;
}

// comprobar que todos los nodos estan activados
bool check_solution(Grafo &G, VB &activados, double ratio)
{
    VB activados_aux = activados;
    int t = 0;
    Grafo G_aux = G;
    QueueInt Q;
    for (int i = 0; i < activados.size(); ++i)
    {
        if (activados[i])
            Q.push(i);
    }
    VI C = difusionLT(G_aux, activados_aux, ratio, Q, &t);
    int nodes_activados = 0;
    for (auto i : activados_aux)
    {
        if (i)
            ++nodes_activados;
    }
    return (nodes_activados == G.first.size());
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
        int num_nodes = G.first.size();
        for (int i = 0; i < num_nodes; i++)
        {
            if (rand() % 2 == 1)
            {
                activados_aux[i] = true;
                sol_actual_aux.push_back(i);
            }
        }
        QueueInt punt_ini;
        punt_ini.push(sol_actual_aux[0]);
        if (check_solution(G, activados_aux, ratio))
            ok = true;
    }
    activados = activados_aux;
    sol_actual = sol_actual_aux;
}

int heuristic_value(VI &sol_actual)
{
    return sol_actual.size();
}

VI simulated_Annealing(Grafo &G, double ratio)
{
    int num_nodes = G.first.size();
    VI sol_actual;
    VI best_sol;
    VB best_activados;
    VB activados_actual(num_nodes, false);
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

    double initial_temperature = 10000;
    double cooling_rate = 0.5;
    double temperature = initial_temperature;
    while (temperature > 0.001)
    {
        for (int i = 0; i < 100; i++) // número de iteraciones por temperatura
        {
            // Escoger un vecino aleatorio
            int random_index = rand() % sol_actual.size();

            VI neighbor_sol = sol_actual;
            VB neighbor_activate = best_activados;
            neighbor_activate[neighbor_sol[random_index]] = false;
            neighbor_sol.erase(neighbor_sol.begin() + random_index);

            int t = 0;
            if (check_solution(G, neighbor_activate, ratio))
            {
                int neighbor_score = heuristic_value(neighbor_sol);

                // Calcular la diferencia de energía (score) entre la solución actual y el vecino
                int difference = neighbor_score - best_score;

                if (difference < 0)
                {
                    best_score = neighbor_score;
                    best_sol = neighbor_sol;
                    best_activados = neighbor_activate;
                    sol_actual = neighbor_sol;
                }
                else if (exp(-difference / temperature) > ((double)rand() / (double)RAND_MAX))
                {
                    sol_actual = neighbor_sol;
                }
            }
        }

        // Enfriar la temperatura
        temperature *= cooling_rate;
    }

    return best_sol;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    if (argc != 2)
    {
        cout << "El uso del programa es: ./localSearchLT <fichero_grafo>" << endl;
        exit(1);
    }
    else
    {
        double ratio;
        QueueInt Q;
        VB activados;
        VNode nodos;
        Grafo G = leerGrafo(&ratio, argv[1]);
        cout << G.first.size() << endl;
        auto start = std::chrono::steady_clock::now();
        VI Solution = simulated_Annealing(G, ratio);
        auto end = std::chrono::steady_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "Tamaño de solucion :" << Solution.size() << endl;
        cout << endl;
        std::cout << "El tiempo de ejecución fue de " << elapsed_seconds.count() << " segundos." << std::endl;
    }
}