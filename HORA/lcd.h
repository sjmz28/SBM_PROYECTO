#ifndef __LCD_H
#define __LCD_H

 void LCD_Reset(void);
 void LCD_init(void);
 void LCD_update(void);
 void dataToBuffer(char data_L1[32], char data_L2[32]);

#endif
