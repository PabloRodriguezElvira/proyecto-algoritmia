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
using QueueInt = queue<int>;

struct Solucion
{
    VI C;  // Conjunto de Nodos activados
    int t; // Pasos de tiempo realizados por la difusión.
};

// Función que se encarga de leer el grafo, la probabilidad y el subconjunto S de un fichero.
Grafo leerGrafo(double *prob, VI &S, const char *nombreFichero)
{
    // Fichero que contiene el grafo:
    ifstream file(nombreFichero);
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

    // Subconjunto S (en una cola Q).
    int w;
    getline(file, line);
    w = atoi(line.c_str());
    S.push_back(w);
    while (w != -1)
    {
        getline(file, line);
        w = atoi(line.c_str());
        if (w != -1)
            S.push_back(w);
    }

    file.close();
    return G;
}

// Función que visita cada nodo adyacente a u y los activa con una probabilidad prob (condición del modelo IC).
// Si activamos un nodo, lo encolamos para realizar el mismo proceso partiendo de él.
void influenciarNodos(const Grafo &G, VB &activados, Solucion &sol, const double &prob, const int &u, QueueInt &Q)
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
    // Aumentamos en 1 los pasos de difusión.
    sol.t = sol.t + 1;
}

// La función se encarga de para cada vértice activado inicialmente, ir activando sus vecinos según la probabilidad prob.
Solucion difusionIC(const Grafo &G, const VI &S, const double &prob)
{
    Solucion sol;
    sol.t = 0;
    int n = G.size();
    QueueInt Q;
    VB activados(n, false);

    // Activamos los vértices de S y los volcamos a la cola Q.
    for (int v : S)
    {
        activados[v] = true;
        Q.push(v);
    }

    // Mientras la cola no esté vacía, hacemos un paso de difusión desde el nodo que esté en el front de la cola.
    while (not Q.empty())
    {
        // Añadimos el nodo al conjunto solución de nodos activados.
        int w = Q.front();
        Q.pop();
        sol.C.push_back(w);

        // Realizamos la difusión:
        influenciarNodos(G, activados, sol, prob, w, Q);
    }

    return sol;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "El uso del programa es: ./difusioIC <fichero_grafo>" << endl;
        exit(1);
    }
    else
    {
        // Semilla de los números aleatorios.
        srand(time(NULL));

        double prob;
        VB activados;
        VI S;
        Grafo G = leerGrafo(&prob, S, argv[1]);

        // VI C = difusionIC(G, activados, prob, Q, &t);
        Solucion sol = difusionIC(G, S, prob);
        // A difusionIC le pasamos la entrada -> el grafo G, el subconjunto de vertices S y la prob p. (o ratio r).

        // Mostrar respuesta:
        int size = sol.C.size();
        cout << "Tamaño del conjunto C: " << size << endl;
        cout << "Número de pasos: " << sol.t << endl;
        cout << "Nodos activados: {";
        for (int i = 0; i <= size - 2; ++i)
            cout << sol.C[i] << ", ";
        cout << sol.C[size - 1] << "}" << endl;
    }
}
