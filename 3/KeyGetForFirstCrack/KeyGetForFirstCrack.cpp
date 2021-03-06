// KeyGetForFirstCrack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "pch.h"
#include <iostream>
#include <string>
#include <algorithm>


uint32_t HashLogin(const std::string& sLogin)
{
	std::string str = sLogin;
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	uint32_t res = 0;
	for (size_t i = 0, sz = str.length(); i < sz; ++i)
	{
		res += str[i];
	}

	return res ^ 0x5678;
}

uint32_t HashPWD(const std::string& sPwd)
{
	return atoi(sPwd.c_str()) ^ 0x1234;
}

std::string FindPWD(const std::string& sLogin)
{
	uint32_t hashLog = HashLogin(sLogin) ^ 0x1234;
	return std::to_string(hashLog);	 
}

int main()
{

	std::string sLogin;

	std::cout << "Enter login: ";
	std::cin >> sLogin;

	uint32_t hashLog = HashLogin(sLogin);
	std::string pwd =  FindPWD(sLogin);

	uint32_t hashPwd =  HashPWD(pwd);


	if (hashPwd == hashLog)
	{
		std::cout << "\n pwd: " << pwd;
	}
	else
	{
		std::cout << "\n can't find pwd for login " << sLogin;
	}

	return 0;
}
