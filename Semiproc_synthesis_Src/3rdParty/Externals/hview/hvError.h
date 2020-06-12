/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

#if !defined(AFX_ERROR_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_)
#define AFX_ERROR_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <iostream>
#include <cstdlib>

inline void hvFatal(const char *str)
{
	std::cout<<"Internal error:"<<str<<"\n";
	char buffer[16];
	std::cin.getline(buffer,16);
	abort();
}

#endif // !defined(AFX_ARRAY1_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_)
