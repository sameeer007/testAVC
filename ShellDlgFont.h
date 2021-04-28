/* 
 *  Filename:    ShellDlgFont.h
 *  Description: Helper class to select system dialog font into device context
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

// CShellDlgFont helper class
// Creates system dialog font and selects it into
// device context provided and will automatically 
// delete it when object is destroyed
class CShellDlgFont
{
public:
	CShellDlgFont(CDC* dc, LONG fontWeight = FW_NORMAL) : m_dc(dc)
	{
		LOGFONT lf = { 0 };
		lf.lfHeight = -MulDiv(8, ::GetDeviceCaps(*dc, LOGPIXELSY), 72);
		lf.lfWidth = 0;
		lf.lfWeight = fontWeight;
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		_tcscpy_s(lf.lfFaceName, _T("MS Shell Dlg 2"));
		m_font.CreateFontIndirect(&lf);

		m_oldFont = dc->SelectObject(&m_font);
	}

	~CShellDlgFont()
	{
		m_dc->SelectObject(m_oldFont);
		m_font.DeleteObject();
	}

private:
	CDC* m_dc;
	CFont m_font;
	CFont* m_oldFont;
};
