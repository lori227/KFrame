#include "KFDeployUploadConfig.h"

namespace KFrame
{


    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFDeployUploadConfig::KFDeployUploadConfig()
    {
        _ftp_id = 1;
    }

    KFDeployUploadConfig::~KFDeployUploadConfig()
    {

    }

    bool KFDeployUploadConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto root = kfxml.FindNode( "Setting" );
            _ftp_id = root.GetUInt32( "FtpId" );
            /////////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}