#include "KFFilterModule.hpp"

namespace KFrame
{

#define __WORD_PACE__ 1

    void KFFilterData::Clear()
    {
        _child.clear();
    }

    bool KFFilterData::IsEmpty()
    {
        return _child.empty();
    }

    KFFilterData* KFFilterData::FindChild( int8 word )
    {
        auto iter = _child.find( word );
        if ( iter == _child.end() )
        {
            return nullptr;
        }

        return &iter->second;
    }

    KFFilterData* KFFilterData::InsertChild( int8 word )
    {
        auto kfdata = FindChild( word );
        if ( kfdata == nullptr )
        {
            kfdata = &( _child.insert( std::make_pair( word, KFFilterData() ) ).first->second );
        }

        return kfdata;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void KFFilterModule::AfterLoad()
    {
        if ( KFFilterConfig::Instance()->_load_ok )
        {
            for ( auto& iter : KFFilterConfig::Instance()->_settings._objects )
            {
                auto kfsetting = iter.second;
                InsertWord( &_root_word_data, kfsetting->_word.data(), static_cast< uint32 >( kfsetting->_word.length() ), 0 );
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    void KFFilterModule::InsertWord( KFFilterData* kffilterdata, const char* source, uint32 length, uint32 index )
    {
        if ( index >= length )
        {
            return;
        }

        auto word = source[ index ];
        auto kfchilddata = kffilterdata->InsertChild( word );
        InsertWord( kfchilddata, source, length, ++index );
    }

    uint32 KFFilterModule::FindWord( KFFilterData* kffilterdata, const char* source, uint32 length, uint32 index, uint32& count )
    {
        // 已经查找完
        if ( index >= length )
        {
            return 0u;
        }

        auto word = source[ index ];
        auto kfchilddata = kffilterdata->FindChild( word );
        if ( kfchilddata == nullptr )
        {
            // 不在字典中
            return 0u;
        }

        // 字典查完了, 说明已经找到屏蔽字
        ++count;
        if ( kfchilddata->IsEmpty() )
        {
            return count;
        }

        return FindWord( kfchilddata, source, length, ++index, count );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFFilterModule::CheckFilter( const std::string& source )
    {
        // 不能有 空格 %
        if ( source.find( " " ) != std::string::npos ||
                source.find( "%" ) != std::string::npos )
        {
            return true;
        }

        auto data = source.data();
        auto length = static_cast< uint32 >( source.size() );

        for ( auto i = 0u; i < length; ++i )
        {
            auto filter = CheckFilter( data + i, length - i );
            if ( filter )
            {
                return true;
            }
        }

        return false;
    }

    bool KFFilterModule::CheckFilter( const char* source, uint32 length )
    {
        auto count = 0u;
        count = FindWord( &_root_word_data, source, length, 0, count );
        return count != 0u;
    }

    bool KFFilterModule::CensorFilter( std::string& source )
    {
        bool filter = false;
        auto length = static_cast< uint32 >( source.size() );

        for ( auto i = 0u; i < length; ++i )
        {
            auto data = source.data();
            auto count = CensorFilter( data + i, length - i );
            if ( count > 0 )
            {
                filter = true;
                source.replace( i, count, "**" );

                i += 1;
                length = static_cast< uint32 >( source.size() );
            }
        }

        return filter;
    }

    uint32 KFFilterModule::CensorFilter( const char* source, uint32 length )
    {
        auto count = 0u;
        return FindWord( &_root_word_data, source, length, 0, count );
    }
}