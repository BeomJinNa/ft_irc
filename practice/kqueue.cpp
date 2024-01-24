#include <fcntl.h>
#include <stdio.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	int kq = kqueue();
	if (kq == -1)
	{
		perror("kqueue");
		return 1;
	}

	// 관심 있는 파일 디스크립터 설정
	int fd = 0; // 표준 입력을 예로 들기 위함

	struct kevent changes;
	EV_SET(&changes, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

	struct kevent events;
	while (1)
	{
		int nev = kevent(kq, &changes, 1, &events, 1, NULL);
		if (nev < 0)
		{
			perror("kevent");
			break;
		}

		if (nev > 0)
		{
			if (events.filter == EVFILT_READ)
			{
				char buf[512];
				ssize_t n = read(fd, buf, sizeof(buf));
				if (n > 0)
				{
					// 입력 데이터 처리
					write(STDOUT_FILENO, buf, n);
				}
			}
		}
	}

	close(kq);
	return 0;
}
