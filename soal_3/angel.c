#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define PID_FILE "/tmp/angelll.pid"
#define OUTPUT_FILE "LoveLetter.txt"
#define LOG_FILE "ethereal.log"

char* kalimatalay[] = {
    "aku akan fokus pada diriku sendiri",
    "aku mencintaimu dari sekarang hingga selamanya",
    "aku akan menjauh darimu, hingga takdir mempertemukan kita di versi yang "
    "terbaik",
    "kalau aku dilahirkan kembali, aku tetap akan terus menyayangimu"};

// base64 from chatgpt and stackoverflow
const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* base64_encode(const char* input) {
  int len = strlen(input);
  int output_len = 4 * ((len + 2) / 3);
  char* encoded = malloc(output_len + 1);

  int i, j;
  for (i = 0, j = 0; i < len;) {
    int octet_a = i < len ? input[i++] : 0;
    int octet_b = i < len ? input[i++] : 0;
    int octet_c = i < len ? input[i++] : 0;

    int triple = (octet_a << 16) + (octet_b << 8) + octet_c;

    encoded[j++] = base64_chars[(triple >> 18) & 0x3F];
    encoded[j++] = base64_chars[(triple >> 12) & 0x3F];
    encoded[j++] = base64_chars[(triple >> 6) & 0x3F];
    encoded[j++] = base64_chars[triple & 0x3F];
  }

  for (int k = 0; k < (3 - len % 3) % 3; k++) encoded[output_len - 1 - k] = '=';

  encoded[output_len] = '\0';
  return encoded;
}

int base64_index(char c) {
  if ('A' <= c && c <= 'Z') return c - 'A';
  if ('a' <= c && c <= 'z') return c - 'a' + 26;
  if ('0' <= c && c <= '9') return c - '0' + 52;
  if (c == '+') return 62;
  if (c == '/') return 63;
  return -1;
}

char* base64_decode(const char* input) {
  int len = strlen(input);
  char* decoded = malloc(len);
  int i, j;

  for (i = 0, j = 0; i < len;) {
    int sextet_a = input[i] == '=' ? 0 : base64_index(input[i]);
    i++;
    int sextet_b = input[i] == '=' ? 0 : base64_index(input[i]);
    i++;
    int sextet_c = input[i] == '=' ? 0 : base64_index(input[i]);
    i++;
    int sextet_d = input[i] == '=' ? 0 : base64_index(input[i]);
    i++;

    int triple =
        (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d;

    decoded[j++] = (triple >> 16) & 0xFF;

    if (input[i - 2] != '=') decoded[j++] = (triple >> 8) & 0xFF;

    if (input[i - 1] != '=') decoded[j++] = triple & 0xFF;
  }

  decoded[j] = '\0';
  return decoded;
}

// func catat log dengan input biar modularr
void write_log(const char* process, const char* status) {
  FILE* f = fopen(LOG_FILE, "a");
  if (!f) return;

  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  // log tanggal, waktu dan nama status dan processnya
  fprintf(f, "[%02d:%02d:%04d]-[%02d:%02d:%02d]_%s_%s\n", t->tm_mday,
          t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec,
          process, status);

  fclose(f);
}

// masukkan template daemon pada fungsi
// fungsi surprise dan secret
void encryptDaemon() {
  write_log("surprise", "running");
  pid_t pid = fork();

  if (pid < 0) exit(EXIT_FAILURE);
  if (pid > 0) exit(EXIT_SUCCESS);

  umask(0);
  setsid();

  FILE* f = fopen(PID_FILE, "w");
  if (f) {
    fprintf(f, "%d", getpid());
    fclose(f);
  }

  // loop daemon
  while (1) {
    FILE* f = fopen(OUTPUT_FILE, "w");

    if (f) {
      // fungsi secret
      write_log("secret", "running");
      // get random kalimatnya bosss
      char* encoded = base64_encode(kalimatalay[rand() % 4]);
      fprintf(f, "%s\n", encoded);
      fclose(f);
      free(encoded);
      write_log("secret", "success");
    } else {
      write_log("secret", "error");
      printf("[ERROR] Secret error\n");
      return;
    }
    sleep(10);
    write_log("surprise", "success");
  }
}

// func handle kill daemon
void killDaemon() {
  write_log("kill", "running");
  FILE* f = fopen(PID_FILE, "r");
  if (!f) {
    write_log("kill", "error");
    printf("[ERROR] Daemon file tidak ditemukan\n");
    return;
  }
  int pid;
  // simpan pid ke
  fscanf(f, "%d", &pid);
  fclose(f);

  // setelag get id, kill dan remove dari temporary
  kill(pid, SIGTERM);
  remove(PID_FILE);
  write_log("kill", "success");
  printf("[INFO] Daemon dimatikan\n");
}

// handle decrypt file , panggil decoder
void decrpytFile() {
  write_log("decrypt", "running");

  FILE* f = fopen(OUTPUT_FILE, "r");
  if (!f) {
    printf("[ERROR] LoveLetter file tidak ditemukan\n");
    write_log("decrypt", "error");

    return;
  }

  char line[512];
  // get line stau persatu
  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\n")] = 0;
    char* decoded = base64_decode(line);
    printf("%s\n", decoded);
    free(decoded);
  }

  fclose(f);
  write_log("decrypt", "success");
}

int main(int argc, char* argv[]) {
  srand(time(NULL));

  // ubah nama menjadi maya
  memset(argv[0], 0, strlen(argv[0]));
  strcpy(argv[0], "maya");

  // handle help atau no argumen
  if (argc == 1) {
    printf("Penggunaan:\n");
    printf("./angel -daemon   : jalankan sebagai daemon\n");
    printf("./angel -decrypt  : decrypt LoveLetter.txt\n");
    printf("./angel -kill     : kill daemon\n");
    // printf("%s\n", base64_decode(base64_encode("selamanya")));
    return 0;
  }

  // handle jika ada argument
  //  handle daemon
  if (strcmp(argv[1], "-daemon") == 0) {
    encryptDaemon();
  } else if (strcmp(argv[1], "-decrypt") == 0) {
    // handle decrypt
    decrpytFile();
  } else if (strcmp(argv[1], "-kill") == 0) {
    // handle killl
    killDaemon();
  } else {
    // handle jika tdk ada
    printf("Argumen tidak dikenal\n");
  }

  return 0;
}
