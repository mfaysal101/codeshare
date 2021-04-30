#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>
#include<set>
#include<map>

#define FILENAME "email-Eu-core.txt"

using namespace std;

int main(int argc, char* argv[])
{
	if(argc<3)
	{
		cout<<"Please provide input and output file names"<<endl;
		cout<<"usage: ./program_name input_filename.txt output_filename.txt"<<endl;
		return 0;
	}
	ifstream inputfile(argv[1]);
	ifstream input(argv[1]);
	ofstream output(argv[2]);
	
	typedef vector<long long> adjacencyList;
	vector<adjacencyList> csr;
	set<long long> Nodes;
	map<long long, long long> nodeMap;
	
	long long a, b, line = 0;
	long long totalNode = -1, totalEdge;
	bool zero = false;
	long long counter = 0;
	
	while(input>>a>>b)
	{
		Nodes.insert(a);
		Nodes.insert(b);
		if(a==0||b==0)
		{
			zero=true;
		}
		line++;
	}
	
	totalEdge = line;
	
	totalNode = Nodes.size();
	csr.reserve(totalNode);
	
	for (set<long long>::iterator it = Nodes.begin(); it != Nodes.end(); it++)
	{
	    //output<<*it<<endl;
	    nodeMap.insert(make_pair(*it, counter));
	    counter++;
	}
	
	
	for(long long i=0; i<totalNode; i++)
	{
		adjacencyList myvector;
		csr.push_back(myvector);
	}
	
	cout<<"totalSize of the csr:"<<csr.size()<<endl;
	
	while(inputfile >> a >> b)
	{
		long long x = nodeMap.at(a);
		long long y = nodeMap.at(b);
		
		//cout<<"x:"<<x<<" y:"<<y<<endl;
		
		if(csr.at(x).empty())
		{
			csr.at(x).push_back(y);
		}
		else
		{
			if(find(csr[x].begin(), csr[x].end(), y) == csr[x].end())
			{
				csr[x].push_back(y);
			}
		}
		
		if(csr.at(y).empty())
		{
			csr.at(y).push_back(x);
		}
		else
		{
			if(find(csr[y].begin(), csr[y].end(), x) == csr[y].end())
			{
				csr[y].push_back(x);
			}
		}
		
	}

	if(zero)
	{
		output<<totalNode<<"\t"<<totalEdge<<"\t"<<0<<endl;		
	}
	else
	{
		output<<totalNode<<"\t"<<totalEdge<<"\t"<<1<<endl;
	}
	long long totalarcs = 0;
	
	for(long long i=0; i<totalNode;i++)
	{
		adjacencyList myvector=csr.at(i);
		if(!myvector.empty())
		{
			for(long long j=0;j<myvector.size();j++)
			{
				output<<myvector.at(j)<<"\t";
				totalarcs++;
			}
			output<<endl;
		}
	}
	
	inputfile.close();
	input.close();
	output.close();
	
	cout<< "output file written"<<" "<<totalarcs<<endl;
	
	return 0;
}
