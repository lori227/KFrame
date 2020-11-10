
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
}

BEGIN_MESSAGE_MAP( CKFGenerateDlg, CDialogEx )
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED( IDC_BUTTON8, &CKFGenerateDlg::OnBnClickedButton8 )
    ON_BN_CLICKED( IDC_BUTTON5, &CKFGenerateDlg::OnBnClickedButton5 )
    ON_BN_CLICKED( IDC_BUTTON1, &CKFGenerateDlg::OnBnClickedButton1 )
    ON_BN_CLICKED( IDC_BUTTON2, &CKFGenerateDlg::OnBnClickedButton2 )
    ON_BN_CLICKED( IDC_BUTTON3, &CKFGenerateDlg::OnBnClickedButton3 )
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

        SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );

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
    return static_cast<HCURSOR>( m_hIcon );
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

void CKFGenerateDlg::InitGenerateDialog()
{
    KFrame::KFGlobal::Initialize( nullptr );
    KFrame::KFMalloc::Initialize( nullptr );

    // 加载xml配置
    LoadXmlData();

    // 初始化界面
    InitControlData();

}

void CKFGenerateDlg::LoadXmlData()
{
    // 加载数据
    _generate_logic->LoadTempXml();

    // 加载文件列表
    _generate_logic->LoadExcelXml();

    // 加载生成配置
    auto ok = _generate_logic->LoadGenerateXml();
    if ( !ok )
    {
        AfxMessageBox( "加载生成配置失败, 请检查_generate.xml文件是否正确!" );
    }
}

void CKFGenerateDlg::InitControlData()
{
    _edit_server_xml_path.SetWindowTextA( _generate_logic->_server_xml_path.c_str() );
    _edit_client_xml_path.SetWindowTextA( _generate_logic->_client_xml_path.c_str() );
    _edit_cpp_path.SetWindowTextA( _generate_logic->_cpp_file_path.c_str() );
    _edit_csharp_path.SetWindowTextA( _generate_logic->_csharp_file_path.c_str() );
    _edit_lua_path.SetWindowTextA( _generate_logic->_lua_file_path.c_str() );
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
        _generate_logic->_server_xml_path = path;
        _generate_logic->SaveTempXml();
        _edit_server_xml_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnBnClickedButton5()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _generate_logic->_client_xml_path = path;
        _generate_logic->SaveTempXml();
        _edit_client_xml_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _generate_logic->_cpp_file_path = path;
        _generate_logic->SaveTempXml();
        _edit_cpp_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _generate_logic->_csharp_file_path = path;
        _generate_logic->SaveTempXml();
        _edit_csharp_path.SetWindowTextA( path.c_str() );
    }
}


void CKFGenerateDlg::OnBnClickedButton3()
{
    // TODO: 在此添加控件通知处理程序代码
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _generate_logic->_lua_file_path = path;
        _generate_logic->SaveTempXml();
        _edit_lua_path.SetWindowTextA( path.c_str() );
    }
}
