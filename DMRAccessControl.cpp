/*
 *   Copyright (C) 2016 by Simon Rune G7RZU
 *   Copyright (C) 2016,2017 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 */

#include "DMRAccessControl.h"
#include "Log.h"

#include <algorithm>
#include <vector>
#include <cstring>
#include <string>

std::vector<unsigned int> CDMRAccessControl::m_blackList;
std::vector<unsigned int> CDMRAccessControl::m_whiteList;

std::vector<unsigned int> CDMRAccessControl::m_prefixes;

std::vector<unsigned int> CDMRAccessControl::m_slot1TGWhiteList;
std::vector<unsigned int> CDMRAccessControl::m_slot2TGWhiteList;

bool CDMRAccessControl::m_selfOnly = false;

unsigned int CDMRAccessControl::m_id = 0U;

void CDMRAccessControl::init(const std::vector<unsigned int>& blacklist, const std::vector<unsigned int>& whitelist, const std::vector<unsigned int>& slot1TGWhitelist, const std::vector<unsigned int>& slot2TGWhitelist, bool selfOnly, const std::vector<unsigned int>& prefixes, unsigned int id)
{
	m_slot1TGWhiteList = slot1TGWhitelist;
	m_slot2TGWhiteList = slot2TGWhitelist;
	m_blackList        = blacklist;
	m_whiteList        = whitelist;
	m_selfOnly         = selfOnly;
	m_prefixes         = prefixes;
	m_id               = id;
}
 
bool CDMRAccessControl::validateSrcId(unsigned int id)
{
	std::string str_id = std::to_string(id);	// DMR ID from RF
	std::string str_m_id = std::to_string(m_id);	// MMDVMHost ID from Config
	if (m_selfOnly) {
//		return id == m_id;
		if (str_m_id.compare(0,7,str_id) == 0) {// if the first 7 digits of the MMDVMHost ID match the WHOLE of the RF ID
			return true;			// then allow the connection
		} else {
			return false;			// if not, reject it
		}
	}	

	if (std::find(m_blackList.begin(), m_blackList.end(), id) != m_blackList.end())
		return false;

	unsigned int prefix = id / 10000U;
	if (prefix == 0U || prefix > 999U)
		return false;

	if (!m_prefixes.empty()) {
		bool ret = std::find(m_prefixes.begin(), m_prefixes.end(), prefix) == m_prefixes.end();
		if (ret)
			return false;
	}

	if (!m_whiteList.empty())
		return std::find(m_whiteList.begin(), m_whiteList.end(), id) != m_whiteList.end();

	return true;
}

bool CDMRAccessControl::validateTGId(unsigned int slotNo, bool group, unsigned int id)
{
	if (!group)
		return true;

	// TG0 is never valid
	if (id == 0U)
		return false;
	
	if (slotNo == 1U) {
		if (m_slot1TGWhiteList.empty())
			return true;

		return std::find(m_slot1TGWhiteList.begin(), m_slot1TGWhiteList.end(), id) != m_slot1TGWhiteList.end();
	} else {
		if (m_slot2TGWhiteList.empty())
			return true;

		return std::find(m_slot2TGWhiteList.begin(), m_slot2TGWhiteList.end(), id) != m_slot2TGWhiteList.end();
	}
}
