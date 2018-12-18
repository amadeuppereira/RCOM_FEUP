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
		printf("User or/and password wrong\n");
		exit(-1);
	}

	pch = strtok(url, "/:@");

	int count = 0;

	while (count != 4)
	{
		int size = sizeof(char) * strlen(pch);

		if (credentials == 0)
		{
			// ip
			if (count == 0)
			{
				ip = malloc(size);
				strcpy(ip, pch);
				pch = strtok(NULL, "");
			}

			// file path
			else
			{
				urlPath = malloc(size);
				strcpy(urlPath, pch);
				count = 3;
			}
		}
		else
		{
			// user
			if (count == 0)
			{
				user = malloc(size);
				strcpy(user, pch);
				pch = strtok(NULL, "/:@");
			}

			// password
			else if (count == 1)
			{
				password = malloc(size);
				strcpy(password, pch);
				pch = strtok(NULL, "/:@");
			}

			// ip
			else if (count == 2)
			{
				ip = malloc(size);
				strcpy(ip, pch);
				pch = strtok(NULL, "");
			}

			// file path
			else if (count == 3)
			{
				urlPath = malloc(size);
				strcpy(urlPath, pch);
			}
		}

		count++;
	}

	if (credentials == 0)
	{
		user = "anonymous";
		password = "anonymous";
	}

	if (user == NULL)
	{
		printf("User can't be null.\n");
		exit(-1);
	}

	if (ip == NULL)
	{
		printf("Ip can't be null.\n");
		exit(-1);
	}

	if (password == NULL)
	{
		printf("Password can't be null.\n");
		exit(-1);
	}

	if(urlPath == NULL){
		printf("URL Path can't be null.\n");
		exit(-1);
	}

	printf("------------------------------------\n");
	printf("Connecting to ftp://%s ...\n", ip);
	printf("User: %s, pass: %s.\n", user, password);
	printf("File path: %s.\n", urlPath);
	printf("------------------------------------\n");

	// Download File
	downloadFile(ip, user, password, urlPath);

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