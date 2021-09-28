
#define MAXBOOK 10000
#define MAXNAME 20

typedef struct Book{
    char name [MAXNAME];
    int ISBN;
    int copies;
}book;

typedef struct Data{
    char secondpipe[MAXNAME];
    int pid;
}data;
