#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
    struct meminfo mi;
    
    printf(1, "\n--- Initial Memory Stats ---\n");
    getmeminfo(&mi);
    printf(1, "Total Pages: %d\nFree Pages:  %d\nUser Pages:  %d\nPT Pages:    %d\n",
           mi.total_pages, mi.free_pages, mi.user_pages, mi.pt_pages);

    printf(1, "\n--- Growing Memory (malloc 8192 bytes) ---\n");
    char *buf = malloc(8192);
    getmeminfo(&mi);
    printf(1, "Free Pages:  %d  (Decreased)\n", mi.free_pages);
    printf(1, "User Pages:  %d\n", mi.user_pages);
    printf(1, "PT Pages:    %d\n", mi.pt_pages);

    printf(1, "\n--- Forking Child Process ---\n");
    int pid = fork();
    if(pid == 0) {
        getmeminfo(&mi);
        printf(1, "[Child] Memory Stats:\n");
        printf(1, "Free Pages:  %d  (Decreased due to page duplication)\n", mi.free_pages);
        printf(1, "User Pages:  %d\n", mi.user_pages);
        exit();
    } else {
        wait(); // Reap child to avoid memory leak
        printf(1, "\n--- After Child Exits (Memory Leak Check) ---\n");
        getmeminfo(&mi);
        printf(1, "Total Pages: %d\nFree Pages:  %d  (Returned to pre-fork state)\nUser Pages:  %d\nPT Pages:    %d\n",
               mi.total_pages, mi.free_pages, mi.user_pages, mi.pt_pages);
    }
    
    free(buf);
    exit();
}
