#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    char* env = getenv("KUBERNETES_PORT");
    printf("%s", env);

    return 0;
}