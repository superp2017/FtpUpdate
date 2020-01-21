#pragma once
//#include "NetComb.h"
#include <sys/types.h>
#include <vector>
#include <map>
#define  FTW_F  1
#define FTW_D  0

//ftw回调函数
int delete_device(const char *file, const struct stat *sb, int flag);
int copyto_device(const char *file, const struct stat *sb, int flag);
const int buffersize = 8192;
class CFTP_C
{
public:
	CFTP_C();
	CFTP_C(const std::string& host, int port = 21);
	~CFTP_C();
	//设置FTP客户端连接的超时时间。单位微秒
	void SetTimeout(int &iTimeout);
	//获取FTP客户端连接的超时时间。单位微秒
	int GetTimeout();
	//设置是否使用PASV模式，0为使用PORT模式，1为使用PASV模式
	void SetPassive(bool bFlag);
	//获取是否使用了PASV模式
	bool GetPassive();
	//建立FTP连接，如果用户名密码不为空，则登录到FTP服务器
    bool Open(const std::string& stHost, int iPort, const std::string& strUsername, const std::string& strPassword);
	//登录到FTP服务器
	void Login(const std::string& strUsername, const std::string& strPassword);
	//退出FTP服务器
	void Logout();
	//退出FTP服务器并断开连接
	void Close();
	//设置文件传输类型,0表示ASCII,1表示BINARY
	void SetFileType(int iType);
	//获取文件传输类型
	int GetFileType();
	//获取FTP服务器类型
	std::string SystemType();
	//更改FTP服务器当前工作目录,strPath为需要更改成的当前目录
	void CwdWorkingDirectory(const std::string& strPath);
	//获取FTP服务器当前工作目录
	std::string GetWorkingDirectory();
	//切换FTP服务器的目录到当前目录的父目录
	void Cdup();
	//重命名FTP服务器上的文件名
	void Rename(const std::string& strOldName, const std::string& strNewName);
	//删除FTP服务器上的文件
	void Remove(const std::string& strPath);
	//在FTP服务器上创建目录
	bool CreateDirectory(const std::string& strPath);
	//在FTP服务器上删除目录
	void RemoveDirectory(const std::string& strPath);
	//从FTP服务器上下载文件
	bool GetOneFile(const std::string& strRemotePath, const std::string& strLocalPath);
	int GetOneBreakFile(const std::string& strRemotePath, const std::string& strLocalPath);
	//上传本地文件到FTP服务器
	bool PutOneFile(const std::string& strRemotePath, const std::string& strLocalPath);
	int PutOneFileFromBuffer(const std::string& strRemotePath, const char* buffer);
	//设置下载文件列表的方式，iMode=0,加-al参数，iMode=1不加-al参数
	void SetFileListMode(int iMode = 0);
	//下载FTP服务器目录文件列表
	bool GetFileList(const std::string& strPath);
	//终止上传或者下载的过程
	void Abort();
	//发送服务器命令，若带参数则传给arg
	int SendCommand(const std::string& command, const std::string& arg, std::string& response);
	//查看FTP服务器是否允许连接
	bool IsOpen();
	//查看是否登录成功
	bool IsLoggedIn();
	//获取本地文件清单,存在返回true，不存在返回false
	bool GetLocalItemList(const std::string& strFileName, const bool bIsWildCard, std::map<std::string, std::vector<std::string>> &vecItemList, std::vector<std::string>&nameList);
	//获取文件大小和修改时间
	int GetFileSizeAndModifyTime(const std::string&  fileName, long long &llSize, time_t &tModifyTime);
	//向服务器发送NOOP命令
	bool SendNoop();
	//删除sorpath目录下的所有节点，含文件和子目录
	bool FtwDelete(const std::string& sorpath);
	//把sorpath目录下的所有结构到复制到despath目录节点下
	bool FtwCopyTo(const std::string& sorpath, const std::string& despath);
	//把sorpath目录下的所有结构到转移despath目录节点下
	bool FtwMoveTo(const std::string& sorpath, const std::string& despath);
private:
	//统配符比对
	bool wildcard_matches(const char *wildcard, const char *str);
	void listFiles(const char * dir, int tab, std::map<std::string, std::vector<std::string>> &vecItemListm, std::vector<std::string> &vnameListm);
private:
	bool isOpen;
	bool isLogin;
	int iFileListMode;
	bool mflag;
	std::vector<std::string> strlist;
};

