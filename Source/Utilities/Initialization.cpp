#include "../BombastEngineStd.h"
#include "../Bombast/BombastApp.h"
#include "Initialization.h"
#include <fstream>
#include <sstream>

using namespace rapidxml;

//Checks if there is window already existing.
bool IsOnlyInstance(LPCTSTR gameTitle)
{
	//Find Window, if active, set and return false

	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);

	if (GetLastError() != ERROR_SUCCESS)
	{
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd)
		{
			BE_ERROR("Instance Exists");
			//An instance of game is already running
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
			return false;
		}
	}

	return true;
}

//Check to see if there is megaBytesNeeded of Storage available
bool CheckStorage(const DWORDLONG megaBytesNeeded)
{
	//Check for enough free storage on current disk
	int const drive = _getdrive();
	struct _diskfree_t diskFree;

	_getdiskfree(drive, &diskFree);

	//Availablespace equals megabytes Free
	unsigned __int64 const availableSpace = (((diskFree.sectors_per_cluster * diskFree.bytes_per_sector) >> 10) * diskFree.avail_clusters) >> 10;

	if (availableSpace < megaBytesNeeded)
	{
		BE_ERROR("CheckStorage Failure: Not enough physical storage");
		return false;
	}

	return true;
}

/**Check to see if there is enough physical and virtual ram to run the game
* @param physicalRAMNeeded - Physical RAM needed in MB
* @param physicalRAMNeeded -  Virtual RAM needed in MB
*/
bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
	MEMORYSTATUSEX status;

	status.dwLength = sizeof(status);

	GlobalMemoryStatusEx(&status);

	unsigned __int64 physMemoryMB = status.ullTotalPhys >> 20;
	unsigned __int64 virtMemoryMB = status.ullAvailVirtual >> 20;

	if (physMemoryMB < physicalRAMNeeded)
	{
		BE_ERROR("CheckMemory Failure: Not Enough Physical RAM");
		return false;
	}

	if (virtMemoryMB < virtualRAMNeeded)
	{
		BE_ERROR("CheckMemory Failure: Not Enough Virtual Memory");
		return false;
	}

	return true;
}

//Get CPU Speed
DWORD ReadCPUSpeed()
{
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;

	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);

	if (lError == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKey, L"~MHz", NULL, &type, (LPBYTE)&dwMHz, &BufSize);
	}

	return dwMHz;
}

GameOptions::GameOptions()
{
	m_level = "";
	m_renderer = "Direct3D 11";
	m_screenSize = Point(1024, 768);

	m_pDoc = NULL;
}

void GameOptions::Init(const char* xmlFilePath, LPSTR lpCmdLine)
{
	std::ifstream file(xmlFilePath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	std::string content(buffer.str());
	xml_document<> doc;
	doc.parse<0>(&content[0]);

	//Get Root node
	xml_node<> *pRoot = doc.first_node("Options");

	if (!pRoot)
	{
		return;
	}

	//Fetch each child and load component
	xml_node<> *pNode = NULL;
	pNode = pRoot->first_node("Graphics");
	if (pNode)
	{
		std::string attribute;
		attribute = pNode->first_attribute("renderer")->value();
		if (attribute != "Direct3D 9" && attribute != "Direct3D 11")
		{
			BE_ERROR("Bad Renderer setting in Graphics options.");
		}
		else
		{
			m_renderer = attribute;
		}
			
		if (pNode->first_attribute("width"))
		{
			attribute = pNode->first_attribute("width")->value();
			m_screenSize.x = atoi(attribute.c_str());
			if (m_screenSize.x < 800) { m_screenSize.x = 800; }
		}

		if (pNode->first_attribute("height"))
		{
			attribute = pNode->first_attribute("height")->value();
			m_screenSize.y = atoi(attribute.c_str());
			if (m_screenSize.y < 600) { m_screenSize.y = 600; }
		}
	}
}