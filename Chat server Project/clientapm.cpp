using namespace std;

#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<unistd.h>
#include<stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<signal.h>

#define BUFFER 100
#define STDIN 0
int flag=0;
int validating(char rbuff[])
{
	char* msg[3];
	
	msg[0]=strtok(rbuff," \n"); 
	if(msg[0]!=NULL)
	{	
		msg[1]=strtok(NULL," \n");
		if(msg[1]!=NULL)
		{
			msg[2]=strtok(NULL,"\n");
		}
	}

	if(strcmp(msg[0],"register")==0)
	{
		if((msg[1]!=NULL)&&(msg[2]!=NULL))
			return 1;
		else 
			return 0;

	}

	if(strcmp(msg[0],"login")==0)
	{
		flag=1;
		if((msg[1]!=NULL)&&(msg[2]!=NULL))
			return 1;
		else 
			return 0;

	}
	
        if(flag==1)
	{
	     if(strcmp(msg[0],"send")==0)
	     {
		if((msg[1]!=NULL)&&(msg[2]!=NULL))
			return 1;
		else 
			return 0;

	     }


	     else if(strcmp(msg[0],"lobbystatus")==0)
	     {
		 if((msg[1]==NULL))
			return 1;
		 else 
			return 0;
             }

	     else if(strcmp(msg[0],"logout")==0)
	     {
		if((msg[1]==NULL))
			return 1;
		else 
			return 0;
	     }
        }
	
        else
		return 0;

}



void myfunction(int myfd)
{
	char rbuff[BUFFER],wbuff[BUFFER];
	bzero(&rbuff,BUFFER);
	bzero(&wbuff,BUFFER);	
	char temp[BUFFER];
	bzero(&temp,BUFFER);	
	int i;
	
	if(!fork())	//child
	{
		while(cin.getline(wbuff,BUFFER))
        	{
			for(int j=0;j<BUFFER;j++)
			    temp[j]=wbuff[j];
			
		
		        i=validating(temp);
			if(i==0)
			    cout<<"INVALID COMMAND!!"<<endl;
			else
			{
			    write(myfd,wbuff,strlen(wbuff));
			    bzero(&wbuff,BUFFER);
			}
		}
	}
	   
	else
	{	
		while(read(myfd,rbuff,BUFFER))
		{
		      	char temp[BUFFER]={'\0'};
			for(int i=0;i<BUFFER;i++)
			{
			    temp[i]=rbuff[i];
			}
	     
			if(strcmp(rbuff,"llogout")==0)
			{
			    pid_t p=getpid();
			    kill(p,0);
			    exit(0);
		        }
		        fputs(rbuff,stdout);
			cout<<endl;
		        bzero(&rbuff,BUFFER);
		}
	   }
	
	   return ;
}

	



int main() 
{
	struct sockaddr_un servaddr;
	int myfd=socket(AF_LOCAL,SOCK_STREAM,0);

 	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family=AF_LOCAL;

	strcpy(servaddr.sun_path,"mysocket");
	int conn_status=connect(myfd,(sockaddr*) &servaddr, sizeof(servaddr));

	if(conn_status==-1)
	{
		std::cerr<<"Cannot connect";
		return -1;

	}
	myfunction(myfd);
	close(myfd);	
}

