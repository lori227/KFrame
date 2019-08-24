#ifndef __KF_MONGO_SELECTOR_H__
#define __KF_MONGO_SELECTOR_H__

#include "KFrame.h"

namespace KFrame
{
    namespace MongoKeyword
    {
        static std::string _id = "_id";
        static std::string _set = "$set";
        static std::string _push = "$addToSet";
        static std::string _each = "$each";
        static std::string _pull = "$pull";
        static std::string _in = "$in";
        static std::string _gt = "$gt";
        static std::string _gte = "$gte";
        static std::string _lt = "$lt";
        static std::string _lte = "$lte";
        static std::string _ne = "$ne";
        static std::string _eq = "";
        static std::string _or = "$or";
        static std::string _and = "$and";
        static std::string _inc = "$inc";
        static std::string _mod = "$mod";
        static std::string _not = "$not";


        // 有效时间字段
        static std::string _expire = "expire";
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    class KFMongoExpression
    {
    public:
        // 字段名
        std::string _name;

        // 判断符号
        std::string _type;

        // 数值
        std::string _min_value;
        std::string _max_value;
    };

    class KFMongoDocument
    {
    public:
        KFMongoDocument()
        {
            _condition = MongoKeyword::_and;
        }
        ~KFMongoDocument()
        {
            for ( auto expression : _expressions )
            {
                __KF_DELETE__( KFMongoExpression, expression );
            }
            _expressions.clear();

            for ( auto document : _documents )
            {
                __KF_DELETE__( KFMongoDocument, document );
            }
            _documents.clear();
        }

        template< class T >
        void AddExpression( const std::string& name, const std::string& type, T value )
        {
            auto expression = __KF_NEW__( KFMongoExpression );
            expression->_name = name;
            expression->_type = type;
            expression->_min_value = __TO_STRING__( value );
            _expressions.push_back( expression );
        }

        template< class T >
        void AddExpression( const std::string& name, const std::string& type, T value1, T value2 )
        {
            auto expression = __KF_NEW__( KFMongoExpression );
            expression->_name = name;
            expression->_type = type;
            expression->_min_value = __TO_STRING__( value1 );
            expression->_max_value = __TO_STRING__( value2 );
            _expressions.push_back( expression );
        }

        template<>
        void AddExpression( const std::string& name, const std::string& type, const std::string& value )
        {
            auto expression = __KF_NEW__( KFMongoExpression );
            expression->_name = name;
            expression->_type = type;
            expression->_min_value = value;
            _expressions.push_back( expression );
        }

        template<>
        void AddExpression( const std::string& name, const std::string& type, const std::string& value1, const std::string& value2 )
        {
            auto expression = __KF_NEW__( KFMongoExpression );
            expression->_name = name;
            expression->_type = type;
            expression->_min_value = value1;
            expression->_max_value = value2;
            _expressions.push_back( expression );
        }

        KFMongoDocument* AddDocument()
        {
            auto document = __KF_NEW__( KFMongoDocument );
            _documents.push_back( document );
            return document;
        }

    public:
        // 条件
        std::string _condition;

        // 表达式列表
        std::vector< KFMongoExpression* > _expressions;

        // 包含的条件列表
        std::vector< KFMongoDocument* > _documents;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 选择器
    class KFMongoSelector
    {
    public:
        // key
        uint64 _key = 0u;

        // 条件列表
        KFMongoDocument _document;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif