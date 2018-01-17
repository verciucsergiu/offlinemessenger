typedef struct User {
    char username[1024];
    int messageId;
} User;

typedef struct LoginModel{
    char username[31];
    char password[31];
} LoginModel;

typedef struct Message {
    char text[1024];
    char username[31];
} Message;