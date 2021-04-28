/* 
 *  Filename:    TestDlg.h
 *  Description: Group Combo Box demo dialog
 *  Copyright:   Julijan Sribar, 2012-2013
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author(s) be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once
#include "GroupComboBox.h"
#include "FontGroupComboBox.h"
#include "GroupComboBoxWithIcons.h"
#include "afxwin.h"


// CTestDlg dialog
class CTestDlg : public CDialogEx
{
// Construction
public:
	CTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTAPPLICATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_sameer()

public:
	afx_msg void OnBnClickedButtonAddRecentOdFixed();
	afx_msg void OnBnClickedButtonRemoveRecentOdFixed();
	afx_msg void OnBnClickedButtonAddRecentOdVariable();
	afx_msg void OnBnClickedButtonRemoveRecentOdVariable();
	afx_msg void OnBnClickedAutowidthComboOdFixed();
	afx_msg void OnBnClickedAutowidthComboOdVariable();
	afx_msg void OnBnClickedAutocompleteComboOdFixed();
	afx_msg void OnBnClickedAutocompleteComboOdVariable();
	afx_msg void OnBnClickedCancelwescapeComboOdFixed();
	afx_msg void OnBnClickedCancelwescComboOdVariable();

private:
	CComboBox m_comboBox;
	CGroupComboBox m_comboBoxODFixed;
	CFontGroupComboBox m_comboBoxODFixed2;
	CFontGroupComboBoxVariable m_comboBoxODVariable;
	CGroupComboBoxWithIcons m_comboCountries;
	CButton m_autowidthComboODFixed;
	CButton m_autowidthComboODVariable;
	CButton m_autocompleteComboODFixed;
	CButton m_autocompleteComboODVariable;
	CButton m_undoByEscComboODFixed;
	CButton m_undoByEscComboODVariable;
	CImageList m_fontTypes;

	void FillOrdinaryComboBox();
	void FillODFixedComboBox();
	void FillODVariableComboBox();
	void FillComboBoxCountries();
};
