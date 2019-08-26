#ifndef __KF_MONGO_SELECTOR_H__
#define __KF_MONGO_SELECTOR_H__

#include "KFrame.h"

namespace KFrame
{
    namespace MongoKeyword
    {
        static std::string _id = "_id";				// mongo 默认的主键字段( 自带索引 )
        static std::string _set = "$set";			// 更新
        static std::string _push = "$addToSet";		// 添加到集合
        static std::string _each = "$each";			// 数组
        static std::string _pull = "$pull";			// 数组
        static std::string _in = "$in";				// 在[x,y]范围内
        static std::string _gt = "$gt";				// 大于
        static std::string _gte = "$gte";			// 大于等于
        static std::string _lt = "$lt";				// 小于
        static std::string _lte = "$lte";			// 小于等于
        static std::string _ne = "$ne";				// 不等于
        static std::string _eq = "";				// 等于
        static std::string _or = "$or";				// 条件或
        static std::string _and = "$and";			// 条件与
        static std::string _inc = "$inc";			// +-操作
        static std::string _mod = "$mod";			// 取模
        static std::string _not = "$not";			// 取反

        static std::string _asc = "1";				// 升序
        static std::string _desc = "-1";			// 降序

        // 有效时间字段
        static std::string _expire = "expire";		// 默认的过期字段
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
        std::string _value1;
        std::string _value2;
    };

    class KFMongoDocument
    {
    public:
        KFMongoDocument()
        {
            _condition = MongoKeyword::_and;
        }
        KFMongoDocument( const std::string& condition )
        {
            _condition = condition;
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
        inline void AddExpression( const std::string& name, const std::string& type, T value )
        {
            auto expression = __KF_NEW__( KFMongoExpression );
            expression->_name = name;
            expression->_type = type;
            expression->_value1 = KFUtility::ToString( value );
            _expressions.push_back( expression );
        }

        template< class T >
        inline void AddExpression( const std::string& name, const std::string& type, T value1, T value2 )
        {
            auto expression = __KF_NEW__( KFMongoExpression );
            expression->_name = name;
            expression->_type = type;
            expression->_value1 = KFUtility::ToString( value1 );
            expression->_value2 = KFUtility::ToString( value2 );
            _expressions.push_back( expression );
        }

        KFMongoDocument* AddDocument( const std::string& condition = MongoKeyword::_and )
        {
            auto document = __KF_NEW__( KFMongoDocument, condition );
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