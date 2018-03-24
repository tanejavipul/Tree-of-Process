#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
// Add your system includes here.

#include "ptree.h"

// Defining the constants described in ptree.h
const unsigned int MAX_PATH_LENGTH = 1024;

// If TEST is defined (see the Makefile), will look in the tests
// directory for PIDs, instead of /proc.//
#ifdef TEST
const char *PROC_ROOT = "tests";
#else
const char *PROC_ROOT = "/proc";
#endif

/*
 * Creates a PTree rooted at the process pid. The root of the tree is
 * placed in root. The function returns 0 if the tree was created
 * successfully and 1 if the tree could not be created or if at least
 * one PID was encountered that could not be found or was not an
 * executing process.
 */
int generate_ptree(struct TreeNode **root, pid_t pid) {
    // Here's a trick to generate the name of a file to open. Note
    // that it uses the PROC_ROOT variable
    static int return_val = 0;
    char procfile[MAX_PATH_LENGTH + 1];
    if (sprintf(procfile, "%s/%d/exe", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a filename for exe\n"); //lstat
        return 1;
    }
    struct stat st;
    if (lstat(procfile, &st) == -1) {
        fprintf(stderr, "Program not Running File exe no found\n");

        return 1;
    }
    //printf("%s\n", procfile);
    (*root) = malloc(sizeof(struct TreeNode));
    (*root)->child_procs = NULL;
    (*root)->next_sibling = NULL;
    (*root)->pid = pid;


    if (sprintf(procfile, "%s/%d/cmdline", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a filename for cmdline\n"); //lstat
        return 1;
    }
    FILE *file;
    if (lstat(procfile, &st) == -1) {
        fprintf(stderr, "File for name not found\n");
        (*root)->name = NULL;
        return_val = 1;
    } else {

        file = fopen(procfile, "r");
        if (file == NULL) {
            printf("%s", procfile);
            (*root)->name = NULL;
            return_val = 1;
        } else {
            // (*root)->name = malloc( sizeof(char *) * MAX_PATH_LENGTH);
            char name[MAX_PATH_LENGTH];
            fseek(file, 0, SEEK_END);
            int len_of_char = ftell(file);
            if (len_of_char == 0) {
                (*root)->name = NULL;
                return_val = 1;
            } else {
                (*root)->name = malloc(sizeof(char *) * MAX_PATH_LENGTH);
                fseek(file, 0, SEEK_SET);
                fread(name, MAX_PATH_LENGTH, sizeof(char), file);
                name[len_of_char] = '\0';
                strcpy((*root)->name, name);
            }
        }

        fclose(file);
    }
    //Reading file and making copies of it
    if (sprintf(procfile, "%s/%d/task/%d/children", PROC_ROOT, pid, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a filename for children\n"); //lstat
        return 1;
    }
    if (lstat(procfile, &st) == -1) {
        fprintf(stderr, "File for Childern not found\n");
        (*root)->child_procs = NULL;
        return 1;
    }
    file = fopen(procfile, "r");
    fseek(file, 0, SEEK_END);
    int len_of_char = ftell(file);
    if (len_of_char == 0) {
        (*root)->child_procs = NULL;
        fclose(file);
        return return_val; //return 0
    }

    fseek(file, 0, SEEK_SET);
    char numbers[len_of_char + 1];
    fread(numbers, len_of_char + 1, sizeof(char), file);
    numbers[len_of_char] = '\0';
    char backup[len_of_char + 1];
    strcpy(backup, numbers);
    char backup2[len_of_char + 1];
    strcpy(backup2, numbers);
    fclose(file);


    //BREAKPOINT
    char SPACE[2] = {" "};
    int child_pid = strtol(strtok(numbers, SPACE), NULL, 10);
    if (child_pid == 0) {
        (*root)->child_procs = NULL;
        return return_val;
    } else {
        struct TreeNode *child = NULL;
        if (generate_ptree(&child, child_pid) == 1) {
            (*root)->child_procs = NULL;
            return_val = 1;
        }
        (*root)->child_procs = child;
    }

    struct TreeNode *sib = (*root)->child_procs;
    //finding number of array
    char *token;
    token = strtok(backup, SPACE);
    int counter = -1;
    int x = 0;
    while (token != NULL) {
        counter++;
        token = strtok(NULL, SPACE);
        if (token != NULL) {
            x = strtol(token, NULL, 10);
        }
    }

    //putting in the arrays
    int siblings[counter];
    token = strtok(backup2, SPACE);
    for (int y = 0; y < counter; y++) {
        token = strtok(NULL, SPACE);
        if (token != NULL) {
            x = strtol(token, NULL, 10);
            siblings[y] = x;
        }
    }



    //going through the recursion for children
    for (int y = 0; y < counter; y++) {
        struct TreeNode *new_sib = NULL;
        if (generate_ptree(&new_sib, siblings[y]) == 1) {
            sib->next_sibling = NULL;
            return_val = 1;
        }
        sib->next_sibling = new_sib;
        sib = sib->next_sibling;
    }
    sib->next_sibling = NULL;
    return return_val;
}











/*
 * Prints the TreeNodes encountered on a preorder traversal of an PTree
 * to a specified maximum depth. If the maximum depth is 0, then the
 * entire tree is printed.
 */
void print_ptree(struct TreeNode *root, int max_depth) {
    // Here's a trick for remembering what depth (in the tree) you're at
    // and printing 2 * that many spaces at the beginning of the line.
    static int depth = 0;
    if (root == NULL) {
        return;
    }
    printf("%*s", depth * 2, "");
    printf("%d", root->pid);
    if (root->name == NULL) {
        printf("\n");
    } else {
        printf(": %s\n", root->name);
    }

    if (depth + 1 == max_depth && max_depth != 0) {
        if (root->next_sibling != NULL) {
            print_ptree(root->next_sibling, max_depth);
        }
        return;
    } else {
        if (root->child_procs != NULL) {
            depth++;
            print_ptree(root->child_procs, max_depth);
            depth--;
        }

        if (root->next_sibling == NULL) {
            return;
        } else {
            print_ptree(root->next_sibling, max_depth);
        }
    }
    return;

}



