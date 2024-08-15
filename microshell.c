#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void	err(char *str)
{
	while (*str)
		write(2, str++, 1);
}

void	set_pipe(int pipe_exist, int *fd, int end)
{
	if (pipe_exist && (dup2(fd[end], end) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
	{
		err("error: fatal\n");
		exit(1);
	}
}

int	cd(char **av, int i)
{
	if (i != 2)
		return (err("error: cd: bad arguments\n"), 1);
	if (chdir(av[1]) == -1)
		return (err("error: cd: cannot change directory to "), err(av[1]), err("\n"), 1);
	return (0);
}

int	exec(char **av, int i, char **envp)
{
	int	exit_status;
	int	pipe_exist = 0;
	int	fd[2];
	int	pid;

	if (av[i] && !strcmp(av[i], "|"))
		pipe_exist = 1;
	if (!pipe_exist && !strcmp(*av, "cd"))
		return (cd(av, i));
	if (pipe_exist && pipe(fd) == -1)
	{
		err("error: fatal\n");
		exit(1);
	}
	pid = fork();
	if (!pid)
	{
		av[i] = 0;
		set_pipe(pipe_exist, fd, 1);
		execve(*av, av, envp);
		err("error: cannot execute ");
		err(*av);
		err("\n");
		exit(1);
	}
	waitpid(pid, &exit_status, 0);
	set_pipe(pipe_exist, fd, 0);
	return WIFEXITED(exit_status) && WEXITSTATUS(exit_status);
}

int	 main(int ac, char **av, char **envp)
{
	int	exit_status = 0;
	int	i = 1;

	(void)ac;
	while (av[i])
	{
		av += i;
		i = 0;
		while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			i++;
		if (i)
			exit_status = exec(av, i, envp);
		if (av[i])
			i++;
	}
	return (exit_status);
}
