#ifndef CONFIG_H
#define CONFIG_H
#include <map>
#include <string>
using namespace std;

#define INVALIDCONFIG -111

extern map<string, string> g_config;

bool ReadConfig(const string & filename);

void PrintConfig(const map<string, string> &m);

int GetConfig(string strKey);

#endif