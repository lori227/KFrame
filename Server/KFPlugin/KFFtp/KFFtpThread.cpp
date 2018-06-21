#include "KFFtpThread.h"
#include "KFFtpConfig.h"
#include "KFTime/KFDate.h"
#include "KFThread/KFThread.h"
#include "KFConvert/KFConvert.h"

namespace KFrame
{
	KFFtpThread::KFFtpThread()
	{
		_ftp_id = 0;
		_ftp_result = KFFtpEnum::Idle;
	}

	void KFFtpThread::StartThread( uint32 id )
	{
		if ( _ftp_result != KFFtpEnum::Idle )
		{
			return;
		}

		_ftp_id = id;
		_ftp_result = KFFtpEnum::Login;		
		KFThread::CreateThread( this, &KFFtpThread::Run, __FUNCTION_LINE__ );
	}

	void KFFtpThread::FinishThread()
	{
		_ftp_result = KFFtpEnum::Idle;
	}

	bool KFFtpThread::IsRun()
	{
		return _ftp_result != KFFtpEnum::Idle;
	}

	bool KFFtpThread::IsFinish()
	{
		return _ftp_result == KFFtpEnum::Failed || _ftp_result == KFFtpEnum::Finish;
	}

	uint32 KFFtpThread::FtpResult()
	{
		return _ftp_result;
	}

	void KFFtpThread::Run()
	{
		auto kfsetting = _kf_ftp_config->FindFtpSetting( _ftp_id );
		if ( kfsetting == nullptr )
		{
			_ftp_result = KFFtpEnum::Failed;
			return;
		}


		nsFTP::CFTPClient ftpclient;
		nsFTP::CLogonInfo logonInfo( kfsetting->_address, kfsetting->_port, kfsetting->_user, kfsetting->_password );
		if ( !ftpclient.Login( logonInfo ) )
		{
			_ftp_result = KFFtpEnum::Failed;
			return;
		}

		_ftp_result = KFFtpEnum::Download;

		// 更新文件
		std::string ftppath = "/" + kfsetting->_ftp_path;
		std::string localpath = kfsetting->_local_path;
		DownloadFiles( &ftpclient, ftppath, localpath );

		ftpclient.Logout();
		_ftp_result = KFFtpEnum::Finish;
	}

	void KFFtpThread::CreateLocalDirectory( const char* path )
	{
#if __KF_SYSTEM__ == __KF_WIN__
		CreateDirectoryA( path, nullptr );
#endif


#if __KF_SYSTEM__ == __KF_LINUX__
		mkdir( directory, 777 );
#endif
	}


	void KFFtpThread::DeleteLocalFile( const char* file )
	{
#if __KF_SYSTEM__ == __KF_WIN__
		DeleteFileA( file );
#endif

#if __KF_SYSTEM__ == __KF_LINUX__
		remove( file );
#endif
	}

	void KFFtpThread::RenameFile( const char* oldfile, const char* newfile )
	{
#if __KF_SYSTEM__ == __KF_WIN__
		rename( oldfile, newfile );
#endif

#if __KF_SYSTEM__ == __KF_LINUX__
		rename( oldfile, newfile );
#endif
	}

	bool KFFtpThread::CheckFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string &localfile )
	{
#if __KF_SYSTEM__ == __KF_WIN__
		return CheckWinFileModifyTime( ftpclient, file, localfile );
#endif

#if __KF_SYSTEM__ == __KF_LINUX__
		return CheckLinuxFileModifyTime( ftpclient, file, localfile );
#endif
	}

	void KFFtpThread::DownloadFiles( nsFTP::CFTPClient* ftpclient, std::string& ftppath, std::string &localpath )
	{
		// 创建文件夹
		CreateLocalDirectory( localpath.c_str() );
	
		// 遍历ftp上的文件
		nsFTP::TFTPFileStatusShPtrVec filelist;
		ftpclient->List( ftppath, filelist, true );
		for ( auto iter = filelist.begin(); iter != filelist.end(); ++iter )
		{
			auto file = (*iter);

			auto asciifile = KFConvert::ToAscii( file->Name() );

			std::string childftppath = ftppath + "/" + file->Name();
			std::string childlocalpath = localpath + "/" + asciifile;

			if ( file->IsCwdPossible() )
			{
				DownloadFiles( ftpclient, childftppath, childlocalpath );
			}
			else
			{
				DownloadFile( ftpclient, file, childftppath, childlocalpath, asciifile );
			}
		}
	}

	void KFFtpThread::DownloadFile( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& ftpfile, std::string &localfile, std::string& asciifile )
	{
		if ( CheckFileModifyTime( ftpclient, file, localfile ) )
		{
			return;
		}

		std::string downloadfile = localfile + ".new";
		if ( !ftpclient->DownloadFile( ftpfile, downloadfile ) )
		{
			DeleteLocalFile( downloadfile.c_str() );
			return KFLogger::LogLogic( KFLogger::Error, "DownLoad [ %s ] Failed!", localfile.c_str() );
		}

		DeleteLocalFile( localfile.c_str() );
		RenameFile( downloadfile.c_str(), localfile.c_str() );

		KFLogger::LogLogic( KFLogger::Info, "DownLoad [ %s ] Ok!", localfile.c_str() );
	}

#if __KF_SYSTEM__ == __KF_WIN__

	bool KFFtpThread::CheckWinFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string &localfile )
	{
		HANDLE hFile = CreateFileA( localfile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			return false;
		}

		FILETIME ftCreate, ftModify, ftAccess;
		GetFileTime( hFile, &ftCreate, &ftAccess, &ftModify );
		CloseHandle( hFile );

		FILETIME stLocal;
		FileTimeToLocalFileTime( &ftModify, &stLocal );

		SYSTEMTIME systemtime;
		FileTimeToSystemTime( &stLocal, &systemtime );

		KFDate localtime( systemtime.wYear, systemtime.wMonth, systemtime.wDay, systemtime.wHour, systemtime.wMinute, systemtime.wSecond );
		auto _localtime = localtime.GetTime();

		tm _tm;
		gmtime_s( &_tm, &file->MTime() );
		KFDate filetime( _tm.tm_year + TimeEnum::SinceYear, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec );
		auto _ftptime = filetime.GetTime();

		return _localtime >= _ftptime;
	}
#endif


#if __KF_SYSTEM__ == __KF_LINUX__

	bool KFFtpThread::CheckLinuxFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string &localfile )
	{
		auto fp = fopen( localfile.c_str(), "r" );
		if ( fp == nullptr )
		{
			return false;
		}

		auto fd = fileno( fp );

		struct stat buf;
		fstat( fd, &buf );
		fclose( fp );
		auto localtime = buf.st_mtime;

		auto _tm = gmtime( &file->MTime() );
		KFRealTime filetime( _tm->tm_year + 1900, _tm->tm_mon + 1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec );
		auto ftptime = filetime.GetTime();

		return localtime >= ftptime;
	}
#endif
}