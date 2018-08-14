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

    bool KFDeployUploadConfig::LoadConfig()
    {
        try
        {
            KFXml kfxml( _file );
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