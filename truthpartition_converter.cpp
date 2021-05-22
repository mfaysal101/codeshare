#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>
#include<set>
#include<map>

using namespace std;

int main(int argc, char* argv[])
{
	if(argc<3)
	{
		cout<<"Please provide input and output file names"<<endl;
		cout<<"usage: ./program_name input_filename.txt output_filename.txt"<<endl;
		return 0;
	}

	ifstream input(argv[1]);
	ofstream output(argv[2]);
	
	long long a, b, line = 0;
	
	while(input>>a>>b)
	{
		output<<(a-1)<<"\t"<<(b-1)<<endl;
		line++;
	}

	input.close();
	output.close();
	
	cout<< "output file written"<<" "<<line<<endl;
	
	return 0;
}