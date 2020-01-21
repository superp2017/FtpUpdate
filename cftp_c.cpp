#include "CFTP_C.h"
#include <QDebug>
#include <QString>
#include "Poco/Net/NetException.h"
#include "Poco/Exception.h"
#include "Poco/StreamCopier.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Ascii.h"
//#include "Poco/Util/Application.h"
////#include "bol_file.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include "Poco/Path.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/FTPClientSession.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <iosfwd>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <Poco/File.h>
#include <Poco/Net/FTPClientSession.h>
#include <Poco/StreamCopier.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#pragma execution_character_set("utf-8")
///#include <boost/math/constants/constants.hpp>
///#include <boost/multiprecision/cpp_dec_float.hpp>
///#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>
#include <corecrt_io.h>
using namespace std;
///using namespace boost::math::constants;
///using namespace boost::multiprecision;
///using namespace boost::asio;
using namespace Poco;
///using namespace boost;
using Poco::Path;
using Poco::Net::FTPClientSession;
using Poco::Net::FTPException;
using Poco::Net::SocketAddress;
using Poco::Net::SocketStream;
using Poco::Net::StreamSocket;
using Poco::StreamCopier;
using Poco::LocalDateTime;
using Poco::File;
const int length = 100;
static char oldpath[length];
static char newpath[length];
static int folder_level = 0;

Poco::Net::FTPClientSession* ftp = NULL;
void parseAddress(const std::string& str, Poco::Net::SocketAddress& addr)
{
    File f;
    f.createDirectories();
    std::string::const_iterator it = str.begin();
    std::string::const_iterator end = str.end();
    while (it != end && *it != '(') ++it;
    if (it != end) ++it;
    std::string host;
    while (it != end && Poco::Ascii::isDigit(*it)) host += *it++;
    if (it != end && *it == ',') { host += '.'; ++it; }
    while (it != end && Poco::Ascii::isDigit(*it)) host += *it++;
    if (it != end && *it == ',') { host += '.'; ++it; }
    while (it != end && Poco::Ascii::isDigit(*it)) host += *it++;
    if (it != end && *it == ',') { host += '.'; ++it; }
    while (it != end && Poco::Ascii::isDigit(*it)) host += *it++;
    if (it != end && *it == ',') ++it;
    Poco::UInt16 portHi = 0;
    while (it != end && Poco::Ascii::isDigit(*it)) { portHi *= 10; portHi += *it++ - '0'; }
    if (it != end && *it == ',') ++it;
    Poco::UInt16 portLo = 0;
    while (it != end && Poco::Ascii::isDigit(*it)) { portLo *= 10; portLo += *it++ - '0'; }
    addr = SocketAddress(host, portHi * 256 + portLo);
}

int delete_device(const char *file, const struct stat *sb, int flag)
{
    //if (flag == FTW_F)
    if (flag == 1)
    {
        //bmco_information_f3(Poco::Util::Application::instance().logger(), "%s|%s|调用bol_file_unlink函数删除文件%s", std::string("0"), std::string("bol_file_unlink"), (std::string)file);
        //bol_file_unlink(file);
        unlink(file);
    }
    //else if (flag == FTW_D)
    else if (flag == 2)
    {
        Poco::Path p;
        p.tryParse(file);
        int level = p.depth();
        if (level + 1 > folder_level)
            folder_level = level + 1;
        //rmdir(file);//删除原文件夹   由于文件夹下有文件所以第一次无法删除,需要记录层次
    }
    return 0;
}

int copyto_device(const char *file, const struct stat *sb, int flag)
{
    //if (flag == FTW_F)
    if (flag == 1)
    {
        //bmco_information_f3(Poco::Util::Application::instance().logger(), "%s|%s|调用link函数复制文件%s", std::string("0"), std::string("link"), (std::string)file);
        const char* suffix = file;
        suffix += strlen(oldpath);
        char realpath[length];
        memset(realpath, 0, sizeof(realpath));
        memcpy(realpath, newpath, strlen(newpath));
        strcat(realpath, suffix);
        //link(file, realpath);
    }
    //else if (flag == FTW_D)
    else if (flag == 2)
    {
        const char* suffix = file;
        suffix += strlen(oldpath);
        char realpath[length];
        memset(realpath, 0, sizeof(realpath));
        memcpy(realpath, newpath, strlen(newpath));
        strcat(realpath, suffix);
        //bol_file_mkdir(realpath,0777);       //在新路径下创建新文件夹名
        //mkdir(realpath, 0777);
    }

    return 0;
}
CFTP_C::CFTP_C()
{
    ftp = NULL;
    isOpen = true;
    isLogin = false;
    iFileListMode = 0;
    mflag = false;
}


CFTP_C::CFTP_C(const std::string& host, int port /*= 21*/)
{
    ftp = new FTPClientSession(host, port);
    isOpen = false;
}

CFTP_C::~CFTP_C()
{
    delete ftp;
    ftp = NULL;
}


void CFTP_C::SetTimeout(int &iTimeout)
{
    try {
        Poco::Timespan timeout(iTimeout);
        ftp->setTimeout(timeout);
    }
    catch (...) {
        throw Exception("连接关闭无法设置超时时间", 1208001);
    }
}

int CFTP_C::GetTimeout()
{
    try {
        Poco::Timespan timeout = ftp->getTimeout();
        return (int)timeout.totalMicroseconds();
    }
    catch (...) {
        throw Exception("连接关闭无法获取超时时间", 1208001);
    }
}

void CFTP_C::SetPassive(bool bFlag)
{
    try {
        ftp->setPassive(bFlag);
    }
    catch (...) {
        throw Exception("连接关闭无法设置连接模式", 1208001);
    }
}

bool CFTP_C::GetPassive()
{
    return ftp->getPassive();
}

bool CFTP_C::Open(const std::string& stHost, int iPort, const std::string& strUsername, const std::string& strPassword)
{
    try {
        ftp = new FTPClientSession(stHost, iPort);
        isOpen = false;
        if (strUsername != "") {
            ftp->login(strUsername, strPassword);
            isLogin = true;
            return true;
        }
    }
    catch (FTPException& ex) {
        qDebug()<<QString(ex.message().c_str());
        //		if (ex.message().find("Cannot login to server") != std::string::npos)
        //			throw Exception("不能登录到服务器", 1208002);
        //		else if (ex.message().find("Login denied") != std::string::npos)
        //			throw Exception("用户名或密码错误", 1208003);
    }
    return false;
}

void CFTP_C::Login(const std::string& strUsername, const std::string& strPassword)
{
    try {
        ftp->login(strUsername, strPassword);
        isLogin = true;
    }
    catch (FTPException& ex) {
        if (ex.message().find("Cannot login to server") != std::string::npos)
            throw Exception("不能登录到服务器", 1208002);
        else if (ex.message().find("Login denied") != std::string::npos)
            throw Exception("用户名或密码错误", 1208003);
    }
}

void CFTP_C::Logout()
{
    std::string response;
    int status = ftp->sendCommand("QUIT", response);
    if (status / 100 != 2)
        throw Exception("连接关闭无法发送退出命令", 1208001);
    isLogin = false;
}

void CFTP_C::Close()
{
    ftp->close();
    isOpen = true;
}

void CFTP_C::SetFileType(int iType)
{
    try {
        if (iType == 0)
            ftp->setFileType(FTPClientSession::TYPE_TEXT);
        else
            ftp->setFileType(FTPClientSession::TYPE_BINARY);
    }
    catch (...) {
        throw Exception("不能设置文件传输类型", 1208004);
    }
}

int CFTP_C::GetFileType()
{
    return ftp->getFileType();
}

std::string CFTP_C::SystemType()
{
    try {
        return ftp->systemType();
    }
    catch (...) {
        throw Exception("不能获取远端服务器类型", 1208005);
    }
}

void CFTP_C::CwdWorkingDirectory(const std::string& strPath)
{
    try {
        ftp->setWorkingDirectory(strPath);
    }
    catch (FTPException& ex) {
        if (ex.code() == 550)
            throw Exception("目录不存在", 550);
        else
            throw Exception("无法更改工作目录", 1208006);
    }
}

std::string CFTP_C::GetWorkingDirectory()
{
    try {
        return ftp->getWorkingDirectory();
    }
    catch (...) {
        throw Exception("无法获取当前工作目录", 1208006);
    }
}

void CFTP_C::Cdup()
{
    try {
        ftp->cdup();
    }
    catch (...) {
        throw Exception("无法更改工作目录", 1208006);
    }
}

void CFTP_C::Rename(const std::string& strOldName, const std::string& strNewName)
{
    try {
        ftp->rename(strOldName, strNewName);
    }
    catch (...) {
        throw Exception("无法更改文件名", 1208008);
    }
}

void CFTP_C::Remove(const std::string& strPath)
{
    try {
        ftp->remove(strPath);
    }
    catch (...) {
        throw Exception("无法删除文件名", 1208009);
    }
}

bool CFTP_C::CreateDirectory(const std::string& strPath)
{
    try {
        ftp->createDirectory(strPath);
    }catch (...) {
        qDebug()<<"创建目录失败:"<<strPath.c_str();
        return false;
        //throw Exception("无法创建目录", 1208010);
    }
    return true;
}

void CFTP_C::RemoveDirectory(const std::string& strPath)
{
    try {
        ftp->removeDirectory(strPath);
    }
    catch (...) {
        throw Exception("无法删除目录", 1208011);
    }
}

bool CFTP_C::GetOneFile(const std::string& strRemotePath, const std::string& strLocalPath)
{
    Poco::FileOutputStream fops(strLocalPath);
    try {
        std::istream& filecontex = ftp->beginDownload(strRemotePath);
        //为实现断点续传，可在此处一部分一部分的接收并存文件
        Buffer<char> buffer(buffersize);
        std::streamsize len = 0;
        filecontex.read(buffer.begin(), buffersize);
        std::streamsize n = filecontex.gcount();
        while (n > 0)
        {
            len += n;
            fops.write(buffer.begin(), n);
            if (filecontex && fops)
            {
                filecontex.read(buffer.begin(), buffersize);
                n = filecontex.gcount();
            }
            else n = 0;
        }
        ftp->endDownload();
        fops.close();
        return true;
    }catch (FTPException& ex) {
        fops.close();
        qDebug()<<ex.message().c_str();
        return false;
        //	if (ex.message().find("PASV command failed") != std::string::npos)
        //	    throw Exception("PASV消息发送失败", 1208014);
        //	else if (ex.message().find("PORT command failed") != std::string::npos)
        //	    throw Exception("PORT消息发送失败", 1208015);
        //	else if (ex.message().find("The server has not initiated a data connection") != std::string::npos)
        //	    throw Exception("服务器没有初始化数据传输连接", 1208013);
        //	else if (ex.message().find("Data transfer failed") != std::string::npos)
        //	    throw Exception("数据传输失败", 1208016);
        //	else
        //	    throw Exception("连接关闭无法下载", 1208001);
    }catch (...) {
        fops.close();
        return false;
        //	throw Exception("连接关闭无法下载", 1208001);
    }
    return false;
}

int CFTP_C::GetOneBreakFile(const std::string& strRemotePath, const std::string& strLocalPath)
{
    Poco::FileStream fops(strLocalPath, std::ios::ate);
    try {
        fops.seekp(0, std::ios::end);
        int length = fops.tellp();
        std::string resp;
        int status = ftp->sendCommand("PASV", resp);
        if (status / 100 != 2)
            throw FTPException("PASV command failed", resp, status);
        SocketAddress sa;
        parseAddress(resp, sa);
        StreamSocket sock(sa);
        SocketStream* filecontex = new SocketStream(sock);
        std::string pos;
        Poco::format(pos, "%d", length);
        int ret = ftp->sendCommand("REST", pos, resp);
        if (ret != 350) {
            throw - 1;
        }
        int ret1 = ftp->sendCommand("RETR", strRemotePath, resp);
        if (ret1 / 100 != 1)
            throw FTPException("RETR command failed", resp, ret1);

        Buffer<char> buffer(buffersize);
        std::streamsize len = 0;
        (*filecontex).read(buffer.begin(), buffersize);
        std::streamsize n = (*filecontex).gcount();
        while (n > 0)
        {
            len += n;
            fops.write(buffer.begin(), n);
            if (*filecontex && fops)
            {
                (*filecontex).read(buffer.begin(), buffersize);
                n = (*filecontex).gcount();
            }
            else n = 0;
        }
        if (filecontex)
        {
            delete filecontex;
            filecontex = 0;
        }
        fops.close();
        return 1;
    }
    catch (FTPException& ex) {
        fops.close();
        if (ex.message().find("PASV command failed") != std::string::npos)
            throw Exception("PASV消息发送失败", 1208014);
        else if (ex.message().find("PORT command failed") != std::string::npos)
            throw Exception("PORT消息发送失败", 1208015);
        else if (ex.message().find("The server has not initiated a data connection") != std::string::npos)
            throw Exception("服务器没有初始化数据传输连接", 1208013);
        else if (ex.message().find("Data transfer failed") != std::string::npos)
            throw Exception("数据传输失败", 1208016);
        else
            throw Exception("连接关闭无法下载", 1208001);
    }
    catch (...) {
        fops.close();
        throw Exception("连接关闭无法下载", 1208001);
    }
}


bool CFTP_C::PutOneFile(const std::string& strRemotePath, const std::string& strLocalPath)
{
    try {
        if (strLocalPath == "")
        {
            return false;
        }
        Poco::FileInputStream fis(strLocalPath);
        std::ostringstream data;
        if (fis.good() == true)
            StreamCopier::copyStream(fis, data);
        std::ostream& ostr = ftp->beginUpload(strRemotePath);
        ostr << data.str();
        ftp->endUpload();
        return true;
    } catch (FTPException& ex) {
        qDebug()<<ex.message().c_str();
        //	if (ex.message().find("PASV command failed") != std::string::npos)
        //	    throw Exception("PASV消息发送失败", 1208014);
        //	else if (ex.message().find("PORT command failed") != std::string::npos)
        //	    throw Exception("PORT消息发送失败", 1208015);
        //	else if (ex.message().find("The server has not initiated a data connection") != std::string::npos)
        //	    throw Exception("服务器没有初始化数据传输连接", 1208013);
        //	else if (ex.message().find("Data transfer failed") != std::string::npos)
        //	    throw Exception("数据传输失败", 1208016);
        //	else
        //	    throw Exception("连接关闭无法下载", 1208001);
    }
    return false;
}

int CFTP_C::PutOneFileFromBuffer(const std::string& strRemotePath, const char* buffer)
{
    try {
        std::string resp = "";
        int status = ftp->sendCommand("PASV", resp);
        if (status / 100 != 2)
            throw FTPException("PASV command failed", resp, status);
        SocketAddress sa;
        parseAddress(resp, sa);
        StreamSocket sock(sa);
        SocketStream* filecontex = new SocketStream(sock);
        int ret = ftp->sendCommand("APPE", strRemotePath, resp);
        if (ret / 100 != 1)
            throw FTPException("APPE command failed", resp, status);
        (*filecontex).write(buffer, strlen(buffer));
        if (filecontex)
        {
            delete filecontex;
            filecontex = 0;
        }
        return 1;

    }
    catch (FTPException& ex) {
        if (ex.message().find("PASV command failed") != std::string::npos)
            throw Exception("PASV消息发送失败", 1208014);
        else if (ex.message().find("PORT command failed") != std::string::npos)
            throw Exception("PORT消息发送失败", 1208015);
        else if (ex.message().find("The server has not initiated a data connection") != std::string::npos)
            throw Exception("服务器没有初始化数据传输连接", 1208013);
        else if (ex.message().find("Data transfer failed") != std::string::npos)
            throw Exception("数据传输失败", 1208016);
        else
            throw Exception("连接关闭无法下载", 1208001);
    }
}

void CFTP_C::SetFileListMode(int iMode)
{
    iFileListMode = iMode;
}

bool CFTP_C::GetFileList(const std::string& strPath)
{
    try {
        Poco::FileOutputStream fos(strPath);
        std::ostringstream dataStr;
        dataStr.clear();
        if (iFileListMode == 0)
            StreamCopier::copyStream(ftp->beginList("-al", true), dataStr);
        else
            StreamCopier::copyStream(ftp->beginList("", true), dataStr);
        ftp->endList();
        fos << dataStr.str();
        fos.close();
        return true;
    }
    catch (FTPException& ex) {
        qDebug()<<ex.message().c_str();
        return false;
        //		if (ex.message().find("PASV command failed") != std::string::npos)
        //			throw Exception("PASV消息发送失败", 1208014);
        //		else if (ex.message().find("PORT command failed") != std::string::npos)
        //			throw Exception("PORT消息发送失败", 1208015);
        //		else if (ex.message().find("The server has not initiated a data connection") != std::string::npos)
        //			throw Exception("服务器没有初始化数据传输连接", 1208013);
        //		else if (ex.message().find("Data transfer failed") != std::string::npos)
        //			throw Exception("数据传输失败", 1208016);
        //		else
        //			throw Exception(ex.message(), 1208001);
    }
    return false;
}

void CFTP_C::Abort()
{
    try {
        ftp->abort();
    }
    catch (...) {
        throw Exception("无法终止传输", 1208012);
    }
}

int CFTP_C::SendCommand(const std::string& command, const std::string& arg, std::string& response)
{
    try {
        return ftp->sendCommand(command, arg, response);
    }
    catch (...) {
        throw Exception("连接关闭无法发送消息", 1103133);
    }
}

bool CFTP_C::IsOpen()
{
    return isOpen;
}

bool CFTP_C::IsLoggedIn()
{
    return isLogin;
}

bool CFTP_C::wildcard_matches(const char *wildcard, const char *str)
{
    if (*wildcard == '\0')
        return *str == '\0';
    if (*wildcard == '?')
        return wildcard_matches(++wildcard, ++str);
    else if (*wildcard == '*') {
        for (++wildcard; *str; ++str)
            if (wildcard_matches(wildcard, str))
                return true;
        return *wildcard == '\0';
    }
    else
        return *wildcard == *str && wildcard_matches(++wildcard, ++str);
}

void CFTP_C::listFiles(const char * dir, int tab, std::map<std::string, std::vector<std::string>> &vecItemList, std::vector<std::string> &vnameListm)
{
    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst(dir, &findData);    // 查找目录中的第一个文件
    if (handle == -1)
    {
        cout << "Failed to find first file!\n";
        return;
    }

    do
    {
        if (findData.attrib & _A_SUBDIR
                && !(strcmp(findData.name, ".") == 0
                     || strcmp(findData.name, "..") == 0
                     )) {    // 是否是子目录并且不为"."或".."
           // cout << findData.name << "\t<dir>\n";

            string subdir(dir);
            mflag = true;
            subdir.insert(subdir.find("*"), string(findData.name) + "/");
            //cout << subdir << endl;
            listFiles(subdir.c_str(), tab + 1, vecItemList, vnameListm);//递归遍历子文件夹
            mflag = false;
            vecItemList[findData.name]=strlist;
            strlist.clear();
        }
        else {
            for (int i = 0; i < tab; ++i)
                cout << "-";
           // cout << findData.name << "\t" << findData.size << endl;
            if (!mflag)
            {
                vnameListm.push_back(findData.name);
            }
            else
            {
                strlist.push_back(findData.name);
            }

        }
    } while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件
    ///system("pause");
    _findclose(handle);    // 关闭搜索句柄

}

bool CFTP_C::GetLocalItemList(const std::string& strFileName, const bool bIsWildCard, std::map<std::string, std::vector<std::string>> &vecItemList, std::vector<std::string>&nameList)
{
    string dir;
    dir = strFileName;
    dir += "*.*";
    listFiles(dir.c_str(),0, vecItemList, nameList);
    return true;
    /*if (bIsWildCard == false) {
        File file(strFileName);
        if (file.exists() == true && file.isFile() == true) {
            vecItemList.push_back(strFileName);
            return true;
        }
        else
            return false;
    }
    else {
        bool isFind = false;
        Path path(strFileName);
        int n = path.depth();
        std::string filename = path[n];
        std::string filepath = path.makeParent().toString();
        File file(filepath);
        std::vector<File> vecFilename;
        file.list(vecFilename);
        for (std::vector<File>::iterator it = vecFilename.begin(); it != vecFilename.end(); it++) {
            if ((*it).isFile() == true) {
                if (wildcard_matches((filepath + filename).c_str(), (*it).path().c_str()) == true) {
                    vecItemList.push_back((*it).path());
                    isFind = true;
                }
            }
        }
        return isFind;
    }*/
}

int CFTP_C::GetFileSizeAndModifyTime(const std::string&  fileName, long long &llSize, time_t &tModifyTime)
{
    try {
        std::istream& istr = ftp->beginList(fileName, true);
        std::ostringstream dataStr;
        StreamCopier::copyStream(istr, dataStr);
        ftp->endList();
        std::string fileInfo = dataStr.str();
        if (fileInfo == "")
            return 0;
        else {
            int fileSizeOff = 5;
            std::string div = " ";
            int find = 0;
            //解析文件大小及时间
            for (int i = 1; i < fileSizeOff; i++)
            {
                find = fileInfo.find_first_of(div);
                fileInfo = fileInfo.substr(find + 1, fileInfo.length() - find - 1);
                //去除连续空格
                while (fileInfo.find_first_of(div) == 0)
                    fileInfo = fileInfo.substr(1, fileInfo.length() - 1);
            }
            find = fileInfo.find_first_of(div);
            std::string size = fileInfo.substr(0, find);
            llSize = (atol)(size.c_str());

            //找时间，格式为Oct 16 15:35
            fileInfo = fileInfo.substr(find + 1, fileInfo.length() - find - 1);
            std::string stempDate = fileInfo.substr(0, 12);
            //转换时间格式
            std::string mon = stempDate.substr(0, 3);
            int imonth = 0;
            if (mon == "Jan") imonth = 1;
            else if (mon == "Feb") imonth = 2;
            else if (mon == "Mar") imonth = 3;
            else if (mon == "Apr") imonth = 4;
            else if (mon == "May") imonth = 5;
            else if (mon == "Jun") imonth = 6;
            else if (mon == "Jul") imonth = 7;
            else if (mon == "Aug") imonth = 8;
            else if (mon == "Sep") imonth = 9;
            else if (mon == "Oct") imonth = 10;
            else if (mon == "Nov") imonth = 11;
            else if (mon == "Dec") imonth = 12;
            int day = atoi(stempDate.substr(4, 2).c_str());
            std::string time = stempDate.substr(7, 5);
            int year = 0;
            int hour = 0;
            int minute = 0;
            LocalDateTime now;
            if (time[0] == ' '&& time[2] != ':')
                year = atoi(time.substr(1, 4).c_str());
            else if (time[2] == ':') {
                year = now.year();
                hour = atoi(time.substr(0, 2).c_str());
                minute = atoi(time.substr(3, 2).c_str());
            }
            if (year * 12 + imonth - now.year() * 12 - now.month() > 3)
                year -= 1;
            LocalDateTime date(year, imonth, day, hour, minute);
            tModifyTime = date.timestamp().epochTime();
        }
        return 1;
    }
    catch (...) {
        throw Exception("连接关闭无法发送消息", 1103133);
    }
}

bool CFTP_C::SendNoop()
{
    try {
        std::string resp;
        int status = ftp->sendCommand("NOOP", resp);
        if (status == 200)
            return true;
        else
            return false;
    }
    catch (...) {
        throw Exception("连接关闭无法发送消息", 1103133);
    }
}

bool CFTP_C::FtwDelete(const std::string& sorpath)
{
    try {
        //folder_level = 0;
        //DIR *sp;
        //if ((sp = opendir(sorpath.c_str())) == 0) //打开copy文件夹，不存在返回0
        //{
        //	throw Exception("使用ftw删除目录下节点失败", 1103140);
        //}
        //int flag_ftw = 0;
        ////bmco_information_f2(Poco::Util::Application::instance().logger(), "%s|%s|调用ftw函数删除文件", std::string("0"), std::string("ftw"));
        //flag_ftw = ftw(sorpath.c_str(), delete_device, 500);
        //Poco::Path p;
        //p.tryParse(sorpath);
        //p.makeDirectory();
        //int count = folder_level - p.depth();
        //for (int i = 0; i <= count; i++)
        //	flag_ftw = ftw(sorpath.c_str(), delete_device, 500);
        //if (flag_ftw != 0) {
        //	closedir(sp);
        //	throw Exception("使用ftw删除目录下节点失败", 1103140);
        //}
        //closedir(sp);
        return true;
    }
    catch (...) {
        throw Exception("使用ftw删除目录下节点失败", 1103140);
    }
}

bool CFTP_C::FtwCopyTo(const std::string& sorpath, const std::string& despath)
{
    memset(oldpath, 0, sizeof(oldpath));
    memset(newpath, 0, sizeof(newpath));
    memcpy(oldpath, sorpath.c_str(), strlen(sorpath.c_str()));
    memcpy(newpath, despath.c_str(), strlen(despath.c_str()));
    //DIR *sp;
    //if ((sp = opendir(sorpath.c_str())) == 0) //打开copy文件夹，不存在返回0
    //{
    //	throw Exception("使用ftw复制目录下节点失败", 1103141);
    //}
    //if ((sp = opendir(despath.c_str())) == 0) //打开copy文件夹，不存在则创建
    //{
    //	mkdir(despath.c_str(), 0777);
    //}
    //int flag_ftw = 0;
    //bmco_information_f2(Poco::Util::Application::instance().logger(), "%s|%s|调用ftw函数复制文件", std::string("0"), std::string("ftw"));
    //flag_ftw = ftw(sorpath.c_str(), copyto_device, 500);
    //if (flag_ftw != 0)
    //	throw Exception("使用ftw复制目录下节点失败", 1103141);
    //closedir(sp);
    return true;
}

bool CFTP_C::FtwMoveTo(const std::string& sorpath, const std::string& despath)
{
    try {
        FtwCopyTo(sorpath, despath);
        FtwDelete(sorpath);
        return true;
    }
    catch (...) {
        throw Exception("使用ftw转移目录下节点失败", 1103142);
    }
}

