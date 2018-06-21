#include "KFProtoFactory.h"


namespace KFrame
{
	KFProtoFactory::KFProtoFactory()
	{
		_importer = nullptr;
		_factory = new google::protobuf::DynamicMessageFactory();
	}

	KFProtoFactory::~KFProtoFactory()
	{
		if ( _importer != nullptr )
		{
			delete _importer;
			_importer = nullptr;
		}

		delete _factory;
		_factory = nullptr;
	}

	KFProtoFactory* KFProtoFactory::Instance()
	{
		static KFProtoFactory _proto_factory;
		return &_proto_factory;
	}

	void KFProtoFactory::ImportPath( const std::string& path )
	{
		// 将当前路径映射为项目根目录
		google::protobuf::compiler::DiskSourceTree sourcetree;

		sourcetree.MapPath( "", path.c_str() );

		if ( _importer != nullptr )
		{
			delete _importer;
			_importer = nullptr;
		}

		_importer = new google::protobuf::compiler::Importer( &sourcetree, nullptr );
	}
	
	bool KFProtoFactory::LoadFile( const std::string& file )
	{
		// 配置动态编译器
		auto filedescriptor = _importer->Import( file );
		return filedescriptor != nullptr;
	}

	google::protobuf::Message* KFProtoFactory::CreateMessage( const std::string& name )
	{
		// 提取类型的描述信息
		auto descriptor = _importer->pool()->FindMessageTypeByName( name );
		if ( descriptor == nullptr )
		{
			return nullptr;
		}

		auto proto = _factory->GetPrototype( descriptor );
		if ( proto == nullptr )
		{
			return nullptr;
		}

		auto message = proto->New();
		return message;
	}

	google::protobuf::Message* KFProtoFactory::ParserMessage( const std::string& name, const char* data, uint32 length )
	{
		auto message = CreateMessage( name );
		message->ParseFromArray( data, length );
		return message;
	}
	
	template<>
	uint32 KFProtoFactory::GetValue<>( google::protobuf::Message* message,  const google::protobuf::FieldDescriptor* filed )
	{
		auto reflection = message->GetReflection();
		return reflection->GetUInt32( *message, filed );
	}

	template<>
	int32 KFProtoFactory::GetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed )
	{
		auto reflection = message->GetReflection();
		return reflection->GetInt32( *message, filed );
	}

	template<>
	uint64 KFProtoFactory::GetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed )
	{
		auto reflection = message->GetReflection();
		return reflection->GetUInt64( *message, filed );
	}

	template<>
	int64 KFProtoFactory::GetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed )
	{
		auto reflection = message->GetReflection();
		return reflection->GetInt64( *message, filed );
	}

	template<>
	double KFProtoFactory::GetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed )
	{
		auto reflection = message->GetReflection();
		return reflection->GetDouble( *message, filed );
	}

	template<>
	float KFProtoFactory::GetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed )
	{
		auto reflection = message->GetReflection();
		return reflection->GetFloat( *message, filed );
	}

	template<>
	const std::string& KFProtoFactory::GetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed )
	{
		static std::string _defaule_string = "";

		auto reflection = message->GetReflection();
		return reflection->GetStringReference( *message, filed, &_defaule_string );
	}

	template<>
	google::protobuf::Message* KFProtoFactory::GetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed )
	{
		auto newmessage = CreateMessage( filed->message_type()->full_name() );

		auto reflection = message->GetReflection();
		newmessage->CopyFrom( reflection->GetMessage( *message, filed ) );

		return newmessage;
	}

	template<>
	void KFProtoFactory::SetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, uint32 value )
	{
		auto reflection = message->GetReflection();
		reflection->SetUInt32( message, filed, value );
	}

	template<>
	void KFProtoFactory::SetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 value )
	{
		auto reflection = message->GetReflection();
		reflection->SetInt32( message, filed, value );
	}

	template<>
	void KFProtoFactory::SetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, uint64 value )
	{
		auto reflection = message->GetReflection();
		reflection->SetUInt64( message, filed, value );
	}

	template<>
	void KFProtoFactory::SetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int64 value )
	{
		auto reflection = message->GetReflection();
		reflection->SetInt64( message, filed, value );
	}

	template<>
	void KFProtoFactory::SetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, float value )
	{
		auto reflection = message->GetReflection();
		reflection->SetFloat( message, filed, value );
	}

	template<>
	void KFProtoFactory::SetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, double value )
	{
		auto reflection = message->GetReflection();
		reflection->SetDouble( message, filed, value );
	}

	template<>
	void KFProtoFactory::SetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, const std::string& value )
	{
		auto reflection = message->GetReflection();
		reflection->SetString( message, filed, value );
	}

	template<>
	void KFProtoFactory::SetValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, google::protobuf::Message* value )
	{
		auto reflection = message->GetReflection();
		auto copymessage = reflection->MutableMessage( message, filed );
		copymessage->CopyFrom( *value );
	}

	int32 KFProtoFactory::GetRepeatedCount( google::protobuf::Message* message, const std::string& name )
	{
		auto filed = message->GetDescriptor()->FindFieldByName( name );
		if ( filed == nullptr )
		{
			return 0;
		}

		auto reflection = message->GetReflection();
		switch ( filed->type() )
		{
		case google::protobuf::FieldDescriptor::TYPE_UINT32:
		{
			auto& repeatedfiled = reflection->GetRepeatedField< google::protobuf::uint32 >( *message, filed );
			return repeatedfiled.size();
		}
		case google::protobuf::FieldDescriptor::TYPE_INT32:
		{
			auto& repeatedfiled = reflection->GetRepeatedField< int32 >( *message, filed );
			return repeatedfiled.size();
		}
		case google::protobuf::FieldDescriptor::TYPE_UINT64:
		{
			auto& repeatedfiled = reflection->GetRepeatedField< uint64 >( *message, filed );
			return repeatedfiled.size();
		}
		case google::protobuf::FieldDescriptor::TYPE_INT64:
		{
			auto& repeatedfiled = reflection->GetRepeatedField< int64 >( *message, filed );
			return repeatedfiled.size();
		}
		case google::protobuf::FieldDescriptor::TYPE_FLOAT:
		{
			auto& repeatedfiled = reflection->GetRepeatedField< float >( *message, filed );
			return repeatedfiled.size();
		}
		case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
		{
			auto& repeatedfiled = reflection->GetRepeatedField< double >( *message, filed );
			return repeatedfiled.size();
		}
		case google::protobuf::FieldDescriptor::TYPE_STRING:
		{
			auto& repeatedfiled = reflection->GetRepeatedPtrField< std::string >( *message, filed );
			return repeatedfiled.size();
		}
		case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
		{
			auto& repeatedfiled = reflection->GetRepeatedPtrField< google::protobuf::Message >( *message, filed );
			return repeatedfiled.size();
		}
		default:
			break;
		}

		return 0;
	}

	template<>
	void KFProtoFactory::AddRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, uint32 value )
	{
		auto reflection = message->GetReflection();
		reflection->AddUInt32( message, filed, value );
	}

	template<>
	void KFProtoFactory::AddRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 value )
	{
		auto reflection = message->GetReflection();
		reflection->AddInt32( message, filed, value );
	}

	template<>
	void KFProtoFactory::AddRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, uint64 value )
	{
		auto reflection = message->GetReflection();
		reflection->AddUInt64( message, filed, value );
	}

	template<>
	void KFProtoFactory::AddRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int64 value )
	{
		auto reflection = message->GetReflection();
		reflection->AddInt64( message, filed, value );
	}

	template<>
	void KFProtoFactory::AddRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, float value )
	{
		auto reflection = message->GetReflection();
		reflection->AddFloat( message, filed, value );
	}

	template<>
	void KFProtoFactory::AddRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, double value )
	{
		auto reflection = message->GetReflection();
		reflection->AddDouble( message, filed, value );
	}

	template<>
	void KFProtoFactory::AddRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, const std::string& value )
	{
		auto reflection = message->GetReflection();
		reflection->AddString( message, filed, value );
	}

	template<>
	void KFProtoFactory::AddRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, google::protobuf::Message* value )
	{
		auto reflection = message->GetReflection();
		auto submessage = reflection->AddMessage( message, filed );
		submessage->CopyFrom( *value );
	}

	template<>
	void KFProtoFactory::SetRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, uint32 value )
	{
		auto reflection = message->GetReflection();
		reflection->SetRepeatedUInt32( message, filed, index, value );
	}

	template<>
	void KFProtoFactory::SetRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, int32 value )
	{
		auto reflection = message->GetReflection();
		reflection->SetRepeatedInt32( message, filed, index, value );
	}

	template<>
	void KFProtoFactory::SetRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, uint64 value )
	{
		auto reflection = message->GetReflection();
		reflection->SetRepeatedUInt64( message, filed, index, value );
	}

	template<>
	void KFProtoFactory::SetRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, int64 value )
	{
		auto reflection = message->GetReflection();
		reflection->SetRepeatedInt64( message, filed, index, value );
	}

	template<>
	void KFProtoFactory::SetRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, float value )
	{
		auto reflection = message->GetReflection();
		reflection->SetRepeatedFloat( message, filed, index, value );
	}

	template<>
	void KFProtoFactory::SetRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, double value )
	{
		auto reflection = message->GetReflection();
		reflection->SetRepeatedDouble( message, filed, index, value );
	}

	template<>
	void KFProtoFactory::SetRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, const std::string& value )
	{
		auto reflection = message->GetReflection();
		reflection->SetRepeatedString( message, filed, index, value );
	}

	template<>
	void KFProtoFactory::SetRepeatedValue<>( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index, google::protobuf::Message* value )
	{
		auto reflection = message->GetReflection();
		auto submessge = reflection->MutableRepeatedMessage( message, filed, index );
		submessge->CopyFrom( *value );
	}

	template<>
	uint32 KFProtoFactory::GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index )
	{
		auto reflection = message->GetReflection();
		return reflection->GetRepeatedUInt32( *message, filed, index );
	}

	template<>
	int32 KFProtoFactory::GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index )
	{
		auto reflection = message->GetReflection();
		return reflection->GetRepeatedInt32( *message, filed, index );
	}

	template<>
	uint64 KFProtoFactory::GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index )
	{
		auto reflection = message->GetReflection();
		return reflection->GetRepeatedUInt64( *message, filed, index );
	}

	template<>
	int64 KFProtoFactory::GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index )
	{
		auto reflection = message->GetReflection();
		return reflection->GetRepeatedInt64( *message, filed, index );
	}

	template<>
	float KFProtoFactory::GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index )
	{
		auto reflection = message->GetReflection();
		return reflection->GetRepeatedFloat( *message, filed, index );
	}

	template<>
	double KFProtoFactory::GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index )
	{
		auto reflection = message->GetReflection();
		return reflection->GetRepeatedDouble( *message, filed, index );
	}

	template<>
	const std::string& KFProtoFactory::GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index )
	{
		static std::string _defaule_string = "";
		auto reflection = message->GetReflection();
		return reflection->GetRepeatedStringReference( *message, filed, index, &_defaule_string );
	}

	template<>
	google::protobuf::Message* KFProtoFactory::GetRepeatedValue( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* filed, int32 index )
	{
		auto reflection = message->GetReflection();
		auto& submessage = reflection->GetRepeatedMessage( *message, filed, index );

		auto newmessage = CreateMessage( filed->message_type()->full_name() );
		newmessage->CopyFrom( submessage );

		return newmessage;
	}
}