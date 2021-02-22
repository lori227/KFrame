#include "KFLoggerConfig.hpp"

namespace KFrame
{
    std::shared_ptr<const KFLoggerSetting> KFLoggerConfig::FindSetting( const std::string& name )
    {
        return _settings.Find( name );
    }

    void KFLoggerConfig::LoadConfig( const std::string& file )
    {
        KFXml xml( file );
        auto root = xml.FindNode( "Config" );
        //////////////////////////////////////////////////////////////////////////
        auto loggers_node = root.FindNode( "Loggers" );
        _default_log_name = loggers_node.ReadString( "Default", true );

        auto node = loggers_node.FindNode( "Logger" );
        while ( node.IsValid() )
        {
            auto name = node.ReadString( "Name", true );
            if ( !name.empty() )
            {
                auto setting = _settings.Create( name );

                setting->_queue_count = node.ReadUInt32( "QueueCount", true );
                setting->_sink_type = node.ReadUInt32( "SinkType", true );
                setting->_step_seconds = node.ReadUInt32( "StepSeconds", true );
                setting->_max_log_size = node.ReadUInt32( "MaxLogSize", true );
                setting->_file_name = node.ReadString( "FileInfo", true );
                setting->_split = node.ReadString( "Split", true );

#if __KF_SYSTEM__ == __KF_WIN__
                setting->_output_path = node.ReadString( "WinPath", true );
                KFUtility::ReplaceString( setting->_output_path, "/", "\\" );
#else
                setting->_output_path = node.ReadString( "LinuxPath", true );
#endif

#ifdef __KF_DEBUG__
                auto mode_node = node.FindNode( "Debug" );
#else
                auto mode_node = node.FindNode( "Release" );
#endif
                setting->_level = mode_node.ReadUInt32( "Level", true );
                setting->_console = mode_node.ReadBool( "Console", true );
                setting->_pattern = mode_node.ReadString( "Pattern", true );
            }

            node.NextNode();
        }
    }
}