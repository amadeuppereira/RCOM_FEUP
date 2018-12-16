#include "clientFTP.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void removeSubstr(char *string, char *sub)
{
	char *match;
	int len = strlen(sub);
	while ((match = strstr(string, sub)))
	{
		*match = '\0';
		strcat(string, match + len);
	}
}

int main(int argc, char **argv)
{
	char *urlPath, *ip, *user, *password, *pch, *url;

	if (argc != 2)
	{
		printf("Not enought arguments.\n");
		printf("Usage: ftp://[<user>:<password>@]<host>/<url-path>\n");
		return -1;
	}

	// remove ftp tag from full argument
	url = malloc(strlen(argv[1]) - 5);
	strncpy(url, &(argv[1][6]), strlen(argv[1]) - 5);
	//protocol = malloc()

	// parse info from url argument
	int credentials = 0; //0 - no credentials, 1 - credentials given
	char *at = strchr(url, '@');
	char *two_dots = strchr(url, ':');
	if (at != NULL && two_dots != NULL)
	{
		credentials = 1;
	}
	else if ((at != NULL && two_dots == NULL) || (at == NULL && two_dots != NULL))
	{
		printf("User or/and password wrong");
		exit(-1);
	}

	pch = strtok(url, "/:@");

	int count = 0;

	while (pch != NULL)
	{
		int size = sizeof(char) * strlen(pch);

		if (credentials == 0)
		{
			// ip
			if (count == 0)
			{
				ip = malloc(size);
				strcpy(ip, pch);
			}

			// file path
			else if (count == 1)
			{
				urlPath = malloc(size);
				strcpy(urlPath, pch);
			}

			else
			{
				urlPath = realloc(urlPath, strlen(urlPath) + size + sizeof(char));
				urlPath[strlen(urlPath)] = '/';
				strcat(urlPath, pch);
			}
		}
		else
		{
			// user
			if (count == 0)
			{
				user = malloc(size);
				strcpy(user, pch);
			}

			// password
			else if (count == 1)
			{
				password = malloc(size);
				strcpy(password, pch);
			}

			// ip
			else if (count == 2)
			{
				ip = malloc(size);
				strcpy(ip, pch);
			}

			// file path
			else if (count == 3)
			{
				urlPath = malloc(size);
				strcpy(urlPath, pch);
			}

			else
			{
				urlPath = realloc(urlPath, strlen(urlPath) + size + sizeof(char));
				urlPath[strlen(urlPath)] = '/';
				strcat(urlPath, pch);
			}
		}

		pch = strtok(NULL, "/:@");
		count++;
	}

	if (credentials == 0)
	{
		user = "anonymous";
		password = "anonymous";
	}

	if (user == NULL)
	{
		printf("User can't be null.");
		exit(-1);
	}

	if (ip == NULL)
	{
		printf("Ip can't be null.");
		exit(-1);
	}

	if (password == NULL)
	{
		printf("Password can't be null.");
		exit(-1);
	}

	printf("------------------------------------\n");
	printf("Connecting to ftp://%s ...\n", ip);
	printf("User: %s, pass: %s.\n", user, password);
	printf("File path: %s.\n", urlPath);
	printf("------------------------------------\n");

	// setup FTP connection.
	downloadFile(ip, user, password, urlPath);

	// TODO: download file
	/*if (downloadFile(urlPath))
	{
		perror("Download file error: ");
		return -1;
	}*/

	free(ip);
	if (credentials == 1)
	{
		free(user);
		free(password);
	}
	free(urlPath);
	free(url);

	// close ftp program
	closeFTP();

	return 0;
}