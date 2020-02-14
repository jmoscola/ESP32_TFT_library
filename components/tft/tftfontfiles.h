/*
 * High level TFT functions
 * Author:  LoBo 04/2017, https://github/loboris
 * 
 */

#ifndef _TFTFONTFILES_H_
#define _TFTFONTFILES_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Compile font c source file to .fon file
 * which can be used in TFT_setFont() function to select external font
 * Created file have the same name as source file and extension .fon
 *
 * Params:
 *        fontfile: pointer to c source font file name; must have .c extension
 *             info: if set to 1, prints debug information
 *
 * Returns:
 *         0 on success
 *         err no on error
 *
 */
//------------------------------------------------
int load_file_font(const char * fontfile, int info);

/*
 * Compile font c source file to .fon file
 * which can be used in TFT_setFont() function to select external font
 * Created file have the same name as source file and extension .fon
 *
 * Params:
 *		fontfile: pointer to c source font file name; must have .c extension
 *			 dbg: if set to 1, prints debug information
 *
 * Returns:
 * 		0 on success
 * 		err no on error
 *
 */
//------------------------------------------------
int compile_font_file(char *fontfile, char *outputfile, uint8_t dbg);

#ifdef __cplusplus
}
#endif

#endif /* _TFTFONTFILES_H_ */
