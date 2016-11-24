/*
 *  testbench.c
 *
 *  - User level testbench for my custom real-time scheduler
 *    & Tests system call int  get_dsn_rt_prio( pid_t )
 *    & Tests system call void set_dsn_rt_prio( pid_t , int )
 *    & Tests priority scheme imposed by dsn_prio
 *
 *  Author	: Kamyar Mirzazad (kammirzazad@ee.sharif.edu)
 *  Created  On	: June 27th , 2014
 *  Modified On : -
 *
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */


using namespace std;

int	main( int argc , char** argv )
{
	/*
	
	if(argc==1) { return -1; }

	pid_t pid = 10;

	cout << " Before calling set_dsn_rt_prio , dsn_prio is : " << syscall(351,pid) << endl;

	syscall(352,pid,atoi(argv[1]));

	cout << " After setting dsn_prio to " << atoi(argv[1]) << " : " << syscall(351,pid) << endl;

	*/

	/***************************/

	pid_t 	p1 = fork();
	struct 	sched_param sp1;

		if( p1 == 0 )
	{
		execlp("./filewriter.o", "./filewriter.o", "1.txt", NULL);
	}
	else	if( p1 <  0 )
	{
		cout << " 1st Fork failed " << endl;
		return -1;
	}
	else
	{
		cout << " Setting schedulign parameters for task 1 " << endl;
		memset( &sp1, 0, sizeof(sp1) );
		sp1.sched_priority = sched_get_priority_max(SCHED_FIFO);		

		if( sched_setscheduler(p1,SCHED_FIFO, &sp1) == -1 )
		{
			if( errno == EINVAL )
			{
				printf(" setscheduler failed due to EINVAL");
			}
			else if ( errno == EPERM )
			{
				printf(" setscheduler failed due to EPERM");
			}
			else
			{
				printf(" setscheduler failed due to ESRCH");
			}
			

			kill(p1, SIGKILL);		
			return -1;
		}		

		syscall(352,p1,10);
	}

	
	/***************************/

	pid_t 	p2 = fork();
	struct 	sched_param sp2;

		if( p2 == 0 )
	{
		execlp("./filewriter.o", "./filewriter.o", "2.txt", NULL);
	}
	else	if( p2 <  0 )
	{
		kill(p1, SIGKILL);
		cout << " 2nd fork failed " << endl;
		return -1;
	}
	else
	{	
		cout << " Setting schedulign parameters for task 2 " << endl;
		memset( &sp2, 0, sizeof(sp2) );
                sp2.sched_priority = sched_get_priority_max(SCHED_FIFO);

                if( sched_setscheduler(p2,SCHED_FIFO,&sp2) < 0 )
                {
			if( errno == EINVAL )
                        {
                                printf(" setscheduler failed due to EINVAL");
                        }
                        else if ( errno == EPERM )
                        {
                                printf(" setscheduler failed due to EPERM");
                        }
                        else
                        {
                                printf(" setscheduler failed due to ESRCH");
                        }

			kill(p1, SIGKILL);
			kill(p2, SIGKILL);                      
                        return -1;
                }
	
		syscall(352,p2,-20);
	}

	/***************************/

	if( wait(NULL) == p1 )
		cout << " There is problem with scheduler " << endl;
	else
		cout << " It works ! " << endl;

	kill(p1, SIGKILL);
	kill(p2, SIGKILL);

	remove("1.txt");
	remove("2.txt");
	
	return 0;
}
