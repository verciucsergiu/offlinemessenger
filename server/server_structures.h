typedef struct thread
{
    int id;
    int client;
    char username[40];
    int connected;
} thread;

typedef struct ClientsCollection
{
    int count;
    thread list[200];
} ClientsCollection;