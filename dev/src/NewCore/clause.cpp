#include "clause.h"

bool clause::has(int l)
{
	return assoc_lit.find(l) != assoc_lit.end();
}

bool clause::has_trigger(int t)
{
	return triggers.find(t) != triggers.end();
}

int clause::other_trigger(int t)
{
	assert(has_trigger(t));
	triggers.erase(t);
	assert(triggers.size() == 1);
	int t2 = *triggers.begin();
	triggers.insert(t);
	return t2;
}

string clause::to_str()
{
	string s = "";
	for(auto l : literal)
		s += to_string(l)+" ";
	s += "0";
	return s;
}