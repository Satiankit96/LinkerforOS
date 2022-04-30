#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
    pid_t pid = fork();
    if (pid == 0){
        printf("I'm a child!");
    }
    else if (pid > 0) {
        printf("I'm the parent! My child is %d", pid);
    }
    else printf("Fail");
    return 1;
}