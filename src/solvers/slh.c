#include <stdio.h>
#include <stdlib.h>
#include "hclib.h"
#include "solvers/slh/slhlib.h"

static int* g_node_count;
static int* g_path;
static int* g_node_index;
static int** g_graph;

static void stage0();
static void stage1();
static void stage2();
static void stage3();
static void float2();
static void float3();
static void float4();
static void float5();

static int flo1(int* gap);
static int flo2(int* gap);
static int flo3(int* gap);
static int flo4(int* gap);
static int flo5(int* gap);


bool slh(int** graph, int* node_count, int* edge_count, int* path)
{
    int gap_count;

    /* set global vars */
    g_node_count = node_count;
    g_path = path;
    g_graph = graph;

    slhlib_init(node_count,path,graph);

    printf("Started with  : %i gaps\n", count_gaps());
//    print_path(g_path, g_node_count);

    stage0(graph, node_count, path);
    stage1(graph, node_count, path);
    /*
    init();
    stage1();
    */

//    printf("Final path:");
//    print_path(path, node_count);
    gap_count = count_gaps();
    printf("Left with  : %i gaps\n", gap_count);

    return gap_count == 0 ? true : false ;
}


/*************************************************************
 *                STAGE 0: Closing Transformations           *
 *************************************************************/
void stage0(int** graph, int* node_count, int* path) 
{
    int cur_node;
    int next_node;
    int prec_node;
    int inner_cur_node;
    int inner_next_node;
    int inner_prec_node;
    int inner_next_next_node;

    for (int i=0; i < *node_count; i++)
    {
        cur_node = path[i];
        next_node = get_next_node(&cur_node);
        prec_node = get_prec_node(&cur_node);

        // found a gap
        if (graph[cur_node][next_node] == 0) {

            //printf("Found gap between %i and %i\n", cur_node, next_node);

            for (int j=0; j < *node_count; j++) {
                inner_cur_node = j+1;

                // don't check nodes involved in current gap
                if (inner_cur_node == cur_node 
                    || inner_cur_node == next_node) {
                    continue;
                } //if

                inner_next_node = get_next_node( &inner_cur_node );
                inner_prec_node = get_prec_node( &inner_cur_node );

                // don't check nodes involved in current gap
                if (inner_next_node == cur_node 
                    || inner_next_node == next_node) {
                    continue;
                } //if

                //printf("Processing %i, next node is %i, preceding node is %i\n", inner_cur_node, inner_next_node, inner_prec_node);

                // check edge that can be used to close the gap
                // closing 2-opt type 1 transformation
                if (graph[next_node][inner_next_node] == 1 
                    && graph[inner_cur_node][inner_next_node] == 0) {

                    swap_nodes( inner_next_node, cur_node);
                    i = 0; // reset i
                    //printf("Closing 2-opt type 1 between nodes %i and %i\n", inner_cur_node, inner_next_node);
                    //print_path(path, node_count);
                    break;
                } //if

                // check edge that can be used to close the gap
                // closing 2-opt type 2 transformation
                if (graph[cur_node][inner_cur_node] == 1 
                    && graph[inner_next_node][next_node] == 1) {

                    swap_nodes( inner_next_node, cur_node);
                    i = 0; // reset i
                    //printf("Closing 2-opt type 2 between nodes %i and %i\n", cur_node, next_node);
                    //print_path(path, node_count);
                    break;

                } //if

                // check edge that can be used to close the gap
                // closing 3-opt type 2 transformation
                if (graph[next_node][inner_cur_node] == 1 ) {

                    inner_next_node = ladder_search(inner_cur_node, cur_node, cur_node);

                    while (inner_next_node != 0) {
                        inner_next_next_node = get_next_node( &inner_next_node );

                        if (inner_next_next_node != cur_node
                            && graph[inner_next_next_node][inner_prec_node]) {
                            
                            //printf("Closing 3-opt between nodes %i and %i\n", cur_node, next_node);
                            //printf("Swapped %i, %i and %i, %i\n", next_node, inner_prec_node, inner_next_next_node, cur_node);
                            //print_path(path, node_count);
                            swap_nodes( next_node, inner_prec_node);
                            swap_nodes( inner_next_next_node, cur_node);

                            //print_path(path, node_count);

                            inner_next_node = 0; // stop looking for next nodes
                            i = 0; // reset i
                        } else {
                            inner_next_node = ladder_search(inner_next_node, cur_node, cur_node);
                        } //if
                    } //while

                } //if

            } //for 

            //printf("Left with  : %i gaps\n", count_gaps());
        } //if
    } //for

}

void stage1(int** graph, int* node_count, int* path) 
{
    
    int** gap_list;
    int** ordering_list;
    int prev_gap_count;
    int gap_count;
    int y,x;
    int found_flo_at;
    int found_new_gap;
    int start_at_flo = 0;
    int start_at_index = 0;
    int* gap;
    int (*flo_functions[5])(int* gap);
    
    flo_functions[0] = flo1;
    flo_functions[1] = flo2;
    flo_functions[2] = flo3;
    flo_functions[3] = flo4;
    flo_functions[4] = flo5;
    
    gap_count = count_gaps();
    prev_gap_count = gap_count;
    gap = malloc( 2 * sizeof(int*));
    
    clear_gap_list();
    //add_ordering_to_list(g_path, 0, 0, gap_count);

    printf("Stage 1\n");
    while (1) {
        printf(".");
        found_flo_at  = -1;
        for (int i=start_at_index; i < *node_count; i++)
        {
            y = path[i];
            x = get_next_node(&y);

            // found a gap
            if (graph[y][x] == 0) {
                gap[0] = y;
                gap[1] = x;
                
                // try each flo function until one
                // can perform a transformation (returns 1)
                for (int j = start_at_flo; j < 5; j++) {
                    if ( (*flo_functions[j])(gap) == 1) {
                        add_gap_to_list(gap);
                        start_at_index = i;
                        found_flo_at = j;
               //         start_at_flo = j;
                        printf("Found transformation in flo%i around gap [%i,%i] and %i gap left\n", j+1, gap[0],gap[1], count_gaps());
                        //print_path(g_path, g_node_count);
                        break;
                    } //if
                } //for

                if (found_flo_at >= 0) {
                    break;
                }  //if

            } //if
        } //for

        gap_count = count_gaps();
        if (gap_count == 0) {
            break; // we found a solution
        } //if


        // 1.1 a) check that new ordering has a new gap not in gap_list
        // add ordering to ordering list 
        found_new_gap = -1;
        for (int i=0; i < *node_count; i++) {
            y = path[i];
            x = get_next_node(&y);

            // found a gap
            if (graph[y][x] == 0) {
                gap[0] = y;
                gap[1] = x;

                if (is_gap_in_list(gap) == 0) {
                    printf("Found gap %i, %i\n", gap[0],gap[1]);
                    found_new_gap = 1;
                    start_at_index = i;
//                    update_ordering(found_flo_at, i, gap_count);
                    add_ordering_to_list(g_path, 0, 0, gap_count);
                    break;
                } //if
            } //if
        } //for

        // 1.1 b) no new gap found revert to prev ordering and try another transformation
        if (found_new_gap == -1) {
            printf("No new gap found backtrack\n");
            if (backtrack_ordering(g_path, &start_at_flo, &start_at_index, &prev_gap_count) == 1) {
                start_at_flo++;
                gap_count = count_gaps();
                printf("continuing index: %i, flo: %i...\n", start_at_index, start_at_flo);
            } else {
                printf("Cannot backtrack ordering list is empty\n");
                break;
            } //if
        } //if

        printf("after if\n");
        // 1.2 if transformation reduced number of gaps clear gaps and ordering. If gaps is 0 stop
        if (gap_count < prev_gap_count) {
            printf("Reduced gaps to %i\n", gap_count);
            clear_gap_list();
            clear_ordering_list();
            prev_gap_count = gap_count;
            start_at_index = 0;
            start_at_flo = 0;
        } //if
        printf("after gap check\n");

    // 1.3 otherwise goto 1.1
    } //while

    free(gap);
}

void stage2(int** graph, int* node_count, int* path) 
{

}



void stage3(int** graph, int* node_count, int* path) 
{

}

int flo1(int* gap)
{
    int x,y,a,b,c,d;
    int prec_node;

    y = gap[0];
    x = gap[1];

    for (int i = 0; i < *g_node_count; i++) {

        a = g_path[i];
        c = get_prec_node(&a);

        // look for a edge between x and a
        if (g_graph[x][a] == 1) {        

            // from a find an edge b connected to a
            // or has a path to a and is connected to y
            // or if the following node d is connected to 
            // c node
            prec_node = a;
            b = get_next_node(&a);
            while (g_graph[prec_node][b] == 1 && b != y) {
                
                d = get_next_node(&b);
                if (g_graph[b][y] == 1
                    || g_graph[d][c] ==1) {
                    swap_nodes(d,y);
                    swap_nodes(x,c);
                    return 1;
                } //if

                // move to next node
                prec_node = b;
                b = get_next_node(&b);

            } //while
        } //if
        
    } //for

    return 0;
}

void float2()
{
		//I added a not_found flag to stop when a pattern has been found
		bool not_found = true;
    for (int i=0; i < *g_node_count && not_found; i++)
    {
		//the name of the nodes, not the index --maxime
        int y = g_path[i];
        int x = get_next_node(&y);

        // found a gap
        if (g_graph[y][x] == 1) continue;

		//printf("Found gap between %i and %i\n", cur_node, next_node);

		//can't be beside x because c must be in between
		//int a = get_next_node(&x);
		int a = get_next_node(&x);
		while(not_found) {
				a = ladder_search(a, y, x);
				if (a == 0) break;
				int c = get_prec_node(&a);
						int b = a;
						//d can't be beside a because b must be in between
						int d = get_next_node(&a);
						while(not_found){
								//b can't be beside y because d must be in between
								b = ladder_search(b, get_prec_node(&y), y);
								if (b == 0) break;
								d = get_next_node(&b);
								not_found = false;
						}
						while(not_found){
								//can't be beside y because d must be in between
								d = ladder_search(d, y, c);
								if (d == 0) break;
								b = get_prec_node(&d);
								not_found = false;
						}
						if(!not_found){
								swap_nodes(x, c);
								swap_nodes(d, y);
								swap_nodes(c,d);
								printf("y = %i, x = %i, a = %i, b = %i, c = %i, d = %i \n", y, x, a, b, c, d );
						}

		}
	} //for
}

int flo2(int* gap)
{
	bool not_found = true;
    int y = gap[0];
    int x = gap[1];
	int a = get_next_node(&x);
	while(not_found) {
			a = ladder_search(a, y, x);
			if (a == 0) break;
			int c = get_prec_node(&a);
			int b = a;
			//d can't be beside a because b must be in between
			int d = get_next_node(&a);
			while(not_found){
					//b can't be beside y because d must be in between
					b = ladder_search(b, get_prec_node(&y), y);
					if (b == 0) break;
					d = get_next_node(&b);
					not_found = false;
			}
			while(not_found){
					//can't be beside y because d must be in between
					d = ladder_search(d, y, c);
					if (d == 0) break;
					b = get_prec_node(&d);
					not_found = false;
			}
			if(!not_found){
					swap_nodes(x, c);
					swap_nodes(d, y);
					swap_nodes(c,d);
					//printf("y = %i, x = %i, a = %i, b = %i, c = %i, d = %i \n", y, x, a, b, c, d );
			}

	}
	return (not_found ? 0 : 1);
}

void float3()
{
		//I added a not_found flag to stop when a pattern has been found
		bool not_found = true;
    for (int i=0; i < *g_node_count && not_found; i++)
    {
		//the name of the nodes, not the index --maxime
        int y = g_path[i];
        int x = get_next_node(&y);

        // found a gap
        if (g_graph[y][x] == 1) continue;

		//printf("Found gap between %i and %i\n", cur_node, next_node);

		//e and c are temporary for now
		int e = get_next_node(&x);
		int c = get_next_node(&e);
		//d and f are temporary for now
		int f = get_prec_node(&y);
		int d = get_prec_node(&f);

		int a = c;
		while(not_found) {

				//b is temporary for now
				int b = get_prec_node(&d);
				a = ladder_search(a, b, x);
				if (a == 0) break;
				b = get_next_node(&a);
				//b must be connected to y
				if (g_graph[b][y] == 0) continue;
				//e is temporary for now
				e = get_next_node(&x);
				c = e;
				while(not_found){
						c = get_next_node(&c);
						if (c == a) break;
						e = get_prec_node(&c);

						int d = b;
						while(not_found){
								d = ladder_search(d, f, c);
								if (d == 0) break;
								f = get_next_node(&d);
								not_found = false;
								swap_nodes(x,e);
								swap_nodes(c,a);
								swap_nodes(b,d);
								swap_nodes(f,y);
								printf("y = %i, x = %i, e = %i, c = %i, a = %i, b = %i, d = %i, f = %i \n", y, x, e, c, a, b, d, f);


						}

				}

		}
	} //for
}

int flo3(int* gap)
{
		//I added a not_found flag to stop when a pattern has been found
		bool not_found = true;
		int y = gap[0];
		int x = gap[1];

		//e and c are temporary for now
		int e = get_next_node(&x);
		int c = get_next_node(&e);
		//d and f are temporary for now
		int f = get_prec_node(&y);
		int d = get_prec_node(&f);

		int a = c;
		while(not_found) {

				//b is temporary for now
				int b = get_prec_node(&d);
				a = ladder_search(a, b, x);
				if (a == 0) break;
				b = get_next_node(&a);
				//b must be connected to y
				if (g_graph[b][y] == 0) continue;
				//e is temporary for now
				e = get_next_node(&x);
				c = e;
				while(not_found){
						c = get_next_node(&c);
						if (c == a) break;
						e = get_prec_node(&c);

						int d = b;
						while(not_found){
								d = ladder_search(d, f, c);
								if (d == 0) break;
								f = get_next_node(&d);
								not_found = false;
								swap_nodes(x,e);
								swap_nodes(c,a);
								swap_nodes(b,d);
								swap_nodes(f,y);
								//printf("y = %i, x = %i, e = %i, c = %i, a = %i, b = %i, d = %i, f = %i \n", y, x, e, c, a, b, d, f);


						}

				}

		}
	return (not_found ? 0 : 1);
}

void float4()
{
		//I added a not_found flag to stop when a pattern has been found
		bool not_found = true;
    for (int i=0; i < *g_node_count && not_found; i++)
    {
		//the name of the nodes, not the index --maxime
        int y = g_path[i];
        int x = get_next_node(&y);

        // found a gap
        if (g_graph[y][x] == 1) continue;

		//printf("Found gap between %i and %i\n", cur_node, next_node);

		//e and c are temporary for now
		int e = get_next_node(&x);
		int c = get_next_node(&e);
		//d and f are temporary for now
		int d = get_prec_node(&y);
		int f = get_prec_node(&d);
		int b = get_prec_node(&f);

		int a = c;
		while(not_found) {
				a = ladder_search(a, b, x);
				if (a == 0) break;
				b = get_next_node(&a);

				//e is temporary for now
				c = e;
				while(not_found){
						c = get_next_node(&c);
						if (c == a) break;
						e = get_prec_node(&c);
						//f is temporary for now
						f = get_next_node(&b);
						int d = f;
						while(not_found){
								d = ladder_search(d, y, c);
								if (d == 0) break;
								f = get_prec_node(&d);
								if(g_graph[e][b] || g_graph[f][y]){
										not_found = false;
										printf("y = %i, x = %i, e = %i, c = %i, a = %i, b = %i, d = %i, f = %i \n", y, x, e, c, a, b, d, f);
										switch_nodes(c,a,b,f);
										switch_nodes(x,e,b,f);
										swap_nodes(x,e);
										swap_nodes(c,a);
										swap_nodes(b,f);
								}
						}
				}
		}
	} //for
}

int flo4(int* gap)
{
		int y = gap[0];
		int x = gap[1];
		bool not_found = true;
		//e and c are temporary for now
		int e = get_next_node(&x);
		int c = get_next_node(&e);
		//d and f are temporary for now
		int d = get_prec_node(&y);
		int f = get_prec_node(&d);
		int b = get_prec_node(&f);

		int a = c;
		while(not_found) {
				a = ladder_search(a, b, x);
				if (a == 0) break;
				b = get_next_node(&a);

				//e is temporary for now
				c = e;
				while(not_found){
						c = get_next_node(&c);
						if (c == a) break;
						e = get_prec_node(&c);
						//f is temporary for now
						f = get_next_node(&b);
						int d = f;
						while(not_found){
								d = ladder_search(d, y, c);
								if (d == 0) break;
								f = get_prec_node(&d);
								if(g_graph[e][b] || g_graph[f][y]){
										not_found = false;
										//printf("y = %i, x = %i, e = %i, c = %i, a = %i, b = %i, d = %i, f = %i \n", y, x, e, c, a, b, d, f);
										switch_nodes(c,a,b,f);
										switch_nodes(x,e,b,f);
										swap_nodes(x,e);
										swap_nodes(c,a);
										swap_nodes(b,f);
								}
						}
				}
		}
	return (not_found ? 0 : 1);
}

void float5()
{
		//I added a not_found flag to stop when a pattern has been found
		bool not_found = true;
    for (int i=0; i < *g_node_count && not_found; i++)
    {
			printf("test1");
		//the name of the nodes, not the index --maxime
        int y = g_path[i];
        int x = get_next_node(&y);

        // found a gap
        if (g_graph[y][x] == 1) continue;

		//printf("Found gap between %i and %i\n", cur_node, next_node);

		//e and c are temporary for now
		int e = get_next_node(&x);
		int c = get_next_node(&e);
		int a = c;
		while(not_found) {
				a = ladder_search(a, y, x);
				if (a == 0) break;
				int f = get_next_node(&a);
				//c is temporary for now
				c = get_prec_node(&a);
				int e = x;
				while(not_found){
						e = ladder_search(e, c, f);
						if (e == 0) break;
						int c = get_next_node(&e);
						//g b, h and j are temporary for now
						int g = get_next_node(&f);
						int b = get_next_node(&g);
						int j = get_next_node(&b);
						int h = get_prec_node(&y);

						int d = j;
						while(not_found){
								d = ladder_search(d, h, c);
								if (d == 0) break;
								h = get_next_node(&d);
								j = get_prec_node(&d);

								int b = g;
								while(not_found){
										b = ladder_search(b, j, y);
										if (b == 0) break;
										g = get_prec_node(&b);
										not_found = false;
										printf("y = %i, x = %i, e = %i, c = %i, a = %i, f = %i, g = %i, b = %i, j = %i, d = %i, h = %i \n", y, x, e, c, a, f, g, b, j, d, h);
										switch_nodes(c,a,f,g);
										switch_nodes(x,e,f,g);
										swap_nodes(x,e);
										swap_nodes(c,a);
										swap_nodes(f,g);
										swap_nodes(b,d);
										swap_nodes(h,y);


								}
						}

				}

		}
	} //for
}

int flo5(int* gap)
{
		int y = gap[0];
		int x = gap[1];
		bool not_found = true;

		//e and c are temporary for now
		int e = get_next_node(&x);
		int c = get_next_node(&e);
		int a = c;
		while(not_found) {
				a = ladder_search(a, y, x);
				if (a == 0) break;
				int f = get_next_node(&a);
				//c is temporary for now
				c = get_prec_node(&a);
				int e = x;
				while(not_found){
						e = ladder_search(e, c, f);
						if (e == 0) break;
						int c = get_next_node(&e);
						//g b, h and j are temporary for now
						int g = get_next_node(&f);
						int b = get_next_node(&g);
						int j = get_next_node(&b);
						int h = get_prec_node(&y);

						int d = j;
						while(not_found){
								d = ladder_search(d, h, c);
								if (d == 0) break;
								h = get_next_node(&d);
								j = get_prec_node(&d);

								int b = g;
								while(not_found){
										b = ladder_search(b, j, y);
										if (b == 0) break;
										g = get_prec_node(&b);
										not_found = false;
										//printf("y = %i, x = %i, e = %i, c = %i, a = %i, f = %i, g = %i, b = %i, j = %i, d = %i, h = %i \n", y, x, e, c, a, f, g, b, j, d, h);
										switch_nodes(c,a,f,g);
										switch_nodes(x,e,f,g);
										swap_nodes(x,e);
										swap_nodes(c,a);
										swap_nodes(f,g);
										swap_nodes(b,d);
										swap_nodes(h,y);


								}
						}

				}

		}
	return (not_found ? 0 : 1);
}

