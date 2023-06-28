#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -lX11 -lasound -- "$@"
#endif

#include <X11/Xlib.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

#include <notify.h>

static void playback_switch_toggle()
{
	snd_mixer_t *handle = 0;
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, "default");
	snd_mixer_selem_register(handle, 0, 0);
	snd_mixer_load(handle);

	snd_mixer_selem_id_t *sid = 0;
	snd_mixer_selem_id_alloca(&sid);
	/* snd_mixer_selem_id_set_index(sid, 0); */
	snd_mixer_selem_id_set_name(sid, "Master");
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	int ival = 0;
	snd_mixer_selem_get_playback_switch(elem, 0, &ival);
	snd_mixer_selem_set_playback_switch(elem, 0, ival ? 0 : 1);
	snd_mixer_close(handle);
}

static void capture_switch_toggle()
{
	snd_mixer_t *handle = 0;
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, "default");
	snd_mixer_selem_register(handle, 0, 0);
	snd_mixer_load(handle);

	snd_mixer_selem_id_t *sid = 0;
	snd_mixer_selem_id_alloca(&sid);
	/* snd_mixer_selem_id_set_index(sid, 0); */
	snd_mixer_selem_id_set_name(sid, "Capture");
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	int ival = 0;
	snd_mixer_selem_get_capture_switch(elem, 0, &ival);
	snd_mixer_selem_set_capture_switch(elem, 0, ival ? 0 : 1);
	snd_mixer_close(handle);
}

static void clear_status_bar()
{
	Display *display = XOpenDisplay(0);
	Window rootWindow = DefaultRootWindow(display);
	XStoreName(display, rootWindow, " ");
	XSync(display, False);
	XCloseDisplay(display);
}

static void set_volume(const Arg *arg)
{
	snd_mixer_t *handle = 0;
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, "default");
	snd_mixer_selem_register(handle, 0, 0);
	snd_mixer_load(handle);

	snd_mixer_selem_id_t *sid = 0;
	snd_mixer_selem_id_alloca(&sid);

	/* Probably not needed, but most code I saw uses the next line. */
	snd_mixer_selem_id_set_index(sid, 0);

	snd_mixer_selem_id_set_name(sid, "Master");
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	long max_volume = 0;
	snd_mixer_selem_get_playback_volume_range(elem, &(long){0}, &max_volume);

	long old_volume = 0;
	snd_mixer_selem_get_playback_volume(elem, 0, &old_volume);

	long new_volume = (long)arg->i + old_volume;
	if (new_volume < 0) {
		new_volume = 0;
	} else if (new_volume > max_volume) {
		new_volume = max_volume;
	}


	snd_mixer_selem_set_playback_volume_all(elem, new_volume);
	char *msg = 0;
	asprintf(&msg, "%ld/%ld", new_volume, max_volume);
	notify_send(msg, 72, 1000);
	free(msg);

	snd_mixer_close(handle);
}

#define DECIMAL_BASE 10U
static void set_brightness(const Arg *arg)
{
	FILE *file =
		fopen("/sys/class/backlight/intel_backlight/max_brightness",
			"re");
	char *line = 0;
	getline(&line, &(unsigned long){0}, file);
	fclose(file);
	long max_brightness = strtol(line, 0, DECIMAL_BASE);
	free(line);

	file = fopen("/sys/class/backlight/intel_backlight/brightness",
		"re");
	getline(&line, &(unsigned long){0}, file);
	fclose(file);
	long brightness = strtol(line, 0, DECIMAL_BASE);
	free(line);

	long new_brightness = brightness + (long)arg->i;

	if (new_brightness < 0) {
		new_brightness = 0;
	} else if (new_brightness > max_brightness) {
		new_brightness = max_brightness;
	}

	file = fopen("/sys/class/backlight/intel_backlight/brightness",
		"we");
	fprintf(file, "%ld", new_brightness);
	fclose(file);

	char *msg = 0;
	asprintf(&msg, "%ld/%ld", new_brightness, max_brightness);
	notify_send(msg, 72, 1000);
	free(msg);
}
