#ifndef __KF_MYSQL_EXECUTE_H__
#define __KF_MYSQL_EXECUTE_H__

#include "KFMySQL.h"
#include "Poco/SharedPtr.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/Extraction.h"
#include "Poco/Data/AbstractBinding.h"
#include "KFMySQLInterface.h"

namespace KFrame
{
	using namespace Poco::Data;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	__MT_CLASS__( KFMySQLExecute ), public KFMySQL, public KFMySQLDriver
	{
	public:
		KFMySQLExecute();
		virtual ~KFMySQLExecute();

		// 查询多个字段的值
		virtual bool Select( const std::string& table, const std::string& key, MapString& outvalue );
		virtual bool Select( const std::string& table, const std::string& key, std::list< MapString >& outvalue );
		virtual bool Select( const std::string& table, const std::string& key, ListString& fields, std::list< MapString >& outvalue );

		// 插入记录
		virtual bool Insert( const std::string& table, const MapString& invalue );

		// 删除
		virtual bool Delete( const std::string& table, const std::string& key );
		virtual bool Delete( const std::string& table, const ListString& keys );

		// 更新多个字段
		virtual bool Update( const std::string& table, const std::string& key, const MapString& invalue );

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
	public:
		// 执行
		virtual bool Execute( const char* format, ... );

		// 事务
		virtual void Pipeline( const VectorString& commands );

	protected:
		// 查询
		virtual bool SelectExecute( const std::string& table, const std::string& key, const std::string& field, std::string& outvalue );
		virtual bool SelectExecute( const std::string& table, const std::string& key, const std::string& field, uint32 limitcount, ListString& outvalue );
		
		// 查询
		bool SelectAll( const std::string& table, const std::string& key, MapString& outvalue );
		bool SelectField( const std::string& table, const std::string& key, MapString& outvalue );

		// 更新
		virtual bool UpdateExecute( const std::string& table, const std::string& key, const std::string& field, const std::string& invalue );
		
	protected:
		
		// 转译字符
		char* FormatSlashes( char* buffer, uint32 length );

		// 执行操作
		bool ExecuteSql( const char* sql );
		bool ExecuteSql( Statement& statement );
		
		// 判断是否断线
		bool CheckDisconnected( int32 code );

	private:
		// 逻辑buffer
		char* _buffer;

		// 长度
		uint32 _length;
	};
}

#endif