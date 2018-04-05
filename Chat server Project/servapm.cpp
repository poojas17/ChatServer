#include<fstream>
#include<iostream>
#include<vector>
using namespace std;
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>

#include<signal.h>
vector<int> online_fd;
vector<string> online_users;
#define BUFFER 100

int register_user(char* user, char* password)
{
	ifstream file1("register.txt");
	string line;
	int p=0,q=0,flag=0;    

	while(getline(file1,line) && flag==0)
        {
		 int i=0,j=0;
	    	 char uname[15]={'\0'};
	    
	   	 while(line[i]!=' ')
	  	 { 
			uname[j]=line[i];
	       		i++;
	        	j++;
	    	 } 
    
	    	 if(strcmp(user,uname)==0)
	    	{   
	    		flag=1;
	    	}
	}
	
	file1.close();
	
	if(flag==0)
	{
		ofstream f;
		f.open("register.txt", ios::out | ios::app);
		f<<user<<' '<<password<<' ';
		f<<'0';       // 0 is for unbanned user and 1 is for banned user
		f<<"\n";
		f.close();
		return 1;
	}
	else
		return 0;
	
}

int login(int myfd,char* user, char* password)
{
	int p=0,q=0,flag=0;
	ifstream f1;	
	string line;
	f1.open("register.txt");
	while(getline(f1,line) && flag==0)
    	{    

		int i=0,j=0;
	    	char uname[15]={'\0'};
	   
	    	while(line[i]!=' ')
	    	{   
			uname[j]=line[i];
	        	i++;
	        	j++;
	    	} 
       
	    	if(strcmp(user,uname)==0)     // means user exists in register.txt file
	    	{                            
	    		char pswd[15]={'\0'};
	    		flag=1;
	    		i=i+1;       
			j=0;     
	    		while(line[i]!=' ')
	        	{   
		    		pswd[j]=line[i];
	            		i++;
	            		j++;	      
	        	}
			i++;
			char b;
		 	b=line[i];		
		    	

			if((strcmp(pswd,password)==0) && (b=='0'))
			{
		    		online_fd.push_back(myfd);
	            		online_users.push_back(user);
		    		return 1;
			}

			else
			{
	            		return 0;
			}
	     	}
         }

	 if(flag==0)
             cout<<"\n You need to register first ! \n";
         f1.close();
	
}


void server_logout()
{
	int a=0,i;
        char msg[]="llogout";
	
	int s;
	while(!online_fd.empty())
	{
		s=online_fd.size();
		a=online_fd[s-1];
		send(a,msg,strlen(msg),0);
		cout<<"Shut down "<<a<<endl;
		online_fd.pop_back();
	}
	exit(0);

}

void broadcast(char msg[])
{	
	int i;
	for(i=0;i<online_fd.size();i++)
	{	
		
		write(online_fd[i],msg,strlen(msg));
	}	
}

void broad(int myfd,char msg[])
{	
	int i;
	
	for(i=0;i<online_fd.size();i++)
	{	
		if(online_fd[i]!=myfd)
			write(online_fd[i],msg,strlen(msg));
	}	
}


void tokenizing(char rbuff[], char* command[])
{
	command[0]=strtok(rbuff," \n");
	if(command[0]!=NULL)
	{	
		command[1]=strtok(NULL," \n");
		if(command[1]!=NULL)
			command[2]=strtok(NULL,"\n");
	}
}

void log_record( int myfd, char* msg, char* msg1)
{
	fstream f;
	f.open("log.txt", ios::out | ios::app);
	time_t t=time(0);
	struct tm *now=localtime(&t);
	if(myfd==0)
	{
		if((strcmp(msg,"broadcast")==0) || (strcmp(msg,"logout")==0))
		{
			f<<"server"<<" "<<msg<<" "<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" ";
			f<<(now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec)<<"\n";
		}
		else
		{
			f<<"server"<<" "<<msg<<" "<<msg1<<" "<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" ";
			f<<(now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec)<<"\n";
		}
	}
	else if(myfd==3)
	{
		f<<"server"<<" "<<"LOGGED IN"<<" "<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" ";
		f<<(now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec)<<"\n";
	}
	else
	{
		f<<msg1<<" "<<msg<<" "<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" ";
		f<<(now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec)<<"\n";		
	}
	f.close();
}

		
int extract_fd(char uname[],int fd)
{
	
	int i,pos=0,flag=0;
	for(i=0;i<online_users.size();i++)
	{	
	
		if(uname==online_users[i])
		{
			pos=i;	
			flag=1;
			break;
		}
	}
	if(flag==0)
	{
		return -1;
	}
	fd=online_fd[pos];

	return fd;

}


int server_ban(char* com[])
{
	string line;
	int flag=0,i;
	fstream f;
	f.open("register.txt" , ios::in | ios::out);
	
	while(getline(f,line) && flag==0)
	{
		i=0;
	        char uname[15]={'\0'};
	     
	        while(line[i]!=' ')
	        {   
		    uname[i]=line[i];
	            i++;
	        }
		   
		if(strcmp(com[1],uname)==0)     // means user which is to be banned exists in register.txt file
		{
		    	i++;    
		
		    	char ch;
		    	flag=1;
		    	while(line[i]!=' ')
		   	 	i++;
		    	i++;
 	            	ch=line[i];
		    	flag=1;
		    	int j=f.tellp();
        	    	f.seekp(j-2);
		    	if(ch=='0')            // 0 = unbanned , 1 = banned
		    	{
				f<<'1';
		          	return 1;   // user has been banned
                        }
	            	else
			  	return 0;
	
	         }
         } 
         f.close();
	
         if(flag==0)  
                cout<<"\n The user doesn't exists "; 
}


void lobbystatus(int myfd)
{
	char l[BUFFER]={'\0'};
	strcpy(l,"The online users are....");
	strcat(l,"\n");
	char const *str=online_users[0].c_str();
	strcat(l,str);
	for(int a=1;a<online_fd.size();a++)
	{	
		strcat(l,"\n");	
		char const* str1 =online_users[a].c_str();				
		strcat(l,str1);
	}
	
	send(myfd,l,strlen(l),0); 
	bzero(&l,BUFFER);
}



void broadcast_lobby()
{
	for(int i=0;i<online_users.size();i++)
	{
		lobbystatus(online_fd[i]);
	}
}



int server_unban(char* com[])
{ 
	string line;
	int flag=0,i;
	fstream f;
	f.open("register.txt" , ios::in | ios::out);
	
	while(getline(f,line) && flag==0)
	{
		i=0;
	        char uname[15]={'\0'};
	     
	        while(line[i]!=' ')
	        {   
		    	uname[i]=line[i];
	            	i++;
	        }
		   
		if(strcmp(com[1],uname)==0)     // means user which is to be unbanned exists in register.txt file
		{
			i++;    
			char ch;
			flag=1;
		   	while(line[i]!=' ')
		   		i++;
		    	i++;
 	            	ch=line[i];
		    	flag=1;
		   	int j=f.tellp();
        	    	f.seekp(j-2);
		    	if(ch=='1')            // 0 = unbanned , 1 = banned
		    	{
			  	f<<'0';
		          	return 1;   // user has been unbanned
                        }
	            	else
			  	return 0;
	
	         }
         } 
         f.close();
	
         if(flag==0)  
                cout<<"\n The user doesn't exists "; 
}

    
int main(int argc, char *argv[])
{
	fd_set master;
	fd_set read_fds;	
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;    
	int fdmax;
	int listener;
	int newfd;
	char buf[BUFFER];
    	int nbytes;
  	int yes = 1;
	int cfd=0;
    	socklen_t addrlen;
	int i, j;

 	FD_ZERO(&master);	
	FD_ZERO(&read_fds);
	
	if((listener = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)	
    	{
    		perror("Server-socket() error!");
		exit(1);
    	}
    	printf("Server-socket() is OK...\n");
	
	if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)	
    	{
    		perror("Server-setsockopt() error lol!");
   		exit(1);
    	} 
   	printf("Server-setsockopt() is OK...\n");
	unlink("mysocket");
  	memset(&serveraddr, 0, sizeof(serveraddr));
  	serveraddr.sun_family = AF_LOCAL;
  	strcpy(serveraddr.sun_path, "mysocket");
     
    	if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)	//bind
    	{
        	perror("Server-bind() error");
       		exit(1);
    	}
   	printf("Server-bind() is OK...\n");
    	char m[]="logged in";
    	char m1[]="";
    	log_record(listener,m,m1);
    	if(listen(listener, 10) == -1)
    	{
        	perror("Server-listen() error!");
         	exit(1);
    	}
    	printf("Server-listen() is OK...\n");
	FD_SET(0,&master);
    
    	FD_SET(listener, &master);	
    	fdmax = listener; 

	for(;;)
    	{
   		read_fds = master;	 

		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
    		{
       	 		perror("Server-select() error!");
        		exit(1);
    		}
    						
   		for(i = 0; i <= fdmax; i++)
    		{
        		if(FD_ISSET(i, &read_fds))
        		{ 	
				if(FD_ISSET(0,&read_fds))
				{	
					bzero(&buf,BUFFER);		
					if(read(i,buf,sizeof(buf)))
					{
						char *com[3];
						char tmp[BUFFER]={'\0'};
						for(j=0;j<BUFFER;j++)
							tmp[j]=buf[j];
						tokenizing(tmp,com);
		

						if(strcmp(com[0],"broadcast")==0)
						{ 						
							char s[BUFFER]={'\0'};
							strcpy(s,com[1]);
							strcat(s," ");
							if(com[2]!=NULL)
							{
								strcat(s,com[2]);
							}
						
							broadcast(s);
							bzero(&s,BUFFER);
							log_record(i,com[0],com[1]);
							bzero(&buf,BUFFER);
						}
					 	if(strcmp(com[0],"kick")==0)
						{
							int a=0,flag=0,pos=-1;
                                        		int s=online_users.size();
							while(a<online_users.size())
         						{    
								string str(com[1]);
	    							if(online_users[a].compare(str)==0)
             							{	
									pos=a; 
									flag=1;
									break;
                						}
            							a++;
							}	
					
							char l[BUFFER]={'\0'};
							if(flag==1)
							{
								strcpy(l,"User with username:");
					        		char const* st =online_users[pos].c_str();
					   			strcat(l,st);
								strcat(l," is kicked out of the lobby ");
								if(com[2]!=NULL)
								{
									strcat(l,"reason:");
									strcat(l,com[2]);
									cout<<l<<endl;
								}
								cout<<"\n"<<com[2]<<endl;
								int cfd=0;
								cfd=extract_fd(com[1],cfd);
												
								int f=online_fd[a];
								online_fd.erase(online_fd.begin()+a);
							
								online_users.erase(online_users.begin()+a);
								broad(f,l);

								char temp[BUFFER]="llogout";
								if(send(cfd,temp,strlen(temp),0) <=0)
									perror("send error");
								FD_CLR(cfd,&master);
								log_record(i,com[0],com[1]);
								broadcast_lobby();
								bzero(&buf,BUFFER);
							
							}
							else
							{	
								cout<<"\nUser is not logged in";
							}
						
                                                }
					 	if(strcmp(com[0],"ban")==0)
						{ 
							int k=server_ban(com);
							if(k==1)
							{
								char msg[BUFFER];
							
								strcpy(msg,"The user ");
								strcat(msg,com[1]);
								strcat(msg," has been banned..");
								if(com[2]!=NULL)
								{	
									strcat(msg," reason is ");
									strcat(msg,com[2]);
								}
							
								int a=0,pos=0;
								int cfd=0,flag=0;
								cfd=extract_fd(com[1],cfd);
								while(a<online_fd.size())
								{
									if(cfd==online_fd[a])
             								{
										pos=a; 
										flag=1;
                								break;
             								}
									a++;
								}
						
								int f=online_fd[pos];  
								online_fd.erase(online_fd.begin()+pos);
							
								online_users.erase(online_users.begin()+pos);
								char temp[BUFFER]="llogout";
								if(send(cfd,temp,strlen(temp),0) <=0)
									perror("send error");
								broadcast(msg);
								log_record(i,com[0],com[1]);
								broadcast_lobby();
								bzero(&buf,BUFFER);
							}
                                       
						}
						if(strcmp(com[0],"unban")==0)
						{
							int k=server_unban(com);
							if(k==1)
							{
								char msg[BUFFER];
								strcpy(msg,"The user ");
								strcat(msg,com[1]);
								strcat(msg," has been unbanned....");
							
								broadcast(msg);
								log_record(i,com[0],com[1]);
								bzero(&buf,BUFFER);
								bzero(&msg,BUFFER);
							} 
						}
						if(strcmp(com[0],"send")==0)
						{	 
							cfd=extract_fd(com[1],cfd);    
				   			char str[BUFFER]={'\0'};
							strcpy(str,"SERVER : ");
							strcat(str,com[2]);
							if(send(cfd,str,strlen(str),0) ==-1)
								perror("send() error !");
					
							bzero(&buf,BUFFER);
							bzero(&str,BUFFER);
							log_record(i,com[0],com[1]);

						}
 						if(strcmp(com[0],"logout")==0)
						{
							log_record(i,com[0],com[1]); 
							server_logout();
							bzero(&buf,BUFFER);
						
						}
					
					}
				
				}
				else if(i == listener)
				{
					addrlen = sizeof(clientaddr);

					if((newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
					{
						perror("Server-accept() error !");
					}
					else
					{

						printf("Server-accept() is OK...\n");
						FD_SET(newfd, &master); 
						if(newfd > fdmax)
						{ 
							fdmax = newfd;	
						}
						printf(" New connection on socket %d\n",newfd);
					}
			
				}
				else
				{
				        bzero(&buf,BUFFER);
					if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
					{
						if(nbytes == 0)
							printf("socket %d hung up\n", i);

						else
							perror("recv() error ");
						close(i);

						FD_CLR(i, &master);
					}
					else 
					{
						char *command[3];
				
						char temp[BUFFER]={'\0'};
						for(j=0;j<BUFFER;j++)
							temp[j]=buf[j];
						tokenizing(temp,command);

				    		if(strcmp(command[0],"register")==0)
						{
							int k;
							int fd;
							k=register_user(command[1],command[2]);
					
							if(k==1)
							{
							char m[BUFFER]="user registered successfully";
							send(i,m,strlen(m),0);
							bzero(&m,BUFFER);
							}
							if(k==0)
							{
								char m[BUFFER]="user already exists!";
								send(i,m,strlen(m),0);   
								bzero(&m,BUFFER);
				
							}
							log_record(i,command[0],command[1]);
						}
						if(strcmp(command[0],"login")==0)
						{
 							int k,o=1;
					
							for(int a=0;a<online_fd.size();a++)
							{
								if(i==online_fd[a])    // fd change
								{
									write(i,"User already logged in",BUFFER);
									o=0;
									break;
							
								}
							}
							k=login(i,command[1],command[2]);
							if((k==1)&&(o==1))
							{
								char m[BUFFER]="Login successful";
						
								send(i,m,strlen(m),0);
								bzero(&m,BUFFER);
								char l[BUFFER]={'\0'};
								strcpy(l,"New user logged in with username:");
						
								strcat(l,command[1]);
								
								broad(i,l);
								bzero(&l,BUFFER);
							}
							if(k==0)
							{
								char m[15]="Invalid login";
								send(i,m,strlen(m),0); 
								bzero(&m,BUFFER);
							}
						
							log_record(i,command[0],command[1]);
							broadcast_lobby();
							bzero(&buf,BUFFER);
						}
				
						if(strcmp(command[0],"logout")==0)
						{
						
							int a=0,flag=0;
							while(a<online_fd.size())
							{	    
								if(online_fd[a]==i)
								{
									flag=1;
									break;
								}
								a++;
							}	
						
							char l[BUFFER]={'\0'};
		
							if(flag==1)
							{
								strcpy(l,"User with username:");
					        		char const* st =online_users[a].c_str();
								strcat(l,st);
								strcat(l," logged out");
								
								broad(i,l);
								online_fd.erase(online_fd.begin()+a);
							
								online_users.erase(online_users.begin()+a);
								
								char temp[BUFFER]="llogout";
								send(i,temp,strlen(temp),0);
								FD_CLR(i,&master);
                        			      		 broadcast_lobby();   
							
								bzero(&l,BUFFER);
						
								log_record(i,command[0],command[1]);
								bzero(&buf,BUFFER);
							}
							else
							{	
								strcpy(l,"\nUser is not logged in");
								broad(i,l);
								bzero(&l,BUFFER);
							}
						
						
						
						}
						if(strcmp(command[0],"lobbystatus")==0)
						{	
							lobbystatus(i);
							bzero(&buf,BUFFER);
						}
								
						if(strcmp(command[0],"send")==0)
              					{	
              				 		cfd=extract_fd(command[1],cfd);    
				   		
							if(cfd!=-1)
							{
								cout<<"\n sending : ";
								cout<<buf<<"\n";
								int f=0;
								for(int j=0;j<online_fd.size();j++)
								{
									if(i==online_fd[j])
									{
										 f=j;
										break;
									}
								}
								char const *ch=online_users[f].c_str();
								char str[BUFFER]={'\0'};
								
								strcpy(str,ch);
								strcat(str," : ");
								strcat(str,command[2]);
								if(send(cfd,str,strlen(str),0) ==-1)
									perror("send() error !");
								cout<<"\n";
								bzero(&buf,BUFFER);
								bzero(&str,BUFFER);
							}
							else 
							{
								bzero(&buf,BUFFER);
								char m[]="User doesn't exist";
								send(i,m,strlen(m),0);
							}	
						}
						bzero(&buf,BUFFER);
				
					}
					
					bzero(&buf,BUFFER);
			
				}
			}
		bzero(&buf,BUFFER);
    	}
    }	

    return 0;

 }

