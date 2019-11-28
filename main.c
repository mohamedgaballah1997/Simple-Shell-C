#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
FILE *f;

void writeToFile(pid_t pid){ //write terminated process in file
printf("Pid %d terminated\n", pid);
fprintf(f,"Pid %d terminated\n", pid);
}
void handler(int sig)
{
//get pid of the terminated process
	pid_t pid=waitpid(-1, NULL, WNOHANG);
	if(pid>0)
	{
	//write in file
	writeToFile(pid);
	}
}



int main()
{
//log file to write terminated processes
	f=fopen("logFile.log","w+");

	while(1)
	{

    	//print current directory (like terminal)
    	printf("%s: ",getcwd(NULL,100));

    	//read command line and split it (space separator)
    	char line[100];
    	int ifAnd=0;
    	gets(line);
    	if(!strcmp(line,""))
        	continue;
    	char *arr[20];
    	arr[0]=strtok(line," ");
    	if(!strcmp(arr[0],"exit"))
        	exit(0);
    	int i=0;
    	while(arr[i]!=NULL){
    	arr[++i]= strtok(NULL," ");
    	// make flag ifAnd=1 if the command ends with &
    	if(arr[i] !=NULL && !strcmp(arr[i],"&"))
    	{
        	arr[i]=NULL;
        	ifAnd=1;
        	signal(SIGCHLD, handler);
        	break;
    	}
    	}
    	// if the command is cd directly change directory without creating child process
    	if(!strcmp(arr[0],"cd"))
    	{

        	if(chdir(arr[1])==-1 && arr[1]!=NULL)
            	printf("bash: cd: 1: No such file or directory\n");
        	continue;
    	}


    	// make a child process
    	pid_t pid=fork();
    	if(pid==0) //child process
    	{
        	// call execvp on input command
        	if(execvp(arr[0],arr)==-1)
            	printf("Unknown Command\n") ;
        	exit(0);

    	}
    	else if(pid>0) // parent process
    	{
        	// wait his child if ther is no &
        	if(!ifAnd)
        	{
            	waitpid(pid,NULL,0);
            	// write in file that child process terminated
           	writeToFile(pid);
        	}

    	}
    	else //fork unsuccessful
    	{
        	printf("Child creation unsuccessful");
    	}
	}
	fclose(f);
	return 0;
}
