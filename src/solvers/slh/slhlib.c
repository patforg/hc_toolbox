#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "hclib.h"
#include "solvers/slh/slhlib.h"

static int* g_node_count;
static int* g_path;
static int* g_node_index;
static int** g_graph;

void slhlib_init(int* node_count, int* path, int** graph)
{
    int cur_node;
    int next_node;

    g_node_count = node_count;
    g_path = path;
    g_graph = graph;

    g_node_index  = malloc( (*g_node_count+1) * sizeof(int*));

    // initialize all path positions to zero
    for (int i=0; i < *g_node_count; i++)
    {
        cur_node = i+1;

        g_path[i] = cur_node;
        g_node_index[cur_node] = i;

    } //for

    g_node_index[ *g_node_count ] = *g_node_count - 1;

    printf("Started with  : %i gaps\n", count_gaps());
    print_path(g_path, g_node_count);
}

int get_prec_node(int* cur_node) {
    int index = g_node_index[ *cur_node ];
    return ( index == 0 ? g_path[ *g_node_count-1 ] : g_path[ index-1 ] );
}

int get_next_node(int* cur_node) {
    int index = g_node_index[ *cur_node ];
    return ( index == *g_node_count-1 ? g_path[0] : g_path[ index+1 ] );
}

void swap_nodes(int node_a, int node_b) {
    int node_a_index;
    int node_b_index;
    int tmp_node;
    int tmp_index;
    int item_to_swap;

    node_a_index = g_node_index[ node_a ]; // eg 4
    node_b_index = g_node_index[ node_b ]; // eg 2
    
    if (node_a_index > node_b_index) {
        item_to_swap = (*g_node_count - node_a_index) + node_b_index + 1;
    } else {
        item_to_swap = node_b_index - node_a_index + 1;
    } //if

    item_to_swap = floor( item_to_swap / 2);

    printf("Count: %i Start: %i, end: %i swaps: %i\n", *g_node_count, node_a_index, node_b_index, item_to_swap);

    while (item_to_swap > 0) {

        // printf("Swapping %i with %i\n", *node_a, *node_b);
        tmp_node = g_path[node_b_index];

        // swap path values
        g_path[node_b_index] = g_path[node_a_index];
        g_path[node_a_index] = tmp_node;

        // update index
        g_node_index[ node_a ] = node_b_index;
        g_node_index[ node_b ] = node_a_index;

        node_a_index = (node_a_index == *g_node_count-1) ? 0 : node_a_index+1;
        node_b_index = (node_b_index == 0) ? *g_node_count-1 : node_b_index-1;
        
        printf("New a index: %i new b index: %i\n",node_a_index, node_b_index);

        node_a = g_path[node_a_index];
        node_b = g_path[node_b_index];

        item_to_swap--;
    } //while

}

//this is an inefficient version which uses swap_nodes to keep code simple
void switch_nodes(int a, int b, int c, int d){
		swap_nodes(a,d);
		swap_nodes(b,a);
		swap_nodes(d,c);
}

int ladder_search(int start_node, int end_node, int target_node)
{
   int found_node = 0;

   while (start_node != end_node) {
        start_node = get_next_node(&start_node);
        if (g_graph[start_node][target_node] == 1) {
            found_node = start_node;
            break;
        } //if
   }

   return found_node;
}

int count_gaps() {

    int gap_count = 0;
    int cur_node;
    int next_node;
    for (int i=0; i < *g_node_count; i++)
    {
        cur_node = g_path[i];
        next_node = get_next_node(&cur_node);
       
       if (g_graph[cur_node][next_node] == 0) {
           gap_count++; 
       }
    }

    return gap_count;
}
