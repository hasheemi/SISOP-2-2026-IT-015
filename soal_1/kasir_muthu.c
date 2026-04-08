#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void run_process(char* path, char* argv[]) {
  // init status dan pid
  pid_t pid = fork();
  int status;

  if (pid < 0) {
    // guard jika fork gagal
    perror("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // masuk child process
    execv(path, argv);
    perror("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan");
    exit(EXIT_FAILURE);
  }

  // wait child process selesai
  if (wait(&status) == -1) {
    perror("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan");
    exit(EXIT_FAILURE);
  }

  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    fprintf(stderr,
            "[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan\n");
    //  perror("");
    exit(EXIT_FAILURE);
  }
}

int main() {
  // child 1 buat folder brankas_kedaii
  // mkdir atau make directory
  char* cmkdir_args[] = {"mkdir", "brankas_kedai", NULL};
  run_process("/bin/mkdir", cmkdir_args);

  // child 2 pindahkan atau copy buku hutang ke dalam folder brankas_kedai
  // cp == copy ] dengan arg file dan destinasi
  char* ccp_args[] = {"cp", "-r", "buku_hutang.csv", "brankas_kedai/", NULL};
  run_process("/bin/cp", ccp_args);

  // child 3 olah data yang belum lunas pada buku hutang tulis seluruhh baris ke
  // daftar oenunggak
  // menggunakan command sh untuk execute command grep yang bisa get per satu
  // baris file
  char* cgrep_args[] = {
      "sh", "-c",
      "grep 'Belum Lunas' buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
      NULL};
  run_process("/bin/sh", cgrep_args);

  // child 4 buat zip direktorii brankas kedai
  char* czip_args[] = {"sh", "-c", "zip -r rahasia_muthu.zip brankas_kedai",
                       NULL};
  run_process("/bin/sh", czip_args);

  printf(
      "[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil "
      "diamankan.\n");

  return 0;
}

// file awal yg saya buat tapi masih if hell
// int main() {
//   // init pid4 proses dan status
//   pid_t pid1, pid2, pid3, pid4;
//   int status;

//   // start fork child 1
//   pid1 = fork();
//   if (pid1 == 0) {
//     // child1
//     // buat folder branks kedai
//     char* args[] = {"mkdir", "brankas_kedai", NULL};
//     execv("/bin/mkdir", args);

//     // perror("Gagal membuat folder brankas_kedai");
//     perror("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan");
//     exit(1);
//   } else {
//     // balik parent dan wait
//     wait(&status);
//     pid2 = fork();
//     if (pid2 == 0) {
//       // child2
//       // copy file buku_hutang ke folder brankas kedai
//       char* args[] = {"cp", "-r", "buku_hutang.csv", "brankas_kedai/", NULL};
//       execv("/bin/cp", args);

//       // perror("Gagal memindahkan file buku_hutang.csv");
//       perror("[ERROR] Aiyaa! Proses gagal, file atau folder tidak
//       ditemukan"); exit(1);
//     } else {
//       // balik parent dan wait
//       wait(&status);
//       pid3 = fork();
//       if (pid3 == 0) {
//         // child3 olah data yang belum lunas
//         execl("/bin/sh", "sh", "-c",
//               "grep 'Belum Lunas' buku_hutang.csv > "
//               "brankas_kedai/daftar_penunggak.txt",
//               NULL);

//         // perror("Gagal mencari belum lunas");
//         perror("[ERROR] Aiyaa! Proses gagal, file atau folder tidak
//         ditemukan"); exit(1);
//       } else {
//         // balik ke parent
//         wait(&status);
//         pid4 = fork();
//         if (pid4 == 0) {
//           // child 4
//           // buat zip
//           execl("/bin/sh", "sh", "-c", "zip -r rahasia_muthu.zip
//           brankas_keai",
//                 NULL);

//           perror(
//               "[ERROR] Aiyaa! Proses gagal, file atau folder tidak
//               ditemukan");
//         } else {
//           wait(&status);
//           printf(
//               "[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan
//               berhasil " "diamankan.\n");
//         }
//       }
//     }
//   }
// }
