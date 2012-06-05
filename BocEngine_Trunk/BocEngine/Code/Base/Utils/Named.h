#pragma once

#ifndef _NAMED_H
#define _NAMED_H

#include <string>

class CNamed
{
protected:
	std::string		m_Name;

public:
	CNamed()
		: m_Name("")
	{
	}

	virtual ~CNamed()
	{
	}

	void SetName ( const std::string &Name )
	{
		m_Name = Name;
	}

	const std::string& GetName () const
	{
		return m_Name;
	}

};

#endif