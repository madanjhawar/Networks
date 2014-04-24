#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <regex.h>
char commands[1000][1000];
int commands_count=0;
struct stat buf;
pid_t pid;
void becomes_client(int x);
void becomes_server(int x);
void hashin(char a[],int *listenfd);
void hashin2(char a[],int *sockfd);
void Filehashin(char a[],int *sockfd);
void filedownload2(char a[],int *sockfd);
void filedwnld(char a[],int *sockfd);
void fileupload(char a[],int *sockfd);
void fileupload2(char a[],int *sockfd);
void IndexGet2(char buf[],int *socketfd);
void IndexGet(char buf[],int *socketfd);
void sig_handler(int signo)
  {
  int nid,status,k;
  if (signo == SIGUSR1);
  else if (signo == SIGKILL);
  else if (signo == SIGQUIT);
  else if (signo == SIGSTOP);			//SIGNAL HANDLING*
  else if (signo == SIGINT);
//else if(signo == SIGTSTP)
}
struct files
{
	char name[100];
	char size[100];
	char date[100];
	char type[10];
};
int compare(struct tm t1,struct tm t2)
{
	if(t1.tm_year>t2.tm_year)
		return 1;
	else if(t2.tm_year>t1.tm_year)
		return 0;
	else
	{
		if(t1.tm_mon>t2.tm_mon)
			return 1;
		else if(t2.tm_mon>t1.tm_mon)
			return 0;
		else
		{
			if(t1.tm_mday>t2.tm_mday)
				return 1;
			else if(t2.tm_mday>t1.tm_mday)
				return 0;
			else
			{
				if(t1.tm_hour>t2.tm_hour)
					return 1;
				else if(t2.tm_hour>t1.tm_hour)
					return 0;
				else
				{
					if(t1.tm_min>t2.tm_min)
						return 1;
					else if(t2.tm_min>t1.tm_min)
						return 0;
					else
					{
						if(t1.tm_sec>t2.tm_sec)
							return 1;
						else if(t2.tm_sec>t1.tm_sec)
							return 0;
						else
							return 0;
					}
				}
			}
		}

	}
}

int main()
{
	signal(SIGUSR1, sig_handler);
	  signal(SIGKILL, sig_handler);
	  signal(SIGSTOP, sig_handler);
	  signal(SIGINT, sig_handler);		
	  signal(SIGQUIT, sig_handler);	

	int peer_sockno,host_sockno;
	printf("Enter the peer socket no.: ");
	scanf("%d",&peer_sockno);
	printf("Enter the host socket no.: ");
	scanf("%d",&host_sockno);

	pid=fork();
	if(pid<0)
	{
		printf("Error while forking\n");
		return 0;
	}
	if(pid==0)
		becomes_client(peer_sockno);
	else if(pid)
		becomes_server(host_sockno);
	return 0;
}
void IndexGet(char buf[],int *socketfd)
{
	int flag=0;
	char t1[100],t2[100];
	if(strncmp(buf,"IndexGet ShortList",18)==0)
		flag=1;
	else if(strncmp(buf,"IndexGet LongList",17)==0)
		flag=0;
	else if(strncmp(buf,"IndexGet RegEx",14)==0)
		flag=2;
	char priBuff[1025];
	struct tm time1,time2,test;
	FILE *fd,*fx;
	bzero(priBuff,1025);
	struct files arr[100];
	struct stat st;
	int size;
	int i,it=0,f=0;
	char n[100];
	char *pch;
	bzero(n,100);
	DIR *dp;
	struct dirent *ep;     
	dp=opendir ("./");
	if (dp != NULL)
	{
		while(ep = readdir (dp))
		{
			strcpy(n,ep->d_name);
			if(n[0]!='.')
			{
				strcpy(arr[it].name,n);
				it++;
		//		 puts(arr[it-1].name);
			}
		}
		(void)closedir(dp);
	}
	else
		perror ("Couldn't open the directory");
	for(i=0;i<it;i++)
	{
		char tok[100];
		stat(arr[i].name, &st);
		size = st.st_size;
		strcpy(arr[i].date,ctime(&st.st_mtime));
		sprintf(arr[i].size,"%d",size);
		strcpy(tok,arr[i].name);
		pch=strtok(tok,".");
		if(pch!=NULL)
			pch=strtok(NULL,".");
		if(pch!=NULL)
		{
			strcpy(arr[i].type,pch);
		}
	}
	system("touch out");
	fx=fopen("out","w");
	if(fx<0)
		perror("[ERROR opening file descriptor]");
	if(flag==0)
	{
		for(i=0;i<it;i++)
		{
			fprintf(fx,"%s\t\t%s\t\t%s\t\t%s",arr[i].name,arr[i].type,arr[i].size,arr[i].date);
		}
	}
	else if(flag==1)
	{
		pch = strtok (buf,"\t");
		//printf("%s\n",pch );
		pch = strtok (NULL,"\t");
		strcpy(t1,pch);
	//	printf("%s\n",pch );
		pch = strtok (NULL,"\t");
		strcpy(t2,pch);
	//	printf("%s\n",pch );
		if(strptime(t1, "%a %b %d %H:%M:%S %Y",&time1) == NULL)
          printf("\nstrptime failed\n");
      	if(strptime(t2, "%a %b %d %H:%M:%S %Y",&time2) == NULL)
          printf("\nstrptime failed\n");
		for(i=0;i<it;i++)
		{
			if(strptime(arr[i].date, "%a %b %d %H:%M:%S %Y",&test) == NULL)
          		printf("\nstrptime failed\n");
          	if((compare(test,time1)==1) && (compare(time2,test)==1))
				fprintf(fx,"%s\t\t%s\t\t%s\t\t%s",arr[i].name,arr[i].type,arr[i].size,arr[i].date);
		}
	}
	else if(flag==2)
	{
	//	printf("I M HERE\n");
		regex_t regex;int reti;char msgbuf[100],reg[100];
		pch = strtok (buf," ");
		pch = strtok (NULL," ");
		pch = strtok (NULL," ");
		strcpy(reg,pch);
	
		reti=regcomp(&regex,reg,0);
		if( reti ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }
		for(i=0;i<it;i++)
		{
		//	printf("!!%s!!\n",arr[i].name);
			reti=regexec(&regex,arr[i].name,0,NULL,0);
			if(!reti)
			{
		//		puts("Match");
				fprintf(fx,"%s\t\t%s\t\t%s\t\t%s",arr[i].name,arr[i].type,arr[i].size,arr[i].date);
			}
		//	else if(reti==REG_NOMATCH)
		//		continue;
		}
			regfree(&regex);
	}
	fclose(fx);
	fd=fopen("out","r");
	//	printf("OutPUT READY\n");
	if(fd<0)
		perror("[ERROR opening file descriptor]");
	while((f=fread(priBuff,sizeof(char),1025,fd))>0)
	{
		//printf("111111\n");
		if(write(*socketfd,priBuff,1025)<0)
		{
			printf("\nERROR: Writing to socket\n");
			break;
		}
		bzero(priBuff,1025);
	}
	bzero(priBuff,1025);
	strcpy(priBuff,"******FILE LIST*******");
	if(write(*socketfd,priBuff,1025)<0)
	{
		printf("\nERROR: Writing to socket\n");
		exit(1);
	}
	//system("rm -rf out");
}
void IndexGet2(char buf[],int *socketfd)
{
	int n,f,flag=0;
	if(strncmp(buf,"IndexGet ShortList",18)==0)
		flag=1;
	else if(strncmp(buf,"IndexGet LongList",17)==0)
		flag=0;
	else if(strncmp(buf,"IndexGet RegEx",14)==0)
		flag=2;
	n=write(*socketfd,buf,1025);
	if(n<0)
	{
		printf("\nERROR: Writing to socket\n");
		exit(1);
	}
	char readBuff[1025];
	bzero(readBuff,1025);
	//printf("\nRecieved List: %s\n",readBuff);
	sleep(1);
	if(flag==0||flag==1)
	{
		printf("File\t\tType\t\tSize\t\tTimeStamp\n");
		while((f=read(*socketfd,readBuff,1025))>0)
		{
			//	printf("LISTing\n");
			if(strcmp(readBuff,"******FILE LIST*******")==0)
				break;
			else
				printf("%s",readBuff );
		}
	}
	else if(flag==2)
	{
		printf("File\t\tType\t\tSize\n");
		while((f=read(*socketfd,readBuff,1025))>0)
		{
			//	printf("LISTing\n");
			if(strcmp(readBuff,"******FILE LIST*******")==0)
				break;
			else
				printf("%s",readBuff );
		}
	}
	//n=write(*socketfd,buf,1025)
}
void becomes_client(int pno)
{
	int sockfd,n;
	struct sockaddr_in serv_addr;
	char readBuff[1025];
	sockfd=socket(AF_INET,SOCK_STREAM,0);

	if(sockfd<0)
		printf("ERROR in sockfd\n");
	else
		printf("{Client} created\n");

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(pno);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	while(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0);

	while(1)
	{
		bzero(readBuff,1025);
		printf("Enter the message : ");
		gets(readBuff);
		printf("\n");
		if(strcmp(readBuff,"quit")==0)
		{
			strcpy(commands[commands_count],readBuff);commands_count++;
			n=write(sockfd,readBuff,1025);
			if(n<0)
				printf("ERROR writing to SOCKET\n");
			printf("{Client} Ended the Connection\n" );
			kill(pid,SIGTERM);
			break;
		}
		else if(strncmp(readBuff,"IndexGet",8)==0)
		{
			strcpy(commands[commands_count],readBuff);commands_count++;
			IndexGet2(readBuff,&sockfd);
		}
		else if(strncmp(readBuff,"FileHash",8)==0)
  		{
			strcpy(commands[commands_count],readBuff);commands_count++;
  			hashin2(readBuff,&sockfd);
  		}	
		else
		{
			if(strncmp(readBuff,"FileDownload",12)==0)
	  		{
			strcpy(commands[commands_count],readBuff);commands_count++;
	  			filedownload2(readBuff,&sockfd);
	  		}
	  		else
	  		{
	  			if(strncmp(readBuff,"FileUpload",10)==0)
		  		{
			strcpy(commands[commands_count],readBuff);commands_count++;
		  			fileupload2(readBuff,&sockfd);
		  		}
		  		else
		  		{
					n = write(sockfd,readBuff, 1025);
					if (n < 0)
	       				 printf("ERROR writing to socket\n");
	       		}
	       	}
       	}
	}

	close(sockfd);
	return;
}
void becomes_server(int pno)
{
	int listenfd=0,connfd=0,n;
	struct sockaddr_in serv_addr;
	char sendBuff[1025],priBuff[1025];
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	
	if(listenfd<0)
		printf("ERROR MAKIN THE SOCKET\n");
	else
		printf("[SERVER] SOCKET INITIALISED\n");

	memset(&serv_addr, '0', sizeof(serv_addr));
 	memset(sendBuff, '0', sizeof(sendBuff));
 	serv_addr.sin_family = AF_INET;    
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serv_addr.sin_port = htons(pno);
	if(bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
		perror("ERROR binding the socket");
	else
		printf("[Server] Socket Binded Successfully\n");
	if(listen(listenfd, 10) == -1)
	{
      printf("Failed to listen\n");
  	}  
  	printf("[Server] Waiting for Client to connect...\n" );
	while((connfd=accept(listenfd, (struct sockaddr*)NULL ,NULL))<0);
	printf("[CONNECTED]\n");  
 	while(1)
    {
    	bzero(priBuff,1025);
    	bzero(sendBuff,1025);
  		n=read(connfd,sendBuff,1025);
  		if(n<0)
  			printf("ERROR  writing\n");
  		sendBuff[n]='\0';
  		strcpy(priBuff,sendBuff);
  		if(strcmp(sendBuff,"quit")==0)
		{
			printf("\nFinal message: %s\n",sendBuff);
			kill(pid,SIGTERM);
			break;
		}
		else if(strncmp(priBuff,"IndexGet",8)==0)
		{	
			//	printf("Command IG\n");
			IndexGet(priBuff,&connfd);
		}
  		else if(strncmp(priBuff,"FileHash",8)==0)
  		{
  			Filehashin(priBuff,&connfd);
  		}
  		else
  		{
  			if(strncmp(priBuff,"FileDownload",12)==0)
	  		{
	  			filedwnld(priBuff,&connfd);
	  		}
	  		else
	  		{
	  			if(strncmp(priBuff,"FileUpload",10)==0)
		  		{
		  			fileupload(priBuff,&connfd);
		  		}
		  		else
				printf("\nMessage from peer: %s\n",priBuff);
	  		}
  		}
  		while(waitpid(-1, NULL, WNOHANG) > 0);
    }
    close(connfd);
		printf("\n Connection closed by peer\n");
	close(listenfd);
	exit(0);
    return;

}

void Filehashin(char a[],int *sockfd)
{
	char write_buffer[1024];
	bzero(write_buffer,1024);

	char *token=strtok(a," ");
	token=strtok(NULL," ");
	if(strcmp(token,"Verify")==0)
	{
		token=strtok(NULL," ");
		hashin(token,sockfd);

		bzero(write_buffer,1024);
		strcpy(write_buffer,"END");
		if(write(*sockfd, write_buffer, 1024) < 0)
		{
			perror("\nERROR: Writing to socket");
			exit(1);
		}
	}
	else
	{
		
			char *b=NULL;
			size_t len=0;
			token=strtok(NULL," ");
			system("ls -l |grep -v ^d | tr -s ' '| cut -d ' ' -f9 | tail -n +2 > result");
			FILE *fp=fopen("result","r");
			if(fp==NULL)
			{
				perror("\nERROR: Could not open file");
				exit(1);
			}
			while((getline(&b,&len,fp))!=-1)
			{
				int l=strlen(b);
				b[l-1]='\0';
				hashin(b,sockfd);	
			}
			bzero(write_buffer,1024);
			strcpy(write_buffer,"END");
			if(write(*sockfd, write_buffer, 1024) < 0)
			{
				perror("\nERROR: Writing to socket");
				exit(1);
			}
		
		//system("rm result");
	}
}

void hashin(char a[],int *listenfd)
{
	int l,i;
	char str[1000];
	char write_buffer[2000];
	FILE *jun=fopen(a,"r");
	if(jun==NULL)
	{
		if(strcmp(a,"result")==0 || strcmp(a,"result1")==0)
		{
			return;

		}
		else
		{
			perror("No such file exists");
			return;
		}
	}
	sprintf(str,"md5sum %s > result",a);
	system("touch result");
	char another[1000];
	sprintf(another,"stat %s | grep Modify | cut -d ' ' -f3 > result1",a);
	system(str);
	system(another);
	FILE *f=fopen("result","r");
	if(f==NULL)
	{
		perror("\nERROR: Could not open file");
		exit(1);	
	}
	FILE *f1=fopen("result1","r");
	if(f1==NULL)
	{
		perror("\nERROR: Could not open file");
		_exit(1);	
	}
	char u[1000],v[1000],x[1000];
	char c;i=0;
	while(1)
	{
		c = fgetc(f);
	    if( feof(f) )
	    {
	        break ;
	    }
	    u[i++]=c;
	}
	u[i]='\0';
	i=0;
	while(1)
	{
		c = fgetc(f1);
	    if( feof(f1) )
	    {
	        break ;
	    }
	    v[i++]=c;
	}
	v[i]='\0';
	system("rm result");
	system("rm result1");
	i=0;
	while(u[i]!=' ')
	{
		x[i++]=u[i];
	}
	x[i]='\0';
	i=strlen(v);
	v[i-1]=v[i];
	sprintf(a,"%s\t%s\t%s",a,v,x);
	strcpy(write_buffer,a);
	i=write(*listenfd, write_buffer, 1024);
	if(i<0)
	{
		perror("Error while writing");
		_exit(1);
	}
}

void hashin2(char write_buffer[],int *sockfd)
{
	int n;
	char read_buffer[1024];
	n=write(*sockfd,write_buffer,1024);
	if (n < 0)
	{
		perror("ERROR writing to socket");
		_exit(1);
	}

	bzero(read_buffer, 1024);
	printf("File\t\tLatest-Timestamp\t\tMd5hash\n");
	while((read(*sockfd, read_buffer,1024)) > 0)
	{
		if(strcmp(read_buffer,"END")==0)
		{
			break;
		}
		else
			puts(read_buffer);
		bzero(read_buffer, 1024);
	}
	
}

void filedwnld(char a[],int *sockfd)
{
	
	char write_buffer[1024];
	bzero(write_buffer,1024);
	char c[500];
	char *token=strtok(a," ");
	token=strtok(NULL," ");

	int n,i;

	FILE *fp=fopen(token,"r");
	 
	if(fp==NULL)
	{
		printf("Error While opening the File\n");
		strcpy(write_buffer,"NoFile");
		n=write(*sockfd, write_buffer, 1024);
		return;
	} 
//	puts(a);
	sprintf(c,"stat %s | grep Modify | cut -d ' ' -f3 > result",token);
	system(c);
	char v[200];
	FILE *f1=fopen("result","r");
	i=0;
	char cc;
	while(1)
	{
		cc = fgetc(f1);
	    if( feof(f1) )
	    {
	        break ;
	    }
	    v[i++]=cc;
	}
	v[i]='\0';
	fclose(f1);
	sprintf(write_buffer,"TimeStamp: %s",v);
	puts(write_buffer);
	if(n=write(*sockfd,write_buffer,1024)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(write_buffer,1024);
	system("rm -rf result");

	sprintf(c,"stat %s | grep Modify | cut -d ' ' -f2 > result",token);
	system(c);
	char u[200];
	FILE *f3=fopen("result","r");
	i=0;
	while(1)
	{
		cc = fgetc(f3);
	    if( feof(f3) )
	    {
	        break ;
	    }
	    v[i++]=cc;
	}
	v[i]='\0';
	fclose(f3);
	system("rm -rf result");
	sprintf(write_buffer,"Size: %s",v);
	puts(write_buffer);
	if(n=write(*sockfd,write_buffer,1024)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(write_buffer,1024);
	sprintf(write_buffer,"Name: %s",token);
	puts(write_buffer);
	if(n=write(*sockfd,write_buffer,1024)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(write_buffer,1024);
	sprintf(v,"md5sum %s > result",token);
	system(v);
	f3=fopen("result","r");
	i=0;
	while(1)
	{
		cc = fgetc(f3);
	    if( feof(f3) )
	    {
	        break ;
	    }
	    v[i++]=cc;
	}
	v[i]='\0';
	system("rm -rf result");
	i=0;
	while(v[i]!=' ')
	{
		u[i++]=v[i];
	}
	u[i]='\0';
	sprintf(write_buffer,"md5sum hash: %s",u);
	puts(write_buffer);
	if(n=write(*sockfd,write_buffer,1024)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(write_buffer,1024);
	
	char b[1000];
	strcpy(b,token);
	FILE *fd;
	fd=fopen(b,"rb");
	char msg[1025];
	bzero(msg,1025);
	
	while((i=fread(msg,sizeof(char),1025,fd))>0)
	{
		if(write(*sockfd,msg,i)<0)
		{
			printf("ERROR: Writing to socket\n");
			break;
		}
		bzero(msg,1025);
	}
	bzero(msg,1025);
	strcpy(msg,"FILE LIST");
	sleep(1);
	if(write(*sockfd,msg,9)<0)
	{
		printf("\nERROR: Writing to socket\n");
		exit(1);
	}
	printf("---------SENT------------\n");
}

void filedownload2(char a[],int *sockfd)
{
	
	char read_buffer[1024];
	int i,n;
	n = write(*sockfd,a,1024);
    if (n < 0)
        perror("ERROR writing to socket");

    char *hhash,*timestamp,*size,*name;
	printf("[Client] File recieving from server\n");

	char *token;
	token=strtok(a," ");
	token=strtok(NULL," ");
	
	bzero(read_buffer,1024); 

	bzero(read_buffer,1024);
	if((n= read(*sockfd, read_buffer,1024)) > 0)
	{
			if(strcmp(read_buffer,"NoFile")==0)
			{
				printf("Error: Cannot read file\n");
				return;
			}

			if(strncmp(read_buffer,"TimeStamp:",10)==0)
			{
				timestamp=strtok(read_buffer," ");
				timestamp=strtok(NULL," ");
			}

	}
	else
		perror("Error in reading from socket");

	timestamp[strlen(timestamp)-1]='\0';
	
	printf("Timestamp	%s\n",timestamp);
	
	bzero(read_buffer,1024);
	if(n= read(*sockfd, read_buffer,1024) > 0)
	{
			if(strncmp(read_buffer,"Size:",5)==0)
			{
				size=strtok(read_buffer," ");
				size=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	size[strlen(size)-1]='\0';
	printf("Size		%s\n",size);
	bzero(read_buffer,1024);
	
	if(n= read(*sockfd, read_buffer,1024) > 0)
	{
			if(strncmp(read_buffer,"Name:",5)==0)
			{
				name=strtok(read_buffer," ");
				name=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	char name2[100];
	strcpy(name2,name);
	printf("Name		%s\n",name2);
	bzero(read_buffer,1024);
	

	if(n= read(*sockfd, read_buffer,1024) > 0)
	{
			if(strncmp(read_buffer,"md5sum hash:",12)==0)
			{
				hhash=strtok(read_buffer," ");
				hhash=strtok(NULL," ");
				hhash=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	printf("Hash		%s\n",hhash);
	

	
	bzero(read_buffer,1024); 

	strcat(name,"DownloadedFile");
	char readBuff[1025];
	bzero(readBuff,1025);

	FILE *fd;
	fd=fopen(name,"wb");
	if(fd==NULL)
	{
		perror("Error while opening file");
		_exit(1);
	}
	int f=0,f1=0;
	while((f=read(*sockfd,readBuff,1025))>0)
	{
		if(strcmp(readBuff,"FILE LIST")==0)
			break;
		else
			fwrite(readBuff,sizeof(char),f,fd);
		bzero(readBuff,1025);
	}
	printf("-----------Recieved-------------\n");
	fclose(fd);
	return;
}

void fileupload2(char a[],int *sockfd)
{
	
	char read_buffer[1024];
	int i,n;
	n = write(*sockfd,a,1024);
    if (n < 0)
    {
        perror("ERROR writing to socket");
        _exit(1);
    }

    char perm[1000];
    n=read(*sockfd,perm,1024);
    if (n < 0)
    {
        perror("ERROR reading from socket");
    }

    if(strcmp(perm,"Yes")==0)
    {
	    char *hhash,*timestamp,*size,*name;
		printf("[Client] File Uploading\n");

		char write_buffer[1024];
		bzero(write_buffer,1024);
		char c[500];
		char *token=strtok(a," ");
		token=strtok(NULL," ");
	
		FILE *fx=fopen(token,"r");
		 
		if(fx==NULL)
		{
		//	printf("Here\n");
			strcpy(write_buffer,"FileError");
			n=write(*sockfd, write_buffer, 1024);
			return;
		} 
	
		sprintf(c,"stat %s | grep Modify | cut -d ' ' -f3 > result",token);
		system(c);
		char v[200];
		FILE *f1=fopen("result","r");
		if(f1==NULL)
		{
			perror("Error while opening file");
			_exit(1);
		}
		i=0;
		char cc;
		while(1)
		{
			cc = fgetc(f1);
		    if( feof(f1) )
		    {
		        break ;
		    }
		    v[i++]=cc;
		}
		v[i]='\0';
		fclose(f1);
		sprintf(write_buffer,"TimeStamp: %s",v);
	
		if(n=write(*sockfd,write_buffer,1024)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(write_buffer,1024);
		system("rm -rf result");

		sprintf(c,"stat %s | grep Modify | cut -d ' ' -f2 > result",token);
		system(c);
		char u[200];
		FILE *f3=fopen("result","r");
		i=0;
		while(1)
		{
			cc = fgetc(f3);
		    if( feof(f3) )
		    {
		        break ;
		    }
		    v[i++]=cc;
		}
		v[i]='\0';
		fclose(f3);
		system("rm -rf result");
		sprintf(write_buffer,"Size: %s",v);
	
		if(n=write(*sockfd,write_buffer,1024)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(write_buffer,1024);
		sprintf(write_buffer,"Name: %s",token);
	
		if(n=write(*sockfd,write_buffer,1024)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(write_buffer,1024);
		sprintf(v,"md5sum %s > result",token);
		system(v);
		f3=fopen("result","r");
		i=0;
		while(1)
		{
			cc = fgetc(f3);
		    if( feof(f3) )
		    {
		        break ;
		    }
		    v[i++]=cc;
		}
		v[i]='\0';
		system("rm -rf result");
		i=0;
		while(v[i]!=' ')
		{
			u[i++]=v[i];
		}
		u[i]='\0';
		sprintf(write_buffer,"md5sum hash: %s",u);
	//	puts(write_buffer);
		if(n=write(*sockfd,write_buffer,1024)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(write_buffer,1024);
		
	//	printf("-----------------------------------------------------------------------------\n");

		char b[1000];
		strcpy(b,token);
		FILE *fd;
		fd=fopen(b,"rb");
		char msg[1025];
		bzero(msg,1025);
		
		while((i=fread(msg,sizeof(char),1025,fd))>0)
		{
			if(write(*sockfd,msg,i)<0)
			{
				printf("ERROR: Writing to socket\n");
				break;
			}
			bzero(msg,1025);
		}
		bzero(msg,1025);
		strcpy(msg,"FILE LIST");
		sleep(1);
		if(write(*sockfd,msg,9)<0)
		{
			printf("\nERROR: Writing to socket\n");
			exit(1);
		}
		printf("SENT\n");
	}
	else
		printf("Permission Denied");
}

void fileupload(char a[],int *sockfd)
{
	int i,n;
	char req[1000];
	sprintf(req,"Request from client: %s",a);
	puts(req);
	printf("Enter your response: ");
	scanf("%s",req);
	n=write(*sockfd,req,1024);
	if(n<0)
	{
		perror("Error while writing to socket");
	}
	if(strcmp(req,"Yes")==0)
	{
		char read_buffer[1024];
		
		
	    char *hhash,*timestamp,*size,*name;
		printf("[Server] File recieving from Client\n");

		char *token;
		token=strtok(a," ");
		token=strtok(NULL," ");
		
		bzero(read_buffer,1024); 
	
		bzero(read_buffer,1024);
		if((n= read(*sockfd, read_buffer,1024)) > 0)
		{
				if(strcmp(read_buffer,"FileError")==0)
				{
					printf("Error in file uploading\n");
					return;
				}
				if(strncmp(read_buffer,"TimeStamp:",10)==0)
				{
					timestamp=strtok(read_buffer," ");
					timestamp=strtok(NULL," ");
				}

		}
		else
			perror("Error in reading from socket");
		timestamp[strlen(timestamp)-1]='\0';
		
		printf("Timestamp	%s\n",timestamp);
		
		bzero(read_buffer,1024);
		if(n= read(*sockfd, read_buffer,1024) > 0)
		{
				if(strncmp(read_buffer,"Size:",5)==0)
				{
					size=strtok(read_buffer," ");
					size=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		size[strlen(size)-1]='\0';
		printf("Size		%s\n",size);
		bzero(read_buffer,1024);
		
		if(n= read(*sockfd, read_buffer,1024) > 0)
		{
				if(strncmp(read_buffer,"Name:",5)==0)
				{
					name=strtok(read_buffer," ");
					name=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		char name2[100];
		strcpy(name2,name);
		printf("Name		%s\n",name2);
		bzero(read_buffer,1024);
		

		if(n= read(*sockfd, read_buffer,1024) > 0)
		{
				if(strncmp(read_buffer,"md5sum hash:",12)==0)
				{
					hhash=strtok(read_buffer," ");
					hhash=strtok(NULL," ");
					hhash=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		printf("Hash		%s\n",hhash);
		

//		printf("----------------------------------------------------------------------------------------\n");

		bzero(read_buffer,1024); 

		strcat(name,"UploadedFile");
		char readBuff[1025];
		bzero(readBuff,1025);

		FILE *fd;
		fd=fopen(name,"wb");
		if(fd==NULL)
		{
			perror("Could not open file");
			_exit(1);
		}
		int f=0,f1=0;
		while((f=read(*sockfd,readBuff,1025))>0)
		{
			if(strcmp(readBuff,"FILE LIST")==0)
				break;
			else
				fwrite(readBuff,sizeof(char),f,fd);
			bzero(readBuff,1025);
		}
		printf("Recieved\n");
		fclose(fd);
		return;
	}
}