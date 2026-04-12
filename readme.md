# Praktikum Sistem Operasi Modul 2

### Soal 1

#### Penjelasan

Diberikan sebuah file csv `buku_hutang.csv` berisi data terkait hutang pelanggan. Termasuk juga nama, nominal dan status pembayaran. Kita ditugaskan untuk mengolah data tersebut kemudian juga menyimpan data pada folder yang rapi dan zip.

Pada penerapannya untuk membuat folder, memindahkan file, mengolah data dan membuat zip, kita menggunakan child process agar tidak mengganggu proses utama dan juga agar berjalan secara urut dan menghindari kerusakan karena adanya file atau folder yang belum terbuat.

**Mengatur child process**

```c
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
```

Mengapa dibuat menjadi function ialah pada awalnya di `main` terdapat 4 child process yang harus di fork dan di wait, sehingga akan terjadi if hell yang panjang dan sulit dibaca, maka dibuatlah function `run_process` untuk mempermudah pembacaan kode dan modularitas

Menggunakan logika proses yang sama yaitu fork, wait, dan error handling namun dibungkus dalam function `run_process` yang juga menerima argumen command array agar tidak menulis berulang execvnya

**Membuat direktori brankas_kedai**

```c
  char* cmkdir_args[] = {"mkdir", "brankas_kedai", NULL};
  run_process("/bin/mkdir", cmkdir_args);
```

Child pertama menggunakan commad `mkdir` untuk membuat folder dengan argumen `brankas_kedai` dan dijalankan menggunakan child process via `run_process`

**Memindahkan buku_hutang.csv ke direktori brankas_kedai**

```c
  char* ccp_args[] = {"cp", "-r", "buku_hutang.csv", "brankas_kedai/", NULL};
  run_process("/bin/cp", ccp_args);
```

Selanjutnya `cp` atau copy file digunakan untuk memindahkan file dengan argumen pertama adalah file yang dipindahkan dan argumen kedua adalah destinasi, disini adalah `brankas_kedai` sesuai child process yang pertama

**Mengolah data yang belum lunas pada buku_hutang.csv dan menyimpannya ke dalam file daftar_penunggak.txt**

```c
  char* cgrep_args[] = {
      "sh", "-c",
      "grep 'Belum Lunas' buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
      NULL};
  run_process("/bin/sh", cgrep_args);
```

Dikarenakan grep tidak termasuk pada `bin` namun ketentuan soal juga dilarang menggunakan `systemd`, maka kita menggunakan `sh` untuk mengeksekusi command `grep` untuk mencari per baris yangg mengandung 'Belum Lunas' dan menyimpan hasilnya di direktori `brankas_kedai/daftar_penunggak.txt`

**Membuat zip direktori brankas_kedai**

```c
  char* czip_args[] = {"sh", "-c", "zip -r rahasia_muthu.zip brankas_kedai",
                       NULL};
  run_process("/bin/sh", czip_args);
```

Child terakhir kita gunakan untuk mengompress folder `brankas_kedai` menjadi file `rahasia_muthu.zip` menggunakan command `zip` yang sama seperti sebelumnya tidak tersedia di `bin` sehingga harus menggunakan `/bin/sh`

#### Output

**Output utama (build exe , ls sebelum dan sesudah)**
![outputsoal1_001](./assets/Screenshot%202026-04-12%20170504.png)

**Output file penunggak**
![outputsoal1_002](./assets/Screenshot%202026-04-12%20170556.png)

**Output error handling**
![outputsoal1_003](./assets/Screenshot%202026-04-12%20170819.png)

#### Kendala

- awalnya kalau dari seslab dan modul dijelaskan cara spawning child process dengan fork dan if, namun ketika ada 3++ child maka akan susah dibaca dan di maintain

### Soal 2

#### Penjelasan

Pada soal kedua kita diminta untuk membuat daemon yang memonittor file `contract.txt` dan mencatat perubahannya pada file `work.log`. pada awalnya dibuat file `contract.txt` dengan isi

```
"A promise to keep going, even when unseen."
created at: 2026-04-11 10:34:24
```

kemudian jika file `contract.txt` dihapus atau kalo di edit maka daemon akan membuat file `contract.txt` kembali dengan isi

```
"A promise to keep going, even when unseen."
restored at: 2026-04-11 10:34:24
```

Semua aktifitas daemon akan dicatat pada file `work.log` dengan ketentuan, misalnya sukses dan tidak terjadi apa apa maka akan menambahkan baris `still working...` dengan akhiran status random berupa `[awake]`, `[drifting]`, `[numbness]` kemudian jika file `contract.txt` dihapus atau diedit maka akan menambahkan baris `contract violated.` di log.

Dan terakhir jika daemon di kill maka akan menutup log dengan baris `I really weren't meant to be together`.

**Handle write contract**

```c
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
```

Function yang menghandle pembuatan file `contract.txt` untuk pertama kali dan juga ketika file tersebut dihapus maupun diedit isinya

**Bandingkan isi contract.txt**

```c
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
```

Function boolean int yang membandingkan isi file `contract.txt` dengan isi yang seharusnya. Dilakukan perbandingan `strcmp` dikarenakan file memiliki baris yang lumayan sedikit sehingga hnaya menggunakan pendekatan yang sederhana

```c
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

```

Control flow utama pada daemon, pertama cek apakah file `contract.txt` ada atau tidak, jika tidak ada maka cek apakah ini pertama kali dibuat atau tidak, jika pertama kali maka buat file `contract.txt` dengan isi `created at:` jika tidak maka buat file `contract.txt` dengan isi `restored at:`

Jika file `contract.txt` ada maka cek apakah isi file `contract.txt` termodified atau tidak setiap iterasi, jika termodified maka buat file `contract.txt` dengan isi `restored at` dan menambahkan log `contract violated.`

**Handle kill signal**

```c
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
```

Function yang menghandle signal SIGINT dan SIGTERM, ketika signal diterima maka akan menambahkan log `I really weren't meant to be together` dan kemudian mematikan daemon loop dengan `isRunning = 0`.

**Main loop dan init daemon**

```c
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

   // --- refer ke control flow check contract yg sudah ada

    sleep(5);
  }

  return 0;
}

```

#### Output

**Output hasil file dan daemon berjalan (work.log)**
![outputsoal2_001](./assets/Screenshot%202026-04-12%20171855.png)

**Output file contract.txt (ketika normal dan saat terhapus/restore)**
![outputsoal2_002](./assets/Screenshot%202026-04-12%20171946.png)

**Output saat daemon di kill**
![outputsoal2_003](./assets/Screenshot%202026-04-12%20172054.png)

#### Kendala

- pengecekan contract.txt cukup tricky jika harus mengecek ke baris 2 , sehingga saya hanya mengecek baris pertama yang memang static

#### Revisi

- Error handling terlewat pada saat contract dihapus yang seharusnya menulis log "Contract violated" sebelum menulis contract lagi, revive

### Soal 3

#### Penjelasan

Mirip seperti soal sebelumnya, soal ini mengharuskan pengggunaan daemon. namun disini setiap 10 detik file `LoveLetter.txt` akan direwrite dengan pesan rahasia dengan beberapa pilihan pesan yang ualay dan acak dengan `srand`. namun agar tetap rahasia dilakukan enkripsi `base64` pada pesan tersebut.

Untuk mendecode file `LoveLetter.txt` dapat menggunakan argumen `-decrypt` pada executable file menggunakan `base64_decode` dan kemudian ditambahkan juga argumen `-kill` untuk mematikan daemon. dan juga handle jika tidak ada argumen maka akan mengirimkan pesan help berisi semua argumen yang valid , dan juga jika mengirimkan argumen selain yang valid maka akan menampilkan pesan error.

**Base64 Encoder dan Decoder**

```c
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
```

Kode fungsi encoder dan decoder diatas saya ambil dari stackoverflow dan tuning dengan AI, berfungsi pada bagian encode kalimat random dan decoder untuk `./angel -decrypt` yang mendecode isi file `LoveLetter.txt`

**Handle daemon untuk enkripsi dan write pesan**

```c
void encryptDaemon() {
  write_log("surprise", "running");
  pid_t pid = fork();

  if (pid < 0) exit(EXIT_FAILURE);
  if (pid > 0) exit(EXIT_SUCCESS);

  umask(0);
  setsid();
  write_log("surprise", "running");

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
```

Pada fungsi ini mula2 akan menggunakan template daemon seperti biasa, kemudian akan membuat file PID di folder tmp untuk menyimpan pid dari daemon (keperluan kill), kemudian akan masuk ke loop yang akan berjalan setiap 10 detik, pada loop ini akan membuat file `LoveLetter.txt` dan menuliskan kalimat random sesuai soal yang sudah diencode dengan base64, kemudian akan sleep selama 10 detik dan mengulangi prosesnya.

**Handle decrypt file**

```c
void decrpytFile() {
  write_log("decrypt", "running");

  FILE* f = fopen(OUTPUT_FILE, "r");
  if (!f) {
    printf("[ERROR] LoveLetter file tidak ditemukan");
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
```

Fungsi decrypt file `LoveLetter.txt` menggunakan `base64_decode` dan kemudian mencetak hasilnya ke console menggunakan printf.

**Handle kill daemon**

```c
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
```

Handle kill daemon menggunakan data PID dari file yang telah disimpan di `/tmp/` jika masih ada get PID nya kemudian lakukan kill(pid, SIGTERM) dan remove dari temporary dan menambahkan info jika daemon telah dimatikan.

#### Output

**Output ketika noargumen (help) dan ketika daemon berjalan**
![outputsoal3_001](./assets/Screenshot%202026-04-12%20172659.png)

**Output argumen -decrypt dan kill**
![outputsoal3_002](./assets/Screenshot%202026-04-12%20172751.png)

#### Kendala

- tidak ada kendala yang berarti, hanya cara mengubah nama executable file yang harus cari-cari dan coba-coba

#### Revisi
