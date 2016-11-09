//Map.cpp
//Created by Yicheng Wang

#include "Map.h"
#include <iostream>
#include<string>

Map::Map()
{
	m_numofkeys = 0;
	head = nullptr;
}

Map::~Map()
{
	Node* n = head;
	if (n != nullptr)
	{
		while (n->next != nullptr)
		{
			n = n->next;
			delete n->prev;
		}
		delete n;
	}
}
Map::Map(const Map& other)
{
	m_numofkeys = other.m_numofkeys;
	Node* copy = new Node;
	head = copy;	//a new copy list
	Node* n = nullptr;
	Node* m = other.head;

	if (m == nullptr)
		copy = nullptr;
	while (m != nullptr)
	{
		copy->key = m->key;
		copy->value = m->value;
		copy->prev = n;
		if (m->next != nullptr)

			copy->next = new Node;

		else
			copy->next = nullptr;

		n = copy;
		m = m->next;
		copy = copy->next;
	}
}
Map& Map::operator=(const Map& other)
{
	if (this != &other)
	{
		Map temp(other);
		swap(temp);
	}
	return*this;

}
bool Map::empty() const
{
	if (head == nullptr)
	{
		return true;
	}
	return false;
}

int Map::size() const
{
	return m_numofkeys;

}

bool Map::insert(const KeyType& key, const ValueType& value)
{
	if (this->contains(key))
	{
		return false;
	}
	Node* n = new Node;	//create a new node
	n->key = key;
	n->value = value;
	n->next = head;		//add the new node to the head
	n->prev = nullptr;
	if (n->next != nullptr)
	{
		n->next->prev = n;
	}
	head = n;
	m_numofkeys++;
	return true;

}

bool Map::update(const KeyType& key, const ValueType& value)
{
	Node* n = head;
	while (n != nullptr)
	{
		if (n->key == key)
		{
			n->value = value;
			return true;
		}
		n = n->next;
	}
	return false;
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
	Node* n = head;
	while (n != nullptr)
	{
		if (n->key == key)	//update
		{
			n->value = value;
			return true;
		}
		n = n->next;
	}
	//insert
	n = new Node;	//create a new node
	n->key = key;
	n->value = value;
	n->next = head;		//add the new node to the head
	n->prev = nullptr;
	if (n->next != nullptr)
	{
		n->next->prev = n;
	}
	head = n;
	m_numofkeys++;
	return true;
}

bool Map::erase(const KeyType& key)
{
	Node* n = head;
	while (n != nullptr)
	{
		if (n->key == key)
		{
			if (n->next == nullptr)
			{
				if (n->prev == nullptr)
					head = nullptr;
				else
					n->prev->next = nullptr;
			}
			else
			{
				if (n->prev == nullptr)
				{
					head = head->next;
					head->prev = nullptr;
				}
				else
				{
					n->prev->next = n->next;
					n->next->prev = n->prev;
				}
			}
			delete n;
			m_numofkeys--;
			return true;
		}
	}
	return false;
}

bool Map::contains(const KeyType& key) const
{
	Node* n = head;

	while (n != nullptr)
	{
		if (n->key == key)
		{
			return true;
		}
		n = n->next;
	}
	return false;

}

bool Map::get(const KeyType& key, ValueType& value) const
{
	Node* n = head;
	while (n != nullptr)
	{
		if (n->key == key)
		{
			value = n->value;
			return true;
		}
		n = n->next;
	}
	return false;

}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
	if (i >= this->size() || i < 0)	//out of scope
		return false;
	Node* n = head;
	for (int j = 0; j < i; j++)
	{
		n = n->next;	//move to the data
	}
	key = n->key;	//update key
	value = n->value;	//update value
	return true;
}

void Map::swap(Map& other)
{
	Node* temp1 = head;
	head = other.head;
	other.head = temp1;

	int temp2 = m_numofkeys;
	m_numofkeys = other.m_numofkeys;
	other.m_numofkeys = temp2;
}

void Map::dump()
{
	Node* printer = head;
	if (printer == nullptr)
	{
		std::cerr << "the map is empty!" << std::endl;
	}
	while (printer != nullptr) //Prints the keys and values in list
	{
		std::cerr << "Key: " << printer->key << " Value: " << printer->value << std::endl;
		std::cerr << "next: " << printer->next << " prev: " << printer->prev << std::endl;
		printer = printer->next;
	}

}
bool combine(const Map& m1, const Map& m2, Map& result)
{
	Map temp;
	KeyType key_1, key_2;
	ValueType value_1, value_2;
	bool addflag, returnflag;
	returnflag = true;

	for (int i = 0; i < m1.size(); i++)	//add nodes from m1
	{
		addflag = true;
		m1.get(i, key_1, value_1);
		for (int j = 0; j < m2.size(); j++)
		{
			m2.get(j, key_2, value_2);
			if ((key_1 == key_2) && (value_1 != value_2))
			{
				returnflag = false;
				addflag = false;
			}
		}
		if (addflag == true)
			temp.insert(key_1, value_1);
	}

	for (int j = 0; j < m2.size(); j++)	//add nodes from m2
	{
		addflag = true;
		m2.get(j, key_2, value_2);
		for (int i = 0; i < m1.size(); i++)
		{
			m1.get(i, key_1, value_2);
			if (key_1 == key_2)		//duplicated
			{
				addflag = false;
			}
		}
		if (addflag == true)
			temp.insert(key_2, value_2);
	}
	result = temp;
	return returnflag;
}

void subtract(const Map& m1, const Map& m2, Map& result)
{
	Map temp;
	KeyType key_1, key_2;
	ValueType value_1, value_2;
	bool addflag;

	for (int i = 0; i < m1.size(); i++)
	{
		addflag = true;
		m1.get(i, key_1, value_1);
		for (int j = 0; j < m2.size(); j++)
		{
			m2.get(j, key_2, value_2);
			if (key_1 == key_2)
				addflag = false;
		}
		if (addflag == true)
			temp.insert(key_1, value_1);
	}
	result = temp;

}