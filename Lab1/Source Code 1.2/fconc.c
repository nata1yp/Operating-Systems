#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void doWrite(int fd, const char *buff, int len)
{
	size_t idx = 0;
	ssize_t wcnt;

	do
	{
		wcnt = write(fd, buff + idx, len - idx);
		if (wcnt == -1) /*error*/
		{
			perror("write");
			exit(1);
		}
		idx += wcnt;
	} while (idx < len);
}


void write_file(int fd, const char *infile)
{
	char buff[1024];
	ssize_t rcnt;
	int fd_in;
	fd_in = open(infile, O_RDONLY);
	if (fd_in == -1)
	{
		perror(infile);
		exit(1);
	}
	do{
		rcnt = read(fd_in, buff, sizeof(buff)-1);
		if (rcnt == -1){
			perror("read");
			exit(1);
		}
		doWrite(fd, buff, rcnt);
	}while (rcnt !=0);
	close(fd_in);
}


int main(int argc, char **argv)
{
	char *out = "fconc.out";
	int fd_out, oflags, mode;

	if (argc<3 || argc>4)
	{
		printf("Usage: ./fconc infile1 infile2 [outfile (default:fconc.out)]\n");
		return -1;
	}

	if (argc == 4)
	{
		out = argv[3];
	}

	oflags = O_CREAT | O_WRONLY | O_TRUNC;
	mode = S_IRUSR |S_IWUSR;
	fd_out = open(out, oflags, mode);
	if (fd_out == -1)
	{
		perror("open");
		exit(1);
	}

	write_file(fd_out, argv[1]);
	write_file(fd_out, argv[2]);

	close(fd_out);
	return 0;
}


