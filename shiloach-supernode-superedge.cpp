#include <stdio.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <set>
#include <chrono>
#include <omp.h>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;




typedef std::vector<std::pair<int, int> > EdgeList;
typedef std::vector<std::vector<int> > AdjList;


AdjList adjlist;

std::vector<int>parent;
std::map<std::pair<int, int>, int> p;
int numEdges;
int numVertices;
bool hooking;
size_t maxk;
std::vector<std::vector<int>> intersectlist;
std::map<pair<int, int>, int>trussk;
std::map<pair<int, int>, int> edge2index;
std::vector<std::vector<pair<pair<int, int>, pair<int, int>>>> super_edges;
std::vector<std::pair<int, int>> summary_graph;

namespace
{
	template <class InputIterator1, class InputIterator2>
	vector<int> IntersectionSize(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, int n)
	{
		vector<int> result;
		while (first1 != last1 && first2 != last2)
		{
			if (*first1 >= n || *first2 >= n)
			{
				break;
			}
			if (*first1 < *first2)
			{
				++first1;
			}
			else if (*first1 > *first2)
			{
				++first2;
			}
			else
			{
				result.push_back(*first1);
				++first1;
				++first2;
			}
		}
		return result;
	}
}


size_t getNumVertices(const EdgeList& edges)
{
	int num = 0;

#pragma omp parallel for reduction (max:num)
	for (size_t i = 0; i < edges.size(); i++)
	{
		num = max(num, 1 + max(edges[i].first, edges[i].second));
	}

	numVertices = num;

	cout << "Total number of vertices:" << numVertices << endl;

	return numVertices;
}

EdgeList ReadEdgeListFromFile(const char* filename)
{
	EdgeList edgelist;

	cout << "Reading network " << filename << " file\n";

	ifstream infile(filename);

	if (infile)
	{
		printf("File open successful\n");
	}
	else
	{
		printf("Failed to read file\n");
		exit(1);
	}

	string line = "";
	numEdges = 0;

	int index = 0;

	while (getline(infile, line))
	{
		istringstream iss(line);
		int src, dst;
		if ((iss >> src >> dst))
		{
			edgelist.push_back(make_pair(src, dst));
			edge2index[{src, dst}] = index++;
		}
	}

	numEdges = edgelist.size();

	cout << "Total number of edges:" << numEdges << endl;

	sort(edgelist.begin(), edgelist.end(), [](const pair<int, int>& edge1, const pair<int, int>& edge2) {
		return (edge1.first < edge2.first) || (edge1.first == edge2.first && edge1.second < edge2.second);
		});

	return edgelist;

}

void EdgeToAdjList(const EdgeList& edges)
{
	adjlist.resize(getNumVertices(edges));

	for (auto edge : edges)
	{
		adjlist[edge.first].push_back(edge.second);
		adjlist[edge.second].push_back(edge.first);
	}

}

void printParent(ofstream& out)
{
	for (int i = 0; i < numVertices; i++)
	{
		out << i << "\t" << parent[i] << endl;
	}
	out.close();
}

void initializeParent()
{
	parent.resize(numVertices);

	for (int i = 0; i < numVertices; i++)
	{
		parent[i] = i;
	}
}

void parallel_hooking()
{
#pragma omp parallel for
	for (int u = 0; u < numVertices; u++)
	{
		vector<int>& temp = adjlist[u];

#pragma omp parallel for
		for (int j = 0; j < temp.size(); j++)
		{
			int v = temp[j];

			if (parent[u] < parent[v] && parent[v] == parent[parent[v]])
			{
				parent[parent[v]] = parent[u];
				hooking = true;
			}
		}
	}
}

void serial_hooking()
{
	for (int u = 0; u < numVertices; u++)
	{
		vector<int>& temp = adjlist[u];

		for (int j = 0; j < temp.size(); j++)
		{
			int v = temp[j];

			if (parent[u] < parent[v] && parent[v] == parent[parent[v]])
			{
				parent[parent[v]] = parent[u];
				hooking = true;
			}
		}
	}
}

void parallel_compression()
{
#pragma omp parallel for
	for (int v = 0; v < numVertices; v++)
	{
		while (parent[parent[v]] != parent[v])
		{
			parent[v] = parent[parent[v]];
		}
	}
}

void serial_compression()
{
	for (int v = 0; v < numVertices; v++)
	{
		while (parent[parent[v]] != parent[v])
		{
			parent[v] = parent[parent[v]];
		}
	}
}

void shiloach_vishkin_parallel()
{
	hooking = true;

	while (hooking)
	{
		hooking = false;
		parallel_hooking();
		parallel_compression();
	}
}

void shiloach_vishkin_serial()
{
	hooking = true;

	while (hooking)
	{
		hooking = false;
		serial_hooking();
		serial_compression();
	}
}

void populateIntersectList(const EdgeList& edges)
{
	const int n = adjlist.size();

	maxk = 0;

	for (int i = 0; i < edges.size(); i++)
	{
		int u = edges[i].first;
		int v = edges[i].second;

		intersectlist[i] = IntersectionSize(adjlist[u].begin(), adjlist[u].end(), adjlist[v].begin(), adjlist[v].end(), n);

		maxk = max(maxk, intersectlist[i].size());
	}
}

void bucketSortedEdgelist(int kmax, const EdgeList& edges, vector<vector<int>>& sp, vector<pair<int, int>>& sorted_elbys, map<int, int>& svp, map<pair<int, int>, int>& sorted_ep)
{
	vector<int> bucket((kmax + 1), 0);

	for (auto it = sp.begin(); it != sp.end(); it++)
	{
		bucket[(*it).size()]++;
	}

	int temp;

	int pt = 0;

	for (int i = 0; i < kmax + 1; i++)
	{
		temp = bucket[i];
		bucket[i] = pt;
		pt = pt + temp;
	}

	for (int i = 0; i < sp.size(); i++)
	{
		sorted_elbys[bucket[sp[i].size()]] =  edges[i];
		sorted_ep.insert(make_pair(edges[i], bucket[sp[i].size()]));
		if (svp.find(sp[i].size()) == svp.end())
		{
			svp.insert(make_pair(sp[i].size(), bucket[sp[i].size()]));
		}
		bucket[sp[i].size()] = bucket[sp[i].size()] + 1;
	}

}


void computeTruss(const EdgeList& edges)
{
	map<int, pair<int, int>> klistdict;
	vector<pair<int, int>> sorted_elbys(p.size());
	map<pair<int, int>, int> sorted_ep;
	map<int, int> svp;

	bucketSortedEdgelist(maxk, edges, intersectlist, sorted_elbys, svp, sorted_ep);

}


void edge_hooking(const EdgeList& edges)
{

#pragma omp parallel
	{
		size_t numThreads = omp_get_num_threads();
		super_edges.resize(numEdges);
	}
#pragma omp parallel for
	for (int i = 0; i < edges.size(); i++)
	{
		auto e = edges[i];		//got the edge in index i

		vector<int>& temp = intersectlist[i];		//got the corresponding intersecting edges that makes triangle with e

#pragma omp parallel for
		for (int j = 0; j < temp.size(); j++)
		{
			size_t tid = omp_get_thread_num();

			int w = temp[j];
			int u = e.first;
			int v = e.second;

			pair<int, int> e1;
			pair<int, int> e2;

			if (u < w)
			{
				e1.first = u;
				e1.second = w;
			}
			else
			{
				e1.first = w;
				e1.second = u;
			}
			
			if (v < w)
			{
				e2.first = v;
				e2.second = w;
			}
			else
			{
				e2.first = w;
				e2.second = v;
			}

			int k = trussk[e];

			if (trussk[e1] >= k && trussk[e2] >= k)	//ensuring k-triangle
			{
				if (trussk[e1] > k)
				{
					int index = edge2index[e1];
					super_edges[tid].push_back({ e, e1 });
				}

				if (p[e] < p[e1] && p[e1] == p[edges[p[e1]]] && k == trussk[e1])
				{
					p[edges[p[e1]]] = p[e];
					hooking = true;
				}

				if (trussk[e2] > k)
				{
					int index = edge2index[e2];
					super_edges[tid].push_back({ e, e2 });
				}

				if (p[e] < p[e2] && p[e2] == p[edges[p[e2]]] && k == trussk[e2])
				{
					p[edges[p[e2]]] = p[e];
					hooking = true;
				}
			}

			

			

		}
	}
}


void edge_compression(const EdgeList& edges)
{
#pragma omp parallel for
	for (int i = 0; i < edges.size(); i++)
	{
		auto e = edges[i];

		while (p[edges[p[e]]] != p[e])    // p[e] will return me an index of an edge, using edges[p[e]] will return me the parent edge, applying p[edges[p[e]]] will return me the index of the parent edge
		{
			p[e] = p[edges[p[e]]];
		}
	}
}

void conn_comp_edge(const EdgeList& edges)
{
	hooking = true;

	while (hooking)
	{
		hooking = false;
		edge_hooking(edges);
		edge_compression(edges);
	}
}

void readTruss(ifstream& in)
{
	
	string line = "";

	if (in.is_open())
	{
		while (getline(in, line))
		{
			istringstream iss(line);
			string token;
			vector<int>tuple;

			while (getline(iss, token, ','))
			{
				tuple.push_back(stoi(token));
			}

			pair<int, int> temp;
			temp.first = tuple[0];
			temp.second = tuple[1];
			trussk.insert(make_pair(temp, tuple[2]));
		}
	}
	in.close();
}


void sortEachAdjList()
{
	int len = adjlist.size();

#pragma omp parallel for
	for (int i = 0; i < len; i++)
	{
		sort(adjlist[i].begin(), adjlist[i].end());
	}
}

void printEdgeParent(ofstream& out)
{
	for (auto it = p.begin(); it != p.end(); it++)
	{
		out << it->first.first << "\t" << it->first.second << "\t" << it->second << endl;
	}
	out.close();
}

void printSummaryGraph(ofstream& out)
{
	for (auto sp_edge : summary_graph)
	{
		out << sp_edge.first << "\t" << sp_edge.second << endl;
	}
	out.close();
}

void initializeEdgeParent(const EdgeList& edges)
{
	for (int i = 0; i < edges.size(); i++)
	{
		p[edges[i]] = i;
	}
}

void createSummaryGraph(const EdgeList& edges)
{
	set <pair<int, int>> visited;

	for (int i = 0; i < super_edges.size(); i++)
	{
		vector<pair<pair<int, int>, pair<int, int>>> temp = super_edges[i];

		for (int j = 0; j < temp.size(); j++)
		{
			int p1 = p[temp[j].first];
			int p2 = p[temp[j].second];

			if (!visited.count({ p1, p2 }))
			{
				summary_graph.push_back({ p1, p2 });
				visited.insert({ p1, p2 });
			}
		}
	}
}

int main(int argc, char* argv[])
{
	string networkfile;
	string trussfile;

	networkfile = argv[1];
	trussfile = argv[2];


	EdgeList edgelist = ReadEdgeListFromFile(networkfile.c_str());

	ofstream out1("out1.txt"), out2("out2.txt"), out3("out3.txt"), out4("out4.txt");

	ifstream trussinput(trussfile.c_str());

	EdgeToAdjList(edgelist);

	intersectlist.resize(edgelist.size());

	initializeParent();

	auto s_start = std::chrono::high_resolution_clock::now();

	shiloach_vishkin_serial();

	auto s_end = std::chrono::high_resolution_clock::now();

	auto s_time = std::chrono::duration_cast<std::chrono::nanoseconds>(s_end - s_start).count();

	printParent(out1);

	cout << "\nnow the parallel\n\n";

	initializeParent();

	auto p_start = std::chrono::high_resolution_clock::now();

	shiloach_vishkin_parallel();

	auto p_end = std::chrono::high_resolution_clock::now();

	auto p_time = std::chrono::duration_cast<std::chrono::nanoseconds>(p_end - p_start).count();

	printParent(out2);

	printf("========serial_time:%0.9f===========\n", s_time * (1e-9));
	printf("========parallel_time:%0.9f===========\n", p_time * (1e-9));

	initializeEdgeParent(edgelist);

	sortEachAdjList();

	populateIntersectList(edgelist);

	readTruss(trussinput);

	conn_comp_edge(edgelist);

	createSummaryGraph(edgelist);

	printEdgeParent(out3);

	printSummaryGraph(out4);

	return 0;
}

class Edge
{
public:

	int s;
	int t;


	Edge()
	{

	}

	Edge(int source, int target)
	{
		s = source;
		t = target;
	}

	// "<" operator overloading required by c++ map for custom object type
	bool operator<(const Edge& ob) const
	{
		return s < ob.s || (s == ob.s && t < ob.t);
	}

};
