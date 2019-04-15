#pragma once


// CAddAgentDlg 对话框

class CAddAgentDlg : public CDialogEx
{
    DECLARE_DYNAMIC( CAddAgentDlg )

public:
    CAddAgentDlg( CWnd* pParent = nullptr ); // 标准构造函数
    virtual ~CAddAgentDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG1 };
#endif

    // 初始化
    void InitDialog();

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage( MSG* pMsg );

    CEdit _edit_agent_id;
    afx_msg void OnBnClickedButtonAddAgent();
    CIPAddressCtrl _ip_control;
};
