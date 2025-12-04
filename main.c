    #include<string.h>
    #include <avr/io.h>
    #include <util/delay.h>
    #include "uart.h"

    void f1(){
        const char * response = "command 1";
        UART__write_chars(response);
    }

    void f2(){
        const char * response = "command 2";
        UART__write_chars(response);
    }

    void f3(){
        const char * response = "command 3";
        UART__write_chars(response);
    }


    int main(void) {
        UART__init();
        UART__init_stdio();


        const char * sentence = "type your command";
        UART__write((const uint8_t*) sentence, strlen(sentence));
        
        while (1) {
            uint8_t command = UART__getc();
            
            switch (command) {
                case '1':
                    f1();
                    break;
                    
                case '2':
                    f2();
                    break;
                    
                case '3':
                    f3();
                    break;
                    
                default:
                    break;
            }
        }
        
        return 0;
    }
