#include "KFGenerateGit.h"
#include "git2/global.h"
#include "git2/repository.h"
#include "git2/errors.h"
#include "git2/deprecated.h"
#include "git2/signature.h"
#include "git2/branch.h"
#include "KFGenerateEvent.h"
#include "KFUtility/KFConvert.h"

namespace KFrame
{
    void KFGenerateGit::Initialize()
    {
        git_libgit2_init();
        return KFRepository::Initialize();
    }

    void KFGenerateGit::Shutdown()
    {
        git_libgit2_shutdown();
        return KFRepository::Shutdown();
    }

    bool KFGenerateGit::Open( const RepositoryData* data )
    {
        auto result = git_repository_open( &_git_repository, data->_path.c_str() );
        if ( result < 0 )
        {
            auto error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "打开git仓库失败[{}:{}]", error->klass, error->message ) );
            return false;
        }

        return KFRepository::Open( data );
    }

    int index_matched_path_cb( const char* path, const char* matched_pathspec, void* payload )
    {
        auto filename = KFConvert::ToAscii( path );
        _event->ShowEventMessage( __FORMAT__( "文件=[{}]添加成功", filename ) );
        return 0;
    }

    bool KFGenerateGit::AddAllFile( const std::string& path )
    {
        // get index
        git_index* index = nullptr;
        git_repository_index( &index, _git_repository );

        char* paths[ 1 ];
        paths[ 0 ] = ( char* )path.c_str();

        git_strarray gitpath = { nullptr, 0 };
        gitpath.strings = paths;
        gitpath.count = sizeof( paths ) / sizeof( char* );

        auto result = git_index_add_all( index, &gitpath, GIT_INDEX_ADD_DEFAULT, index_matched_path_cb, nullptr );
        if ( result < 0 )
        {
            const git_error* error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "添加文件失败[{}:{}]", error->klass, error->message ) );
            return false;
        }

        git_index_write( index );
        return true;
    }

    bool KFGenerateGit::Commit( const std::string& message )
    {
        // head ref
        git_reference* ref_head = nullptr;
        auto result = git_repository_head( &ref_head, _git_repository );
        if ( result != 0 && result != GIT_EUNBORNBRANCH )
        {
            _event->ShowEventMessage( __FORMAT__( "获取git仓库的头信息失败=[{}]", result ) );
            return false;
        }

        // get parent commit
        git_commit* parent_commit = nullptr;
        git_commit_lookup( &parent_commit, _git_repository, git_reference_target( ref_head ) );
        git_reference_free( ref_head );

        const git_commit* parents[] = { nullptr };
        parents[ 0 ] = parent_commit;
        auto parent_count = 1;

        // get index
        git_index* index = nullptr;
        git_repository_index( &index, _git_repository );

        // write index to tree
        git_oid new_tree_id;
        auto writeresult = git_index_write_tree( &new_tree_id, index );
        if ( writeresult < 0 )
        {
            const git_error* error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "写入git仓库树失败[{}:{}]", error->klass, error->message ) );
            return false;
        }
        git_index_free( index );

        git_tree* new_tree = nullptr;
        git_tree_lookup( &new_tree, _git_repository, &new_tree_id );

        // signature
        git_signature* author = nullptr;
        git_signature_now( &author, _data->_user.c_str(), _data->_mail.c_str() );

        // new commit
        git_oid new_commit;
        auto commitresult = git_commit_create( &new_commit, _git_repository, "HEAD", author, author,
                                               "UTF-8", KFConvert::ToUTF8( message ).c_str(), new_tree, parent_count, parents );
        if ( commitresult < 0 )
        {
            const git_error* error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "提交git仓库失败[{}:{}]", error->klass, error->message ) );
            return false;
        }

        git_commit_free( parent_commit );
        git_tree_free( new_tree );
        git_signature_free( author );
        return true;
    }

    bool KFGenerateGit::Push()
    {
        const char* refs[] = { "refs/heads/master:refs/heads/master" };
        git_strarray strarr = { ( char** )refs, 1 };

        git_credential_userpass_payload user_pass =
        {
            _data->_user.c_str(), _data->_password.c_str()
        };

        git_remote* remote = nullptr;
        git_remote_lookup( &remote, _git_repository, "origin" );

        git_push_options opts = GIT_PUSH_OPTIONS_INIT;
        opts.callbacks.credentials = git_credential_userpass;
        opts.callbacks.payload = &user_pass;
        auto result = git_remote_push( remote, &strarr, &opts );
        if ( result == 0 )
        {
            _event->ShowEventMessage( __FORMAT__( "推送git仓库成功" ) );
        }
        else
        {
            const git_error* error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "推送git仓库失败[{}:{}]", error->klass, error->message ) );
        }

        return result == 0;
    }

    bool KFGenerateGit::Pull()
    {
        // get a remote
        git_remote* remote = nullptr;
        auto result = git_remote_lookup( &remote, _git_repository, "origin" );
        if ( result < 0 )
        {
            const git_error* error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "拉取git仓库失败[{}:{}]", error->klass, error->message ) );
            return false;
        }

        // git fetch
        git_credential_userpass_payload userpass =
        {
            _data->_user.c_str(), _data->_password.c_str()
        };

        git_fetch_options fetchopts = GIT_FETCH_OPTIONS_INIT;
        fetchopts.callbacks.credentials = git_credential_userpass;
        fetchopts.callbacks.payload = &userpass;

        auto fetchresult = git_remote_fetch( remote, nullptr, &fetchopts, nullptr );
        if ( fetchresult < 0 )
        {
            const git_error* error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "拉取git仓库失败[{}:{}]", error->klass, error->message ) );
            return false;
        }

        // merge
        git_reference* originmaster = nullptr;
        git_branch_lookup( &originmaster, _git_repository, "origin/master", GIT_BRANCH_REMOTE );
        git_reference* localmaster = nullptr;
        git_branch_lookup( &localmaster, _git_repository, "master", GIT_BRANCH_LOCAL );

        git_repository_set_head( _git_repository, git_reference_name( localmaster ) );

        const git_annotated_commit* theirhead[ 100 ];
        git_annotated_commit_from_ref( ( git_annotated_commit** )&theirhead[ 0 ], _git_repository, originmaster );

        git_merge_options mergeopt = GIT_MERGE_OPTIONS_INIT;
        git_checkout_options checkoutopt = GIT_CHECKOUT_OPTIONS_INIT;
        auto mergteresult = git_merge( _git_repository, theirhead, 1, &mergeopt, &checkoutopt );
        if ( mergteresult < 0 )
        {
            const git_error* error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "合并git仓库失败[{}:{}]", error->klass, error->message ) );
            return false;
        }

        // reslove conflicts
        git_index* index = nullptr;
        git_repository_index( &index, _git_repository );
        if ( git_index_has_conflicts( index ) )
        {
            const git_index_entry* ancestorout = nullptr;
            const git_index_entry* ourout = nullptr;
            const git_index_entry* theirout = nullptr;

            git_index_conflict_iterator* conflictiterator = nullptr;
            git_index_conflict_iterator_new( &conflictiterator, index );

            while ( git_index_conflict_next( &ancestorout, &ourout, &theirout, conflictiterator ) != GIT_ITEROVER )
            {
                if ( ancestorout != nullptr )
                {
                    _event->ShowEventMessage( __FORMAT__( "git冲突: ancestor=[{}] ", ancestorout->path ) );
                }

                if ( ourout != nullptr )
                {
                    _event->ShowEventMessage( __FORMAT__( "git冲突: mine=[{}] ", ourout->path ) );
                }

                if ( theirout != nullptr )
                {
                    _event->ShowEventMessage( __FORMAT__( "git冲突: their=[{}] ", theirout->path ) );
                }
            }

            // git checkout --theirs <file>
            git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
            opt.checkout_strategy |= GIT_CHECKOUT_USE_THEIRS;
            git_checkout_index( _git_repository, index, &opt );

            git_index_conflict_iterator_free( conflictiterator );
        }

        git_commit* theircommit = nullptr;
        git_commit_lookup( &theircommit, _git_repository, git_reference_target( originmaster ) );
        git_commit* ourcommit = nullptr;
        git_commit_lookup( &ourcommit, _git_repository, git_reference_target( localmaster ) );

        // add and commit
        git_index_update_all( index, nullptr, nullptr, nullptr );
        git_index_write( index );
        git_oid newtreeid;
        git_index_write_tree( &newtreeid, index );
        git_tree* newtree = nullptr;
        git_tree_lookup( &newtree, _git_repository, &newtreeid );

        // signature
        git_signature* author = nullptr;
        git_signature_now( &author, _data->_user.c_str(), _data->_mail.c_str() );

        static auto message = KFConvert::ToUTF8( "配置表生成工具自动提交" );

        git_oid commit_id;
        auto commitresult = git_commit_create_v( &commit_id, _git_repository, git_reference_name( localmaster ), author, author, "UTF-8", message.c_str(), newtree, 2, ourcommit, theircommit );
        if ( commitresult < 0 )
        {
            const git_error* error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "合并提交git仓库失败[{}:{}]", error->klass, error->message ) );
            return false;
        }

        git_repository_state_cleanup( _git_repository );
        return true;
    }
}
