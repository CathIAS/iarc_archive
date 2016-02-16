#include <iostream>
#include <fstream>
#include "config.h"
using namespace std;

#define COMMENT_CHAR '#'

map<string, string> g_config;

bool IsSpace(char c)
{
	if (' ' == c || '\t' == c)
	{
		return true;
	}
	return false;
}

bool IsCommentChar(char c)
{
	switch(c)
	{
	case COMMENT_CHAR:
		return true;
	default:
		return false;
	}
}

void Trim(string & str)
{
	if (str.empty())
	{
		return;
	}
	unsigned int i, start_pos, end_pos;
	for (i = 0; i < str.size(); ++i)
	{
		if (!IsSpace(str[i]))
		{
			break;
		}
	}
	if (i == str.size())
	{ // 全部是空白字符串
		str = "";
		return;
	}

	start_pos = i;

	for (i = str.size() - 1; i >= 0; --i)
	{
		if (!IsSpace(str[i]))
		{
			break;
		}
	}
	end_pos = i;

	str = str.substr(start_pos, end_pos - start_pos + 1);
}

bool AnalyseLine(const string & line, string & key, string & value)
{
	if (line.empty())
	{
		return false;
	}
	int start_pos = 0, end_pos = line.size() - 1, pos;
	if ((pos = line.find(COMMENT_CHAR)) != -1)
	{
		if (0 == pos)
		{  // 行的第一个字符就是注释字符
			return false;
		}
		end_pos = pos - 1;
	}
	string new_line = line.substr(start_pos, start_pos + 1 - end_pos);  // 预处理，删除注释部分

	if ((pos = new_line.find('=')) == -1)
	{
		return false;  // 没有=号
	}

	key = new_line.substr(0, pos);
	value = new_line.substr(pos + 1, end_pos + 1- (pos + 1));

	Trim(key);
	if (key.empty())
	{
		return false;
	}
	Trim(value);
	char *pValue = (char*)value.c_str();
	for ( int n = 0; pValue[n]; ++n )
	{
		if ( pValue[n] == '\n' || pValue[n] == '\r' )
		{
//			printf("n:%d %s\n",n,pValue);
			pValue[n] = 0;
		}
	}
	return true;
}

void PrintConfig(const map<string, string> &m)
{
	map<string, string>::const_iterator mite = m.begin();
	for (; mite != m.end(); ++mite)
	{
		cout <<"map:  "<< mite->first << "=" << mite->second << endl;
	}
}

bool ReadConfig(const string & filename)
{
	g_config.clear();
	ifstream infile(filename.c_str());
	if (!infile)
	{
		cout << "file open error" << endl;
		return false;
	}
	string line, key, value;
	while (getline(infile, line))
	{
		cout<<line<<endl;
		if (AnalyseLine(line, key, value))
		{
			g_config[key] = value;
		}
	}

	infile.close();
	return true;
}

int GetConfig(string strKey)
{
	int nValue;
	if ( sscanf(g_config[strKey].c_str(),"%d",&nValue) != 1 )
	{
		return INVALIDCONFIG;
	}

	return nValue;
}
