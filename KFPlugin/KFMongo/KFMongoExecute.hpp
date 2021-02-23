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
#include "Poco/MongoDB/Binary.h"
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
        virtual void InitMongo( const KFMongoConnectOption* connect_option, const KFMongoConnnectData* connect_data );

        // 创建索引
        bool CreateIndex( const std::string& table, const std::string& index_name, const MongoIndexType& values, bool unique, uint32 ttl );

    protected:
        // 添加选择器
        void AddPocoDocument( Document& poco_document, const KFMongoSelector& selector_data );
        void AddPocoDocument( Document& poco_document, const KFMongoDocument* mongo_document );

        // 添加数值
        template<class T>
        void AddDocumentValue( Document& poco_document, const KFMongoExpression<T>* expression )
        {
            if ( expression->_keyword == MongoKeyword::_eq )
            {
                poco_document.add( expression->_name, expression->_value1 );
            }
            else if ( expression->_keyword == MongoKeyword::_in )
            {
                Poco::MongoDB::Array::Ptr array_value = new Poco::MongoDB::Array();
                array_value->add( "0", expression->_value1 );
                array_value->add( "1", expression->_value2 );

                Poco::MongoDB::Document::Ptr child = new Poco::MongoDB::Document();
                child->add( MongoKeyword::_in, array_value );
                poco_document.add( expression->_name, child );
            }
            else
            {
                Poco::MongoDB::Document::Ptr child = new Poco::MongoDB::Document();
                child->add( expression->_keyword, expression->_value1 );
                poco_document.add( expression->_name, child );
            }
        }

        bool IsIndexCreate( const std::string& table, const std::string& index_name );

    private:
        // 创建的索引列表
        std::unordered_map<std::string, StringSet> _table_index_list;
    };
}

#endif