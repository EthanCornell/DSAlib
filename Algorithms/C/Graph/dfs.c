#include <stdio.h>
#include <stdlib.h>

// Define the maximum number of vertices in the graph
#define MAX_VERTICES 100

// Adjacency list node
typedef struct Node
{
    int vertex;
    struct Node *next;
} Node;

// Graph structure
typedef struct Graph
{
    int numVertices;
    Node *adjLists[MAX_VERTICES];
    int visited[MAX_VERTICES];
} Graph;

// Function to create a node
Node *createNode(int v)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

// Function to create a graph
Graph *createGraph(int vertices)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->numVertices = vertices;

    for (int i = 0; i < vertices; i++)
    {
        graph->adjLists[i] = NULL;
        graph->visited[i] = 0;
    }
    return graph;
}

// Add edge to the graph
void addEdge(Graph *graph, int src, int dest)
{
    // Add edge from src to dest
    Node *newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;

    // Add edge from dest to src, for undirected graph
    newNode = createNode(src);
    newNode->next = graph->adjLists[dest];
    graph->adjLists[dest] = newNode;
}

// DFS algorithm
void DFS(Graph *graph, int vertex)
{
    Node *adjList = graph->adjLists[vertex];
    Node *temp = adjList;

    graph->visited[vertex] = 1;
    printf("Visited %d \n", vertex);

    while (temp != NULL)
    {
        int connectedVertex = temp->vertex;

        if (graph->visited[connectedVertex] == 0)
        {
            DFS(graph, connectedVertex);
        }
        temp = temp->next;
    }
}

// Main function
int main()
{
    // Create a graph with 100 vertices
    Graph *graph = createGraph(100);

    // Example: Adding a cycle and some extra edges to make the graph connected and interesting
    for (int i = 0; i < 99; i++)
    {
        addEdge(graph, i, i + 1); // Create a simple path
    }
    addEdge(graph, 99, 0); // Add an edge to make a cycle

    // Optionally, add more edges to create a more complex graph
    addEdge(graph, 0, 50);
    addEdge(graph, 25, 75);
    addEdge(graph, 50, 25);

    // Perform DFS starting from vertex 0
    DFS(graph, 0);

    return 0;
}

// gcc -pg -fsanitize=address -g -std=c17 ./dfs.c -o dfs -O3  && ./dfs