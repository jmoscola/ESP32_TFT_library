/* TFT module
 *
 */

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "esp_log.h"
#include "tftfontfiles.h"


uint8_t *userfont = NULL;


int load_file_font(const char * fontfile, int info)
{
    int err = 0;
    char err_msg[256] = {'\0'};
    
    ESP_LOGI("LOAD_FILE_FONT", "Loading font file %s ...", fontfile);
    
    if (userfont != NULL) {
        free(userfont);
        userfont = NULL;
    }
    
    struct stat sb;
    
    // Open the file
    FILE *fhndl = fopen(fontfile, "r");
    if (!fhndl) {
        //        sprintf(err_msg, "Error opening font file '%s'", fontfile);
        ESP_LOGE("LOAD_FILE_FONT", "Error opening font file '%s'", fontfile);
        err = 1;
        goto exit;
    }
    
    // Get file size
    if (stat(fontfile, &sb) != 0) {
        //        sprintf(err_msg, "Error getting font file stats");
        ESP_LOGE("LOAD_FILE_FONT", "Error getting font file stats");
        err = 2;
        goto exit;
    }
    int fsize = sb.st_size;
    if (fsize < 30) {
        //        sprintf(err_msg, "Error getting font file size");
        ESP_LOGE("LOAD_FILE_FONT", "Error getting font file size");
        err = 3;
        goto exit;
    }
    
    userfont = malloc(fsize+4);
    if (userfont == NULL) {
        //        sprintf(err_msg, "Font memory allocation error");
        ESP_LOGE("LOAD_FILE_FONT", "Font memory allocation error");
        fclose(fhndl);
        err = 4;
        goto exit;
    }
    
    int read = fread(userfont, 1, fsize, fhndl);
    
    fclose(fhndl);
    
    if (read != fsize) {
        //        sprintf(err_msg, "Font read error");
        ESP_LOGE("LOAD_FILE_FONT", "Font read error");
        err = 5;
        goto exit;
    }
    
    userfont[read] = 0;
    if (strstr((char *)(userfont+read-8), "RPH_font") == NULL) {
        //        sprintf(err_msg, "Font ID not found");
        ESP_LOGE("LOAD_FILE_FONT", "Font ID not found");
        err = 6;
        goto exit;
    }
    
    // Check size
    int size = 0;
    int numchar = 0;
    int width = userfont[0];
    int height = userfont[1];
    uint8_t first = 255;
    uint8_t last = 0;
    //int offst = 0;
    int pminwidth = 255;
    int pmaxwidth = 0;
    
    if (width != 0) {
        // Fixed font
        numchar = userfont[3];
        first = userfont[2];
        last = first + numchar - 1;
        size = ((width * height * numchar) / 8) + 4;
    }
    else {
        // Proportional font
        size = 4; // point at first char data
        uint8_t charCode;
        int charwidth;
        
        do {
            charCode = userfont[size];
            charwidth = userfont[size+2];
            
            if (charCode != 0xFF) {
                numchar++;
                if (charwidth != 0) size += ((((charwidth * userfont[size+3])-1) / 8) + 7);
                else size += 6;
                
                if (info) {
                    if (charwidth > pmaxwidth) pmaxwidth = charwidth;
                    if (charwidth < pminwidth) pminwidth = charwidth;
                    if (charCode < first) first = charCode;
                    if (charCode > last) last = charCode;
                }
            }
            else size++;
        } while ((size < (read-8)) && (charCode != 0xFF));
    }
    
    if (size != (read-8)) {
        //        sprintf(err_msg, "Font size error: found %d expected %d)", size, (read-8));
        ESP_LOGE("LOAD_FILE_FONT", "Font size error: found %d expected %d)", size, (read-8));
        err = 7;
        goto exit;
    }
    
    if (info) {
        if (width != 0) {
            //            printf("Fixed width font:\r\n  size: %d  width: %d  height: %d  characters: %d (%d~%d)\n",
            //                   size, width, height, numchar, first, last);
            ESP_LOGI("LOAD_FILE_FONT", "Fixed width font:\r\n  size: %d  width: %d  height: %d  characters: %d (%d~%d)\n",
                     size, width, height, numchar, first, last);
        }
        else {
            //            printf("Proportional font:\r\n  size: %d  width: %d~%d  height: %d  characters: %d (%d~%d)\n",
            //                   size, pminwidth, pmaxwidth, height, numchar, first, last);
            ESP_LOGI("LOAD_FILE_FONT", "Proportional font:\r\n  size: %d  width: %d~%d  height: %d  characters: %d (%d~%d)\n",
                     size, pminwidth, pmaxwidth, height, numchar, first, last);
        }
    }
    
exit:
    if (err) {
        if (userfont) {
            free(userfont);
            userfont = NULL;
        }
        if (info) {
            //            printf("Error: %d [%s]\r\n", err, err_msg);
            ESP_LOGE("LOAD_FILE_FONT", "Error: %d [%s]\r\n", err, err_msg);
        }
    }
    return err;
}


//------------------------------------------------
int compile_font_file(char *fontfile, char *outputfile, uint8_t dbg)
{
    int err = 0;
    char err_msg[128] = {'\0'};
    char outfile[128] = {'\0'};
    size_t len;
    size_t oflen;
    struct stat sb;
    FILE *ffd = NULL;
    FILE *ffd_out = NULL;
    char *sourcebuf = NULL;
    
    len = strlen(fontfile);
    oflen = strlen(outputfile);
    
    // check here that input filename end with ".c".
    if ((len < 3) || (len > 125) || (strcmp(fontfile + len - 2, ".c") != 0)) {
        sprintf(err_msg, "input file not a .c file");
        err = 1;
        goto exit;
    }
    
    // check here that output filename end with ".fon".
    if ((oflen < 3) || (oflen > 125) || (strcmp(outputfile + oflen - 4, ".fon") != 0)) {
        sprintf(err_msg, "output file not a .fon file");
        err = 1;
        goto exit;
    }
    
    sprintf(outfile, "%s", outputfile);
    
    // Open the source file
    if (stat(fontfile, &sb) != 0) {
        sprintf(err_msg, "Error opening source file '%s'", fontfile);
        err = 2;
        goto exit;
    }
    // Open the file
    ffd = fopen(fontfile, "rb");
    if (!ffd) {
        sprintf(err_msg, "Error opening source file '%s'", fontfile);
        err = 3;
        goto exit;
    }
    
    // Open the font file
    ffd_out= fopen(outfile, "wb");
    if (!ffd_out) {
        sprintf(err_msg, "error opening destination file");
        err = 4;
        goto exit;
    }
    
    // Get file size
    int fsize = sb.st_size;
    if (fsize <= 0) {
        sprintf(err_msg, "source file size error");
        err = 5;
        goto exit;
    }
    
    sourcebuf = malloc(fsize+4);
    if (sourcebuf == NULL) {
        sprintf(err_msg, "memory allocation error");
        err = 6;
        goto exit;
    }
    char *fbuf = sourcebuf;
    
    int rdsize = fread(fbuf, 1, fsize, ffd);
    fclose(ffd);
    ffd = NULL;
    
    if (rdsize != fsize) {
        sprintf(err_msg, "error reading from source file");
        err = 7;
        goto exit;
    }
    
    *(fbuf+rdsize) = '\0';
    
    fbuf = strchr(fbuf, '{');			// beginning of font data
    char *fend = strstr(fbuf, "};");	// end of font data
    
    if ((fbuf == NULL) || (fend == NULL) || ((fend-fbuf) < 22)) {
        sprintf(err_msg, "wrong source file format");
        err = 8;
        goto exit;
    }
    
    fbuf++;
    *fend = '\0';
    char hexstr[5] = {'\0'};
    int lastline = 0;
    
    fbuf = strstr(fbuf, "0x");
    int size = 0;
    char *nextline;
    char *numptr;
    
    int bptr = 0;
    
    while ((fbuf != NULL) && (fbuf < fend) && (lastline == 0)) {
        nextline = strchr(fbuf, '\n'); // beginning of the next line
        if (nextline == NULL) {
            nextline = fend-1;
            lastline++;
        }
        else nextline++;
        
        while (fbuf < nextline) {
            numptr = strstr(fbuf, "0x");
            if ((numptr == NULL) || ((fbuf+4) > nextline)) numptr = strstr(fbuf, "0X");
            if ((numptr != NULL) && ((numptr+4) <= nextline)) {
                fbuf = numptr;
                if (bptr >= 128) {
                    // buffer full, write to file
                    if (fwrite(outfile, 1, 128, ffd_out) != 128) goto error;
                    bptr = 0;
                    size += 128;
                }
                memcpy(hexstr, fbuf, 4);
                hexstr[4] = 0;
                outfile[bptr++] = (uint8_t)strtol(hexstr, NULL, 0);
                fbuf += 4;
            }
            else fbuf = nextline;
        }
        fbuf = nextline;
    }
    
    if (bptr > 0) {
        size += bptr;
        if (fwrite(outfile, 1, bptr, ffd_out) != bptr) goto error;
    }
    
    // write font ID
    sprintf(outfile, "RPH_font");
    if (fwrite(outfile, 1, 8, ffd_out) != 8) goto error;
    
    fclose(ffd_out);
    ffd_out = NULL;
    
    // === Test compiled font ===
    sprintf(outfile, "%s", outputfile);
    
    uint8_t *uf = userfont; // save userfont pointer
    userfont = NULL;
    if (load_file_font(outfile, 1) != 0) {
        sprintf(err_msg, "Error compiling file!");
        err = 10;
    }
    else {
        free(userfont);
        sprintf(err_msg, "File compiled successfully.");
    }
    userfont = uf; // restore userfont
    
    goto exit;
    
error:
    sprintf(err_msg, "error writing to destination file");
    err = 9;
    
exit:
    if (sourcebuf) free(sourcebuf);
    if (ffd) fclose(ffd);
    if (ffd_out) fclose(ffd_out);
    
    if (dbg) printf("%s\r\n", err_msg);
    
    return err;
}
