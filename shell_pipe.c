#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <sys/resource.h>
#define FGETS_BUFSIZE 5
typedef struct node *list;
typedef struct node {
	list next;
	char *name;
	char **arg;
	int pid;
//	int argcount;
} node;
typedef struct zmbnode *zmblst;
typedef struct zmbnode{
	char *str;
	int pid;
	zmblst next;
}zmbnode;

int bg = 0;
char *getline_unlim(){
	int i=5;
	char *s;
	char *p=(char*)malloc(FGETS_BUFSIZE);
	if (fgets(p,FGETS_BUFSIZE,stdin)==NULL)
		if (feof(stdin))
			return NULL;
		else fprintf(stderr,"%s",strerror(errno));

	else if ((strlen(p) != FGETS_BUFSIZE-1) || (*(p + strlen(p) - 1) == '\n')){
		*(p+strlen(p)-1)='\0';
		return p;
	}
	else{
		s=(char*)malloc(FGETS_BUFSIZE);
		*s='\0';
		s=strcat(s,p);
		while ((fgets(p,FGETS_BUFSIZE,stdin)!=NULL)&&(strlen(p)==FGETS_BUFSIZE-1)&&(*(p+strlen(p)-1)!='\n')){
			s=(char*)realloc(s,i+=5);
			s=strcat(s,p);
		}

	*(p+strlen(p)-1)='\0';
	s=strcat(s,p);

	
	}
	return s;

}

list formlist(char *s){
	int i = 0;
	list l = malloc(sizeof(node)),head = l;
	l->name = malloc(100);
	int len = 1,lenm = 100;
	
	  while (s[i] != '\0'){
		while (s[i] == ' ') i++;
		int j = 0;
		l->arg = malloc(100);
		l->arg[0] = malloc(100);
		int size = 100;
		int narg = 100;
		int len =0;
		while (s[i] !='|'  && s[i] != '\0' && s[i] != ' '){
			l->name[j] = s[i]; 
			//l->name[j+1] = '\0';
			len++;
			if (len >= size){
				size*=10;
				l->name=realloc(l->name,size);
				l->arg[0] = realloc(l->arg[0],size);
			}
			l->arg[0][j] = s[i];
		//	l->arg[0][j+1] = '\0';
		//	len = strlen(l->name);
			i++;
			j++;
		}
		l->arg[0][j]='\0';
		l->name[j] = '\0';
		//printf("%s\n",l->name);
		//printf("%s\n",l->arg[0]);
		while (s[i] == ' ') i++;
		if (s[i] == '\0'){
			//l->arg = malloc(10);
			l->arg[1] = malloc(10);
			l->arg[1] = NULL;
			l->next = NULL;
			return head;
		}
		int k=1;
			
		l->arg[1] = malloc(100);
		int argc = 1;

		while (s[i] !='|' && s[i] != '\0'){

			j = 0;
			len = 0;
			while (s[i] !='|' && s[i] != '\0' && s[i] != ' '){
				len ++;
				if (len >= size){
					size*=10;
					l->arg[k] = realloc(l->arg[k],size);
				}
				
				*(l->arg[k] + j) = s[i];

				j++;
				i++;
			}
			*(l->arg[k] + j) = '\0';
			argc++;
			//l->argcount = k+1;
			//printf("%s\n",l->arg[k]);
			if (s[i] == '\0'){
				l->arg[k+1]=NULL;
				l->next = NULL; 
				return head;
			}
			//if (s[i] == '|') {
			//	i++;
				
			//	goto next;
		//	}
			while (s[i] == ' ')
			i++;
			//if (++k > lenm){
			//	l->arg = realloc(l->arg,lenm+20);
			//	lenm +=20;
			//}
			if (argc>=narg){
				narg*=10;
				l->arg = realloc(l->arg,narg);
			}
			if (strcmp(l->arg[k] ,"&") ==0)
				bg = 1;
			k++;
			l->arg[k] = malloc(100);
			//l->arg[k] = NULL;

		}
		if (s[i] == '\0'){
			l->arg[k]=NULL;
			l->next = NULL; 
			return head;
		}
		//printf("next\n");
		l->arg[k] = NULL;
		l->next = malloc(sizeof(node));
		l=l->next;
		l->name = malloc(10);
		if (s[i] == '|') i++;
		while (s[i] == ' ')
		i++;
	}
	l->next = NULL;
	return head;
}

void freelist(list l){
	while (l->next != NULL){
		list tmp = l;
		l=l->next;
		free(tmp);
	}
	free(l);
}

void printlist(list l){
	int i=0;
	while (l != NULL){
		printf("%s\n",l->name);
		//for (i=0;i<= l->argcount;i++)
		i=0;
		while (l->arg[i] != NULL)
			printf("=%s\n",l->arg[i++]);
	
		l = l->next;
	}

}
		
int main(void){
	char *s;
	int n = 0,j,i,zombie = 0,status;
	zmblst zmb = malloc(sizeof(s)+sizeof(node)),zmbhead = zmb;
	zmb->next = NULL;

	while ((s = getline_unlim()) != NULL){
	//	printf("zombie == %d\n",zombie);
		n=0;
		list conv = malloc(sizeof(node)),head,start = conv,prev;

		if (s[0] == '\0') continue;
		conv = formlist(s);
		head = conv;
	
		while (conv!= NULL){
			//printf("%d  \n",n);
			
			//conv->arg[conv->argcount] = NULL;
			n++;
			prev = conv;
			conv = conv->next;
			//printf("%s\n%s\n",conv->name,conv->arg[0]);
		}
		i=0;
	 	while (prev->arg[i+1] != NULL){
			//printf("%s\n",prev->arg[i]);
			i++;
		}
		//printf("%s\n",prev->arg[i]);
		bg = 0;
		if (strcmp(prev->arg[i],"&") == 0){
			prev->arg[i] = NULL;
			bg = 1;
		}
		//printf("bg = %d \n",bg);
		//head = head->next;
		conv = head;
		int w = 0;
		//printlist(conv);
		int fd[n-1][2];
		
		if (!bg){
			if (n == 1){
				if ((conv->pid = fork()) == 0){				
					execvp(conv->name,conv->arg);
					perror("exec");
					exit(1);
				}
				else	
				waitpid(conv->pid,&status,0);
				printf("exit status of %s is %d\n",s,status);
				if (zombie){
					
					while ((w=waitpid(-1,&status,WNOHANG))>0){
						zmb=zmbhead;
						while (zmb != NULL){
						if (w == zmb->pid){
							printf("exit status of %s: %d\n",zmb->str,status);
							zmb->pid = 0;
							break;
						}
						zmb=zmb->next;
						}
					}
					if (w == -1)//all children have been waited for
						zombie = 0;
					
				}
			}
			else{
				//printlist(conv);
				//freelist(conv);
				for (i = 0; i<n-1; i++)
					pipe(fd[i]);
				if ((conv->pid = fork()) == 0){
					dup2(fd[0][1],1);
					for (i = 0;i<n-1;i++){
						close(fd[i][0]);
						close(fd[i][1]);
					}
					execvp(conv->name,conv->arg);
					perror("exec");
					exit(1);
				}
				for (j = 1;j<n-1; j++){
					conv=conv->next;
					if ((conv->pid = fork()) == 0){
						dup2(fd[j][1],1);
						dup2(fd[j-1][0],0);
						for (i = 0;i<n-1;i++){
							close(fd[i][0]);
							close(fd[i][1]);
						}
						execvp(conv->name,conv->arg);
						perror("exec");
						exit(1);
					}
				}	
				conv = conv->next;
				if ((conv->pid = fork()) == 0){
					dup2(fd[n-2][0],0);
					for (i = 0;i<n-1;i++){
						close(fd[i][0]);
						close(fd[i][1]);
					}
					execvp(conv->name,conv->arg);
					perror("exec");
					exit(1);
				}
				for (i=0;i<n-1;i++){
					close(fd[i][0]);
					close(fd[i][1]);
				}
				for (i = 0; i < n; i ++){
					
					waitpid(head->pid,&status,0);
				
					head=head->next;
				}
				printf("exit status of %s is %d\n",s,status);
				if (zombie){
				
					while ((w = waitpid(-1,&status,WNOHANG))>0){
						zmb=zmbhead;
						while (zmb != NULL){
						if (w == zmb->pid){
							printf("exit status of %s: %d\n",zmb->str,status);
							zmb->pid = 0;
							break;
						}
						zmb=zmb->next;
						}
					}
					if (w == -1)//all children have been waited for
						zombie = 0;
					
				}
				
			}	
			
		
			
		}	
		else{//background
			int f = open("/dev/null",0);
			 
			if (n == 1){
				if ((conv->pid = fork()) == 0){		
					dup2(f,0);		
					execvp(conv->name,conv->arg);
					perror("exec");
					exit(1);
				}
				if ((w = waitpid(conv->pid,&status,WNOHANG)) == conv->pid)
					printf("exit status of %s is %d\n",s,status);
				else
				if (w == -1) zombie = 0;
				else if (w == 0) {
					zmb=zmbhead;
					while (zmb->next != NULL)
						zmb=zmb->next;
					zmb->next = malloc(sizeof(s)+sizeof(zmbnode)+sizeof(int));
					zmb = zmb->next;
					zmb->pid = conv->pid;
					zmb->str = s;
					zmb->next = NULL;
					zombie = 1;
				}
				
					while ((w = waitpid(-1,&status,WNOHANG))>0){
						zmb=zmbhead;
						while (zmb!=NULL){
							if (w == zmb->pid){
								printf("exit status of %s: %d\n",zmb->str,status);
								zmb->pid = 0;
							}
							zmb=zmb->next;
						}
						if (w == -1){//all children have been waited for
							zombie = 0;
							break;
						}
						
					}
					
				}

			else{
				//printlist(conv);
				//freelist(conv);
				for (i = 0; i<n-1; i++)
					pipe(fd[i]);
				if ((conv->pid = fork()) == 0){
					dup2(f,0);
					dup2(fd[0][1],1);
					for (i = 0;i<n-1;i++){
						close(fd[i][0]);
						close(fd[i][1]);
					}
					execvp(conv->name,conv->arg);
					perror("exec");
					exit(1);
				}
				for (j = 1;j<n-1; j++){
					conv=conv->next;
					if ((conv->pid = fork()) == 0){
						dup2(fd[j][1],1);
						dup2(fd[j-1][0],0);
						for (i = 0;i<n-1;i++){
							close(fd[i][0]);
							close(fd[i][1]);
						}
						execvp(conv->name,conv->arg);
						perror("exec");
						exit(1);
					}
				}	
				conv = conv->next;
				if ((conv->pid = fork()) == 0){
					dup2(fd[n-2][0],0);
					for (i = 0;i<n-1;i++){
						close(fd[i][0]);
						close(fd[i][1]);
					}
					execvp(conv->name,conv->arg);
					perror("exec");
					exit(1);
				}
				for (i=0;i<n-1;i++){
					close(fd[i][0]);
					close(fd[i][1]);
				}
				int w=0;
				
				if ((w = waitpid(conv->pid,&status,WNOHANG))== conv->pid)
					printf("exit status of %s is %d\n",s,status);
				
				if (w == -1) zombie = 0;
				else 
				    if (w == 0) {
					zmb=zmbhead;
					while (zmb->next != NULL)
						zmb = zmb->next;
					zmb->next = malloc(sizeof(s)+sizeof(zmbnode)+sizeof(int));
					zmb = zmb->next;
					zmb->pid = conv->pid;
					zmb->str = s;
					zmb->next = NULL;
					zombie = 1;
				    }
			
				while ((w = waitpid(-1,&status,WNOHANG))>0){
					zmb=zmbhead;
					while (zmb != NULL){
						if (w == zmb->pid){
							printf("exit status of %s: %d\n",zmb->str,status);
							zmb->pid = 0;
						}
						if (w == -1){//all children have been waited for
							zombie = 0;
							break;
						}
						zmb=zmb->next;
					}
					
				}
					
				
				
			}
		}
		freelist(start);
			
		}
	

return 0;
}


