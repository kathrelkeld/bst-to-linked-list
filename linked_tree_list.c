#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct node {
    int value;
    struct node *left;
    struct node *right;
} node;

typedef struct {
    node *left;
    node *right;
} pair;

void bst_to_list_helper(node *n, pair *p_parent) {
    /* LEFT  */
        /* if NULL leaf, connect parent's left to self and self to left of parent */
    if (n->left == NULL) {
        n->left = p_parent->left;
        p_parent->left = n;
    } else {
        pair *p_toleft = (pair *)malloc(sizeof(pair));
        p_toleft->left = p_parent->left;
        p_toleft->right = n;
        /* modify p_toleft with child's values */
        bst_to_list_helper(n->left, p_toleft);
        p_parent->left = p_toleft->left;
        n->left = p_toleft->right;
        free(p_toleft);
    }

    /* RIGHT */
        /* if NULL leaf, connect parent's right to self and self to right of parent */
    if (n->right == NULL) {
        n->right = p_parent->right;
        p_parent->right = n;
    } else {
        pair *p_toright = (pair *)malloc(sizeof(pair));
        p_toright->right = p_parent->right;
        p_toright->left = n;
        /* modify p_toright with child's values */
        bst_to_list_helper(n->right, p_toright);
        p_parent->right = p_toright->right;
        n->right = p_toright->left;
        free(p_toright);
    }
    
    return;
}
node * bst_to_list(node *bst) {
    if (bst == NULL) {
        return bst;
    }
    pair *p = (pair *)malloc(sizeof(pair));
    p->left = NULL;
    p->right = NULL;
    bst_to_list_helper(bst, p);
    (p->left)->left = p->right;
    (p->right)->right = p->left;
    node *result = p->left;
    free(p);
    return result;
}

/* creates a random bst from an array of sorted integers */
node * bst_create(int input[], int len) {
    if (len == 0) {
        return NULL;
    }
    /* use a random index as the head of this tree */
    int i = rand() % len;
    node *bst = (node *)malloc(sizeof(node));
    bst->value = input[i];
    bst->left = bst_create(input, i);
    bst->right = bst_create(input+i+1, len-i-1);
    return bst;
}

/* Destroys a given bst by freeing its nodes */
void bst_destroy(node *bst) {
    if (bst == NULL) {
        return;
    }
    if (bst->right != NULL) {
        bst_destroy(bst->right);
    }
    if (bst->left != NULL) {
        bst_destroy(bst->left);
    }
    free(bst);
    return;
}

typedef struct ll_node {
    node *value;
    struct ll_node *next;
} ll_node;

ll_node **ll_new() {
    ll_node **new_list = (ll_node **)malloc(sizeof(ll_node *));
    *new_list = NULL;
    return new_list;
}

void ll_append(ll_node **n, node *value) {
    ll_node *new_node = (ll_node *)malloc(sizeof(ll_node));
    new_node->value = value;
    new_node->next = NULL;
    ll_node *current_node = *n;
    if (current_node == NULL) {
        *n = new_node;
        return;
    }
    while (current_node->next) {
        current_node = current_node->next;
    }
    current_node->next = new_node;
}

void ll_destroy(ll_node **n) {
    ll_node *current_node = *n;
    while (current_node) {
        ll_node *next = current_node->next;
        free(current_node);
        current_node = next;
    }
    free(n);
}

int ll_all_null(ll_node **n) {
    ll_node *current = *n;
    while (current) {
        if (current->value != NULL) {
            return false;
        }
        current = current->next;
    }
    return true;
}

/* A super dumb way of doing this, but there doesn't appear to be an easy way in printf. */
void print_centered(char *s, int width) {
    int s_length = strlen(s);
    if (s_length > width) {
        printf("Error: print_centered width greater than string length.\n");
        exit(1);
    }
    int padding_left = (width - s_length) / 2;
    int padding_right = width - s_length - padding_left;
    for (int i = 0; i < padding_left; ++i) {
        printf(" ");
    }
    printf("%s", s);
    for (int i = 0; i < padding_right; ++i) {
        printf(" ");
    }
}

/* child nodes return their depth to parent */
int bst_depth(node *bst) {
    if (bst == NULL) {
        return 0;
    }
    int left = bst_depth(bst->left);
    int right = bst_depth(bst->right);
    return (left > right) ? left + 1 : right + 1;
}

/* Prints the given bst */
/*    First calculates depth of tree and width of largest int */
void bst_print(node *bst) {
    if (bst == NULL) {
        printf("<EMPTY TREE>\n");
        return;
    }
    int depth = bst_depth(bst);
    node *rightmost = bst;
    while (rightmost->right != NULL) {
        rightmost = rightmost->right;
    }
    /* Get the length of the maximum value */
    int max_width = floor(log10(abs(rightmost->value))) + 1;
    int cell_width = max_width + 2;
    int current_width = cell_width * pow(2, depth - 1);

    ll_node **current_row = ll_new();
    ll_append(current_row, bst);
    ll_node **next_row = ll_new();

    while (!ll_all_null(current_row)) {
        ll_node *current_ll_node = *current_row;
        while (current_ll_node) {
            /* Append all children to next_row */
            node *current_node = current_ll_node->value;
            if (current_node) {
                ll_append(next_row, current_node->left);
                ll_append(next_row, current_node->right);
            } else {
                /* Insert a pair of fake children into the next row */
                ll_append(next_row, NULL);
                ll_append(next_row, NULL);
            }

            /* Print out the current node */
            char number_string[max_width + 1];
            if (current_node) {
                snprintf(number_string, max_width + 1, "%d", current_node->value);
            } else {
                number_string[0] = '\0';
            }
            print_centered(number_string, current_width);


            current_ll_node = current_ll_node->next;
        }
        current_width = current_width >> 1;
        printf("\n");
        ll_destroy(current_row);
        current_row = next_row;
        next_row = ll_new();
    }
    ll_destroy(current_row);
    ll_destroy(next_row);
}

/* One-line version of bst_print */
void bst_print_oneline_helper(node *bst, bool is_rightmost) {
    if (bst == NULL) {
        return;
    }
    if (bst->left != NULL) {
        bst_print_oneline_helper(bst->left, false);
    }
    if (is_rightmost && (bst->right == NULL)) {
        printf("%d\n", bst->value);
    } else {
        printf("%d,", bst->value);
    }
    if (bst->right != NULL) {
        bst_print_oneline_helper(bst->right, is_rightmost);
    }
    return;
}
void bst_print_oneline(node *bst) {
    bst_print_oneline_helper(bst, true);
}

/* Creates graphviz compatible dotfile format of given bst */
void graphviz_print_bst_helper(node *bst, FILE *fp) {
    if (bst->left) {
        fprintf(fp, "%d -> %d;\n", bst->value, bst->left->value);
        graphviz_print_bst_helper(bst->left, fp);
    }
    if (bst->right) {
        fprintf(fp, "%d -> %d;\n", bst->value, bst->right->value);
        graphviz_print_bst_helper(bst->right, fp);
    }
    return;
}
void graphviz_print_bst(node *bst) {
    FILE *fp = fopen("bst.dot", "w");
    if (!fp) {
        printf("ERROR: Could not open graphviz file for write");
        return;
    }
    fprintf(fp, "digraph bst {\n");
    if (bst) {
        graphviz_print_bst_helper(bst, fp);
    }
    fprintf(fp, "}\n");
    fclose(fp);
}

/* Prints the given circular list and verifies that it is circular */
void circular_list_print_helper(node *current, node *head, node *prev) {
    /* Check that links for current node match what previous has */
    if ((current == NULL) || (prev != current->left)) {
        printf("\nError found in element of circular list\n");
        return;
    }
    printf(",%d", current->value);
    /* Verify that the next node is not where we started */
    if (current->right != head) {
        circular_list_print_helper(current->right, head, current);
    }
    return;
}
void circular_list_print(node *list) {
    if (list == NULL){
        printf("\n");
        return;
    }
    printf("%d", list->value);
    if (list->right != list) {
        circular_list_print_helper(list->right, list, list);
        /* Check the one remaining link pair matches */
        if (list->left->right != list) {
            printf("\nError found in first element of circular list\n");
        }
    }
    printf("\n");
    return;
}

void graphviz_print_circular_list(node *list) {
    FILE *fp = fopen("list.dot", "w");
    if (!fp) {
        printf("ERROR: Could not open graphviz file for write");
        return;
    }
    fprintf(fp, "digraph list {\n");
    if (list) {
        node *current = list;
        while (current->right != list){
            fprintf(fp, "%d -> %d\n", current->value, current->right->value);
            fprintf(fp, "%d -> %d\n", current->value, current->left->value);
            current = current->right;
        }
        fprintf(fp, "%d -> %d\n", current->value, current->right->value);
        fprintf(fp, "%d -> %d\n", current->value, current->left->value);
    }
    fprintf(fp, "}\n");
    fclose(fp);
    return;
}

/* Destroy the given circular list */
void circular_list_destroy(node *list) {
    if (!list) {
        return;
    }
    list->left->right = NULL;
    while (list) {
        node *next = list->right;
        free(list);
        list = next;
    }
}

int main() {
    srand(time(NULL));

    /* Create an of value 1 to N where N is randomly chosen from 0...10 */
    int input_size = rand() % 11 + 5;
    int *test_input = (int *)malloc(input_size * sizeof(int));
    for (int i = 0; i < input_size; ++i) {
        test_input[i] = i + 1;
    }
    node *bst = bst_create(test_input, input_size);
    free(test_input);
    /*bst_print(bst);*/
    graphviz_print_bst(bst);
    system("dot bst.dot -Tpng -o bst.png");
    system("xdg-open bst.png");
    bst = bst_to_list(bst);
    graphviz_print_circular_list(bst);
    system("circo list.dot -Tpng -o list.png");
    system("xdg-open list.png");
    circular_list_destroy(bst);

    return 0;
}
