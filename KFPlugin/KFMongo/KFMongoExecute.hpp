#ifndef __KF_MONGO_EXECUTE_H__
#define __KF_MONGO_EXECUTE_H__

#include "KFMongo.hpp"
#include "Poco/MongoDB/InsertRequest.h"
#include "Poco/MongoDB/UpdateRequest.h"
#include "Poco/MongoDB/DeleteRequest.h"
#include "Poco/MongoDB/QueryRequest.h"
#include "Poco/MongoDB/Array.h"
#include "Poco/MongoDB/ObjectId.h"
#include "Poco/MongoDB/Database.h"
#include "KFMongoSelector.h"

namespace KFrame
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMongoExecute : public KFMongo
    {
    public:
        KFMongoExecute() = default;
        virtual ~KFMongoExecute() = default;

        // 初始化
        virtual void InitMongo( const KFMongoSetting* kfsetting );

        // 创建索引
        bool CreateIndex( const std::string& table, const std::string& indexname, const MongoIndexType& values, bool unique, uint32 ttl );

    protected:
        // 添加选择器
        void AddPocoDocument( Document& pocodocument, const KFMongoSelector& kfselector );
        void AddPocoDocument( Document& pocodocument, const KFMongoDocument* kfdocument );

        // 添加数值
        template< class T >
        void AddDocumentValue( Document& pocodocument, const KFMongoExpression< T >* kfexpression )
        {
            if ( kfexpression->_keyword == MongoKeyword::_eq )
            {
                pocodocument.add( kfexpression->_name, kfexpression->_value1 );
            }
            else if ( kfexpression->_keyword == MongoKeyword::_in )
            {
                Poco::MongoDB::Array::Ptr valuearray = new Poco::MongoDB::Array();
                valuearray->add( "0", kfexpression->_value1 );
                valuearray->add( "1", kfexpression->_value2 );

                Poco::MongoDB::Document::Ptr child = new Poco::MongoDB::Document();
                child->add( MongoKeyword::_in, valuearray );
                pocodocument.add( kfexpression->_name, child );
            }
            else
            {
                Poco::MongoDB::Document::Ptr child = new Poco::MongoDB::Document();
                child->add( kfexpression->_keyword, kfexpression->_value1 );
                pocodocument.add( kfexpression->_name, child );
            }
        }

        bool IsIndexCreate( const std::string& table, const std::string& indexname );

    private:
        // 创建的索引列表
        std::unordered_map<std::string, StringSet> _table_index_list;
    };
}

#endif