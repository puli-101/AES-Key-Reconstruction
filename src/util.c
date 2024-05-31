#include "util.h"

int VERBOSE = 1;

//print usage header
void print_header(char* bin_name, char* format) {
    print_color(stderr,"Input Formatting Error","red",'\n');
    print_color(stderr, "Usage :","yellow",' ');
    print_color(stderr,bin_name, "cyan",' ');
    print_color(stderr,format, "cyan", '\n');
}

//sets a specific color for printing on terminal
void set_color(FILE* output, char* color) {
    char* buff = get_color(color);
    fprintf(output,"%s",buff);
    free(buff);
}

//prints a message msg with a certain color and a specific end character
void print_color(FILE* output, char* msg, char* color, char end) {
    set_color(output, color);
    fprintf(output, "%s%c",msg,end);
    set_color(output, "default");
}

//returns a specific color for printing on terminal
char* get_color(char* color) {
    if (!strcmp(color,"red")) {
        return strdup("\033[1;31m"); 
    } else if (!strcmp(color,"yellow")) {
        return strdup("\033[1;33m"); 
    } else if (!strcmp(color, "cyan")) {
        return strdup("\033[0;36m"); 
    } else if (!strcmp(color,"green")) {
        return strdup("\033[0;32m"); 
    }
    return strdup("\033[0m");
}

//extracts all bytes of a text and stores it in a buffer
//returns the size of the textfile
int extract_text(char* file, char* buffer) {
    int length;
    FILE* f = fopen(file, "r");
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    fread (buffer, sizeof(char), length, f);
    fclose(f);
    return length;
}

//xor of a and b where a and b are characters in {'0','1'}
char ascii_xor(char a, char b) {
    if ((a == '0' || a == '1') && (b == '0' || b == '1'))
        return ((a == '1' && b == '1') || (a == '0' && b == '0')) ? '0' : '1';
    else
        fprintf(stderr,"Error while x-oring a : %c , b : %c \n",a,b);
    exit(EXIT_FAILURE);
}

// Function to create a new node
list* create_cell(int *data, int size) {
    list* newNode = (list*)malloc(sizeof(list));
    check(newNode);
    newNode->data = (int*)malloc(sizeof(int)*size);
    check(newNode->data);
    newNode->size = size;
    for(int i = 0; i < size; i++)
        newNode->data[i] = data[i];
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

// Function to insert a node at the beginning
void insert(list** head, int* data, int size) {
    list* newNode = create_cell(data, size);
    check(newNode);
    
    if (*head == NULL) {
        *head = newNode;
    } else {
        newNode->next = *head;
        (*head)->prev = newNode;
        *head = newNode;
    }
}

// Function to delete a node given a pointer to the node
void delete_elt(list** head, list* node) {
    if (*head == NULL || node == NULL) return;
    
    // If the node to be deleted is the head node
    if (*head == node) {
        *head = node->next;
        if (*head != NULL) {
            (*head)->prev = NULL;
        }
        free_cell(node);
        return;
    }
    
    // If the node to be deleted is not the last node
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    
    // If the node to be deleted is not the first node
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    
    free_cell(node);
}

// Function to print the list from the end
void print_list(list* head) {
    if (head == NULL) return;
    
    list* temp = head;
    
    while (temp != NULL) {
        printf("(");
        for (int i = 0; i < temp->size-1; i++)
            printf("%d, ", temp->data[i]);
        printf("%d)",temp->data[temp->size-1]);
        printf("|");
        temp = temp->next;
    }
    printf("\n");
}

void check(void* d) {
    if (!d) {
        fprintf(stderr,"Heap error while malloc\n");
        exit(EXIT_FAILURE);
    }
}

void free_cell(list* cell) {
    if (cell) {
        if (cell->data)
            free(cell->data);
        free(cell);
    }
}

void free_list(list** head) {
    list* current = *head;
    list* next;
    
    while (current != NULL) {
        next = current->next;
        free_cell(current);
        current = next;
    }
    
    *head = NULL;
}

//returns the n_th element of a list 
list* getFromIndex(list* lst, int index) {
    int i = 0;
    list* iter;
    for(iter = lst; iter && index != i; iter = iter->next, i++);
    return iter;
}