#include "KFKernelConfig.h"

namespace KFrame
{
    KFKernelConfig::KFKernelConfig()
    {
        _class_file = "class.config";
        _global_class_name = "Global";
        _delay_save_time = 60000;
    }

    KFKernelConfig::~KFKernelConfig()
    {

    }


    bool KFKernelConfig::IsMultipleData( const std::string& dataname ) const
    {
        return _multiple_data.find( dataname ) != _multiple_data.end();
    }

    bool KFKernelConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            /////////////////////////////////////////////////////
            auto kernel = config.FindNode( "Kernel" );
            _class_file = kernel.GetString( "ClassFile" );
            _global_class_name = kernel.GetString( "GlobalClassName" );
            _delay_save_time = kernel.GetUInt32( "SelaySaveTime" );

            /////////////////////////////////////////////////////
            {
                auto multiplenode = config.FindNode( "Multiple" );
                auto datanode = multiplenode.FindNode( "Data" );
                while ( datanode.IsValid() )
                {
                    auto name = datanode.GetString( "Name" );
                    _multiple_data.insert( name );
                    datanode.NextNode();
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