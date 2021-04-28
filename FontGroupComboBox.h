/* 
 *  Filename:    FontGroupComboBox.h
 *  Description: Adapter to CGroupComboBox class for displaying fonts
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

// CGroupComboBoxFontItem definition
// Item displaying font
class CGroupComboBoxFontItem : public CGroupComboBoxItem
{
public:
	CGroupComboBoxFontItem(LOGFONT* font);
	~CGroupComboBoxFontItem();

	virtual void Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual CSize GetSize(CDC* dc);

private:
	CFont* m_font;
};

// CGroupComboBoxFontItem definition
// Item displaying font
class CGroupComboBoxFontItemWithIcon : public CGroupComboBoxItem
{
public:
	CGroupComboBoxFontItemWithIcon(LOGFONT* font, CImageList* imageList, int imageIndex);
	~CGroupComboBoxFontItemWithIcon();

	virtual void Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual CSize GetSize(CDC* dc);

private:
	CFont* m_font;
	CImageList* m_imageList;
	int m_imageIndex;
};

// CFontGroupComboBoxVariable
// Adapter to CGroupComboBox
class CFontGroupComboBoxVariable : public CGroupComboBoxVariable
{
	DECLARE_DYNAMIC(CFontGroupComboBoxVariable)

public:
	int AddGroup(LPCTSTR caption, bool isSorted = false);
	int AddGroup(LPCTSTR caption, CComboBoxItemCompare& itemCompare);
	int AddItem(LOGFONT* font, int groupIndex);
	int AddItem(LOGFONT* font, CImageList* imageList, int imageIndex, int groupIndex);
	int InsertItem(LOGFONT* font, int groupIndex);
	int InsertRecentItem(LPCTSTR caption);

protected:
	DECLARE_MESSAGE_MAP()

};


// CFontGroupComboBox
// Adapter to CGroupComboBox with fixed item height
class CFontGroupComboBox : public CGroupComboBox
{
	DECLARE_DYNAMIC(CFontGroupComboBox)

public:
	int AddGroup(LPCTSTR caption, bool isSorted = false);
	int AddGroup(LPCTSTR caption, CComboBoxItemCompare& itemCompare);
	int AddItem(LOGFONT* font, int groupIndex);
	int AddItem(LOGFONT* font, CImageList* imageList, int imageIndex, int groupIndex);
	int InsertItem(LOGFONT* font, int groupIndex);
	int InsertRecentItem(LPCTSTR caption);

protected:
	DECLARE_MESSAGE_MAP()
};


