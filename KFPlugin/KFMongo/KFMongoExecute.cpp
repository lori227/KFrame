#include "KFMongoExecute.hpp"
#include "Poco/MongoDB/InsertRequest.h"
#include "Poco/MongoDB/UpdateRequest.h"
#include "Poco/MongoDB/Database.h"

namespace KFrame
{
    void KFMongoExecute::InitMongo( const KFMongoSetting* kfsetting )
    {
        _database = kfsetting->_database;
        return KFMongo::InitMongo( kfsetting );
    }

    /////////////////////////////////////////////////////////////////////////////
    bool KFWriteExecute::Insert( const std::string& table, const std::string& key, const MapString& invalue )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        InsertRequest request( fullname );
        auto& documents = request.addNewDocument();
        documents.add( "_id", key );

        for ( auto& iter : invalue )
        {
            documents.add( iter.first, iter.second );
        }

        return SendRequest( request );
    }
    /////////////////////////////////////////////////////////////////////////////
    bool KFWriteExecute::Update( const std::string& table, const std::string& key, const MapString& invalue )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, UpdateRequest::UPDATE_DEFAULT );
        auto& selector = request.selector();
        selector.add( "_id", key );

        auto& updates = request.update();
        for ( auto& iter : invalue )
        {
            updates.add( iter.first, iter.second );
        }

        //Database db( "team" );
        //Poco::SharedPtr<Poco::MongoDB::QueryRequest> request = _connection( "$cmd" );

        return SendRequest( request );
    }
}
