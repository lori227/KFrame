#include "KFFilterConfig.hpp"

namespace KFrame
{
#define __WORD_PACE__ 1

    void KFWordData::Clear()
    {
        _child.clear();
    }

    bool KFWordData::IsEmpty()
    {
        return _child.empty();
    }

    KFWordData* KFWordData::FindChild( int8 word )
    {
        auto iter = _child.find( word );
        if ( iter == _child.end() )
        {
            return nullptr;
        }

        return &iter->second;
    }

    KFWordData* KFWordData::InsertChild( int8 word )
    {
        auto kfdata = FindChild( word );
        if ( kfdata == nullptr )
        {
            kfdata = &( _child.insert( std::make_pair( word, KFWordData() ) ).first->second );
        }

        return kfdata;
    }

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    void KFFilterConfig::InsertWord( KFWordData* kfworddata, const char* source, uint32 length, uint32 index )
    {
        if ( index >= length )
        {
            return;
        }

        auto word = source[ index ];
        auto kfchilddata = kfworddata->InsertChild( word );
        InsertWord( kfchilddata, source, length, ++index );
    }

    uint32 KFFilterConfig::FindWord( KFWordData* kfworddata, const char* source, uint32 length, uint32 index, uint32& count )
    {
        // 已经查找完
        if ( index >= length )
        {
            return 0u;
        }

        auto word = source[ index ];
        auto kfchilddata = kfworddata->FindChild( word );
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

    bool KFFilterConfig::CheckFilter( const char* source, uint32 length )
    {
        auto count = 0u;
        count = FindWord( &_root_word_data, source, length, 0, count );
        return count != 0u;
    }

    uint32 KFFilterConfig::CensorFilter( const char* source, uint32 length )
    {
        auto count = 0u;
        return FindWord( &_root_word_data, source, length, 0, count );
    }

    void KFFilterConfig::LoadConfig( const std::string& file )
    {
        _root_word_data.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();
        auto wordnode = config.FindNode( "item" );
        while ( wordnode.IsValid() )
        {
            auto word = wordnode.GetString( "Word" );
            InsertWord( &_root_word_data, word.data(), static_cast< uint32 >( word.length() ), 0 );

            wordnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////
    }
}