#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <iostream>

using namespace std;

int	main()
{
	int x = syscall(__NR_get_dsn_rt_prio,10);

	cout << " Before : " << x << endl;

	syscall(__NR_set_dsn_rt_prio,10,100);

	x = syscall(__NR_set_dsn_rt_prio,10);

	cout << " After : " << x << endl;

	return 0;
}
