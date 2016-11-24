#include <fstream>

using namespace std;

int	main( int argc , char** argv)
{
	ofstream file(argv[1]);

	for( int i=0; i<10000000; i++ )
		file << i << endl;

	file.close();

	return 0;
}

