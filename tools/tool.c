#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(X, NAME) ({\
	int ret = X; \
	printf("%s: %d (e:%d)\n", NAME, ret, errno); \
	if (ret < 0) return 1; \
	ret; \
})

int unescape(const char* s, char** out, int* length) {
	int len = strlen(s);
	int result = 0;
	char* start = calloc(len, sizeof(char));
	char* position = start;
	*out = position;
	for (int i=0; i<len; i++) {
		char c = s[i];
		if (s[i] == '\\' && i != len-1) {
			int correct = 1;
			switch(s[i+1]) {
				case '\\': c='\\'; break;
				case 'n': c='\n'; break;
				case 'r': c='\r'; break;
				case '0': c='\0'; break;
				default: correct = 0; break;
			}
			if (correct != 0) {
				i++;
				result++;
			}
		}
		*(position++) = c;
	}
	*length = position - start;
	return result;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("First arg should be char device.");
		return 1;
	}
	char* filename = argv[1];
	if (argc > 2) {
		for (int i=2; i<argc; i++) {
			int fd = CHECK(open(filename, O_RDWR), "open");

			char* s;
			int len;
			int unesc = unescape(argv[i], &s, &len);
			printf("\tUnescaped %d; writing %d\n", unesc, len);

			CHECK(write(fd, s, len), "write");

			CHECK(close(fd), "close");
		}
	} else {
		int size = 1024*16;
		char* buf = malloc(size);

		int fd = CHECK(open(filename, O_RDWR), "open");
		int bytes_read = CHECK(read(fd, buf, size), "read");
		CHECK(close(fd), "close");

		write(2, buf, bytes_read);
	}
}
