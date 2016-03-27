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
            // and ith node was not visited already use it
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

        // check if we checked all nodes
        // if no suitable paths were found
        // current node is a dead end
        if (i > *node_count && pos < *node_count) {
            // check if we have other nodes
            // to try for current position
            if (path[pos] > 0 && 
                    path[pos] < *node_count) 
            {
                visited_nodes[path[pos]] = 0;
                i = path[pos]+1;
            } else  // backtrack
            {

                //printf("Backtracking from pos %i\n", pos);

                visited_nodes[path[pos]] = 0;
                path[pos] = 0; // clear current value

                // go back by one position
                pos--;
                i = path[pos]+1; // continue at next node

                visited_nodes[path[pos]] = 0;
                path[pos] = 0; // clear current path value
            }

        }
    }

    free(visited_nodes);

    return ( pos == *node_count && graph [ path[pos-1] ][1] == 1) ? true : false ;
}
