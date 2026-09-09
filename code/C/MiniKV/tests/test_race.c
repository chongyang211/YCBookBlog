/* test_race（§10 造 BUG 实验）：裸 int 计数器两线程各加 1000
 * 期望 2000，实际 1700~2000 摆动——counter++ 三步真身被插队
 * 对照组：atomic 版永远 2000 */
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>

static int counter = 0;               /* 裸 int，未保护 */
static atomic_int atomic_counter = 0; /* 对照组 */

static void *adder(void *arg) {
    (void)arg;
    for (int i = 0; i < 1000; i++) {
        counter++;                    /* 看似一句，实为 load/add/store 三步 */
        atomic_fetch_add(&atomic_counter, 1);
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, adder, NULL);
    pthread_create(&t2, NULL, adder, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("bare counter   = %d (expect 2000, may lose updates)\n", counter);
    printf("atomic counter = %d (expect 2000, always)\n",
           atomic_load(&atomic_counter));
    return atomic_load(&atomic_counter) == 2000 ? 0 : 1;
}
