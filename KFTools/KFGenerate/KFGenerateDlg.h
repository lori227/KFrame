
// KFGenerateDlg.h: 头文件
//

#pragma once
#include "KFGenerateLogic.h"


// CKFGenerateDlg 对话框
class CKFGenerateDlg : public CDialogEx
{
    // 构造
public:
    CKFGenerateDlg( CWnd* pParent = nullptr );	// 标准构造函数

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_KFGENERATE_DIALOG };
#endif

protected:
    virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV 支持


    // 初始化
    void InitGenerateDialog();
    // 加载配置
    void LoadXmlData();
    // 初始化界面数据
    void InitControlData();

    // 选择文件夹
    std::string BrowsePath();

    // 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL PreTranslateMessage( MSG* pMsg );
    CEdit  _edit_cpp_path;
    CEdit _edit_server_xml_path;
    afx_msg void OnBnClickedButton8();
    CEdit _edit_client_xml_path;
    afx_msg void OnBnClickedButton5();
    CEdit _edit_csharp_path;
    CEdit _edit_lua_path;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
};
