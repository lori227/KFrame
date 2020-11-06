#include "KFLoggerConfig.hpp"

namespace KFrame
{
    const KFLoggerSetting* KFLoggerConfig::FindSetting( const std::string& name )
    {
        return _settings.Find( name );
    }

    void KFLoggerConfig::LoadConfig( const std::string& file )
    {
        KFXml kfxml( file );
        auto root = kfxml.FindNode( "Config" );
        //////////////////////////////////////////////////////////////////////////
        auto loggersnode = root.FindNode( "Loggers" );
        _default_log_name = loggersnode.ReadString( "Default", true );

        auto node = loggersnode.FindNode( "Logger" );
        while ( node.IsValid() )
        {
            auto name = node.ReadString( "Name", true );
            if ( !name.empty() )
            {
                auto kfsetting = _settings.Create( name );

                kfsetting->_queue_count = node.ReadUInt32( "QueueCount", true );
                kfsetting->_sink_type = node.ReadUInt32( "SinkType", true );
                kfsetting->_step_seconds = node.ReadUInt32( "StepSeconds", true );
                kfsetting->_max_log_size = node.ReadUInt32( "MaxLogSize", true );
                kfsetting->_file_name = node.ReadString( "FileInfo", true );
                kfsetting->_split = node.ReadString( "Split", true );

#if __KF_SYSTEM__ == __KF_WIN__
                kfsetting->_output_path = node.ReadString( "WinPath", true );
                KFUtility::ReplaceString( kfsetting->_output_path, "/", "\\" );
#else
                kfsetting->_output_path = node.ReadString( "LinuxPath", true );
#endif

#ifdef __KF_DEBUG__
                auto modenode = node.FindNode( "Debug" );
#else
                auto modenode = node.FindNode( "Release" );
#endif
                kfsetting->_level = modenode.ReadUInt32( "Level", true );
                kfsetting->_console = modenode.ReadBoolen( "Console", true );
                kfsetting->_pattern = modenode.ReadString( "Pattern", true );
            }

            node.NextNode();
        }
    }
}