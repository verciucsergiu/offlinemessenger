typedef struct User
{
    char username[1024];
    char messageId[100];
} User;

typedef struct LoginModel
{
    char username[31];
    char password[31];
} LoginModel;

typedef struct Message
{
    char id[100];
    char text[1024];
    char username[31];
    char replyTo[100];
} Message;