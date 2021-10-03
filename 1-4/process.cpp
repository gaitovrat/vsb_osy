#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <string.h>

#include "log.h"
#include "random.h"

#define BUFFER_SIZE 1024

int createProcess();

int main(void)
{
    int l_pipe[2];
    if (pipe(l_pipe) < 0)
    {
        log(error, "Unable to create pipe!");
        return 1;
    }

    int l_processFirst = createProcess();
    int l_processSecond = createProcess();

    /* First child */
    if (l_processFirst == 0 && l_processSecond != 0)
    {
        close(l_pipe[0]);

        char buffer[BUFFER_SIZE];

        int number = randomNumber();
        sprintf(buffer, "%d", number);
        
        if (write(l_pipe[1], buffer, BUFFER_SIZE) < 0)
        {
            log(error, "First child unable to write to pipe!");
            return 1;
        }

        log(info, "First child write random number.");

        close(l_pipe[1]);
    } 
    if (l_processSecond == 0 && l_processFirst != 0)
    {
        close(l_pipe[1]);

        char buffer[BUFFER_SIZE];
        int sum = 10;

        if (read(l_pipe[0], buffer, BUFFER_SIZE) < 0)
        {
            log(error, "Second child unable to read form pipe!");
            return 1;
        }

        sum += atoi(buffer);
        memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer, "Second child sum: %d", sum);
        log(info, "Second child read number from first child");
        log(info, buffer);

        close(l_pipe[0]);
    } 
    if(l_processFirst > 0 || l_processSecond > 0)
    {
        for (int i = 0; i < 2; ++i)
        {
            close(l_pipe[i]);
            wait(NULL);
        }
    }

    return 0;
}

int createProcess()
{
    int l_process;
    if ((l_process = fork()) < 0)
    {
        log(error, "Unable to fork a child process");
        exit(1);
    }

    return l_process;
}