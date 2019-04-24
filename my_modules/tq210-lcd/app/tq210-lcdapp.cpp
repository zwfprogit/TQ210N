#include <iostream>
 
unsigned long buffer[480][800] = {0};
 
void put_long_hex(unsigned long v){ 
    for(int i = 0; i != 4; ++i){
        std::cout.put(static_cast<char>(0xff&(v>>(8*(3-i)))));
    }   
}
 
int main(){
    for(int i = 0; i != 480; ++i){
        buffer[i][0]   = 0x00ff0000;
        buffer[i][799] = 0x0000ff00;
    }   
 
    for(int i = 0; i != 800; ++i){
        buffer[0][i]   = 0xff000000;
        buffer[479][i] = 0x00ffff00;
    }   
 
    for(int i = 0; i != 480; ++i){
        for(int j = 0; j != 800; ++j){
            put_long_hex(buffer[i][j]); 
        }   
    }   
}