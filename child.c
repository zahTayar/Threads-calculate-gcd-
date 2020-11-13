// Threads calculate gcd
/*
exercise one in operation systems,
Read from text numbers and build two process 
From each get two numbers and calculate the gcd 
The communication between them happen with pipes
*/

//v2_child



        #include <stdio.h>
        #include <unistd.h>
        #include <stdlib.h>
        #include <sys/stat.h>
        #include <sys/wait.h>
        int gcd(int a, int b){
	        if (b == 0)
		        return a;
	        return gcd(b, a % b);
        }
        int main(int argc, char* argv[]) {
	        int a,b,answer,finish=-1;
	        while(1){
		        if(read(STDIN_FILENO,&a,sizeof(int))==EOF){
			        write(STDOUT_FILENO,&finish,sizeof(int));
			        break;
		        }
		        if(read(STDIN_FILENO,&b,sizeof(int))==EOF){
			        break;
		        }
		        answer=gcd(a,b);
		        if (write(STDOUT_FILENO,&answer,sizeof(int))==-1){
			        break;
		        }
		        if(a==-1){
			        write(STDOUT_FILENO,&finish,sizeof(int));
		        	break;
		        }
	        }       


        return EXIT_SUCCESS;


        }
