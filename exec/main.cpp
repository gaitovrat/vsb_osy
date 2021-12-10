#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

#define SUCCESS 0
#define FAILURE 1

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ_END 0
#define WRITE_END 1

int main(int argc, char **argv)
{
    char *commands[2][BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int stat = 0;

    while(true)
    {
        printf("shell> ");

        fgets(buffer, BUFFER_SIZE, stdin);
        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';

        if (!strncmp(buffer, "exit", strlen("exit")))
            break;
        if (!strncmp(buffer, "$?", strlen("$?"))) 
        {
            sprintf(buffer, "%d", stat);
            printf("%s\n", buffer);
            continue;
        }

        char *ptr = strchr(buffer, '|');
        char *out = strchr(buffer, '>');
        int count_commands;
        int n;

        if (out != NULL)
            buffer[out++ - buffer - 1] = '\0';

        if (ptr == NULL)
            count_commands = 1;
        else
        {
            buffer[ptr++ - buffer - 1] = '\0';
            count_commands = 2;
        }

        char *buffers[] = { buffer, ptr };
        for (int i = 0; i < count_commands; ++i)
        {
            int j = 0;
            n = 0;
            int buffer_length = strlen(buffers[i]);

            while (j < buffer_length)
            {
                while (j < buffer_length && isspace(buffers[i][j])) buffers[i][j++] = '\0';
                if (j > buffer_length) break;

                commands[i][n++] = &buffers[i][j];
                while (j < buffer_length && !isspace(buffers[i][j])) j++;
            }

            commands[i][n] = NULL;
        }
        if (!n) continue;

        int command_pipe[2];
        if (pipe(command_pipe) < 0)
        {
            fprintf(stderr, "Unable to create pipe");

            exit(FAILURE);
        }       

        int pids[2];
        if ((pids[0] = fork()) < 0)
        {
            fprintf(stderr, "Unable to create new process 1!");
            continue;
        }

        if (pids[0] == 0)
        {
            close(command_pipe[READ_END]);
            if (count_commands == 2)
                dup2(command_pipe[WRITE_END], STDOUT);
            else if (out != NULL)
            {
                char filename[BUFFER_SIZE];
                int j = 0;
                int k = 0;
                int len = strlen(out);

                while(j < len)
                {
                    while (j < len && isspace(out[j])) j++;
                    if (j >= len) break;

                    filename[k++] = out[j++];
                }

                int fd = open(filename, O_WRONLY | O_CREAT, 0666);
                dup2(fd, STDOUT);
                close(fd);
            }
            close(command_pipe[WRITE_END]);

            int cmd_stat = execvp(commands[0][0], commands[0]);

            if (cmd_stat < 0)
                fprintf(stderr, "Unable to execute command!");
            exit(FAILURE);
        } 

        if (count_commands == 2)
        {
            if ((pids[1] = fork()) < 0)
            {
                fprintf(stderr, "Unable to create new process 2!");
                continue;

            }

            if (pids[1] == 0)
            {
                close(command_pipe[WRITE_END]);
                if (out != NULL)
                {
                    char filename[BUFFER_SIZE];
                    int j = 0;
                    int k = 0;
                    int len = strlen(out);

                    while(j < len)
                    {
                        while (j < len && isspace(out[j])) j++;
                        if (j >= len) break;

                        filename[k++] = out[j++];
                    }

                    int fd = open(filename, O_WRONLY | O_CREAT, 0666);
                    dup2(fd, STDOUT);
                    close(fd);
                }
                dup2(command_pipe[READ_END], STDIN);
                close(command_pipe[READ_END]);

                int cmd_stat = execvp(commands[1][0], commands[1]);

                if (cmd_stat < 0)
                    fprintf(stderr, "Unable to execute command!");
                exit(FAILURE);
            }
        }

        for (int i = 0; i < 2; ++i)
            close(command_pipe[i]);

        if (count_commands == 2)
            waitpid(pids[1], &stat, NULL);
        else
            waitpid(pids[0], &stat, NULL);
    }

    exit(SUCCESS);
}
