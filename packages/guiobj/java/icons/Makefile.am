noinst_DATA= \
button_cursor.gif \
button.gif \
button_selected.gif \
cursor_in.gif \
define_cursor.gif \
define.gif \
define_selected.gif \
display_cursor.gif \
display.gif \
display_selected.gif \
floatbox_cursor.gif \
floatbox.gif \
floatbox_selected.gif \
fork_cursor.gif \
fork.gif \
fork_selected.gif \
inlet_cursor.gif \
inlet.gif \
inlet_selected.gif \
input_cursor.gif \
input.gif \
input_selected.gif \
intbox_cursor.gif \
intbox.gif \
intbox_selected.gif \
jcomment_cursor.gif \
jcomment.gif \
jcomment_selected.gif \
jpatcher_cursor.gif \
jpatcher.gif \
jpatcher_selected.gif \
matdisplay_cursor.gif \
matdisplay.gif \
matdisplay_selected.gif \
messconst_cursor.gif \
messconst.gif \
messconst_selected.gif \
monitor_cursor.gif \
monitor.gif \
monitor_selected.gif \
outlet_cursor.gif \
outlet.gif \
outlet_selected.gif \
scope_cursor.gif \
scope.gif \
scope_selected.gif \
slider_cursor.gif \
slider.gif \
slider_selected.gif \
standard_cursor.gif \
standard.gif \
standard_selected.gif \
toggle_cursor.gif \
toggle.gif \
toggle_selected.gif \
vecdisplay_cursor.gif \
vecdisplay.gif \
vecdisplay_selected.gif \
xypad_cursor.gif \
xypad.gif \
xypad_selected.gif 

CLEANFILES= time.stamp

EXTRA_DIST=$(noinst_DATA)

GIF_SRC_DIR=$(top_srcdir)/packages/guiobj/java/icons
GIF_TARGET_DIR=$(top_builddir)/packages/guiobj/java/icons

all-local: time.stamp
	if test "$(GIF_SRC_DIR)" != "$(GIF_TARGET_DIR)"; then \
		for i in $(noinst_DATA); do cp $(GIF_SRC_DIR)/$$i $(GIF_TARGET_DIR); done; \
	fi


time.stamp: $(noinst_DATA)
	echo timestamp > time.stamp
