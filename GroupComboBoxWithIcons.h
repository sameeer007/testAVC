/* 
 *  Filename:    GroupComboBoxItemWithIcon.h
 *  Description: Adapter to CGroupComboBox class for displaying items with icon
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

class CGroupComboBoxItemWithIcon : public CGroupComboBoxItem
{
public:
	CGroupComboBoxItemWithIcon(LPCTSTR caption, UINT iconID);

	virtual void Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual CSize GetSize(CDC* dc);

private:
	UINT m_iconId;
};

// CGroupComboBoxWithIcons

class CGroupComboBoxWithIcons : public CGroupComboBox
{
	DECLARE_DYNAMIC(CGroupComboBoxWithIcons)

public:
	CGroupComboBoxWithIcons();
	virtual ~CGroupComboBoxWithIcons();

	int AddGroup(LPCTSTR caption, bool isSorted = false);
	int AddItem(LPCTSTR caption, UINT iconID, int groupIndex);

protected:
	DECLARE_MESSAGE_MAP()
};


