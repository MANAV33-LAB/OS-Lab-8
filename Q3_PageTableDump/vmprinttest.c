#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
    printf(1, "\n--- Initial Page Table Dump ---\n");
    vmprint();

    printf(1, "\n--- After sbrk(8 * 4096) ---\n");
    char *p = sbrk(8 * 4096);
    (void)p;
    vmprint();

    printf(1, "\n--- After sbrk(-4096) (Shrinking Heap) ---\n");
    sbrk(-4096);
    vmprint();

    exit();
}
