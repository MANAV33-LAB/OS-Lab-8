typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

struct meminfo {
  int total_pages;
  int free_pages;
  int user_pages;
  int pt_pages;
};
