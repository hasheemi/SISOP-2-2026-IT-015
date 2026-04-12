#include <stdio.h>
#include <stdlib.h>

int main (){
    FILE* fptr;
    char c;

    // Hint 1: Gunakan fungsi untuk membuka file "misteri.txt" dengan mode membaca (read)
    fptr = fopen("misteri.txt", "r");
    
    if (fptr == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    // Hint 2: Buatlah perulangan (loop) untuk membaca karakter demi karakter hingga akhir file
    while ((c = fgetc(fptr)) != EOF) {
        printf("[%c]", c);
    }

        // Hint 3: Di dalam perulangan, cetak karakter yang dibaca dengan format [%c]
        
    

    fclose(fptr);
    return 0;
}
