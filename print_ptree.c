#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "ptree.h"

void free_root(struct TreeNode *root);
int all_numbers(char *char_numbers);

int main(int argc, char **argv) {
    // TODO: Update error checking and add support for the optional -d flag
    // printf("Usage:\n\tptree [-d N] PID\n");
    int index=0, a=0, pid=0, int_depth=0,check=1;
    char *depth = NULL;

    if(argc > 4 || argc < 2)
    {
        fprintf(stderr, "Usage:\n\tptree [-d N] PID\n");
        return 1;
    }
    if(argc == 2)
    {
        if(all_numbers(argv[1]) == 1)
        {
            fprintf(stderr, "PID not correct\n");
            return 1;
        }
        pid = strtol(argv[1], NULL, 10);
        int_depth = 0;
    }


    while((a = getopt(argc,argv,"d:")) != -1)
    {
        check = 0;
        if(optarg == NULL)
        {
            fprintf(stderr, "Input Format Not Correct\n");
            return 1;
        }
        switch(a)
        {
            case 'd':
                depth = optarg;
                index = optind - 1;
                if(all_numbers(depth) == 1)
                {
                    fprintf(stderr, "No number found for depth\n");
                    return 1;
                }
                int_depth = strtol(depth, NULL, 10);
                if(argc == 3)
                {
                    if(index == 1) // ex ./print_ptree -d44 22
                    {
                        if(all_numbers(argv[2]) == 1)
                        {
                            fprintf(stderr, "PID not correct\n");
                            return 1;
                        }
                        pid = strtol(argv[2], NULL, 10);
                    }
                    else if(index == 2) // ex ./print_ptree 22 -d44
                    {
                        if(all_numbers(argv[1]) == 1)
                        {
                            fprintf(stderr, "PID not correct\n");
                            return 1;
                        }
                        pid = strtol(argv[1], NULL, 10);
                    }
                    else
                    {
                        fprintf(stderr, "Input Format Not Correct\n");
                        return 1;
                    }

                }
                if(argc == 4)
                {
                    if(argv[index][0] == '-' && argv[index][1] == 'd' && all_numbers(&argv[index][2]) ==0 )
                    {
                        fprintf(stderr, "Input Format Not Correct\n");
                        return 1;
                    }
                    if(index == 2) // ex ./print_ptree 1123 -d 2
                    {
                        if(all_numbers(argv[3]) == 1)
                        {
                            fprintf(stderr, "PID not correct\n");
                            return 1;
                        }
                        pid = strtol(argv[3], NULL, 10);
                    }
                    else if(index == 3) // ex
                    {
                        if(all_numbers(argv[1]) == 1)
                        {
                            fprintf(stderr, "PID not correct\n");
                            return 1;
                        }
                        pid = strtol(argv[1], NULL, 10);
                    }
                    else
                    {
                        fprintf(stderr, "Input Format Not Correct\n");
                        return 1;
                    }
                }

        }
    }

    // NOTE: This only works if no -d option is provided and does not
    // error check the provided argument or generate_ptree. Fix this!
    if(optarg == NULL && argc > 2 && check == 1)
    {
        fprintf(stderr, "Input Format Not Correct\n");
        return 1;
    }
    struct TreeNode *root = NULL;
    //generate_ptree(&root, strtol(argv[1], NULL, 10));
    if(generate_ptree(&root, pid) == 1)
    {
        //print_ptree(root,int_depth);
        return 2;
    }
    print_ptree(root,int_depth);
    free_root(root);
    return 0;

}

void free_root(struct TreeNode *root)
{
    if((*root).next_sibling != NULL)
    {
        free_root((*root).next_sibling);
    }
    if((*root).child_procs != NULL)
    {
        free_root((*root).child_procs);
    }
    if((root)->name != NULL)
    {
	free((root)->name);
    }
    if((root) != NULL)
    {
    free((root));
    }
}

int all_numbers(char *num)
{
    int x = 0;
    while(x<strlen(num))
    {
        if(isdigit(num[x]) == 0)
        {
            return 1;
        }
        x++;
    }
    return 0;

}
