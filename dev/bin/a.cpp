#include <cstdio>
#include <vector>
#include <map>
#include <set>

using namespace std;

int main()
{
	map<pair<int,bool>,set<pair<int,bool>>> a;
	a[make_pair(3,true)] = set<pair<int,bool>>();
	if(a.find(make_pair(3,true)) != a.end())
		printf("YOUPI!\n");
}
