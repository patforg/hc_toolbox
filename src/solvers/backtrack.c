#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
 
bool backtrack(int** graph, int* node_count, int* edge_count, int* path)
{
    int pos = 0;

    /* track which nodes have been visited */
    int* visited_nodes;
    visited_nodes = malloc( (*node_count+1) * sizeof(int*));
   
    // initialize all path positions to zero
    for (int i=0; i < *node_count; i++)
    {
        path[i] = 0;
        visited_nodes[i] = 0;
    }
    visited_nodes[*node_count] = 0;

    // set starting position at node 1
    int i = 2;
    path[0] = 1;
    visited_nodes[1] = 1;
    pos = 1;

    while (pos > 0 && pos < *node_count) {

        while (i <= *node_count) {
            // is there an edege from previous node to ith node
            // and ith node was not visited already, use it
            if (graph[ path[pos-1] ][i] == 1 &&
                visited_nodes[i] == 0) {
                path[pos] = i;
                visited_nodes[i] = 1;
                pos++;
                i = 2;
            }  
            else {
                i++;
            }

        }

        // if we haven't constructed a path
        // with the number of nodes we backtrack
        // or if we have a path proper lengh but
        // it doesn't complete the cycle
        if (pos < *node_count ||
            graph[ path[pos-1] ][ path[0] ] == 0) {


            //printf("Backtracking from pos %i\n", pos);

            // go back by one position
            pos--;
            i = path[pos]+1; // continue search at next node

            // clear current path removed flag that node was visited
            visited_nodes[path[pos]] = 0;
            path[pos] = 0;

        }
    }

    free(visited_nodes);

    return ( pos == *node_count && graph [ path[pos-1] ][1] == 1) ? true : false ;
}
