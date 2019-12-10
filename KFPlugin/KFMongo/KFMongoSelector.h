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
        static std::string _count = "count";		// 查询纪录数量


        static std::string _expire = "expire";		// 默认的过期字段

        static int32 _asc = 1;						// 升序
        static int32 _desc = -1;					// 降序
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::map<std::string, int32> MongoIndexType;
    //////////////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    class KFMongoExpression
    {
    public:
        // 字段名
        std::string _name;

        // 字段关键字
        std::string _keyword;

        // 数值
        T _value1;
        T _value2;
    };
    //////////////////////////////////////////////////////////////////////////////////////////////
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
            for ( auto expression : _int_expressions )
            {
                __KF_DELETE__( KFMongoExpression<uint64>, expression );
            }
            _int_expressions.clear();

            for ( auto expression : _str_expressions )
            {
                __KF_DELETE__( KFMongoExpression<std::string>, expression );
            }
            _str_expressions.clear();

            for ( auto document : _documents )
            {
                __KF_DELETE__( KFMongoDocument, document );
            }
            _documents.clear();
        }

        inline void AddExpression( const std::string& name, const std::string& keyword, uint64 value )
        {
            auto expression = __KF_NEW__( KFMongoExpression<uint64> );
            expression->_name = name;
            expression->_keyword = keyword;
            expression->_value1 = value;
            _int_expressions.push_back( expression );
        }

        inline void AddExpression( const std::string& name, const std::string& keyword, uint64 value1, uint64 value2 )
        {
            auto expression = __KF_NEW__( KFMongoExpression<uint64> );
            expression->_name = name;
            expression->_keyword = keyword;
            expression->_value1 = value1;
            expression->_value2 = value2;
            _int_expressions.push_back( expression );
        }

        inline void AddExpression( const std::string& name, const std::string& keyword, const std::string& value )
        {
            auto expression = __KF_NEW__( KFMongoExpression<std::string> );
            expression->_name = name;
            expression->_keyword = keyword;
            expression->_value1 = value;
            _str_expressions.push_back( expression );
        }

        inline void AddExpression( const std::string& name, const std::string& keyword, const std::string& value1, const std::string& value2 )
        {
            auto expression = __KF_NEW__( KFMongoExpression<std::string> );
            expression->_name = name;
            expression->_keyword = keyword;
            expression->_value1 = value1;
            expression->_value2 = value2;
            _str_expressions.push_back( expression );
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
        std::list< KFMongoExpression< uint64 >* > _int_expressions;
        std::list< KFMongoExpression< std::string >* > _str_expressions;

        // 包含的条件列表
        std::list< KFMongoDocument* > _documents;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 选择器
    class KFMongoSelector
    {
    public:
        void AddReturn( const std::string& value, int32 ordertype = MongoKeyword::_asc )
        {
            _returns[ value] = ordertype;
        }

        void AddLimitReturn( const std::string& value )
        {
            _limit_returns.insert( value );
        }

    public:
        // key
        uint64 _key = 0u;

        // 返回数量限制
        uint32 _limit_count = 0u;

        // 条件列表
        KFMongoDocument _document;

        // 返回的字段
        std::map< std::string, int32 > _returns;

        // 不返回的字段
        std::set<std::string> _limit_returns;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif