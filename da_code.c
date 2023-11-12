#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h> // Include this for strtok

#define MAX_NODES 5000
#define MAX_LINE_LENGTH 1024

struct Edge {
    int dest;
    double weight;
    struct Edge* next;
};

struct Node {
    int id;
    double distance;
    struct Node* parent;
    struct Edge* edges;
};

struct Graph {
    struct Node* nodes[MAX_NODES];
    int numNodes;
};

// Function to create a new edge
struct Edge* createEdge(int dest, double weight) {
    struct Edge* newEdge = (struct Edge*)malloc(sizeof(struct Edge));
    newEdge->dest = dest;
    newEdge->weight = weight;
    newEdge->next = NULL;
    return newEdge;
}

// Function to create a new node
struct Node* createNode(int id) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->id = id;
    newNode->distance = INT_MAX; // Initialize distance to infinity
    newNode->parent = NULL;
    newNode->edges = NULL;
    return newNode;
}

// Function to create a new graph
struct Graph* createGraph(int numNodes) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->numNodes = numNodes;
    for (int i = 0; i < numNodes; i++) {
        graph->nodes[i] = createNode(i);
    }
    return graph;
}

// Function to add an edge to the graph
void addEdge(struct Graph* graph, int src, int dest, double weight) {
    if (src >= 0 && src < graph->numNodes && dest >= 0 && dest < graph->numNodes) {
        struct Edge* newEdge = createEdge(dest, weight);
        newEdge->next = graph->nodes[src]->edges;
        graph->nodes[src]->edges = newEdge;
    }
}

// Function to perform Dijkstra's algorithm
void dijkstra(struct Graph* graph, int start, int end) {
    struct Node* nodes[MAX_NODES];
    double newDistance;
    
    // Initialize nodes and distances
    for (int i = 0; i < graph->numNodes; i++) {
        nodes[i] = graph->nodes[i];
        nodes[i]->distance = INT_MAX;
        nodes[i]->parent = NULL;
    }
    nodes[start]->distance = 0;
    
    // Priority queue to keep track of nodes to visit next
    bool visited[MAX_NODES] = {false};
    
    while (true) {
        // Find the node with the smallest distance among unvisited nodes
        int minDist = INT_MAX;
        int minNode = -1;
        for (int i = 0; i < graph->numNodes; i++) {
            if (!visited[i] && nodes[i]->distance < minDist) {
                minDist = nodes[i]->distance;
                minNode = i;
            }
        }
        
        if (minNode == -1) {
            break; // All nodes have been visited
        }
        
        visited[minNode] = true;
        
        // Explore neighbors of the current node
        struct Edge* edge = nodes[minNode]->edges;
        while (edge != NULL) {
            int neighbor = edge->dest;
            newDistance = nodes[minNode]->distance + edge->weight;
            
            if (newDistance < nodes[neighbor]->distance) {
                nodes[neighbor]->distance = newDistance;
                nodes[neighbor]->parent = nodes[minNode];
            }
            
            edge = edge->next;
        }
    }
    
    // Reconstruct the shortest path
    struct Node* currentNode = nodes[end];
    printf("Shortest Path from %d to %d: ", start, end);
    while (currentNode != NULL) {
        printf("%d ", currentNode->id);
        currentNode = currentNode->parent;
    }
    printf("\nShortest Distance: %.2lf\n", nodes[end]->distance);
}

int main() {
    int numNodes = 5000;
    struct Graph* graph = createGraph(numNodes);
    
    FILE* file = fopen("DataSet.csv", "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    char* token; // To store tokens from strtok

    if (fgets(line, sizeof(line), file) == NULL) {
        perror("Error reading header");
        fclose(file);
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        // Use strtok to tokenize the line based on commas
        token = strtok(line, ",");
        if (token == NULL) {
            // Handle invalid CSV line (missing data)
            fprintf(stderr, "Error: Invalid CSV line\n");
            continue;
        }
        int FromNodeId = atoi(token);

        token = strtok(NULL, ",");
        if (token == NULL) {
            // Handle invalid CSV line (missing data)
            fprintf(stderr, "Error: Invalid CSV line\n");
            continue;
        }
        int ToNodeId = atoi(token);

        token = strtok(NULL, ",");
        if (token == NULL) {
            // Handle invalid CSV line (missing data)
            fprintf(stderr, "Error: Invalid CSV line\n");
            continue;
        }
        double Weights = atof(token);

        addEdge(graph, FromNodeId, ToNodeId, Weights);
    }

    fclose(file);

    int startNode, endNode;
    
    printf("Enter the start node ID: ");
    scanf("%d", &startNode);
    printf("Enter the end node ID: ");
    scanf("%d", &endNode);
    
    dijkstra(graph, startNode, endNode);

    return 0;
}