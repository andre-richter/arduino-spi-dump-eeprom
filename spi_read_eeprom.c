/*
 * ==========================================================
 *
 * Small program for connecting to an Arduino running the
 * spi_read_eeprom.ino sketch.
 * 
 * Reads a number of bytes from the eeprom and saves them
 * in eeprom.bin
 *
 * Execute with --help to see instructions.
 *
 * ==========================================================
 *
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Andre Richter
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ==========================================================
 *
 * Author(s):
 *    Andre Richter, andre.o.richter a t gmail d o t com
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <getopt.h>

int ctrlc = 0;

void handle_sig(int signum)
{
	ctrlc = 1;
}

struct sigaction int_handler = {
	.sa_handler = handle_sig,
};

int to_termios_baudrate(unsigned int baudrate)
{
	switch(baudrate) {
	case 50:     return B50;
	case 75:     return B75;
	case 110:    return B110;
	case 134:    return B134;
	case 150:    return B150;
	case 200:    return B200;
	case 300:    return B300;
	case 600:    return B600;
	case 1200:   return B1200;
	case 1800:   return B1800;
	case 2400:   return B2400;
	case 4800:   return B4800;
	case 9600:   return B9600;
	case 19200:  return B19200;
	case 38400:  return B38400;
	case 57600:  return B57600;
	case 115200: return B115200;
	case 230400: return B230400;
	default:     return -1;
	}
}

static void print_usage(FILE *stream, const char *program_name)
{
	fprintf(stream, "Usage: %s [OPTION]...\n", program_name);
	fprintf(stream, "\n");
	fprintf(stream, "  -t, --tty=/dev/*           tty the arduino is connected to.\n");
	fprintf(stream, "  -b, --baudrate=N           baudrate of tty.\n");
	fprintf(stream, "  -n, --num_bytes=N          the number of bytes to read from the tty.\n");
	fprintf(stream, "  -h, --help                 print this screen.\n");
	exit(EXIT_FAILURE);
}

int transmit_num_bytes(int fd, unsigned int num_bytes)
{
	int i;
	unsigned char num_bytes_array[4];
    
	for (i = 0; i < 4; i++)
		num_bytes_array[i] = (unsigned char)((num_bytes >> (i * 8)) & 0xff);

	return write(fd, num_bytes_array, sizeof(num_bytes_array));
}

void eeprom_read(int fd, unsigned int num_bytes)
{
	int i = 0;
	unsigned char byte;
	FILE *fp = fopen("eeprom.bin", "w");

	while (!ctrlc) {
		if (read(fd, &byte, sizeof(byte)) < 0) {
			if (errno == EINTR)
				printf("\nInterrupted by CTRL-C\n");
			else
				printf("\nInterrupted, errno: %d\n", errno);
		}
		else {
			printf("0x%08x: 0x%02x\n", i, byte);		
			fwrite(&byte, sizeof(char), 1, fp);
			i++;
			if (i == num_bytes)
				break;            
		}
	}
	fclose(fp);
	return;
}
    
int main(int argc, char *argv[])
{		
	int fd, err;
	char *tty_name = NULL;
	unsigned int num_bytes = 0;
	int baudrate = B115200, baudrate_by_user = 0;
	struct termios tty_attr, tty_attr_orig;
	speed_t i_speed, o_speed, user_speed;

	int next_option;	
	extern char *optarg;  	
	const char* short_options = "t:b:n:h";
	const struct option long_options[] = {
		{ "tty",        required_argument, NULL, 't' },
		{ "baudrate",   required_argument, NULL, 'b' },
		{ "num_bytes",  required_argument, NULL, 'n' },
		{ "help",       no_argument,       NULL, 'h' },
		{ 0,            0,                 0,     0  }
	};

	do {
		next_option = getopt_long(argc, argv, short_options,long_options, NULL);
		switch(next_option) {
		case 't':
			tty_name = optarg;
			break;
		case 'b':
			baudrate = to_termios_baudrate(atoi(optarg));
			baudrate_by_user = 1;
			break;
		case 'n':
			num_bytes = atoi(optarg);
			break;
		case 'h':
			print_usage(stdout, argv[0]);
			break;
		case '?':
			print_usage(stderr, argv[0]);
			break;
		case -1:
			break;
		default:
			abort();
		}
	} while(next_option != -1);

	/* check if setup parameters given and valid */
	if (baudrate < 0) {
		printf("\nSorry, you did not provide a valid baudrate.\n\n");
		print_usage(stdout, argv[0]);
	} else if (!baudrate_by_user)
		printf("You did not provide a baudrate, defaulting to 115200.\n");
	user_speed = (speed_t)baudrate;

	if (!tty_name) {
		printf("\nSorry, you did not provide the name of "
		       "your arduino's tty device.\n\n");
		print_usage(stdout, argv[0]);
	}
    
	if (!num_bytes) {
		printf("\nSorry, you did not provide how many bytes you "
		       "want to read out from the eeprom.\n\n");
		print_usage(stdout, argv[0]);
	}

	/* Install ctrl-c signal handler */
	sigaction(SIGINT, &int_handler, 0);
	
	/* Open tty */
	fd = open(tty_name, O_RDWR);
	if (fd == -1) {
		printf("Error opening %s\n"
		       "Did you give me the correct tty?\n",
		       tty_name);
		return EXIT_FAILURE;
	}
	printf("%s open.\n", tty_name);

	/* Get tty config and make backup */
	tcgetattr(fd, &tty_attr);
	tty_attr_orig = tty_attr;

	/* Configure tty */
	i_speed = cfgetispeed(&tty_attr);
	if (i_speed != user_speed)
		cfsetispeed(&tty_attr, user_speed);
	
	o_speed = cfgetospeed(&tty_attr);
	if (o_speed != user_speed)
		cfsetospeed(&tty_attr, user_speed);

	tty_attr.c_cc[VMIN]  = 1; /* Block reads until 1 byte is available */
	tty_attr.c_cc[VTIME] = 0; /* Never return from read due to timeout */

	tty_attr.c_iflag &= ~(ICRNL | IXON);
	tty_attr.c_oflag &= ~OPOST;
	tty_attr.c_lflag &= ~(ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
	
	err = tcsetattr(fd, TCSANOW, &tty_attr);
	if (err) {
		printf("Error while setting tty options.\n");
		goto out_fd;
	}

	printf("tty successfully configured.\n");
	sleep(1); /* Wait a second; Prevents that first byte send to arduino gets corrupted */
    
	if ((transmit_num_bytes(fd, num_bytes)) < 0) {
		printf("Error while transfering dump size to arduino.\n");
		err = EXIT_FAILURE;
	} else {
		eeprom_read(fd, num_bytes);
	}

	/* Revert to original tty config */
	tcsetattr(fd, TCSANOW, &tty_attr_orig);

out_fd:
	close(fd);
	return err;
}
