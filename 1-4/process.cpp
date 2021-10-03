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
void createPipe(int *t_pipe);

int main(void)
{
    int l_pipeFirst[2];
    int l_pipeSecond[2];
    createPipe(l_pipeFirst);
    createPipe(l_pipeSecond);

    int l_processFirst = createProcess();
    int l_processSecond = createProcess();

    /* First child */
    if (l_processFirst == 0 && l_processSecond != 0)
    {
        close(l_pipeFirst[0]);
        for (int i = 0; i < 2; ++i)
            close(l_pipeSecond[i]);

        while (1)
        {
            char buffer[BUFFER_SIZE];

            int number = randomNumber();
            sprintf(buffer, "%d", number);
            if (write(l_pipeFirst[1], buffer, BUFFER_SIZE) < 0)
            {
                log(error, "First child unable to write to first pipe!");
                continue;
            }
        }
        close(l_pipeFirst[1]);
    } 
    /* Second child */
    if (l_processSecond == 0 && l_processFirst != 0)
    {
        close(l_pipeFirst[1]);
        close(l_pipeSecond[0]);

        int sum = 0;
        char buffer[BUFFER_SIZE];

        while (1)
        {
            if (read(l_pipeFirst[0], buffer, BUFFER_SIZE) < 0)
            {
                log(error, "Second child unable to read from first pipe!");
                continue;
            }

            sum += atoi(buffer);

            memset(buffer, 0, BUFFER_SIZE);
            sprintf(buffer, "%d", sum);
            if (write(l_pipeSecond[1], buffer, BUFFER_SIZE) < 0)
            {
                log(error, "Second child unable to write to second pipe!");
            }
        }

        close(l_pipeFirst[0]);
        close(l_pipeSecond[1]);
    } 
    /* Parent */
    if(l_processFirst > 0 || l_processSecond > 0)
    {
        for (int i = 0; i < 2; ++i)
            close(l_pipeFirst[i]);
        close(l_pipeSecond[1]);

        char buffer[BUFFER_SIZE];
        int count = 0;
        int sum = 0;

        while (1)
        {
            if (read(l_pipeSecond[0], buffer, BUFFER_SIZE) < 0)
            {
                log(error, "Parent unable to read from second pipe!");
                continue;
            }

            sum = atoi(buffer);
            count++;

            memset(buffer, 0, BUFFER_SIZE);
            sprintf(buffer, "%d %d %f", sum, count, sum / static_cast<float>(count));

            log(info, buffer);
        }

        close(l_pipeSecond[0]);
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

void createPipe(int *t_pipe)
{
    if (pipe(t_pipe) < 0)
    {
        log(error, "Unable to create pipe!");
        exit(1);
    }
}