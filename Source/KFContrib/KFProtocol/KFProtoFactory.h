#ifndef __KF_PROTO_FACTORY_H__
#define __KF_PROTO_FACTORY_H__

#include "KFDefine.h"
#include "google/protobuf/compiler/importer.h"
#include "google/protobuf/dynamic_message.h"

namespace KFrame
{
	class KFProtoFactory
	{
	public:
		~KFProtoFactory();

		// 单间接口
		static KFProtoFactory* Instance();

		// 加载Proto文件
		void ImportPath( const std::string& path );
		bool LoadFile( const std::string& file );


		// 创建消息
		google::protobuf::Message* CreateMessage( const std::string& name );

		// 解析消息
		google::protobuf::Message* ParserMessage( const std::string& name, const char* data, uint32 length );
		
		// 获得值
		template< class T >
		T GetValue( google::protobuf::Message* message, const std::string& name )
		{		
			auto filed = message->GetDescriptor()->FindFieldByName( name );
			if ( filed == nullptr )
			{
				return ReturnValue< T >();
			}

			return GetValue< T >( message, filed );
		}

		// 设置值
		template< class T >
		void SetValue( google::protobuf::Message* message, const std::string& name, T value )
		{
			auto filed = message->GetDescriptor()->FindFieldByName( name );
			if ( filed == nullptr )
			{
				return;
			}

			SetValue< T >( message, filed, value );
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 获得Repeated字段数量
		int32 GetRepeatedCount( google::protobuf::Message* message, const std::string& name );

		// 获得Repeated数值
		template< class T >
		T GetRepeatedValue( google::protobuf::Message* message, const std::string& name, int32 index )
		{
			auto filed = message->GetDescriptor()->FindFieldByName( name );
			if ( filed == nullptr )
			{
				return ReturnValue< T >();
			}

			return GetRepeatedValue< T >( message, filed, index );
		}

		// 添加Repeated数值
		template< class T >
		void AddRepeatedValue( google::protobuf::Message* message, const std::string& name, T value )
		{
			auto filed = message->GetDescriptor()->FindFieldByName( name );
			if ( filed == nullptr )
			{
				return;
			}

			return AddRepeatedValue< T >( message, filed, value );
		}

		// 设置Repeared数值
		template< class T >
		void SetRepeatedValue( google::protobuf::Message* message, const std::string& name, int32 index, T value )
		{
			auto filed = message->GetDescriptor()->FindFieldByName( name );
			if ( filed == nullptr )
			{
				return;
			}

			return SetRepeatedValue< T >( message, filed, index, value );
		}

		template< class T >
		T ReturnValue()
		{
			static T _t;
			return _t;
		}

	protected:
		KFProtoFactory();
		
		// 获得值
		template< class T >
		T GetValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed );

		// 设置值
		template< class T >
		void SetValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, T value );

		// 获得Repreaed数值
		template< class T >
		T GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index );

		// 添加Repreaed数值
		template< class T >
		void AddRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, T value );

		// 设置Repreaed数值
		template< class T >
		void SetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, T value );

	private:
		// 动态编译器
		google::protobuf::compiler::Importer* _importer;
		google::protobuf::DynamicMessageFactory* _factory;
	};
		
	template<>
	inline const std::string& KFProtoFactory::ReturnValue<>()
	{
		static std::string _default = "";
		return _default;
	}
}


#endif
