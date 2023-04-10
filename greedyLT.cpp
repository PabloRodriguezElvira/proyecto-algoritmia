#include "difusioLT.cpp"
#include <algorithm>
#include <set>
#include <chrono>
using namespace std;

using VPair = vector<pair<int, Node>>;

const bool cmp(pair<int, Node> p1, pair<int, Node> p2) {
    return p1.second.llindar > p2.second.llindar;
}

// Función que se encarga de leer el grafo y la probabilidad.
Grafo leerGrafo(double* ratio, const char* nombreFichero) {
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

    AdjList adj(n);
    VNode nodos(n);
    Grafo G = {adj, nodos};

    //Probabilidad:
    getline(file, line);
    ss = stringstream(line);
    ss >> str; ss >> (*ratio);

    int u, v;
    //Leer aristas:
    while (m--) {
        getline(file, line);
        ss = stringstream(line);
        ss >> str;
        ss >> u; ss >> v;
        --u; --v;

        //Adyacencias:
        G.first[u].push_back(v);
        G.first[v].push_back(u);

        //Umbral
        G.second[u].llindar += *ratio;
        G.second[v].llindar += *ratio;

        //Influencia
        G.second[u].influencia = 0;
        G.second[v].influencia = 0;
    }

    file.close();
    return G;
}    


//Busca el nodo con mayor grado que esté disponible (que no haya sido activado).
int buscar_nodo_mayor(const VPair& ord, const VB& disp) {
    for (int i = 0; i < ord.size(); ++i) {
        if (disp[ord[i].first]) return ord[i].first;
    }
    return -1; //No se ha encontrado.
}

// Escoge los nodos activados en el instante t = 0
pair<queueInt, Solucion> greedy(Grafo& G, const double& ratio) {
    int n = G.first.size();


    //Vamos a tener un vector "disponibles" para "eliminar" los nodos que ya hayan sido activados
    //de la estructura de "ordenados". En "ordenados" vamos a tener los nodos ordenados decrecientemente
    //por su umbral.
    VB disponibles(n, true);
    VPair ordenados(n);

    //Introducir los nodos.
    for (int i = 0; i < n; ++i) {
        ordenados[i] = {i, G.second[i]};
    }

    //Ordenar:
    sort(ordenados.begin(), ordenados.end(), cmp);

    //Coger el nodo con umbral más grande.
    queueInt S;
    S.push(ordenados[0].first);
    Solucion sol;

    bool found = false;
    while (!found) { 

        sol = difusionLT(G, S, ratio);
    
        //Quitar los nodos que ya hayan sido activados.
        for (int v : sol.C) {
            disponibles[v] = false;
        }

        //Hemos activado todos los nodos.
        if (sol.C.size() == n) found = true;
        else {
            //Escoger el siguiente de mayor grado que NO esté activado. 
            int nodo = buscar_nodo_mayor(ordenados, disponibles);
            S.push(nodo);
        }
    }

    return {S, sol};
}
 
int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "El uso del programa es: ./greedyLT <fichero_grafo>" << endl;
        exit(1);
    }
    else {
        double ratio;
        VB activados;
        Grafo G = leerGrafo(&ratio, argv[1]);

        //A partir de aquí contamos el tiempo.
        auto start = std::chrono::steady_clock::now();

        auto Greedy = greedy(G, ratio);
    
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        //Aquí se acaba de cronometrar el tiempo.

        queueInt S = Greedy.first;
        Solucion sol = Greedy.second;

        // Mostrar respuesta:
        int tamS = S.size();
        int tamC = sol.C.size(); 
        cout << "Tamaño del conjunto S: " << tamS << endl;
        cout << "Tamaño del conjunto C: " << tamC << endl;
        cout << "Número de pasos: " << sol.t << endl;

        std::cout << "El tiempo de ejecución fue de " << elapsed_seconds.count() << " segundos." << std::endl;
    }
}
