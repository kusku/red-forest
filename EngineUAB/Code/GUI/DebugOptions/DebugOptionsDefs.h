#pragma once
#ifndef _DEBUG_OPTIONS_DEFS_H
#define _DEBUG_OPTIONS_DEFS_H

#include <string>
#include <vector>

typedef enum TETypePage
{
	OPTIONS = 0,
	STADISTICS, 
	NONE
};

typedef enum TETypeOfValues
{
	BOOL_TYPE = 0,
	INT_TYPE, 
	FLOAT_TYPE,
	NONE_TYPE
};

struct SElementOnPage
{
	std::string		Name;
	std::string		Init_Script;
	std::string		Script;
	TETypeOfValues	Type;
};

typedef std::vector<SElementOnPage>			TElementsOnPage;

struct SPage
{
	TETypePage			Type;
	TElementsOnPage		ElementsVector;
};	

#endif