#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_list
{
	char **cmd;
	int pipe[2];
	int type;
	struct s_list *previous;
	struct s_list *next;
}				t_list;

int	orignals[2];

# define CD_ARGUMENTS "error: cd: bad arguments"
# define CD_FAILED "error: cd: cannot change directory to "
# define EXIT_FATAL "error: fatal"
# define EXECVE_FAIL "error: cannot execute "

void *ft_calloc(size_t size)
{
	void 	*ret;
	size_t	count;
	
	ret = (void *)malloc(size);
	if (!ret)
		return (NULL);
	count = 0;
	while (count < size)
	{
		((char *)ret)[count] = 0;
		count++;
	}
	return (ret);
}

int	ft_printstrendl(int fd, char *str, char *str2)
{
	while (*str)
		write(fd, str++, 1);
	while (str2 && *str2)
		write(fd, str2++, 1);
	write(fd, "\n", 1);
	if (!strncmp(str, EXIT_FATAL, 40))
		exit(-1);
	return (1);
}

int ft_darrsize(char **arr)
{
	int size;

	size = 0;
	while (arr && arr[size])
		size++;
	return size;
}

int	ft_strappend(char ***org, char *str)
{
	char **newarr;
	int	count;

	newarr = (char **)malloc(sizeof(char *) * (ft_darrsize(*org) + 2));
	if (!newarr)
		return (0);
	count = 0;
	while (*org && (*org)[count])
	{
		newarr[count] = (*org)[count];
		count++;
	}
	newarr[count] = str;
	newarr[count + 1] = NULL;
	if (*org)
		free(*org);
	*org = newarr;
	return (1);
}

int	ft_parser(int argc, char *argv[], t_list *cur)
{
	t_list *buf;
	int count;

	count = 1;
	while (count < argc)
	{
		if (!strncmp(argv[count], ";", 2) && !cur->cmd)
		{}
		else if (!strncmp(argv[count], ";", 2) && cur->cmd)
		{
			buf = (t_list *)ft_calloc(sizeof(t_list));
			if (!buf)
				return (ft_printstrendl(2, EXIT_FATAL, NULL));
			cur->type = 2;
			cur->next = buf;
			buf->previous = cur;
			cur = buf;
		}
		else if(!strncmp(argv[count], "|", 2))
		{
			buf = (t_list *)ft_calloc(sizeof(t_list));
			if (!buf)
				return (ft_printstrendl(2, EXIT_FATAL, NULL));
			cur->type = 1;
			cur->next = buf;
			buf->previous = cur;
			cur = buf;
		}
		else
		{
			if (!ft_strappend(&cur->cmd, argv[count]))
				return (ft_printstrendl(2, EXIT_FATAL, NULL));
		}
		count++;
	}
	return (1);
}

int	ft_clearlist(t_list *node)
{
	t_list *buf;

	while (node->previous)
		node = node->previous;
	while (node)
	{
		buf = node->next;
		if (node->cmd)
			free(node->cmd);
		free(node);
		node = buf;
	}
	return (0);
}

int	ft_execute(t_list *start, char **envp)
{
	int	pid;
	int	ret;

	ret = 0;
	while (start)
	{
		if (!start->cmd)
			break ;

		if (pipe(start->pipe) == -1)
			ft_printstrendl(2, EXIT_FATAL, NULL);
		
		if (!strncmp(start->cmd[0], "cd", 3))
		{
			if (ft_darrsize(start->cmd) != 2)
				ft_printstrendl(2, CD_ARGUMENTS, NULL);
			else if (chdir(start->cmd[1]) == -1)
				ft_printstrendl(2, CD_FAILED, start->cmd[1]);
		}
		else
		{
			pid = fork();
			if (pid == -1)
				ft_printstrendl(2, EXIT_FATAL, NULL);
			if (pid == 0)
			{
				if (start->type == 1)
					if (dup2(start->pipe[1], 1) == -1)
						ft_printstrendl(2, EXIT_FATAL, NULL);
				if (start->previous && start->previous->type == 1)
					if (dup2(start->previous->pipe[0], 0) == -1)
						ft_printstrendl(2, EXIT_FATAL, NULL);
				if (execve(start->cmd[0], start->cmd, envp) == -1)
					ft_printstrendl(2, EXECVE_FAIL, start->cmd[0]);
				exit(1);
			}
			else
			{
				wait(&ret);
				if (ret == -1)
					exit(-1);
				close(start->pipe[1]);
				if (start->type != 1)
					close(start->pipe[0]);
				if (start->previous && start->previous->type == 1)
					close(start->previous->pipe[0]);
				if (dup2(orignals[0], 0) == -1)
					ft_printstrendl(2, EXIT_FATAL, NULL);
				if (dup2(orignals[1], 1) == -1)
					ft_printstrendl(2, EXIT_FATAL, NULL);
			}
		}
		start = start->next;
	}
	return (1);
}

int main(int argc, char *argv[], char *envp[])
{
	t_list *start;

	start = (t_list *)ft_calloc(sizeof(t_list));
	if (!start)
		return (ft_printstrendl(2, EXIT_FATAL, NULL));
	orignals[0] = dup(0);
	orignals[1] = dup(1);
	if (!ft_parser(argc, argv, start))
		return (ft_clearlist(start));
	if (!ft_execute(start, (char **)envp))
		return (ft_clearlist(start));
	ft_clearlist(start);
	return (1);
}