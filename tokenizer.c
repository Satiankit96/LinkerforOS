#include <string.h>
#include <stdio.h>

int getToken(char **argv){
    
    
}


void main(int argc, char **argv){
    
    if (argc == 2){

        char line[1024];
        char *token, *last; 
        FILE *f = fopen(argv[1], "r");
        int lineno=0, pos, line_length;
    
    while (fgets(line, sizeof(line), f) != NULL){
            lineno++;
            pos=0;
            line_length = strlen(line);  
            token = strtok(line, " \t\n");
            while(token != NULL){
                pos = token - line + 1;
                printf("Token: %d:%d :  %s\n",lineno, pos,token);
                token = strtok(NULL, " \t\n"); 
            }
        }
        printf("Final Spot in File: line=%d offset=%d", lineno, line_length);
    }
    else {printf("INVALID NUMBER OF ARGUMENTS");}
           
}