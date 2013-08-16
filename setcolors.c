#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>

#define NUM_COLORS 16

static const char *console_paths[] = {
	"/proc/self/fd/0",
	"/dev/tty",
	"/dev/tty0",
	"/dev/vc/0",
	"/dev/systty",
	"/dev/console",
	NULL
};

static const char default_color_set[NUM_COLORS][6] = {
	"000000","aa0000","00aa00","aa5500",
	"0000aa","aa00aa","00aaaa","aaaaaa",
	"555555","ff5555","55ff55","ffff55",
	"5555ff","ff55ff","55ffff","ffffff" };

/**
 * The palette struct is the type epxected by ioctl PIO_CMAP
 */
struct palette { unsigned char colors[NUM_COLORS * 16]; };

/**
 * Convert a list of colors in hex format to their actual hex formats suitable
 * for passing to the ioctl function
 */
static struct palette
get_palette_from_color_set(const char colors[][6])
{
	struct palette palette;
	unsigned int red, green, blue;
	int i, k;

	for (i = k = 0; i < NUM_COLORS; ++i)
	{
		if (sscanf(colors[i], "%2x%2x%2x", &red, &green, &blue) != 3)
			perror("Malformed hex color code");

		palette.colors[k++] = red;
		palette.colors[k++] = green;
		palette.colors[k++] = blue;
	}

	return palette;
}

/**
 * Get a file descriptor for a console to write to
 */
static int
get_console_fd(const char *console_path)
{
	int i, fd;

	// Use one of the default console paths
	if ( ! console_path)
	{
		for (i = 0; console_paths[i]; ++i)
		{
			if ((fd = get_console_fd(console_paths[i])) > 0)
				return fd;
		}

		return -1;
	}

	// Attempt to open the FD, and make sure it's a tty
	if ((fd = open(console_path, O_RDWR | O_NOCTTY)) < 0)
		return -1;

	return fd;
}


int main(int argc, char *argv[])
{
	// Default the color set to the default colors if none specified
	char color_set[NUM_COLORS][6];
	memcpy(color_set, default_color_set, NUM_COLORS * 6);

	if (ioctl(fd, PIO_CMAP, &new_colors) < 0)
		perror("Failed to set new color map on console");

	close(fd);
	return 0;
}
