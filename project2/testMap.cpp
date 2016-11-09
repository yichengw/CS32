#include "Map.h"
#include <iostream>
#include <cassert>
#include <type_traits>

using namespace std;

//two test in spec of Project2
void ThisFunctionWillNeverBeCalled();
void test();

int main()
{
	//default constructor
	Map m;  // maps strings to doubles
	assert(m.empty()); //test empty
	assert(m.size()==0);//test size
	ValueType v = -2.5;
	assert(!m.get("abc", v) && v == -2.5); // v unchanged by get failure
	assert(m.insert("xyz", 9876.5)); //test insert
	assert(m.contains("xyz")); //test contain
	assert(m.insert("abc", 22)); //test insert
	assert(!m.insert("xyz", 23)); //can't insert the same key
	assert(m.size() == 2);//test size
	KeyType k = "hello";
	assert(m.get(1, k, v) && k == "xyz"  &&  v == 9876.5);//test get(int i, KeyType& key, ValueType& value)
	assert(!m.update("aaa", 23)); //can't update the key not inside
	assert(m.update("abc", 23)); //test update
	assert(m.get("abc", v) && v == 23); // update is success and test get(const KeyType& key, ValueType& value)
	Map b;
	b.insert("bbb", 1);
	//test swap
	m.swap(b);
	ValueType x;
	assert(m.size() == 1);
	assert(m.get("bbb", x) && x == 1);
	assert(b.size() == 2);
	//test copy constructor
	Map m2(m);
	assert(m2.size() == 1);
	assert(m2.get("bbb", x) && x == 1);
	//test insertOrupdate
	assert(m2.insertOrUpdate("bbb",2));//update
	assert(m2.get("bbb", x) && x == 2);
	assert(m2.size() == 1);
	assert(m2.insertOrUpdate("ccc", 3));//insert
	assert(m2.get("ccc", x) && x == 3);
	assert(m2.get(0,k,x) && k=="ccc" && x == 3);
	assert(m2.size() == 2);
	//test erase
	assert(!m2.erase("ddd"));
	assert(m2.erase("ccc"));
	assert(m2.size() == 1);
	assert(m2.get("bbb", x) && x == 2);
	//test operator = 
	Map m3;
	m3 = m2;
	assert(m3.size() == 1);
	assert(m3.get("bbb", x) && x == 2);
	//test combine
	Map n1, n2,result;
	n1.insert("Fred",123);
	n1.insert("Ethel",456);
	n1.insert("Lucy",789);
	n2.insert("Lucy", 789);
	n2.insert("Ricky",321);
	assert(combine(n1, n2, result));
	assert(result.size() == 4);
	assert(result.get("Fred",x) && x == 123);
	assert(result.get("Ricky", x) && x == 321);
	assert(result.get("Lucy", x) && x == 789);
	assert(result.get("Ethel", x) && x == 456);
	n2.update("Lucy", 654);
	assert(!combine(n1, n2, result));
	assert(result.size() == 3);
	assert(result.get("Fred", x) && x == 123);
	assert(result.get("Ricky", x) && x == 321);
	assert(result.get("Ethel", x) && x == 456);
	//test subtract
	Map n3;
	n3.insert("Lucy", 789);
	n3.insert("Ricky", 321);
	n3.insert("Ethel", 654);
	subtract(n1, n3, result);
	assert(result.size() == 1);
	assert(result.get("Fred", x) && x == 123);
	test();
	cout << "Passed all tests" << endl;
}

#define CHECKTYPE(f, t) { auto p = (t)(f); (void)p; }

static_assert(std::is_default_constructible<Map>::value,
	"Map must be default-constructible.");
static_assert(std::is_copy_constructible<Map>::value,
	"Map must be copy-constructible.");

void ThisFunctionWillNeverBeCalled()
{
	CHECKTYPE(&Map::operator=, Map& (Map::*)(const Map&));
	CHECKTYPE(&Map::empty, bool (Map::*)() const);
	CHECKTYPE(&Map::size, int  (Map::*)() const);
	CHECKTYPE(&Map::insert, bool (Map::*)(const KeyType&, const ValueType&));
	CHECKTYPE(&Map::update, bool (Map::*)(const KeyType&, const ValueType&));
	CHECKTYPE(&Map::insertOrUpdate, bool (Map::*)(const KeyType&, const ValueType&));
	CHECKTYPE(&Map::erase, bool (Map::*)(const KeyType&));
	CHECKTYPE(&Map::contains, bool (Map::*)(const KeyType&) const);
	CHECKTYPE(&Map::get, bool (Map::*)(const KeyType&, ValueType&) const);
	CHECKTYPE(&Map::get, bool (Map::*)(int, KeyType&, ValueType&) const);
	CHECKTYPE(&Map::swap, void (Map::*)(Map&));

	CHECKTYPE(combine, bool(*)(const Map&, const Map&, Map&));
	CHECKTYPE(subtract, void(*)(const Map&, const Map&, Map&));
}

void test()
{
	Map m;
	assert(m.insert("Fred", 123));
	assert(m.insert("Ethel", 456));
	assert(m.size() == 2);
	double d = 42;
	assert(m.get("Fred", d) && d == 123);
	d = 42;
	string s1;
	assert(m.get(0, s1, d) &&
		((s1 == "Fred"  &&  d == 123) || (s1 == "Ethel"  &&  d == 456)));
	string s2;
	assert(m.get(1, s2, d) && s1 != s2 &&
		((s2 == "Fred"  &&  d == 123) || (s2 == "Ethel"  &&  d == 456)));
}