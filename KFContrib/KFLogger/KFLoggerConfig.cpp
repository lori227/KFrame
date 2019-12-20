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
        _default_log_name = loggersnode.GetString( "Default", true );

        auto node = loggersnode.FindNode( "Logger" );
        while ( node.IsValid() )
        {
            auto name = node.GetString( "Name", true );
            if ( !name.empty() )
            {
                auto kfsetting = _settings.Create( name );

#if __KF_SYSTEM__ == __KF_WIN__
                kfsetting->_output_path = node.GetString( "WinPath", true );
                KFUtility::ReplaceString( kfsetting->_output_path, "/", "\\" );
#else
                kfsetting->_output_path = node.GetString( "LinuxPath", true );
#endif


#ifdef __KF_DEBUG__
                kfsetting->_pattern = node.GetString( "DebugPattern", true );
#else
                kfsetting->_pattern = node.GetString( "ReleasePattern", true );
#endif

                kfsetting->_level = node.GetBoolen( "Level", true );
                kfsetting->_console = node.GetBoolen( "Console", true );
                kfsetting->_queue_count = node.GetUInt32( "QueueCount", true );
                kfsetting->_sink_type = node.GetUInt32( "SinkType", true );
                kfsetting->_step_seconds = node.GetUInt32( "StepSeconds", true );
                kfsetting->_max_log_size = node.GetUInt32( "MaxLogSize", true );
            }

            node.NextNode();
        }
    }
}