#include<iostream>
#include<fstream>
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<metis.h>

using namespace std;

//vector<int> xadj, adjacency;


void CSRFileParser(char* filename, int totalParts, char* outputFile)
{
	long vertices = 0, edges=0;
	long pointer = 0;
	char* token;
	idx_t* adj;
	idx_t* adjcny;
	idx_t* part;
	long index = 0;
	long edgeindex = 0;
	
	
	FILE* fp = fopen(filename, "r");
	
	if(fp==NULL)
	{
		exit(EXIT_FAILURE);
	}
	
	cout<<"File open successful\n";
	
	char* line = NULL;
	size_t len = 0;
	
	//read the network attributes
	if((getline(&line, &len, fp)!= -1))
	{
		//printf("before reading line:%s\n",line);
		token = strtok(line," \n\t");
		//printf("after reading line:%s\n",line);
		if(token!= NULL)
		{
			vertices= atol(token);
			printf("number of vertices:%s\n", token);
			//xadj.reserve(vertices+1);
			//xadj.push_back(0);
			adj = (idx_t*)malloc((vertices+1)*sizeof(idx_t));
			part = (idx_t*)malloc(vertices*sizeof(idx_t));
			adj[index] = 0;
			//printf("%d\n",adj[0]); 
		}
		token = strtok(NULL," \n\t");
		if(token!= NULL)
		{
			edges = atol(token);
			printf("number of edges:%s\n", token);
			//adjacency.reserve(2*edges);
			adjcny= (idx_t*)malloc((2*edges)*sizeof(idx_t));	
		}
	}
	
	while((getline(&line, &len, fp)!= -1))
	{
		if(strlen(line)<=2)
		{
			printf("why not reached?");
			break;
		}
		//printf("reading edgelist:%s, strlen:%d\n",line, strlen(line));
		token = strtok(line," \n\t");
		while(token != NULL){
			//printf("%s",token);
			long edge = atol(token);
			//adjacency.push_back(edge);
			adjcny[pointer] = edge;
			//edgeindex++;
			pointer++;
			token = strtok(NULL," \n\t");
		}
		index++;
		//xadj.push_back(pointer);
		adj[index] = pointer;
	}

	if(line)
	{
		free(line);
	}
	
	/*
	cout<<"Showing list indices..."<<endl;
	for(int i=0;i<xadj.size();i++)
	{
		cout<<xadj.at(i)<<" "<<adj[i]<<endl;

	}
	
	cout<<"Showing adjacency list..."<<endl;	
	for(int i=0;i<adjacency.size();i++)
	{
		cout<<adjacency.at(i)<<" "<<adjcny[i]<<endl;
	}
	*/
	
	
	idx_t nvtxs = index;
	
	idx_t ncon = 1;
	
	idx_t nParts = totalParts;
	
	idx_t objval;
	
	idx_t allparts[index];
	
	cout<<nvtxs<<endl<<endl;
	
	int ret = METIS_PartGraphKway ( &nvtxs, &ncon, adj, adjcny, NULL, NULL, 
    	NULL, &nParts, NULL, NULL, NULL, &objval, part);
    	
    	ofstream output(outputFile);
    	
	for ( long part_i = 0; part_i < nvtxs; part_i++ )
	{
		//cout << "     " << part_i << "     " << part[part_i] << endl;
		output<<part_i<<'\t'<<part[part_i]<<endl<<flush;
	}
	
	//cout << "  Return code = " << ret << "\n";
	
	free(adj);
	free(adjcny);
	free(part);
	fclose(fp);
	output.close();
	
	
}

void partgraphkway_test ( )
{
	//The number of vertices
	idx_t nvtxs = 6;		
}

int main(int argc, char* argv[])
{
	if(argc<2)
	{
		cout<<"usage: inputfile argument"<<endl;
		return 0;
	}
	
	CSRFileParser(argv[1], atoi(argv[2]), argv[3]);
	
	
	
	
	return 0;		
}
