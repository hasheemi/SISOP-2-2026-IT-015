#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define LOG_FILE "work.log"
#define CONTRACT_FILE "contract.txt"

// set isrunning
int isRunning = 1;
// handle kill  signal
void handleKill(int sig) {
  FILE* log = fopen(LOG_FILE, "a");
  if (log) {
    fprintf(log, "I really weren't meant to be together\n");
    fclose(log);
  } else {
    printf("[ERROR] log file tidak ditemukan");
    return;
  }
  // set isrunning 0 biar mati daemonnya
  isRunning = 0;
}

// cek apakah file ada
int file_exists(const char* filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

// handle buat file contract
void writeContract(int isRestore) {
  FILE* f = fopen(CONTRACT_FILE, "w");
  if (f) {
    // get time
    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    // baris1
    fprintf(f, "\"A promise to keep going, even when unseen.\"\n");

    // cek kalau restore tulis restore jika ngga tulis created
    // baris2
    if (isRestore) {
      fprintf(f, "restored at: %04d-%02d-%02d %02d:%02d:%02d\n",
              t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour,
              t->tm_min, t->tm_sec);
    } else {
      fprintf(f, "created at: %04d-%02d-%02d %02d:%02d:%02d\n",
              t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour,
              t->tm_min, t->tm_sec);
    }

    fclose(f);
  }
}

// func cek apakh contract berubah
int isContrMod() {
  FILE* f = fopen(CONTRACT_FILE, "r");
  if (!f) return 0;

  char buffer[256];
  fgets(buffer, sizeof(buffer), f);
  fclose(f);

  // cek baris 1 ajah
  return strcmp(buffer, "\"A promise to keep going, even when unseen.\"\n") !=
         0;
}

int main() {
  int isFirstCreate = 0;
  const char* status[] = {"[awake]", "[drifting]", "[numbness]"};

  pid_t pid;
  pid = fork();

  // template daemon
  // matikan parent
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  // set unmask
  umask(0);
  // set id baru
  if (setsid() < 0) {
    int isFirstCreate = 0;
    exit(EXIT_FAILURE);
  }

  // chdir("/");
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  signal(SIGINT, handleKill);
  signal(SIGTERM, handleKill);

  // loop deamon
  while (isRunning) {
    srand(time(NULL));

    FILE* log = fopen(LOG_FILE, "a");
    // append baris log
    if (log) {
      int idx = rand() % 3;
      fprintf(log, "still working... %s\n", status[idx]);
      fclose(log);
    } else {
      printf("[ERROR] log tidak ditemukan");
    }

    // jika tdk ada atau mungkin terhapus
    if (!file_exists(CONTRACT_FILE)) {
      if (!isFirstCreate) {
        writeContract(0);  // created pertama kali
        isFirstCreate = 1;
      } else {
        sleep(rand() % 2 + 1);
        writeContract(1);  // restored
      }
    } else {  // jika ada , kemudian cek apakah termodified
      if (isContrMod()) {
        FILE* log = fopen(LOG_FILE, "a");
        if (log) {
          fprintf(log, "contract violated.\n");
          fclose(log);
        } else {
          printf("[ERROR] log tidak ditemukan");
        }
        writeContract(1);  // restore ulang
      }
    }

    sleep(5);
  }

  return 0;
}
