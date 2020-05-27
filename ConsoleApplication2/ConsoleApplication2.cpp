#include <omp.h>
#include <iostream>
#include <cstdio>
#include "bitmap_image.hpp"

// Получить матрицы модифицированной интенсивности, а также модифицированные цветовые каналы 
//для красного, зеленого, синего и желтого канала при следующих значениях 
//a)	D = 12, S = 12  4096*4096
//b)	D = 13, S = 13  8192*8192
//c)	D = 14, S = 13  16384*8192

int main(void)
{ 
    bitmap_image image("input.bmp");      

    unsigned char r, g, b;
    const unsigned int h = image.height();
    const unsigned int w = image.width();

    std::cout << h << "*" << w << "\n";

    unsigned char *red = new unsigned char[w * h]();
    unsigned char *green = new unsigned char[w * h]();
    unsigned char *blue = new unsigned char[w * h]();
    unsigned char *intensity = new unsigned char[w * h]();
    unsigned char *yellow = new unsigned char[w * h]();


    int max = intensity[0];
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            rgb_t colour;
            image.get_pixel(i, j, colour);
     
          
            int avg = ((int)colour.red + (int)colour.green + (int)colour.blue) / 3;

            red[j * w + i] = colour.red;
            green[j * w + i] = colour.green;
            blue[j * w + i] = colour.blue;
            

            intensity[j * w + i] = (unsigned char)avg;
            if (intensity[j * w + i] >= max)
            {
                max = intensity[j * w + i]; 
            }
        }
        
    }
    std::cout << "max intensity: " << max << "\n";


        bitmap_image G(w, h);  
        omp_set_num_threads(6);
        double start_time = omp_get_wtime(); 

#pragma omp parallel for 
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                if (int(intensity[j * w + i]) < 0.1 * max)
                {
                    //std::cout << "intensity: " << int(intensity[j * w + i]) << "\n";
                    unsigned char a = int(intensity[0]);
                    G.set_pixel(i, j, a, a, a);  
                }

             
                else if ((int(intensity[j * w + i]) > 0.1 * max)) {
                    unsigned char a = (unsigned char)intensity[j * w + i];
                    unsigned char r = red[j * w + i] - (green[j * w + i] + blue[j * w + i]) / 2;
                    unsigned char g = green[j * w + i] - (red[j * w + i] + blue[j * w + i]) / 2;
                    unsigned char b = blue[j * w + i] - (green[j * w + i] + red[j * w + i]) / 2;
                    
                    unsigned char yellow = red[j * w + i] + green[j * w + i] - 2 * (abs(red[j * w + i] - green[j * w + i]) + blue[j * w + i]) / 2;
                    G.set_pixel(i, j, r, g, b); 
                } 
            }
        }

        double end_time = omp_get_wtime();  
        std::cout << "thread: " << end_time - start_time << "\n";
      
        G.save_image("output.bmp");
    
    delete[]red;
    delete[]green;
    delete[]blue;
    delete[]intensity;
    delete[]yellow;
    return 0;
}