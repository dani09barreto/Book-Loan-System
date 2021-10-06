
#define MAXBOOK 10000
#define MAXNAME 50
#define TAMMENSAJE 10
#define MAXNUM 10
#define MAXLINE 1000

typedef struct Book{
    char operation;
    char name [MAXNAME];
    int ISBN;
    char secondpipe[MAXNAME];
}book;

typedef struct Date{
    int day;
    int month;
    int year;
}date;

typedef struct Request{
    int stock;
    char operation;
    date initialDate;
}request;

typedef struct Data{
    char name [MAXNAME];
    int ISBN;
    int stocks;
    request requests [MAXNUM];
}data;






