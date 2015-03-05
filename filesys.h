#include <iostream>
#include <list>
#include <string>

#ifndef NOSTLFS

#include <filesystem>

using namespace std;
using namespace std::tr2::sys;

namespace xmv
{
	//����ָ��Ŀ¼�е������ļ�
	string GetFileName(string path);

	template<class ListOfString>
	void GetFiles(string folderName, ListOfString & filepathList, string extName = "*.*",  bool findSubFolder = false)
	{
		path folerPath(folderName);

		if ( ! exists(folerPath) ) return; 

		bool noFilter = extName == "*" || extName == "*.*" || extName == "" || extName.empty();
		if (! noFilter)
		{
			if (extName.size() >= 2 && extName.substr(0, 2) == "*.") extName = extName.substr(1);
			if (extName.size() >= 1 && extName.substr(0, 1) != ".") extName = "." + extName;
		}

		directory_iterator end_itr; // ȱʡ��������һ������������ 
		for (directory_iterator itr(folerPath); itr != end_itr; ++itr) 
		{
			string fn = itr->path();
			if (is_directory(itr->status())) 
			{ 
				if (findSubFolder)
				{
					GetFiles(fn, filepathList, extName, findSubFolder); //�ݹ���� 
				}
			} 
			else
			{ 
				if (noFilter)
					filepathList.push_back(fn);
				else
				{
					if (itr->path().extension() == extName)
						filepathList.push_back(fn);
				}
			}
		}
	}

#undef CreateDirectory
	void CreateDirectory(string directoryName);
	string StartupPath();
}


#endif