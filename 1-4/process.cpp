#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "log.h"
#include "random.h"

#define BUFFER_SIZE 1024

int main(void)
{
    int l_pipe[2];
    if (pipe(l_pipe) < 0)
    {
        log(error, "Unable to create pipe!");
        return 1;
    }

    int l_process;
    if ((l_process = fork()) < 0)
    {
        log(error, "Unable to fork a child process");
        return 1;
    }

    /* Parent process */
    if (l_process > 0)
    {
        char buffer[BUFFER_SIZE];

        int number = randomNumber();
        sprintf(buffer, "Number from parent: %d\n", number);

        if (write(l_pipe[1], buffer, BUFFER_SIZE) < 0)
        {
            log(error, "Parent: Unable to write to pipe!");
            return 1;
        } else
            log(info, "Parent: writes to pipe.");
    }
    /* Child process */
    else
    {
        char buffer[BUFFER_SIZE];

        if (read(l_pipe[0], buffer, BUFFER_SIZE) < 0)
        {
            log(error, "Child: Unable to read from pipe!");
        } else
        {
            log(info, "Child: Read successfully from pipe.");
            log(info, buffer);
        }
    }

    return 0;
}