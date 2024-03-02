/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */

//directed graph

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Edge {
    int dest;
    int weight;
    struct Edge* next;
} Edge;

typedef struct Vertex {
    int value;
    Edge* head; // Head of the list of edges
} Vertex;

typedef struct Graph {
    int numVertices;
    Vertex* vertices; // Dynamic array of vertices
} Graph;

// Time and Space Complexity Analysis
// adjacent(G, x, y): O(V) in the worst case, where V is the number of vertices, as it may need to traverse the entire edge list of vertex x.
// neighbors(G, x): O(V) in the worst case, for the same reason as adjacent.
// add_vertex(G, x): O(1) if we ignore the time taken to check if the vertex exists and reallocating memory. Otherwise, O(V).
// remove_vertex(G, x): O(V + E), where E is the number of edges, because it needs to remove the vertex and all edges connected to it.
// add_edge(G, x, y, z), remove_edge(G, x, y): O(V) in the worst case, to find the vertices and update their edge lists.
// get_vertex_value(G, x), set_vertex_value(G, x, v): O(1), assuming direct access to the vertex.
// get_edge_value(G, x, y), set_edge_value(G, x, y, v): O(V), as it may need to traverse the edge list of x to find y.
// printGraph operating in O(V+E) time, where V is the number of vertices and E is the number of edges, to print all vertices and their edges. The space complexity of the graph is O(V+E), accounting for all vertices and their edge lists.

// Function prototypes for graph operations
Graph* createGraph(int numVertices);
void add_edge(Graph* graph, int src, int dest, int weight);
bool adjacent(Graph* graph, int x, int y);
void add_vertex(Graph* graph, int x);
void remove_vertex(Graph* graph, int vertex);
void remove_edge(Graph* graph, int src, int dest);
int get_vertex_value(Graph* graph, int x);
void set_vertex_value(Graph* graph, int x, int value);
int get_edge_value(Graph* graph, int src, int dest);
void set_edge_value(Graph* graph, int src, int dest, int weight);
void printGraph(Graph* graph);
void freeGraph(Graph* graph);

// createGraph: Allocates memory for the Graph structure and its vertices array, initializing each vertex's edge list head to NULL and their value to 0. It's crucial to check for successful memory allocation to avoid memory access errors.
Graph* createGraph(int numVertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) {
        printf("Memory allocation failed for graph.\n");
        return NULL; // Always check for successful memory allocation
    }

    graph->numVertices = numVertices;
    graph->vertices = (Vertex*)malloc(numVertices * sizeof(Vertex));

    if (!graph->vertices) {
        printf("Memory allocation failed for vertices.\n");
        free(graph); // Clean up previously allocated memory before returning NULL
        return NULL;
    }

    for (int i = 0; i < numVertices; i++) {
        graph->vertices[i].head = NULL;
        graph->vertices[i].value = 0; // Optionally initialize vertex value to 0 or any identifier
    }

    return graph;
}


// add_edge(G, x, y, z): Before adding an edge, use adjacent(G, x, y) to check if an edge already exists. If not, proceed to add the new edge.
// Function to add an edge if it does not already exist
void add_edge(Graph* graph, int src, int dest, int weight) {
    if (!adjacent(graph, src, dest)) {
        Edge* newEdge = (Edge*)malloc(sizeof(Edge));
        newEdge->dest = dest;
        newEdge->weight = weight;
        newEdge->next = graph->vertices[src].head;
        graph->vertices[src].head = newEdge;
    }
}

// adjacent(G, x, y): Iterate over the edge list of vertex x. If a vertex y is found, return true; otherwise, return false.
// Function to check if an edge exists between x and y
bool adjacent(Graph* graph, int x, int y) {
    Edge* temp = graph->vertices[x].head;
    while (temp != NULL) {
        if (temp->dest == y) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}


// remove_vertex(G, x): Iterate through G's vertices to find x. Remove x and adjust the vertices array. Also, go through all vertices' edge lists and remove any edges pointing to x.
void add_vertex(Graph* graph, int x) {
    // Dynamically increase the size of the vertices array to add a new vertex
    // This is a simplified approach and might not be the most efficient way to handle vertices dynamically
    graph->numVertices++;
    graph->vertices = realloc(graph->vertices, graph->numVertices * sizeof(Vertex));
    graph->vertices[graph->numVertices - 1].head = NULL;
    graph->vertices[graph->numVertices - 1].value = x;
    printf("Vertex %d added.\n", x);
}


// remove_edge(G, x, y): Traverse the edge list of vertex x to find and remove the edge pointing to y.
// Function to remove an edge from x to y
void remove_edge(Graph* graph, int src, int dest) {
    Edge** curr = &graph->vertices[src].head;
    while (*curr) {
        Edge* entry = *curr;
        if (entry->dest == dest) {
            *curr = entry->next;
            free(entry);
            return;
        }
        curr = &entry->next;
    }
}


// get_vertex_value(G, x) and set_vertex_value(G, x, v): Access the vertex directly via its index x in the vertices array and get/set its value.
// Function to get the value of a vertex
int get_vertex_value(Graph* graph, int x) {
    return graph->vertices[x].value;
}

// Function to set the value of a vertex
void set_vertex_value(Graph* graph, int x, int value) {
    graph->vertices[x].value = value;
}


// get_edge_value(G, x, y) and set_edge_value(G, x, y, v): Traverse the edge list of vertex x to find the edge pointing to y and get/set its weight.
// Function to get the value (weight) of an edge from x to y
// Return the value (weight) associated with the edge from x to y
int get_edge_value(Graph* graph, int src, int dest) {
    Edge* temp = graph->vertices[src].head;
    while (temp) {
        if (temp->dest == dest) {
            return temp->weight;
        }
        temp = temp->next;
    }
    return -1; // Indicate that the edge does not exist
}


// set_edge_value(G, x, y, v)
// Set the value (weight) of the edge from x to y to v.
// Function to set the value (weight) of an edge from x to y
void set_edge_value(Graph* graph, int src, int dest, int weight) {
    Edge* temp = graph->vertices[src].head;
    while (temp) {
        if (temp->dest == dest) {
            temp->weight = weight;
            return;
        }
        temp = temp->next;
    }
}

// neighbors(G, x) : Return the list of vertices connected by an edge from vertex x.
void neighbors(Graph* graph, int x) {
    printf("Neighbors of Vertex %d:\n", x);
    Edge* temp = graph->vertices[x].head;
    while (temp != NULL) {
        printf("%d ", temp->dest);
        temp = temp->next;
    }
    printf("\n");
}


void remove_vertex(Graph* graph, int vertex) {
    if (vertex < 0 || vertex >= graph->numVertices) {
        printf("Vertex %d does not exist.\n", vertex);
        return;
    }

    // First, free the adjacency list of the vertex to be removed.
    Edge* tempEdge = graph->vertices[vertex].head;
    while (tempEdge != NULL) {
        Edge* toDelete = tempEdge;
        tempEdge = tempEdge->next;
        free(toDelete);
    }

    // Shift vertices down to fill the gap.
    for (int i = vertex; i < graph->numVertices - 1; i++) {
        graph->vertices[i] = graph->vertices[i + 1];
    }

    // Adjust the numVertices.
    graph->numVertices--;

    // Update the adjacency lists of the remaining vertices.
    for (int i = 0; i < graph->numVertices; i++) {
        Edge** curr = &graph->vertices[i].head;
        while (*curr != NULL) {
            Edge* entry = *curr;
            if (entry->dest == vertex) {
                *curr = entry->next; // Remove edge pointing to the removed vertex.
                free(entry);
            } else {
                if (entry->dest > vertex) {
                    entry->dest--; // Adjust the destination vertex index.
                }
                curr = &entry->next;
            }
        }
    }

    // Optionally, resize the vertices array.
    graph->vertices = realloc(graph->vertices, graph->numVertices * sizeof(Vertex));
}

void printGraph(Graph* graph) {
    if (!graph) {
        printf("Graph is NULL.\n");
        return;
    }

    for (int i = 0; i < graph->numVertices; i++) {
        Vertex vertex = graph->vertices[i];
        printf("Vertex %d (Value %d) has edges to: ", i, vertex.value);

        Edge* edge = vertex.head;
        while (edge) {
            printf("%d (Weight %d) ", edge->dest, edge->weight);
            edge = edge->next;
        }

        printf("\n");
    }
}

void freeGraph(Graph* graph) {
    if (!graph) return;

    for (int i = 0; i < graph->numVertices; i++) {
        Edge* edge = graph->vertices[i].head;
        while (edge) {
            Edge* temp = edge;
            edge = edge->next;
            free(temp);
        }
    }

    free(graph->vertices);
    free(graph);
}




int main() {
    // Create a graph with 5 vertices
    Graph* graph = createGraph(5);

    // Add some edges
    add_edge(graph, 0, 1, 10);
    add_edge(graph, 0, 4, 20);
    add_edge(graph, 1, 2, 30);
    add_edge(graph, 1, 3, 40);
    add_edge(graph, 2, 3, 60);
    add_edge(graph, 3, 4, 70);
    
    // Set initial values for each vertex
    // Directly use the vertex index for setting values
    set_vertex_value(graph, 0, -1);
    set_vertex_value(graph, 1, 1);
    set_vertex_value(graph, 2, 2);
    set_vertex_value(graph, 3, 3);
    set_vertex_value(graph, 4, 4);
    
    
    printf("Initial graph:\n");
    printGraph(graph);
    int getV = get_vertex_value(graph, 4);
    printf("get vertex value from Vertex 4 : %d \n", getV);
    // Test removing an edge
    printf("\nRemoving edge from 1 to 3.\n");
    remove_edge(graph, 1, 3);
    printGraph(graph);

    // Test removing a vertex
    printf("\nRemoving vertex 2.\n");
    remove_vertex(graph, 2);
    printGraph(graph);

    // Test getting and setting vertex and edge values
    printf("\nSetting vertex 0 value to 100.\n");
    set_vertex_value(graph, 0, 100);
    printf("Vertex 0 value: %d\n", get_vertex_value(graph, 0));

    printf("\nSetting edge value from 0 to 4 to 200.\n");
    set_edge_value(graph, 0, 4, 200);
    printf("Edge value from 0 to 4: %d\n", get_edge_value(graph, 0, 4));

    // Cleanup
    freeGraph(graph);

    return 0;
}

