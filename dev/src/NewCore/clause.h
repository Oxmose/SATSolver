#ifndef DEF_CLAUSE_H
#define DEF_CLAUSE_H

#include <cassert>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>

using namespace std;

struct clause
{
	clause(int id) : id(id) {}

	bool has(int l);
	bool has_trigger(int t);
	int other_trigger(int t);

	string to_str();
	
	int id;
	vector<int> literal;
	unordered_map<int,bool> assoc_lit;

	/* STD */
	set<int> alive_lit;

	/* WL */
	set<int> triggers;
};

#endif // DEF_CLAUSE_H
