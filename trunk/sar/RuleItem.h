#pragma once

#include "interfaces\isettings.h"

/// a typical rule item
/// contains info to reply to specific user
typedef struct _RULE_ITEM : ISettingsStream
{
	LPTSTR	RuleName,		/// item name
			ContactName,	/// contact name
			ReplyText,		/// reply text
			ReplyAction;	/// reply action
}RULE_ITEM, *PRULE_ITEM;

/// common item... - a rule that is 
/// applied to all users.
typedef struct _COMMON_RULE_ITEM : ISettingsStream
{
	LPTSTR Header;
	LPTSTR Message;
}COMMON_RULE_ITEM, *P_COMMON_RULE_ITEM;

typedef struct _RULE_METAINFO
{
	LPTSTR ContactName;
}RULE_METAINFO, *PRULE_METAINFO;
