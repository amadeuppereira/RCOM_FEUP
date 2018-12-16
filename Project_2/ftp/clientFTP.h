#ifndef _CLIENT_FTP_H_
#define _CLIENT_FTP_H_

int closeFTP();

int downloadFile(char *ip, char *user, char *password, char *filePath);

#endif // _CLIENT_FTP_H_
