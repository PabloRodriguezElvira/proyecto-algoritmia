#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Estructura para representar un nodo en la red
struct Node {
    int id;
    vector<int> neighbors;
};

// Función para simular la propagación de influencia utilizando el modelo Independent Cascade
int simulate_independent_cascade(vector<Node>& network, vector<int>& target_set) {
    int num_nodes = network.size();
    vector<bool> activated(num_nodes, false);
    vector<int> queue;

    // Agregamos los nodos del conjunto objetivo a la cola
    for (int i = 0; i < target_set.size(); i++) {
        int node_id = target_set[i];
        activated[node_id] = true;
        queue.push_back(node_id);
    }

    // Propagamos la influencia a través de la red
    int num_activated = target_set.size();
    while (!queue.empty()) {
        int node_id = queue.front();
        queue.erase(queue.begin());

        Node node = network[node_id];
        for (int i = 0; i < node.neighbors.size(); i++) {
            int neighbor_id = node.neighbors[i];
            if (!activated[neighbor_id]) {
                double probability = (double)rand() / RAND_MAX;
                if (probability <= 0.5) {
                    activated[neighbor_id] = true;
                    queue.push_back(neighbor_id);
                    num_activated++;
                }
            }
        }
    }

    return num_activated;
}

// Función para calcular el conjunto objetivo óptimo utilizando el algoritmo de Hill Climbing
vector<int> hill_climbing(vector<Node>& network, int k) {
    int num_nodes = network.size();
    vector<int> current_set;
    vector<int> best_set;
    int best_score = 0;

    // Generamos un conjunto inicial aleatorio
    for (int i = 0; i < k; i++) {
        int node_id = rand() % num_nodes;
        current_set.push_back(node_id);
    }

    // Iteramos hasta que no podamos encontrar una solución mejor
    bool improved = true;
    while (improved) {
        improved = false;

        // Evaluamos todos los vecinos del conjunto actual
        for (int i = 0; i < num_nodes; i++) {
            if (find(current_set.begin(), current_set.end(), i) == current_set.end()) {
                vector<int> neighbor_set = current_set;
                neighbor_set.push_back(i);
                int neighbor_score = simulate_independent_cascade(network, neighbor_set);

                // Si encontramos un vecino mejor, actualizamos la solución actual
                if (neighbor_score > best_score) {
                    best_score = neighbor_score;
                    best_set = neighbor_set;
                    improved = true;
                }
            }
        }

        // Actualizamos el conjunto actual al mejor vecino encontrado
        if (improved) {
            current_set = best_set;
        }
    }

    return best_set;
}

int main() {
    srand(time(NULL));

    // Ejemplo de red de 5 nodos
    vector<Node> network = {
        {0, {1, 2}},
        {1, {0, 2, 3}},
        {2, {0, 1, 3}},
        {3, {1, 2, 4}},
        {4, {3}}
    };

    // Ej
