#pragma once


// CAddServerDlg 对话框

class CAddServerDlg : public CDialogEx
{
    DECLARE_DYNAMIC( CAddServerDlg )

public:
    CAddServerDlg( CWnd* pParent = nullptr ); // 标准构造函数
    virtual ~CAddServerDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG2 };
#endif

public:
    void InitDialog();

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage( MSG* pMsg );
    CComboBox _combo_name;
    CComboBox _combo_type;
    CComboBox _combo_net;
    CComboBox _combo_mode;
    CComboBox _combo_agent;
    CComboBox _combo_startup;
    CComboBox _combo_service;
    CEdit _edit_zone;
    CEdit _edit_id;
    CEdit _edit_param;
    CEdit _edit_path;
    afx_msg void OnBnClickedButtonAddServer();
    afx_msg void OnCbnSelchangeComboName();
    afx_msg void OnCbnSelchangeComboType();
};
