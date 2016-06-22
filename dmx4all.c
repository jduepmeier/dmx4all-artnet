#include <stdio.h>
#include <errno.h>
#include <sys/fcntl.h>

bool check_connection(int fd) {

	if (write(fd, "C?", 2)  < 0) {
		perror(NULL);
		return false;
	}

	char ans;

	if (read(fd, &ans, 1) < 0) {
		perror(NULL);
		return false;
	}

	if (ans == 'G') {
		return true;
	} else {
		return false;
	}
}

bool set_output_channel(int fd) {

	if (write(fd, "N511", 4) < 0) {
		perror(NULL);
		return -1;
	}

	char ans[4];

	if (read(fd, ans, 4) < 0) {
		perror(NULL);
		return -1;
	}

	if (ans[3] != '=') {
		fprintf(stderr, "Last char is not a = (%c)\n", ans[3]);
		return false;
	}

	ans[3] = 0;
	int channels = strtol(ans, NULL, 10);

	printf("Number of output channels are now %d\n", channels);

	return true;
}

bool send_dmx_header(int fd, char start_lo, char start_hi, char len) {

	char out[4] = {255, 0, 0, 0};
	out[1] = start_lo;
	out[2] = start_hi;
	out[3] = len;


	if (write(fd, out, 4) < 0) {
		perror(NULL);
		return false;
	}

	return true;
}

bool send_dmx_512(int fd, char data[512]) {
	if (!send_dmx_header(fd, 0, 0, 200)) {
		return false;
	}

	if (write(fd, data, 200) < 0) {
		perror(NULL);
		return false;
	}

	char ans;

	if (read(fd, &ans, 1) < 0) {
		perror(NULL);
		return false;
	}

	if (!ans == 'G') {
		fprintf(stderr, "Last char is not a G (%c)\n", ans);
		return false;
	}
	if (!send_dmx_header(fd, 200, 0, 200)) {
		return false;
	}

	if (write(fd, data + 200, 200) < 0) {
		perror(NULL);
		return false;
	}

	if (read(fd, &ans, 1) < 0) {
		perror(NULL);
		return false;
	}

	if (!ans == 'G') {
		fprintf(stderr, "Last char is not a G (%c)\n", ans);
		return false;
	}
	if (!send_dmx_header(fd, 0x90, 0x01, 112)) {
		return false;
	}

	if (write(fd, data + 400, 112) < 0) {
		perror(NULL);
		return false;
	}

	if (read(fd, &ans, 1) < 0) {
		perror(NULL);
		return false;
	}

	if (!ans == 'G') {
		fprintf(stderr, "Last char is not a G (%c)\n", ans);
		return false;
	}
	return true;

}

int get_output_channel(int fd) {
	if (write(fd, "N?", 2) < 0) {
		perror(NULL);
		return -1;
	}

	char ans[4];

	if (read(fd, ans, 4) < 0) {
		perror(NULL);
		return -1;
	}

	if (ans[3] != 'G') {
		fprintf(stderr, "Last char is not a G (%s)\n", ans);
		return -1;
	}

	ans[3] = 0;
	return strtoul(ans, NULL, 10);
}

bool set_blackout(int fd, bool value) {

	char out[2] = "B0";
	if (value) {
		out[1] = '1';
	}

	if (write(fd, out, 2) < 0) {
		perror(NULL);
		return false;
	}

	char ans;

	if (read(fd, &ans, 1) < 0) {
		perror(NULL);
		return false;
	}

	return true;
}

bool is_blackout(int fd) {

	if (write(fd, "B?", 2) < 0) {
		perror(NULL);
		return false;
	}

	char ans[2];

	if (read(fd, &ans, 2) < 0) {
		perror(NULL);
		return false;
	}

	if (ans[1] != 'G') {
		fprintf(stderr, "Last char is not a G (%c)\n", ans[1]);
		return false;
	} else {
		ans[1] = 0;
	}

	return strtoul(ans, NULL, 10);
}

bool list_infos(int fd) {

	char buf[1024];

	if (write(fd, "I", 1) < 0) {
		perror(NULL);
		return false;
	}
	read(fd, buf, 1024);
	printf("%s\n", buf);

	return true;
}

int open_device(char* device_path) {

	int fd = open(device_path, O_RDWR);

	if (!fd) {
		fprintf(stderr, "Cannot open file on path %s.\n", device_path);
		perror(NULL);
		return 0;
	}

	if (check_connection(fd)) {
		list_infos(fd);
		if (is_blackout(fd)) {
			printf("Blackout is on.\n");
			set_blackout(fd, false);
		} else {
			printf("Blackout is off.\n");
		}

		int channel = get_output_channel(fd);

		if (channel < 0) {
			fprintf(stderr, "No output channel.\n");
			close(fd);
			return 0;
		}
		printf("Number of output channels: %d\n", channel);

		if (channel < 511) {
			set_output_channel(fd);
		}
		return fd;
	} else {
		fprintf(stderr, "Device is not an dmx4all device.\n");
		close(fd);
		return 0;
	}
}

