/*
 * 2_alarm.c — 정해진 주기마다 시그널을 반복해서 받는다
 *
 * [핵심 개념]
 *   alarm(n) 은 1회성이므로 핸들러가 실행될 때 다시 alarm(n)을 걸어야 반복된다.
 *   명령행 인자로 간격(초)과 반복 횟수를 전달받아 실행한다.
 *   출처: man 2 alarm, man 2 sigaction
 *
 * [컴파일·실행]
 *   gcc -Wall -Wextra -o 2_alarm 2_alarm.c
 *   ./2_alarm 2 5      # 2초 간격으로 5회 반복
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t alarm_triggered = 0;
static int interval_sec = 0;

static void on_alarm(int sig)
{
    (void)sig;
    alarm_triggered = 1;
    alarm(interval_sec);   /* 핸들러가 처리한 뒤 다시 알람을 걸어 주기적으로 반복 */
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("사용법: %s <간격초> <반복횟수>\n", argv[0]);
        return 1;
    }

    int max_count = 0;
    sscanf(argv[1], "%d", &interval_sec);
    sscanf(argv[2], "%d", &max_count);

    struct sigaction sa;
    sa.sa_handler = on_alarm;   /* SIGALRM 이 오면 이 함수를 부르게 등록한다 */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    printf("타이머 시작: %d초 간격으로 %d회 반복합니다.\n", interval_sec, max_count);

    alarm(interval_sec);   /* 첫 번째 알람 예약 */

    int count = 0;
    while (count < max_count) {
        pause();   /* SIGALRM 시그널이 올 때까지 대기 */
        if (alarm_triggered) {
            alarm_triggered = 0;
            count++;
            printf("[알람 %d/%d] %d초 경과\n", count, max_count, interval_sec);
        }
    }

    alarm(0);   /* 반복이 끝났으므로 예약 취소 */
    printf("타이머가 완료되어 프로그램을 종료합니다.\n");
    return 0;
}