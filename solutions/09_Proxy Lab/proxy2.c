#include <stdio.h>
#include "csapp.h"
#include "sbuf.h"
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define SBUFSIZE 16
#define NTHREADS 4
sbuf_t sbuf; //连接缓冲区
struct Uri
{
    char host[MAXLINE]; //hostname
    char port[MAXLINE]; //端口
    char path[MAXLINE]; //路径
};

void doit(int connfd);
void parse_uri(char *uri, struct Uri *uri_data);
void build_header(char *http_header, struct Uri *uri_data, rio_t *client_rio);
void *thread(void *vargp);
/* 处理SIGPIPE信号 */
void sigpipe_handler(int sig)
{
    printf("haha?");
    return;
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    char hostname[MAXLINE], port[MAXLINE];

    struct sockaddr_storage clientaddr;

    pthread_t tid;

    if (argc != 2)
    {
        fprintf(stderr, "usage :%s <port> \n", argv[0]);
        exit(1);
    }
    signal(SIGPIPE, sigpipe_handler);
    listenfd = Open_listenfd(argv[1]);

    sbuf_init(&sbuf, SBUFSIZE);
    //创建工作者线程
    for(int i = 0; i < NTHREADS; i++)
    {
        Pthread_create(&tid, NULL, thread, NULL);
    }

    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        sbuf_insert(&sbuf, connfd);
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("Accepted connection from (%s %s).\n", hostname, port);
    }
    return 0;
}
void *thread(void *vargp)
{
    Pthread_detach(pthread_self());
    while(1){
    int connfd = sbuf_remove(&sbuf);
    doit(connfd);
    //关闭客户端的连接描述符
    Close(connfd);}
}
void doit(int connfd)
{
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char server[MAXLINE];
 
    rio_t rio, server_rio;

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);

    if (strcasecmp(method, "GET"))
    {
        printf("Proxy does not implement the method");
        return;
    }

    struct Uri *uri_data = (struct Uri *)malloc(sizeof(struct Uri));
    //解析uri
    parse_uri(uri, uri_data);
    
    //设置header
    build_header(server, uri_data, &rio);

    //连接服务器
    int serverfd = Open_clientfd(uri_data->host, uri_data->port);
    if (serverfd < 0)
    {
        printf("connection failed\n");
        return;
    }

    Rio_readinitb(&server_rio, serverfd);
    Rio_writen(serverfd, server, strlen(server));

    size_t n;
    while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0)
    {
        printf("proxy received %d bytes,then send\n", (int)n);
        Rio_writen(connfd, buf, n);
    }
    Close(serverfd);
}

void build_header(char *http_header, struct Uri *uri_data, rio_t *client_rio)
{
    char *User_Agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
    char *conn_hdr = "Connection: close\r\n";
    char *prox_hdr = "Proxy-Connection: close\r\n";
    char *host_hdr_format = "Host: %s\r\n";
    char *requestlint_hdr_format = "GET %s HTTP/1.0\r\n";
    char *endof_hdr = "\r\n";

    char buf[MAXLINE], request_hdr[MAXLINE], other_hdr[MAXLINE], host_hdr[MAXLINE];
    sprintf(request_hdr, requestlint_hdr_format, uri_data->path);
    while (Rio_readlineb(client_rio, buf, MAXLINE) > 0)
    {
        if (strcmp(buf, endof_hdr) == 0)
            break; /*EOF*/

        if (!strncasecmp(buf, "Host", strlen("Host"))) /*Host:*/
        {
            strcpy(host_hdr, buf);
            continue;
        }

        if (!strncasecmp(buf, "Connection", strlen("Connection")) && !strncasecmp(buf, "Proxy-Connection", strlen("Proxy-Connection")) && !strncasecmp(buf, "User-Agent", strlen("User-Agent")))
        {
            strcat(other_hdr, buf);
        }
    }
    if (strlen(host_hdr) == 0)
    {
        sprintf(host_hdr, host_hdr_format, uri_data->host);
    }
    sprintf(http_header, "%s%s%s%s%s%s%s",
            request_hdr,
            host_hdr,
            conn_hdr,
            prox_hdr,
            User_Agent,
            other_hdr,
            endof_hdr);

    return;
}

//解析uri
void parse_uri(char *uri, struct Uri *uri_data)
{
    char *hostpose = strstr(uri, "//");
    if (hostpose == NULL)
    {
        char *pathpose = strstr(uri, "/");
        if (pathpose != NULL)
            strcpy(uri_data->path, pathpose);
        strcpy(uri_data->port, "80");
        return;
    }
    else
    {
        char *portpose = strstr(hostpose + 2, ":");
        if (portpose != NULL)
        {
            int tmp;
            sscanf(portpose + 1, "%d%s", &tmp, uri_data->path);
            sprintf(uri_data->port, "%d", tmp);
            *portpose = '\0';
        }
        else
        {
            char *pathpose = strstr(hostpose + 2, "/");
            if (pathpose != NULL)
            {
                strcpy(uri_data->path, pathpose);
                strcpy(uri_data->port, "80");
                *pathpose = '\0';
            }
        }
        strcpy(uri_data->host, hostpose + 2);
    }
    return;
}