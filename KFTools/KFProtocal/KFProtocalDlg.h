
// KFProtocalDlg.h: 头文件
//

#pragma once
#include <string>

// CKFProtocalDlg 对话框
class CKFProtocalDlg : public CDialogEx
{
    // 构造
public:
    CKFProtocalDlg( CWnd* pParent = nullptr );	// 标准构造函数

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_KFPROTOCAL_DIALOG };
#endif

protected:
    virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV 支持

    // 加载资源配置
    void LoadPathFile();

    // 初始化空间信息
    void InitDialogData();

    // 处理所有的.proto文件
    void ProtocConfigFiles();

    // 复制文件到对应路径
    void CopyConfigFiles();

    // 编译Protocol项目
    void BuildProtocolProj();

    // 浏览选择目录
    std::string BrowsePath();

    // 处理文件
    void OnProtocFile( const std::string& filename );

    // 处理客户端lua文件
    void OnProtocClientLuaFile( const std::string& filename );

    // 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnBnClickedButton5();
    CEdit _server_cpp_path;
    CEdit _server_lua_path;
    CEdit _client_cpp_path;
    CEdit _client_lua_path;
    CButton _operate_btn;

    afx_msg void OnTimer( UINT_PTR nIDEvent );
    afx_msg void OnEnChangeEdit1();
    afx_msg void OnEnChangeEdit2();
    afx_msg void OnEnChangeEdit3();
    afx_msg void OnEnChangeEdit4();
};
