// XDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CXDlg �Ի���
class CXDlg : public CDialog
{
// ����
public:
	CXDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_XFFNEWS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic NoAdmin;
	CStatic Wowr;
public:
	afx_msg void OnBnClickedClean();
public:
public:
	CListBox Dolist;
public:
	CButton StartB;
};
