#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <ncurses.h>

extern int errno;

#define PORT 3001

int main (int argc, char *argv[])
{
  int sd;
  struct sockaddr_in server;
  char buf[10];

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror ("Eroare la socket.\n");
    return errno;
  }

  server.sin_family = AF_INET;

  server.sin_addr.s_addr = inet_addr(argv[1]);

  server.sin_port = htons (PORT);

  // if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  // {
  //   perror ("Eroare la connect.\n");
  //   return errno;
  // }
  initscr();
  clear();
  noecho();
  cbreak();
  if(fork()) 
  {
    char msg[256];
    int i=0;
    while(1)
    {
      int ch;
      ch = getch();
      printw("%c", ch);
      // if (ch != '\0')
      // {
      //   printw("%s", msg);
      // } 
      // else if (ch != '\b')
      // {
      //   msg[i++] = ch;
      // }
      // else 
      // {
      //   msg[i] = '\0';
      //   i--;
      // }
      refresh();
    }
    // char msg[256]; // stringul trimis cater server.
    // while(1)
    // {
    //   /* citirea mesajului */
    //   printf ("$:");
    //   fflush (stdout);
    //   fgets(&msg, 256, stdin);
    //   /* trimiterea mesajului la server */

    //   if (write (sd,&msg, sizeof(msg)) <= 0)
    //   {
    //     perror ("Eroare la write() spre server.\n");
    //     return errno;
    //   }
    // }
  }
  else
  {
    char msg2[256]; // stringul primit de la server.
    // while(1) 
    // {
    //   /* citirea raspunsului dat de server 
    //     (apel blocant pina cind serverul raspunde) */
    //   if (read (sd, &msg2, 256) < 0)
    //   {
    //     perror ("Eroare la read() de la server.\n");
    //     return errno;
    //   }
    //   /* afisam mesajul primit */
    //   printf ("$: %s", msg2);
    //   fflush (stdout);
    // }
  }
  /* inchidem conexiunea, am terminat */
  close (sd);
}
