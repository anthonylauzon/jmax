noinst_DATA= \
 messbox.gif \
 messbox_selected.gif \
 messbox_cursor.gif 


EXTRA_DIST= $(noinst_DATA)

CLEANFILES= time.stamp

GIF_SRC_DIR=$(top_srcdir)/packages/ispw/java/icons
GIF_TARGET_DIR=$(top_builddir)/packages/ispw/java/icons

all-local: time.stamp
	if test "$(GIF_SRC_DIR)" != "$(GIF_TARGET_DIR)"; then \
		for i in $(noinst_DATA); do cp $(GIF_SRC_DIR)/$$i $(GIF_TARGET_DIR); done; \
	fi

clean-local:
	if test "$(GIF_SRC_DIR)" != "$(GIF_TARGET_DIR)"; then \
		for i in $(noinst_DATA); do rm -f $(GIF_TARGET_DIR)/$$i; done; \
	fi

time.stamp: $(noinst_DATA)
	echo timestamp > time.stamp
