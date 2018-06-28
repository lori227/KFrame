#include "KFFtpConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFFtpConfig::KFFtpConfig()
	{
	}

	KFFtpConfig::~KFFtpConfig()
	{
		
	}

	const KFFtpSetting* KFFtpConfig::FindFtpSetting( uint32 id )
	{
		return _kf_ftp_setting.Find( id );
	}
	
	/////////////////////////////////////////////////////////////////////////////
	bool KFFtpConfig::LoadConfig( const char* file )
	{
		_kf_ftp_setting.Clear();

		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			//////////////////////////////////////////////////////////////////
			auto ftpsnode = config.FindNode( "Ftps" );
			if ( ftpsnode.IsValid() )
			{
				auto ftpnode = ftpsnode.FindNode( "Ftp" );
				while ( ftpnode.IsValid() )
				{
					auto kfsetting = __KF_CREATE__( KFFtpSetting );

					kfsetting->_id = ftpnode.GetUInt32( "Id" );
					kfsetting->_address = ftpnode.GetString( "Address" );
					kfsetting->_port = ftpnode.GetUInt32( "Port" );
					kfsetting->_user = ftpnode.GetString( "User" );
					kfsetting->_password = ftpnode.GetString( "Password" );
					kfsetting->_ftp_path = ftpnode.GetString( "FtpPath" );
					kfsetting->_local_path = ftpnode.GetString( "LocalPath" );
					_kf_ftp_setting.Insert( kfsetting->_id, kfsetting );

					ftpnode.NextNode();
				}
			}
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}