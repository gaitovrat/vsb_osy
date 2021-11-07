#include <fcntl.h>
#include <semaphore.h>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <ctime>

#define N 100

typedef unsigned int uint;

int buffer[N];

class Semaphore {
private:
    sem_t *_semaphore;
    int _value;

    void updateValue() {
        sem_getvalue(_semaphore, &_value);
    }

public:
    static constexpr const char *EMPTY_NAME = "/sem_empty";
    static constexpr const char *FULL_NAME = "/sem_full";
    static constexpr const char *MUTEX_NAME = "/sem_mutex";

    Semaphore() : _semaphore(nullptr), _value(0) {}

    explicit Semaphore(const char *const name, int flag = O_RDWR) : Semaphore() {
        open(name, flag);
    }

    ~Semaphore() {
        if (opened()) {
            sem_close(_semaphore);
        }
    }

    void open(const char *const name, int flag = O_RDWR) {
        _semaphore = sem_open(name, flag);
        if (opened()) {
            updateValue();
        }
    }

    void open(const char *const name, uint value, int flag = O_CREAT | O_RDWR, mode_t mode = 0660) {
        _semaphore = sem_open(name, flag, mode, value);
        if (opened()) {
            updateValue();
        }
    }

    bool opened() {
        return _semaphore;
    }

    void down() {
        if (sem_trywait(_semaphore) < 0) {
            fprintf(stderr, "Critical section is occupied now. Wait for it.\n");
            if (sem_wait(_semaphore) < 0) {
                fprintf(stderr, "Unable to enter to the critical section.\n");
                exit(1);
            }
        }
        updateValue();
    }

    void up() {
        sem_post(_semaphore);
        updateValue();
    }

    [[nodiscard]] int getValue() const {
        return _value;
    }
};

struct Data {
    Semaphore &mutex;
    Semaphore &full;
    Semaphore &empty;

    void print(int item) const {
        int fullValue = full.getValue();
        int emptyValue = empty.getValue();

        printf("Full: %d, empty: %d, item: %d\n", fullValue, emptyValue, item);
    }
};

int random_number(int min = 0, int max = 100) {
    srand(time(nullptr));
    return (rand() % (max - min + 1)) + min;
}

void insert(int item, Data *data) {
    buffer[data->full.getValue()] = item;
}

int remove(Data *data) {
    int index = N - data->empty.getValue() - 1;
    int value = buffer[index];

    buffer[index] = 0;

    return value;
}

[[noreturn]] void *producer(void *ptr) {
    Data *data = (Data *) ptr;
    int item = 0;

    while (true) {
        item = random_number();
        data->empty.down();
        data->mutex.down();
        insert(item, data);
        data->mutex.up();
        data->full.up();
    }
}

[[noreturn]] void *consumer(void *ptr) {
    Data *data = (Data *) ptr;
    int item;

    while (true) {
        data->full.down();
        data->mutex.down();
        item = remove(data);
        data->mutex.up();
        data->empty.up();
        data->print(item);
    }
}

int main() {
    Semaphore mutex;
    Semaphore full;
    Semaphore empty(Semaphore::EMPTY_NAME);

    if (!empty.opened()) {
        fprintf(stderr, "Unable to open semaphore. Create new one.\n");

        empty.open(Semaphore::EMPTY_NAME, static_cast<uint>(N));
        full.open(Semaphore::FULL_NAME, static_cast<uint>(0));
        mutex.open(Semaphore::MUTEX_NAME, static_cast<uint>(1));

        if (!empty.opened() || !full.opened() || !mutex.opened()) {
            fprintf(stderr, "Unable to create three semaphores.\n");
            return EXIT_FAILURE;
        }

        printf("Semaphores created.\n");
    } else {
        full.open(Semaphore::FULL_NAME);
        mutex.open(Semaphore::MUTEX_NAME);
    }

    if (!empty.opened() || !full.opened() || !mutex.opened()) {
        fprintf(stderr, "Semaphores not available.\n");
    }

    Data data = {
            .mutex = mutex,
            .full = full,
            .empty = empty,
    };

    pthread_t produce;
    pthread_t consume;

    pthread_create(&produce, nullptr, producer, (void *) &data);
    pthread_create(&consume, nullptr, consumer, (void *) &data);

    pthread_join(produce, nullptr);
    pthread_join(consume, nullptr);

    return EXIT_SUCCESS;
}
