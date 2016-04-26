struct ordering_node {
    int* ordering;
    struct ordering_node *next;
    struct ordering_node *prev;
};

void slhlib_init(int* node_count, int* path, int** graph);

int get_prec_node(int* cur_node);
int get_next_node(int* cur_node);
void swap_nodes(int node_a, int node_b);
int ladder_search(int start_node, int end_node, int target_node);
int count_gaps();

void clear_gap_list();
int is_gap_in_list(int* gap);
int add_gap_to_list(int* gap);

void clear_ordering_list();
int is_ordering_in_list(int* ordering);
int add_ordering_to_list(int* ordering);
