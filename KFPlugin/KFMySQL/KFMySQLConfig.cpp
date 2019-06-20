#include "KFMySQLConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLList::Reset()
    {
        _mysql_list.clear();
        _kfseting = nullptr;
    }

    void KFMySQLList::AddSetting( KFMySQLSetting& kfsetting )
    {
        _mysql_list.push_back( kfsetting );
    }

    const KFMySQLSetting* KFMySQLList::FindSetting()
    {
        if ( _kfseting == nullptr )
        {
            auto index = KFGlobal::Instance()->_app_id->GetId() % static_cast< uint32 >( _mysql_list.size() );
            _kfseting = &_mysql_list[ index ];
        }

        return _kfseting;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    KFMySQLList* KFMySQLType::FindMySQLList( uint32 type )
    {
        return _mysql_list.Find( type );
    }

    KFMySQLList* KFMySQLType::AddMySQLList( uint32 type )
    {
        return _mysql_list.Create( type );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    KFMySQLType* KFMySQLConfig::FindMySQLType( const std::string& module, uint32 logicid )
    {
        auto key = ModuleKey( module, logicid );
        return _mysql_type.Find( key );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLConfig::LoadConfig( const std::string& file )
    {
        auto _mysql_id = 0u;
        _mysql_type.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();

        auto modulenode = config.FindNode( "Module" );
        while ( modulenode.IsValid() )
        {
            auto name = modulenode.GetString( "Name" );

            auto mysqlnode = modulenode.FindNode( "MySQL" );
            while ( mysqlnode.IsValid() )
            {
                auto minlogicid = mysqlnode.GetUInt32( "MinLogicId" );
                auto maxlogicid = mysqlnode.GetUInt32( "MaxLogicId" );

                for ( auto i = minlogicid; i <= maxlogicid; ++i )
                {
                    auto kfmysqltype = _mysql_type.Create( ModuleKey( name, i ) );
                    kfmysqltype->_id = ++_mysql_id;

                    // write
                    auto writenode = mysqlnode.FindNode( "Write" );
                    while ( writenode.IsValid() )
                    {
                        KFMySQLSetting kfsetting;

                        kfsetting._name = name;
                        kfsetting._type = KFDatabaseEnum::Write;
                        kfsetting._ip = writenode.GetString( "IP" );
                        kfsetting._port = writenode.GetUInt32( "Port" );
                        kfsetting._database = writenode.GetString( "Database" );
                        kfsetting._user = writenode.GetString( "User" );
                        kfsetting._password = writenode.GetString( "Password" );

                        auto kfmysqllist = kfmysqltype->AddMySQLList( KFDatabaseEnum::Write );
                        kfmysqllist->AddSetting( kfsetting );

                        writenode.NextNode( "Write" );
                    }

                    // read
                    auto readnode = mysqlnode.FindNode( "Read" );
                    while ( readnode.IsValid() )
                    {

                        KFMySQLSetting kfsetting;
                        kfsetting._name = name;
                        kfsetting._type = KFDatabaseEnum::Read;
                        kfsetting._ip = readnode.GetString( "IP" );
                        kfsetting._port = readnode.GetUInt32( "Port" );
                        kfsetting._database = readnode.GetString( "Database" );
                        kfsetting._user = readnode.GetString( "User" );
                        kfsetting._password = readnode.GetString( "Password" );

                        auto kfmysqllist = kfmysqltype->AddMySQLList( KFDatabaseEnum::Read );
                        kfmysqllist->AddSetting( kfsetting );

                        readnode.NextNode( "Read" );
                    }
                }

                mysqlnode.NextNode();
            }

            modulenode.NextNode();
        }
    }
}
