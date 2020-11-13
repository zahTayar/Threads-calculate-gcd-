# Threads calculate gcd
/*
exercise one in operation systems,
Read from text numbers and build two process 
From each get two numbers and calculate the gcd 
The communication between them happen with pipes
*/


/* 
The parent activate the child .

*/

//v2_parent

        #include <stdio.h>
        #include <unistd.h>
        #include <string.h>
        #include <stdlib.h>
        #include <sys/stat.h>
        #include <sys/wait.h>

        #define READ 0
        #define WRITE 1
        #define SIZE 50

        void getNums(char* str, int count, int* argv,int index) {//convert str to numbers and check input
	        argv[index-2] = atoi(strtok(str," "));
	        argv[index-1] = atoi(strtok(NULL," \r"));
	        char* exa=strtok(NULL," ");
	        if ( exa!=NULL && strcmp( exa,"\n")) {
		        printf("illegal input in line %d\n", count);
        	}       
        }
        int* getNumbersFromFile(FILE* f,int* size){//create array of numbers from file data.
	        int index=0;
	        char str[SIZE];
	        int* args=(int*)malloc(sizeof(int));
	        int counter=0;
        	while (!feof(f)) {
	        	fgets(str,SIZE,f);
		        if (!strcmp(str,"\n")) {break;}
		        counter++;
		        index+=2;
		args=(int*)realloc(args,sizeof(int)*index);
		getNums(str,counter,args,index);

	        }
	        args[index]=counter;
	        *size=index;//size of array
	        fclose(f);
	        return args;
        }
        int dealPipes(int* in, int* out){// close pipe and change between input and output pipes
        	if (close(out[READ])==-1)	return 0;
        	if (close(in[WRITE])==-1)	return 0;
        	if (close(STDIN_FILENO)==-1)	return 0;
        	dup(in[READ]);
        	if (close(STDOUT_FILENO)==-1)	return 0;
        	dup(out[WRITE]);
        	return 1;

         }
        int main(int arg, char* argv[]) {
        	if (arg!=2){
        	   	perror("Error input need file name parmater");
	        	return 0;
	        	   }

        	int* args;
        	int size;
        	FILE* f = fopen(argv[1], "r");
        	if (f == NULL) {
        		perror("failed open file\n");
        		return 0;
        	}

               	args= getNumbersFromFile(f,&size);
                int fd0_in[2];//parent write to child one
	        int fd0_out[2];//child one respond to parent
        	int fd1_in[2];//parent write to child two
        	int fd1_out[2];//child tow respond to parent
        
        	if (pipe(fd0_in) == -1 || pipe(fd0_out) == -1 || pipe(fd1_in) == -1 || pipe(fd1_out) == -1) {
        		perror("pipe() failed");
        		return EXIT_FAILURE;
        	}
        	switch(fork()){//create child one 
        	case -1:
        		perror("child one Failed");
        		break;

        	case 0://child one process
	        	if (dealPipes(fd0_in,fd0_out)==0)
	        		printf("deal pipe Failed\n ");
			
	        	if(execlp("./V2_child", "V2_child", NULL)==-1){
	        		perror("Failed excute child");
	        	}
	        	break;
        	default://parent process
	        	switch(fork()){//create child two 
	                 	case -1:
		                	perror("child two Failed");
		                	break;

		                case 0://child two process
	                		if (dealPipes(fd1_in,fd1_out)==0) 
		                		printf("deal pipe Failed\n ");
				
		                	if(execlp("./V2_child", "V2_child", NULL)==-1){
		                		perror("Failed excute child");
	                        	}
                			break;
		                        default:
	                        		break;
	                        	}
	                        }
	                //close pipes
	        if (close(fd0_in[READ])==-1)	return 0;
	        if (close(fd0_out[WRITE])==-1)	return 0;
	        if (close(fd1_in[READ])==-1)	return 0;
	        if (close(fd1_out[WRITE])==-1)	return 0;
	
	        int answer;
	        for(int i=0;i<size;i+=4){
	        	//write per of numbers to pipe
	        	if (write(fd0_in[WRITE],&args[i],sizeof(int))==-1){
	        		perror("failed writing");
		}
		if (write(fd0_in[WRITE],&args[i+1],sizeof(int))==-1){
			perror("failed writing");
		}
		//write another per of numbers to pipe
		if (write(fd1_in[WRITE],&args[i+2],sizeof(int))==-1){
			perror("failed writing");
		}
		if (write(fd1_in[WRITE],&args[i+3],sizeof(int))==-1){
			perror("failed writing");
		}

		if(read(fd0_out[READ],&answer,sizeof(int))==EOF)//read first answer from pipe
			break;
		printf("%d %d gcd: %d\n", args[i] , args[i+1], answer);

		if(read(fd1_out[READ],&answer,sizeof(int))==EOF)//read second answer from pipe
			break;
	        printf("%d %d gcd: %d\n", args[i+2] , args[i+3], answer);


	        }
        	int finish=-1;//write -1 to pipes when finsh reading 
        	if(write(fd0_in[WRITE],&finish,sizeof(int))==-1)
        		perror("cant write to pipe");
        	if(write(fd1_in[WRITE],&finish,sizeof(int))==-1)
	        	perror("cant write to pipe");
        	if (close(fd0_in[WRITE])==-1)	return 0;
	        if (close(fd1_in[WRITE])==-1)	return 0;
	
	        return EXIT_SUCCESS;
        }

