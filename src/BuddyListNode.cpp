/*
 * Copyright (C) 2007 by Mark Pustjens <pustjens@dds.nl>
 * Copyright (C) 2010 by CenterIM developers
 *
 * This file is part of CenterIM.
 *
 * CenterIM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * CenterIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * */

#include "BuddyListNode.h"

#include "Conversations.h"

#include <cppconsui/Keys.h>
#include <libpurple/version.h>
#include "gettext.h"

BuddyListNode *BuddyListNode::CreateNode(PurpleBlistNode *node)
{
	if (PURPLE_BLIST_NODE_IS_BUDDY(node))
		return new BuddyListBuddy(node);
	if (PURPLE_BLIST_NODE_IS_CHAT(node))
		return new BuddyListChat(node);
	else if (PURPLE_BLIST_NODE_IS_CONTACT(node))
		return new BuddyListContact(node);
	else if (PURPLE_BLIST_NODE_IS_GROUP(node))
		return new BuddyListGroup(node);

	LOG->Write(Log::Level_warning, _("Unrecognized BuddyList node.\n"));
	return NULL;
}

BuddyListNode::BuddyListNode(PurpleBlistNode *node)
: Button("")
, node(node)
{
	signal_activate.connect(sigc::mem_fun(this, &BuddyListNode::OnActivate));
}

BuddyListNode::~BuddyListNode()
{
	node->ui_data = NULL;
}

void BuddyListNode::Update()
{
	TreeView *t = dynamic_cast<TreeView *>(parent);
	g_assert(t);

	BuddyListNode *parent_node = GetParentNode();
	// the parent could have changed, so re-parent the node
	if (parent_node)
		t->SetParent(ref, parent_node->GetRefNode());
}

void BuddyListNode::SortIn()
{
	BuddyListNode *parent_node = GetParentNode();
	if (parent_node) {
		TreeView::NodeReference parent_ref = parent_node->GetRefNode();

		TreeView *t = dynamic_cast<TreeView *>(parent);
		g_assert(t);

		TreeView::SiblingIterator i;
		for (i = parent_ref.begin(); i != parent_ref.end(); i++) {
			// skip this node
			if (i == ref)
				continue;

			BuddyListNode *n = dynamic_cast<BuddyListNode *>(i->GetWidget());
			g_assert(n);

			if (LessThan(*n)) {
				t->MoveBefore(ref, i);
				break;
			}
		}
		// the ref is last in a list
		if (i == parent_ref.end())
			t->MoveAfter(ref, --i);
	}
}

BuddyListNode *BuddyListNode::GetParentNode() const
{
	PurpleBlistNode *parent = node->parent;

	if (!parent || !parent->ui_data)
		return NULL;

	return reinterpret_cast<BuddyListNode *>(parent->ui_data);
}

const gchar *BuddyListNode::GetBuddyStatus(PurpleBuddy *buddy) const
{
	if (!purple_account_is_connected(purple_buddy_get_account(buddy)))
		return "";

	PurplePresence *presence = purple_buddy_get_presence(buddy);
	PurpleStatus *status = purple_presence_get_active_status(presence);
	PurpleStatusType *status_type = purple_status_get_type(status);
	PurpleStatusPrimitive prim = purple_status_type_get_primitive(status_type);

	switch (prim) {
		case PURPLE_STATUS_UNSET:
			return "[x] ";
		case PURPLE_STATUS_OFFLINE:
			return "";
		case PURPLE_STATUS_AVAILABLE:
			return "[o] ";
		case PURPLE_STATUS_UNAVAILABLE:
			return "[u] ";
		case PURPLE_STATUS_INVISIBLE:
			return "[i] ";
		case PURPLE_STATUS_AWAY:
			return "[a] ";
		case PURPLE_STATUS_EXTENDED_AWAY:
			return "[A] ";
		case PURPLE_STATUS_MOBILE:
			return "[m] ";
		case PURPLE_STATUS_TUNE:
			return "[t] ";
#if PURPLE_VERSION_CHECK(2, 7, 0)
		case PURPLE_STATUS_MOOD:
			return "[d] ";
#endif
		default:
			return "[X] ";
	}
}

int BuddyListNode::GetBuddyStatusWeight(PurpleBuddy *buddy) const
{
	if (!purple_account_is_connected(purple_buddy_get_account(buddy)))
		return 0;

	PurplePresence *presence = purple_buddy_get_presence(buddy);
	PurpleStatus *status = purple_presence_get_active_status(presence);
	PurpleStatusType *status_type = purple_status_get_type(status);
	PurpleStatusPrimitive prim = purple_status_type_get_primitive(status_type);

	switch (prim) {
		case PURPLE_STATUS_OFFLINE:
			return 0;
		default:
			return 1;
		case PURPLE_STATUS_UNSET:
			return 2;
		case PURPLE_STATUS_UNAVAILABLE:
			return 3;
		case PURPLE_STATUS_AWAY:
			return 4;
		case PURPLE_STATUS_EXTENDED_AWAY:
			return 5;
		case PURPLE_STATUS_MOBILE:
			return 6;
#if PURPLE_VERSION_CHECK(2, 7, 0)
		case PURPLE_STATUS_MOOD:
			return 7;
#endif
		case PURPLE_STATUS_TUNE:
			return 8;
		case PURPLE_STATUS_INVISIBLE:
			return 9;
		case PURPLE_STATUS_AVAILABLE:
			return 10;
	}
}

BuddyListBuddy::BuddyListBuddy(PurpleBlistNode *node)
: BuddyListNode(node)
{
	SetColorScheme("buddylistbuddy");

	buddy = reinterpret_cast<PurpleBuddy *>(node);
	node->ui_data = this;
}

bool BuddyListBuddy::LessThan(const BuddyListNode& other) const
{
	const BuddyListBuddy *o = dynamic_cast<const BuddyListBuddy *>(&other);
	if (o) {
		int a = GetBuddyStatusWeight(buddy);
		int b = GetBuddyStatusWeight(o->buddy);
		if (a != b)
			return a > b;

		return g_utf8_collate(purple_buddy_get_alias(buddy),
				purple_buddy_get_alias(o->buddy)) < 0;
	}
	return false;
}

void BuddyListBuddy::Update()
{
	BuddyListNode::Update();

	gchar *text = g_strdup_printf("%s%s", GetBuddyStatus(buddy),
			purple_buddy_get_alias(buddy));
	SetText(text);
	g_free(text);

	SortIn();
}

void BuddyListBuddy::OnActivate()
{
	CONVERSATIONS->ShowConversation(PURPLE_CONV_TYPE_IM,
			purple_buddy_get_account(buddy), purple_buddy_get_name(buddy));
}

BuddyListChat::BuddyListChat(PurpleBlistNode *node)
: BuddyListNode(node)
{
	SetColorScheme("buddylistchat");

	chat = reinterpret_cast<PurpleChat *>(node);
	node->ui_data = this;
}

bool BuddyListChat::LessThan(const BuddyListNode& other) const
{
	const BuddyListChat *o = dynamic_cast<const BuddyListChat *>(&other);
	if (o) {
		return g_utf8_collate(purple_chat_get_name(chat),
				purple_chat_get_name(o->chat)) < 0;
	}
	return false;
}

void BuddyListChat::Update()
{
	BuddyListNode::Update();

	SetText(purple_chat_get_name(chat));

	SortIn();
}

void BuddyListChat::OnActivate()
{
	CONVERSATIONS->ShowConversation(PURPLE_CONV_TYPE_CHAT,
			purple_chat_get_account(chat), purple_chat_get_name(chat));
}

BuddyListContact::BuddyListContact(PurpleBlistNode *node)
: BuddyListNode(node)
{
	SetColorScheme("buddylistcontact");

	contact = reinterpret_cast<PurpleContact *>(node);
	node->ui_data = this;
}

bool BuddyListContact::LessThan(const BuddyListNode& other) const
{
	const BuddyListContact *o = dynamic_cast<const BuddyListContact *>(&other);
	if (o) {
		PurpleBuddy *buddy_a = purple_contact_get_priority_buddy(contact);
		PurpleBuddy *buddy_b = purple_contact_get_priority_buddy(o->contact);
		int a = GetBuddyStatusWeight(buddy_a);
		int b = GetBuddyStatusWeight(buddy_b);
		if (a != b)
			return a > b;

		return g_utf8_collate(purple_contact_get_alias(contact),
				purple_contact_get_alias(o->contact)) < 0;
	}
	return false;
}

void BuddyListContact::Update()
{
	BuddyListNode::Update();

	PurpleBuddy *buddy = purple_contact_get_priority_buddy(contact);
	gchar *text = g_strdup_printf("%s%s", GetBuddyStatus(buddy),
			purple_contact_get_alias(contact));
	SetText(text);
	g_free(text);

	SortIn();
}

void BuddyListContact::OnActivate()
{
	PurpleBuddy *buddy = purple_contact_get_priority_buddy(contact);
	CONVERSATIONS->ShowConversation(PURPLE_CONV_TYPE_IM,
			purple_buddy_get_account(buddy), purple_buddy_get_name(buddy));
}

BuddyListGroup::BuddyListGroup(PurpleBlistNode *node)
: BuddyListNode(node)
{
	SetColorScheme("buddylistgroup");

	group = reinterpret_cast<PurpleGroup *>(node);
	node->ui_data = this;
}

bool BuddyListGroup::LessThan(const BuddyListNode& other) const
{
	const BuddyListGroup *o = dynamic_cast<const BuddyListGroup *>(&other);
	if (o) {
		return g_utf8_collate(purple_group_get_name(group),
				purple_group_get_name(o->group)) < 0;
	}
	return false;
}

void BuddyListGroup::Update()
{
	BuddyListNode::Update();

	const gchar *text;
	text = purple_group_get_name(group);
	SetText(text);

	SortIn();
}

void BuddyListGroup::OnActivate()
{
	TreeView *t = dynamic_cast<TreeView *>(parent);
	g_assert(t);
	t->ToggleCollapsed(ref);
}

void BuddyListGroup::SortIn()
{
	TreeView *t = dynamic_cast<TreeView *>(parent);
	g_assert(t);
	TreeView::NodeReference parent_ref = t->Root();

	TreeView::SiblingIterator i;
	for (i = parent_ref.begin(); i != parent_ref.end(); i++) {
		// skip this node
		if (i == ref)
			continue;

		BuddyListNode *n = dynamic_cast<BuddyListNode *>(i->GetWidget());
		g_assert(n);

		if (LessThan(*n)) {
			t->MoveBefore(ref, i);
			break;
		}
	}
	// the ref is last in a list
	if (i == parent_ref.end())
		t->MoveAfter(ref, --i);
}
