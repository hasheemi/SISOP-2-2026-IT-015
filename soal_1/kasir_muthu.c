#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  // init pid4 proses dan status
  pid_t pid1, pid2, pid3, pid4;
  int status;

  // start fork child 1
  pid1 = fork();
  if (pid1 == 0) {
    // child1
    // buat folder branks kedai
    char* args[] = {"mkdir", "brankas_kedai", NULL};
    execv("/bin/mkdir", args);

    perror("Gagal membuat folder brankas_kedai");
    perror("ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan");
    exit(1);
  } else {
    // balik parent dan wait
    wait(&status);
    pid2 = fork();
    if (pid2 == 0) {
      // child2
      // copy file buku_hutang ke folder brankas kedai
      char* args[] = {"cp", "-r", "buku_hutang.csv", "brankas_kedai/", NULL};
      execv("/bin/cp", args);

      perror("Gagal memindahkan file buku_hutang.csv");
      perror("ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan");
      exit(1);
    } else {
      // balik parent dan wait
      wait(&status);
      pid3 = fork();
      if (pid3 == 0) {
        // child3 olah data yang belum lunas
        execl("/bin/sh", "sh", "-c",
              "grep 'Belum Lunas' buku_hutang.csv > "
              "brankas_kedai/daftar_penunggak.txt",
              NULL);

        perror("Gagal mencari belum lunas");
        perror("ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan");
        exit(1);
      } else {
        // balik ke parent
        wait(&status);
        pid4 = fork();
        if (pid4 == 0) {
          // child 4
          // buat zip
          execl("/bin/sh", "sh", "-c", "zip -r rahasia_muthu.zip brankas_kedai",
                NULL);

          perror(
              "ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan");
        } else {
          wait(&status);
          printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");
        }
      }
    }
  }
}
