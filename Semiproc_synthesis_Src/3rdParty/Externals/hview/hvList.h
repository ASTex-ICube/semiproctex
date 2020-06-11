// List.h: interface for the List class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIST_H__297E9DA5_208E_4B76_9FA8_57C002399BE0__INCLUDED_)
#define AFX_LIST_H__297E9DA5_208E_4B76_9FA8_57C002399BE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "hvError.h"

namespace  hview {



template <class T> class hvSortedList  : public std::vector<T>
{
public:
	hvSortedList<T>():std::vector<T>() { }
	hvSortedList<T>(unsigned long m): std::vector<T>(m) { }
	// copy
	hvSortedList<T>(const hvSortedList<T> &a)	{ hvFatal("no temporary creation of hvSortedList<T>!"); }
	// affectation
	hvSortedList<T> &operator=(const hvSortedList<T> &a){ hvFatal("no affectation of hvSortedList<T>, use operator <copy>!"); }

	void sort() { partialSort(0,this->size()-1); }
	void partialSort(int start, int end);
	void pointerSort() { partialPointerSort(0, this->size()-1); }
	void partialPointerSort(int start, int end);
	int  pushSorted(T e);	// require operator <=
	int  pushSortedBound(T e, int n);	// require operator <=
	int  pushSortedSet(T e); // require operator <= and ==
	int  search(T e) const; // require operator and ==
	int  searchSorted(T e) const; // require operator <= and ==
	int  searchSortedPos(T e, int start, int end) const; // require operator <= and ==
	int  searchSortedPosSet(T e, int start, int end) const; // require operator <= and ==
	void swap(int pos1, int pos2); 

	void reverse(int start, int end);
	void reverse() { reverse(0, this->size() - 1); }

};



template <class T> void hvSortedList<T>::partialSort(int start, int end)
{
    int a,b;
    int m;
	T x;

    if (start>=end) return;
    a=start; b=end; m=(start+end)>>1;
	x = this->at(m);
    while (a<=b)
    {
       while ( this->at(a)<x ) a++;
       while ( x<this->at(b) ) b--;
       if (a<=b)
       {
          swap(a,b);
          a++; b--;
       }
    }
    partialSort(start,b);
    partialSort(a,end);
}

template <class T> void hvSortedList<T>::partialPointerSort(int start, int end)
{
    int a,b;
    int m;
	T x;

    if (start>=end) return;
    a=start; b=end; m=(start+end)>>1;
	x = this->at(m);
    while (a<=b)
    {
		while ( x->compare(this->at(a),x)<0 ) a++;
		while ( x->compare(x, this->at(b))<0 ) b--;
       if (a<=b)
       {
          swap(a,b);
          a++; b--;
       }
    }
    partialSort(start,b);
    partialSort(a,end);
}

template <class T> int hvSortedList<T>::pushSorted(T e)
{
    int pos;

    if (std::vector<T>::size()==0) { this->push_back(e); return(0); }
    pos = this->searchSortedPos(e,0,this->size());
    if (pos == -1) return(-1);
	std::vector<int>::iterator it;
	it = this->begin();
    this->insert(it+pos,e);
    return(pos);
}
template <class T> int hvSortedList<T>::pushSortedBound(T e, int n)
{
	int pos;

	if (std::vector<T>::size() == 0) { this->push_back(e); return(0); }
	pos = this->searchSortedPos(e, 0, this->size());
	if (pos == -1) return(-1);
	if (this->size() == n)
	{
		if (pos == this->size()) return -1;
		else this->pop_back();
	}
	std::vector<int>::iterator it;
	it = this->begin();
	this->insert(it + pos, e);
	return(pos);
}

template <class T> int hvSortedList<T>::pushSortedSet(T e)
{
    int pos;

    if (std::vector<T>::size() ==0) { this->push_back(e); return(0); }
    pos = this->searchSortedPosSet(e,0, this->size());
    if (pos == -1) return(-1);
	std::vector<int>::iterator it;
	it = this->begin();
	this->insert(it + pos, e);
	return(pos);
}

template <class T> int hvSortedList<T>::search(T e) const
{
	long i;

	if (std::vector<T>::size() == 0) return(-1);
	for (i = 0; i<std::vector<T>::size(); i++)
		if (std::vector<T>::at(i) == e) return(i); 
	return(-1);
}

template <class T> int hvSortedList<T>::searchSorted(T e) const
{
  long i;

  if (std::vector<T>::size() ==0) return(-1);
  i=this->searchSortedPos(e,0, this->size() -1);
  if (std::vector<T>::at(i)==e) return(i); else return(-1);
}

template <class T> int hvSortedList<T>::searchSortedPos(T e, int start, int end) const
{
    int m;

    while (start < end)
    {
        m=(start+end)>>1;
        if (std::vector<T>::at(m)<e ) start=m+1; else end=m;
    }
    return(start);
}

template <class T> int hvSortedList<T>::searchSortedPosSet(T e, int start, int end) const
{
    int m,ee;

	ee=end;
    while (start < end)
    {
        m=(start+end)>>1;

	if (std::vector<T>::at(m) == e) return(-1);
    else if (std::vector<T>::at(m)<e ) start=m+1;
	else end=m;
    }
	if (start==ee) return start;
	if (std::vector<T>::at(start) == e) return(-1);
    else return(start);
}

template <class T> void hvSortedList<T>::swap(int pos1, int pos2)
{
T x;

if (pos1==pos2) return;
x= std::vector<T>::at(pos1);
std::vector<T>::operator[](pos1) = std::vector<T>::at(pos2);
std::vector<T>::operator[](pos2) = x;
}

template <class T> void hvSortedList<T>::reverse(int start, int end)
{
	int i;
	//if (val == 0) { hvFatal("cannot reverse, hvList is void"); }
	if ((start<0) || ((unsigned)start>this->size())) { std::cout<<"illegal start in reverse hvSortedList\n"; hvFatal("out of range"); }
	if ((end<0) || ((unsigned)end>this->size()) || (end<start)) { std::cout << "illegal end in reverse hvSortedList\n"; hvFatal("out of range"); }
	for (i = 0; i <= ((end - start) >> 1); i++) swap(i + start, end - i);
}
}

#endif // !defined(AFX_LIST_H__297E9DA5_208E_4B76_9FA8_57C002399BE0__INCLUDED_)
