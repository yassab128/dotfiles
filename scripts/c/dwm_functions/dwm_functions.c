#include "../my_macros.h"
#include <X11/Xlib.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef RUN_MAIN
typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;
#endif

static void
set_status_bar_text(char *text)
{
	Display *display = XOpenDisplay(0);
	if (!display) {
		return;
	}
	Window rootWindow = DefaultRootWindow(display);
	XStoreName(display, rootWindow, text);
	/* XFlush(display); */
	XCloseDisplay(display);
}

#define FGETS_LINE_MAX 127
static void
set_brightness(const Arg *arg)
{
	FILE *fp;
	char line[FGETS_LINE_MAX];
	int max_brightness;
	int brightness;
	int new_brightness;

	fp = fopen("/sys/class/backlight/intel_backlight/max_brightness", "r");
	fgets(line, FGETS_LINE_MAX, fp);
	fclose(fp);
	max_brightness = atoi(line);

	fp = fopen("/sys/class/backlight/intel_backlight/brightness", "r");
	fgets(line, FGETS_LINE_MAX, fp);
	fclose(fp);
	brightness = atoi(line);

	new_brightness = brightness + arg->i;

	if (new_brightness < 0) {
		new_brightness = 0;
	} else if (new_brightness > max_brightness) {
		new_brightness = max_brightness;
	}

	fp = fopen("/sys/class/backlight/intel_backlight/brightness", "w");
	fprintf(fp, "%d", new_brightness);
	fclose(fp);
}

#define HCTL_NAME "default"
static void
playback_switch_toggle(const Arg *arg)
{
	snd_mixer_selem_id_t *sid;
	snd_mixer_t *handle;
	snd_mixer_elem_t *elem;

	WARNIF(snd_mixer_open(&handle, 0));
	WARNIF(snd_mixer_attach(handle, HCTL_NAME));
	WARNIF(snd_mixer_selem_register(handle, 0, 0));
	WARNIF(snd_mixer_load(handle));
	WARNIF(snd_mixer_selem_id_malloc(&sid));
	snd_mixer_selem_id_set_index(sid, 0);

	snd_mixer_selem_id_set_name(sid, "Master");
	elem = snd_mixer_find_selem(handle, sid);
	WARNIF(!elem);
	switch (arg->i) {
	case 0: {
		int ival;
		WARNIF(snd_mixer_selem_get_playback_switch(elem, 0, &ival));
		WARNIF(
		 snd_mixer_selem_set_playback_switch(elem, 0, ival ? 0 : 1));
		break;
	}
	case 1:
		WARNIF(snd_mixer_selem_set_playback_switch(elem, 0, 1));
		break;
	case 2:
		WARNIF(snd_mixer_selem_set_playback_switch(elem, 0, 0));
		break;
	default:
		break;
	}

	snd_mixer_selem_id_free(sid);
	WARNIF(snd_mixer_detach(handle, HCTL_NAME));
	snd_mixer_free(handle);
	WARNIF(snd_mixer_close(handle));
	snd_config_update_free_global();
}

static void
capture_switch_toggle(const Arg *arg)
{
	snd_mixer_selem_id_t *sid;
	snd_mixer_t *handle;
	snd_mixer_elem_t *elem;

	WARNIF(snd_mixer_open(&handle, 0));
	WARNIF(snd_mixer_attach(handle, HCTL_NAME));
	WARNIF(snd_mixer_selem_register(handle, 0, 0));
	WARNIF(snd_mixer_load(handle));
	WARNIF(snd_mixer_selem_id_malloc(&sid));
	snd_mixer_selem_id_set_index(sid, 0);

	snd_mixer_selem_id_set_name(sid, "Capture");
	elem = snd_mixer_find_selem(handle, sid);
	WARNIF(!elem);
	switch (arg->i) {
	case 0: {
		int ival;
		WARNIF(snd_mixer_selem_get_capture_switch(elem, 0, &ival));
		WARNIF(
		 snd_mixer_selem_set_capture_switch(elem, 0, ival ? 0 : 1));
		break;
	}
	case 1:
		WARNIF(snd_mixer_selem_set_capture_switch(elem, 0, 1));
		break;
	case 2:
		WARNIF(snd_mixer_selem_set_capture_switch(elem, 0, 0));
		break;
	default:
		break;
	}

	snd_mixer_selem_id_free(sid);
	WARNIF(snd_mixer_detach(handle, HCTL_NAME));
	snd_mixer_free(handle);
	WARNIF(snd_mixer_close(handle));
	snd_config_update_free_global();
}

#define CURRENT_VOLUME_STR_SIZE 5

static void
set_volume(const Arg *arg)
{
	snd_mixer_selem_id_t *sid;
	snd_mixer_t *handle;
	snd_mixer_elem_t *elem;
	long min_volume;
	long max_volume;
	long current_volume;
	long new_volume;

	WARNIF(snd_mixer_open(&handle, 0));
	WARNIF(snd_mixer_attach(handle, HCTL_NAME));
	WARNIF(snd_mixer_selem_register(handle, 0, 0));
	WARNIF(snd_mixer_load(handle));
	WARNIF(snd_mixer_selem_id_malloc(&sid));
	snd_mixer_selem_id_set_index(sid, 0);

	snd_mixer_selem_id_set_name(sid, "Master");
	elem = snd_mixer_find_selem(handle, sid);
	WARNIF(!elem);

	WARNIF(snd_mixer_selem_get_playback_volume_range(
	 elem, &min_volume, &max_volume));
	WARNIF(snd_mixer_selem_get_playback_volume(elem, 0, &current_volume));

	new_volume = current_volume + (long)arg->i;
	if (new_volume < min_volume) {
		new_volume = min_volume;
	} else if (new_volume > max_volume) {
		new_volume = max_volume;
	}
	{
		char current_volume_percentage[CURRENT_VOLUME_STR_SIZE];
		sprintf(current_volume_percentage, "%hhu%%",
		 (unsigned char)(((float)new_volume /
				  (float)(max_volume - min_volume)) *
				 100));
		set_status_bar_text(current_volume_percentage);
	}
	WARNIF(snd_mixer_selem_set_playback_volume_all(elem, new_volume));

	snd_mixer_selem_id_free(sid);
	WARNIF(snd_mixer_detach(handle, HCTL_NAME));
	snd_mixer_free(handle);
	WARNIF(snd_mixer_close(handle));
	snd_config_update_free_global();
}

#ifdef RUN_MAIN
int
main()
{
	/*playback_switch_toggle();
	capture_switch_toggle();
	set_brightness(&arg);
	clear_status_bar();*/
	Arg arg;
	arg.i = 1;
	playback_switch_toggle(&arg);
	capture_switch_toggle(&arg);
	/* 1024 is big enough for the volume to exceed 100% */
	arg.i = 1024;
	set_volume(&arg);
}
#endif
