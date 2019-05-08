#include <unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include <signal.h>
#include <errno.h>

pid_t *pid, parent_pid;
int count = -1;
int *child_fuel; /* child's fuel status array */
int *child_status; /* child's alive status array */

static void signal_handler(int signo)
{

    switch( signo )
    {
        case SIGUSR1:

            if(parent_pid == getpid()) /* it is the parent */
            {

            }
            else /* it is the child */
            {
              printf( "\n				Bomber %d to base, bomb away!\n", getpid());
            }
            break;


        case SIGUSR2:

            if(parent_pid == getpid())
            {

                int j;
                for(j=0; j<=count; j++)
                {
                        if(child_status[j] == 0 && pid[j] != 0)
                        {
                                printf( "\n				SOS! Plane( %d ) has crashed!\n",pid[j]);
                                pid[j] = 0;

                                break;
                        }
                }


            }
            else /* it is the child */
            {
                child_fuel[count] = 100;

            }
            break;

        case SIGALRM :
                        if(parent_pid != getpid())
                        {

                                printf( "\n				Bomber %d to base, %d fuel left!\n", getpid(), child_fuel[count]);

                                if( signal( SIGALRM, signal_handler) == SIG_ERR)
                                        printf("Child : unable to create handler for SIGALRM\n");

                                alarm(3);

                        }
                        break;


        default:
                break;
    }

    return;
}


/* function launches child process */

void launch(int count)
{
        if( count >= 0)
        {
                pid = realloc(pid,count+1 * sizeof(pid_t));
                child_fuel = realloc(child_fuel, count+1 * sizeof(int));
                child_status = realloc(child_status, count+1 * sizeof(int));

                int temp = (pid[count] = fork());
                if (temp < 0)
                        printf("fork failed\n");
                else if(temp == 0)
                        {

                                if( signal( SIGUSR1, signal_handler) == SIG_ERR) /*setting SIGUSR1 for child*/
                                        printf("Child : unable to create handler for SIGUSR1\n");


                                if( signal( SIGUSR2, signal_handler) == SIG_ERR)/*setting SIGUSR2 for child*/
                                        printf("Child : unable to create handler for SIGUSR2\n");

                                if( signal( SIGALRM, signal_handler) == SIG_ERR)/*setting SIGALRM for child*/
                                        printf("Child : unable to create handler for SIGALRM\n");


                                int temp = 0;
                                child_status[count] = 1;
                                child_fuel[count] = 100;
                                alarm(3);

                                while( child_fuel[count] > 0)
                                {
                                        temp++;
                                        sleep(1);
                                        child_fuel[count] = child_fuel[count] - 5; /* downgrade 5 fuel per second */  
                                        if(child_fuel[count] == 0)
                                        {
                                                child_status[count] = 0;
                                                kill(getppid(),SIGUSR2); /* killing SIGUSR2 (parent), as chlid fuel reached 0 */
                                                exit(0);
                                        }
                                }
                        }
        }
}

/* validating input commamd */

char* substring_input(char* cmd,int index)
{
        if(strlen(cmd)-1 != index)
        {
                char* temp;
                temp = malloc(index * sizeof(char) + 1);
                int i;
                for(i=0; i<index;i++)
                temp[i] = cmd[i];
                temp[index+1]='\0';
                return temp;
        }
        else
                return cmd;
}

/* function retrieves chlid id from input command */

int retrieve_number(char* cmd)
{
        int length = strlen(cmd)-2;
        int process_id = 0;
        int temp = 1;
        while(cmd[length]!=' ')
        {

                if( (cmd[length]-'0' >= 0 ) && (cmd[length]-'0') <= 9)
                {
                        process_id = process_id + ((cmd[length] - '0') * temp);
                        temp = temp * 10;
                }
                else
                {
                        printf("Invalid Process Id : Please enter the a valid process id\n");
                        return 0;
                }

                length--;
        }

        int temp_count = 0;
        int flag_0 = 0;

        while( temp_count <= count)
        {
                if( pid[temp_count] == process_id)
                {
                        flag_0 = 1;
                        break;
                }
                temp_count++;
        }

        if(flag_0 == 1)
                return process_id;
        else
        {
                printf("Invalid Process Id : Please enter the process id which is running/started\n");
                return 0;
        }
}

void main()
{
        printf("List of commands\n1.launch\n2.bomb N(child process Id)\n3.status\n4.refuel N\n5.quit\n");
        char command[256];
        pid = malloc(sizeof(pid_t));
        child_fuel = malloc(sizeof(int));
        child_status = malloc(sizeof(int));
        parent_pid = getpid();

        if( signal( SIGUSR2, signal_handler) == SIG_ERR)
                                        printf("Parent : unable to create handler for SIGUSR2\n");


        while(1)
        {
                if( parent_pid == getpid())
                {
                        printf("Enter the command : ");
                        fgets(command, sizeof(command), stdin);

                        if(!strcmp(substring_input(command,6), "launch\n"))
                        {
                                count++;
                                launch(count);
                                sleep(1);
                        }
                        else if(!strcmp(substring_input(command,6), "status\n"))
                        {
                                int i;
                                int flag = 0;
                                for(i=0; i<=count; i++)
                                {
                                        if(pid[i]!=0)
                                        {
                                                printf(" Status : %d\n",pid[i]);
                                                flag = 1;
                                        }
                                }

                                if(flag == 0)
                                        printf("Status :\nNo child process started\n");
                        }
                        else if(!strcmp(substring_input(command,6), "refuel\n") || !strcmp(substring_input(command,7), "refuel \n") )
                                printf("Invalid Process Id : Please specify the process Id to refuel\n");

                        else if(!strcmp(substring_input(command,6), "refuel") )
                        {
                                int process_id;
                                process_id = retrieve_number(command);

                                if( process_id == 0)
                                        continue;
                                else
                                {
                                        kill(process_id,SIGUSR2);
                                }
                        }

                        else if(!strcmp(substring_input(command,4), "bomb"))
                        {
                                int process_id;
                                process_id = retrieve_number(command);

                                if( process_id == 0)
                                        continue;
                                else
                                {
                                        kill(process_id,SIGUSR1);
                                }

                        }

                        else if(!strcmp(substring_input(command,5),"quit\n"))
                        {

                                int j;
                                for(j=0;j<=count;j++)
                                        kill(pid[j],SIGKILL);
                                exit(0);

                        }

                        else
                                printf("Invalid command\n");
                }
        }
}




