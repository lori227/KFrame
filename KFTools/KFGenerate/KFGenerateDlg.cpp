
// KFGenerateDlg.cpp: 实现文件
//

#include "framework.h"
#include "KFGenerate.h"
#include "KFGenerateDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif


// CKFGenerateDlg 对话框
#define __FRAME_TIMER__ 700
#define __EVENT_TIMER__ 701


CKFGenerateDlg::CKFGenerateDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_KFGENERATE_DIALOG, pParent )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CKFGenerateDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_EDIT1, _edit_cpp_path );
    DDX_Control( pDX, IDC_EDIT4, _edit_server_xml_path );
    DDX_Control( pDX, IDC_EDIT5, _edit_client_xml_path );
    DDX_Control( pDX, IDC_EDIT2, _edit_csharp_path );
    DDX_Control( pDX, IDC_EDIT3, _edit_lua_path );
    DDX_Control( pDX, IDC_CHECK1, _check_server_type );
    DDX_Control( pDX, IDC_CHECK2, _check_client_type );
    DDX_Control( pDX, IDC_COMBO1, _combo_repository_list );
    DDX_Control( pDX, IDC_COMBO2, _combo_server_list );
    DDX_Control( pDX, IDC_LIST1, _list_excel );
    DDX_Control( pDX, IDC_LIST2, _list_info );
    DDX_Control( pDX, IDC_BUTTON6, _button_generate );
    DDX_Control( pDX, IDC_BUTTON7, _button_repository );
}

BEGIN_MESSAGE_MAP( CKFGenerateDlg, CDialogEx )
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED( IDC_BUTTON8, &CKFGenerateDlg::OnBnClickedButton8 )
    ON_BN_CLICKED( IDC_BUTTON5, &CKFGenerateDlg::OnBnClickedButton5 )
    ON_BN_CLICKED( IDC_BUTTON1, &CKFGenerateDlg::OnBnClickedButton1 )
    ON_BN_CLICKED( IDC_BUTTON2, &CKFGenerateDlg::OnBnClickedButton2 )
    ON_BN_CLICKED( IDC_BUTTON3, &CKFGenerateDlg::OnBnClickedButton3 )
    ON_CBN_SELCHANGE( IDC_COMBO1, &CKFGenerateDlg::OnCbnSelchangeCombo1 )
    ON_CBN_SELCHANGE( IDC_COMBO2, &CKFGenerateDlg::OnCbnSelchangeCombo2 )
    ON_BN_CLICKED( IDC_CHECK1, &CKFGenerateDlg::OnBnClickedCheck1 )
    ON_BN_CLICKED( IDC_CHECK2, &CKFGenerateDlg::OnBnClickedCheck2 )
    ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_NOTIFY( NM_RCLICK, IDC_LIST1, &CKFGenerateDlg::OnNMRClickList1 )
    ON_COMMAND( ID_32771, &CKFGenerateDlg::OnChangeExcelServerType )
    ON_COMMAND( ID_32772, &CKFGenerateDlg::OnChangeExcelClientType )
    ON_COMMAND( ID_32773, &CKFGenerateDlg::OnChangeExcelCommonType )
    ON_BN_CLICKED( IDC_BUTTON6, &CKFGenerateDlg::OnBnClickedButton6 )
    ON_BN_CLICKED( IDC_BUTTON7, &CKFGenerateDlg::OnBnClickedButton7 )
END_MESSAGE_MAP()


// CKFGenerateDlg 消息处理程序

BOOL CKFGenerateDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon( m_hIcon, TRUE );			// 设置大图标
    SetIcon( m_hIcon, FALSE );		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    InitGenerateDialog();
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKFGenerateDlg::OnPaint()
{
    if ( IsIconic() )
    {
        CPaintDC dc( this ); // 用于绘制的设备上下文

        SendMessage( WM_ICONERASEBKGND, reinterpret_cast< WPARAM >( dc.GetSafeHdc() ), 0 );

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics( SM_CXICON );
        int cyIcon = GetSystemMetrics( SM_CYICON );
        CRect rect;
        GetClientRect( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;

        // 绘制图标
        dc.DrawIcon( x, y, m_hIcon );
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CKFGenerateDlg::OnQueryDragIcon()
{
    return static_cast< HCURSOR >( m_hIcon );
}

BOOL CKFGenerateDlg::PreTranslateMessage( MSG* pMsg )
{
    //屏蔽ESC关闭窗体/
    if ( pMsg->message == WM_KEYDOWN )
    {
        if ( pMsg->wParam == VK_ESCAPE ||
                pMsg->wParam == VK_RETURN )
        {
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage( pMsg );
}

void CKFGenerateDlg::OnTimer( UINT_PTR nIDEvent )
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    switch ( nIDEvent )
    {
    case __FRAME_TIMER__:
        KFGlobal::Instance()->_real_time = KFDate::GetTimeEx();
        KFGlobal::Instance()->_game_time = KFClock::GetTime();
        break;
    case __EVENT_TIMER__:
        _event->ExecuteEvent();
        break;
    default:
        break;
    }

    CDialogEx::OnTimer( nIDEvent );
}

void CKFGenerateDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    ShutdownGenerate();
    Sleep( 1000 );

    CDialogEx::OnClose();
}

void CKFGenerateDlg::ShutdownGenerate()
{
    _repository->Shutdown();
    _logic->_thread_run = false;
}

void CKFGenerateDlg::InitGenerateDialog()
{
    KFGlobal::Initialize( nullptr );
    KFMalloc::Initialize( nullptr );

    SetTimer( __FRAME_TIMER__, 1000, nullptr );
    SetTimer( __EVENT_TIMER__, 100, nullptr );

    // 加载xml配置
    LoadXmlData();

    // 初始化界面
    InitControlData();

    // 注册事件函数
    InitEventFunction();

    // 初始化仓库
    InitRepository();

    // 启动文件检查线程
    KFThread::CreateThread( _logic, &KFGenerateLogic::RunCheckExcelMd5Thread, __FILE__, __LINE__ );
}

void CKFGenerateDlg::InitEventFunction()
{
    _event->RegisterEventFunction( EventType::ShowMessage, this, &CKFGenerateDlg::ShowLogicMessage );
    _event->RegisterEventFunction( EventType::AddFile, this, &CKFGenerateDlg::AddExcelFile );
    _event->RegisterEventFunction( EventType::RemoveFile, this, &CKFGenerateDlg::RemoveExcelFile );
    _event->RegisterEventFunction( EventType::ParseOk, this, &CKFGenerateDlg::ParseExcelOk );
    _event->RegisterEventFunction( EventType::ParseFailed, this, &CKFGenerateDlg::ParseExcelFailed );
    _event->RegisterEventFunction( EventType::ParseFinish, this, &CKFGenerateDlg::ParseExcelFinish );
    _event->RegisterEventFunction( EventType::RepositoryOk, this, &CKFGenerateDlg::RepositoryPushOk );
}

void CKFGenerateDlg::LoadXmlData()
{
    // 加载数据
    _logic->LoadTempXml();
    if ( _logic->_server_xml_path.empty() )
    {
        _logic->_server_xml_path = "../config/server/";
    }

    // 加载文件列表
    _logic->LoadExcelXml();

    // 加载生成配置
    auto ok = _logic->LoadGenerateXml();
    if ( !ok )
    {
        AfxMessageBox( "加载生成配置失败, 请检查_generate.xml文件是否正确!" );
    }
}

void CKFGenerateDlg::InitControlData()
{


    _edit_server_xml_path.SetWindowTextA( _logic->_server_xml_path.c_str() );
    _edit_client_xml_path.SetWindowTextA( _logic->_client_xml_path.c_str() );
    _edit_cpp_path.SetWindowTextA( _logic->_cpp_file_path.c_str() );
    _edit_csharp_path.SetWindowTextA( _logic->_csharp_file_path.c_str() );
    _edit_lua_path.SetWindowTextA( _logic->_lua_file_path.c_str() );
    _check_server_type.SetCheck( KFUtility::HaveBitMask( _logic->_file_type, ( uint32 )FileType::Server ) );
    _check_client_type.SetCheck( KFUtility::HaveBitMask( _logic->_file_type, ( uint32 )FileType::Client ) );

    {
        auto index = 0;
        for ( auto& iter : _logic->_repository_list._objects )
        {
            _combo_repository_list.AddString( iter.second->_type.c_str() );
            if ( iter.second->_type == _logic->_repository_type )
            {
                _combo_repository_list.SetCurSel( index );
            }
            ++index;
        }
    }

    {
        auto index = 0;
        for ( auto& iter : _logic->_server_list._objects )
        {
            _combo_server_list.AddString( iter.second->_name.c_str() );
            if ( iter.second->_id == _logic->_server_id )
            {
                _combo_server_list.SetCurSel( index );
            }
            ++index;
        }
    }

    {
        auto style = _list_excel.GetExtendedStyle();
        _list_excel.SetExtendedStyle( style | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

        RECT rect;
        _list_excel.GetClientRect( &rect );
        auto width = rect.right / 10.f;

        int index = 0;
        _list_excel.InsertColumn( index++, "", LVCFMT_CENTER, ( uint32 )width * 2 );
        _list_excel.InsertColumn( index++, "文件名称", LVCFMT_CENTER, ( uint32 )( width * 7.7 ) );
        _list_excel.InsertColumn( index++, "文件类型", LVCFMT_CENTER, ( uint32 )( width * 2.1 ) );
        _list_excel.DeleteColumn( 0 );
    }
}

void CKFGenerateDlg::InitRepository()
{
    _repository->Open( _logic->_repository_type );
}

std::string CKFGenerateDlg::BrowsePath()
{
    std::string result = "";

    BROWSEINFO bi;
    CHAR buffer[ 512 ];
    //初始化入口参数bi开始
    bi.hwndOwner = NULL;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = buffer;//此参数如为NULL则不能显示对话框
    bi.lpszTitle = "选择路径";
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    bi.lpfn = NULL;
    bi.iImage = 0;
    //初始化入口参数bi结束
    LPITEMIDLIST pIDList = SHBrowseForFolder( &bi );//调用显示选择对话框
    if ( pIDList )//选择到路径(即：点了确定按钮)
    {
        SHGetPathFromIDList( pIDList, buffer );
        result = buffer;
    }

    return result;
}

void CKFGenerateDlg::OnBnClickedButton8()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _logic->_server_xml_path = path;
        _logic->SaveTempXml();
        _edit_server_xml_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnBnClickedButton5()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _logic->_client_xml_path = path;
        _logic->SaveTempXml();
        _edit_client_xml_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _logic->_cpp_file_path = path;
        _logic->SaveTempXml();
        _edit_cpp_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _logic->_csharp_file_path = path;
        _logic->SaveTempXml();
        _edit_csharp_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnBnClickedButton3()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _logic->_lua_file_path = path;
        _logic->SaveTempXml();
        _edit_lua_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnCbnSelchangeCombo1()
{
    // TODO: 在此添加控件通知处理程序代码
    CString text;
    _combo_repository_list.GetWindowTextA( text );
    _logic->_repository_type = text.GetBuffer();
    _logic->SaveTempXml();

    _repository->Open( _logic->_repository_type );
}

void CKFGenerateDlg::OnCbnSelchangeCombo2()
{
    // TODO: 在此添加控件通知处理程序代码
    CString text;
    _combo_server_list.GetWindowTextA( text );
    auto name = text.GetBuffer();

    for ( auto& iter : _logic->_server_list._objects )
    {
        auto serverdata = iter.second;
        if ( serverdata->_name == name )
        {
            _logic->_server_id = serverdata->_id;
            _logic->SaveTempXml();
        }
    }
}


void CKFGenerateDlg::OnBnClickedCheck1()
{
    // TODO: 在此添加控件通知处理程序代码
    auto check = _check_server_type.GetCheck();
    if ( check == TRUE )
    {
        KFUtility::AddBitMask( _logic->_file_type, ( uint32 )FileType::Server );
    }
    else
    {
        KFUtility::ClearBitMask( _logic->_file_type, ( uint32 )FileType::Server );
    }
    _logic->SaveTempXml();

    ResetExcelFileList();
}


void CKFGenerateDlg::OnBnClickedCheck2()
{
    // TODO: 在此添加控件通知处理程序代码
    auto check = _check_client_type.GetCheck();
    if ( check == TRUE )
    {
        KFUtility::AddBitMask( _logic->_file_type, ( uint32 )FileType::Client );
    }
    else
    {
        KFUtility::ClearBitMask( _logic->_file_type, ( uint32 )FileType::Client );
    }
    _logic->SaveTempXml();

    ResetExcelFileList();
}


int32 CKFGenerateDlg::FindExcelFile( const std::string& filename )
{
    auto count = _list_excel.GetItemCount();
    for ( auto i = 0; i < count; ++i )
    {
        std::string text = _list_excel.GetItemText( i, 0 ).GetBuffer();
        if ( text == filename )
        {
            return i;
        }
    }

    return -1;
}

void CKFGenerateDlg::AddExcelFile( EventData* eventdata )
{
    auto index = FindExcelFile( eventdata->_str_param );
    if ( index == -1 )
    {
        index = _list_excel.GetItemCount();
        _list_excel.InsertItem( index, eventdata->_str_param.c_str() );
        _list_excel.SetItemText( index, 1, _file_type_name[ eventdata->_int_param ] );
    }
}

void CKFGenerateDlg::RemoveExcelFile( EventData* eventdata )
{
    auto index = FindExcelFile( eventdata->_str_param );
    if ( index != -1 )
    {
        _list_excel.DeleteItem( index );
    }
}

void CKFGenerateDlg::OnNMRClickList1( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast< LPNMITEMACTIVATE >( pNMHDR );
    // TODO: 在此添加控件通知处理程序代码
    CMenu popMenu;
    popMenu.LoadMenu( IDR_MENU1 );

    CPoint posMouse;
    GetCursorPos( &posMouse );

    CMenu* pSubMenu = NULL;
    pSubMenu = popMenu.GetSubMenu( 0 );
    pSubMenu->TrackPopupMenu( 0, posMouse.x, posMouse.y, this );

    *pResult = 0;
}

void CKFGenerateDlg::OnChangeExcelServerType()
{
    // TODO: 在此添加命令处理程序代码
    ChangeExcelType( FileType::Server );

}


void CKFGenerateDlg::OnChangeExcelClientType()
{
    // TODO: 在此添加命令处理程序代码
    ChangeExcelType( FileType::Client );

}


void CKFGenerateDlg::OnChangeExcelCommonType()
{
    // TODO: 在此添加命令处理程序代码
    ChangeExcelType( FileType::All );

}

void CKFGenerateDlg::ChangeExcelType( uint32 type )
{
    auto index = _list_excel.GetSelectionMark();
    if ( index == -1 )
    {
        return;
    }

    auto strtext = _list_excel.GetItemText( index, 0 );
    auto filedata = _logic->_file_list.Find( strtext.GetBuffer() );
    if ( filedata == nullptr )
    {
        return;
    }

    filedata->_type = type;
    _list_excel.SetItemText( index, 1, _file_type_name[ type ] );
    _logic->SaveExcelXml();

    if ( !KFUtility::HaveBitMask( _logic->_file_type, filedata->_type ) )
    {
        _list_excel.DeleteItem( index );
    }
}

void CKFGenerateDlg::ResetExcelFileList()
{
    _list_excel.DeleteAllItems();

    for ( auto& iter : _logic->_file_list._objects )
    {
        auto filedata = iter.second;
        if ( KFUtility::HaveBitMask( filedata->_type, _logic->_file_type ) )
        {
            auto index = _list_excel.GetItemCount();
            _list_excel.InsertItem( index, filedata->_name.c_str() );
            _list_excel.SetItemText( index, 1, _file_type_name[ filedata->_type ] );
        }
    }
}



void CKFGenerateDlg::ParseExcelOk( EventData* eventdata )
{
    auto strinfo = __FORMAT__( "解析=[{}] 完成", eventdata->_str_param );
    _list_info.AddString( strinfo.c_str() );

    auto filedata = _logic->_file_list.Find( eventdata->_str_param );
    if ( filedata != nullptr )
    {
        if ( KFUtility::HaveBitMask( _logic->_file_type, ( uint32 )FileType::Server ) )
        {
            filedata->_md5_server_repository = filedata->_md5_current;
        }

        if ( KFUtility::HaveBitMask( _logic->_file_type, ( uint32 )FileType::Client ) )
        {
            filedata->_md5_client_repository = filedata->_md5_current;
        }
    }
}

void CKFGenerateDlg::ShowLogicMessage( EventData* eventdata )
{
    _list_info.AddString( eventdata->_str_param.c_str() );
}

void CKFGenerateDlg::ParseExcelFailed( EventData* eventdata )
{
    _list_info.AddString( eventdata->_str_param.c_str() );
    AfxMessageBox( eventdata->_str_param.c_str() );
}

void CKFGenerateDlg::ParseExcelFinish( EventData* eventdata )
{
    _list_info.AddString( eventdata->_str_param.c_str() );
    _button_generate.EnableWindow( TRUE );

    _logic->SaveExcelXml();
    _version->SaveVersionXml( _logic->_server_xml_path );
    _version->SaveVersionXml( _logic->_client_xml_path );

    if ( _need_repository )
    {
        KFThread::CreateThread( this, &CKFGenerateDlg::RepositoryPushCommit, __FUNC_LINE__ );
    }
}

void CKFGenerateDlg::ParseAllExcels( bool repository )
{
    _need_repository = repository;
    _button_generate.EnableWindow( FALSE );
    _version->LoadVersionXml( _logic->_server_xml_path );
    _version->LoadVersionXml( _logic->_client_xml_path );

    // 清空list
    _list_info.ResetContent();

    StringList parselist;
    for ( auto& iter : _logic->_file_list._objects )
    {
        auto filedata = iter.second;
        parselist.push_back( filedata->_name );
    }
    _parse->ParseExcels( parselist );
}


void CKFGenerateDlg::OnBnClickedButton6()
{
    // TODO: 在此添加控件通知处理程序代码
    ParseAllExcels( false );
}

void CKFGenerateDlg::OnBnClickedButton7()
{
    // TODO: 在此添加控件通知处理程序代码
    _button_repository.EnableWindow( FALSE );

    // 先拉取更新
    _repository->Pull( false, _logic->_commit_data._merge_message );

    // 生成新配置文件
    ParseAllExcels( true );
}

void CKFGenerateDlg::RepositoryPushCommit()
{
    _repository->Push( _logic->_commit_data._commit_file_list, _logic->_commit_data._push_message );
    _event->AddEvent( EventType::RepositoryOk, 0, _invalid_string );
}

void CKFGenerateDlg::RepositoryPushOk( EventData* eventdata )
{
    _button_repository.EnableWindow( TRUE );
}
