#include "types.h"
#include "stat.h"
#include "user.h"

int global_var = 10;

int main(void) {
    int local_var = 20;
    int *heap_var = (int*)malloc(sizeof(int));
    *heap_var = 30;

    printf(1, "\n--- VA to PA Mapping ---\n");
    int pa_global = va2pa((int)&global_var);
    printf(1, "Global variable: VA=0x%x, PA=0x%x\n", (int)&global_var, pa_global);

    int pa_local = va2pa((int)&local_var);
    printf(1, "Local variable: VA=0x%x, PA=0x%x\n", (int)&local_var, pa_local);

    int pa_heap = va2pa((int)heap_var);
    printf(1, "Heap variable:   VA=0x%x, PA=0x%x\n", (int)heap_var, pa_heap);

    printf(1, "\n--- Invalid Address Test ---\n");
    int pa_invalid = va2pa(0x7FFFFFFF);
    printf(1, "Invalid VA (0x7FFFFFFF): PA=%d\n", pa_invalid);

    printf(1, "\n--- Fork Test ---\n");
    int pid = fork();
    if (pid < 0) {
        printf(1, "Fork failed\n");
    } else if (pid == 0) {
        int pa = va2pa((int)&global_var);
        printf(1, "[Child]  Global variable: VA=0x%x, PA=0x%x\n", (int)&global_var, pa);
    } else {
        wait();
        int pa = va2pa((int)&global_var);
        printf(1, "[Parent] Global variable: VA=0x%x, PA=0x%x\n", (int)&global_var, pa);
    }
    exit();
}
