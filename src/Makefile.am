bin_PROGRAMS = centerim5

# When you add files here, also add them in po/POTFILES.in.
centerim5_SOURCES = \
	AccountStatusMenu.cpp \
	AccountStatusMenu.h \
	AccountWindow.cpp \
	AccountWindow.h \
	Accounts.cpp \
	Accounts.h \
	BuddyList.cpp \
	BuddyList.h \
	BuddyListNode.cpp \
	BuddyListNode.h \
	CenterIM.cpp \
	CenterIM.h \
	Connections.cpp \
	Connections.h \
	Conversation.cpp \
	Conversation.h \
	ConversationRoomList.cpp \
	ConversationRoomList.h \
	Conversations.cpp \
	Conversations.h \
	Footer.cpp \
	Footer.h \
	GeneralMenu.cpp \
	GeneralMenu.h \
	Header.cpp \
	Header.h \
	Log.cpp \
	Log.h \
	Notify.cpp \
	Notify.h \
	OptionWindow.cpp \
	OptionWindow.h \
	PluginWindow.cpp \
	PluginWindow.h \
	Request.cpp \
	Request.h \
	Transfers.cpp \
	Transfers.h \
	Utils.cpp \
	Utils.h \
	git-version.cpp

centerim5_CPPFLAGS = \
	$(PURPLE_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(SIGC_CFLAGS) \
	-I$(top_srcdir)

centerim5_LDADD = \
	$(LTLIBINTL) \
	$(PURPLE_LIBS) \
	$(GLIB_LIBS) \
	$(SIGC_LIBS) \
	$(top_builddir)/cppconsui/libcppconsui.la
