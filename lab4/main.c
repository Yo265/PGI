#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <argz.h>
#include "bitmap.h"

static XImage *CreateImageFromBuffer(Display*, const unsigned char *, long int, long int);


int main(int argc, char *argv[]) {
    struct BITMAPINFOHEADER inf;
    struct BITMAPINFO header;
    int bmp;
    char filename[] = "../Data/4bit bmp.bmp";
    if ((bmp = open(filename, O_RDONLY)) == -1){
        perror("Error while opening file");
        exit(1);
    }

    if(read(bmp, &header, sizeof(struct BITMAPINFO)) != sizeof(struct BITMAPINFO)){
        perror("Error while reading header from file");
        exit(1);
    }
    if(read(bmp, &inf, sizeof(struct BITMAPINFOHEADER)) != sizeof(struct BITMAPINFOHEADER)){
        perror("Error while reading info from file");
        exit(1);
    }

    Display *dis;
    Window win;
    XEvent event;
    GC gc;

    XImage *image;


    if ((dis = XOpenDisplay(getenv("DISPLAY"))) == NULL) {
        printf("Can't connect X server:%s\n", strerror(errno));
        exit(1);
    }

    win = XCreateSimpleWindow(dis, RootWindow(dis, DefaultScreen(dis)), 0, 0, inf.biWidth, inf.biHeight, 5,
                              BlackPixel(dis, DefaultScreen(dis)), WhitePixel(dis, DefaultScreen(dis)));

    XSetStandardProperties(dis, win, argv[1], argv[0], None, argv, argc, NULL);
    gc = DefaultGC(dis, DefaultScreen(dis));

    switch (inf.biBitCount) {
        case 4: {

            unsigned char * color_table;
            size_t cl_size;
            cl_size = header.bfOffBits - sizeof(header) - sizeof (inf);

            color_table = (unsigned char*) malloc(cl_size);

            if(color_table == NULL){
                perror("Color table allocation error");
                exit(1);
            }

            if(read(bmp, color_table, cl_size) != cl_size){
                perror("Error while reading color table");
                exit(1);
            }

            long int width = inf.biWidth;
            long int height = inf.biHeight;
            unsigned long size;

            if(inf.biSizeImage == 0){
                size = width + width % 4;
                size *= height;
            }else
                size = inf.biSizeImage;


            unsigned char * buff = malloc(size);
            if(buff == NULL){
                perror("Buffer allocation error");
                exit(1);
            }

            lseek(bmp, header.bfOffBits, SEEK_SET);
            if(read(bmp, buff, size) != size){
                perror("Possible error while reading pixel table");
                exit(1);
            }
            close(bmp);

            unsigned long new_size;
            new_size = width * 3 + width % 4;
            new_size *= height;

            unsigned char* new_buff = (unsigned char*)malloc(new_size);

            if(new_buff == NULL){
                perror("Error while allocate new pixel buffer");
                exit(1);
            }
            int k = 0;
            for(size_t i = 0; i < height; ++i){
//                k = 0;
                if( i == 100)
                    break;
                for(size_t j = 0; j < width * 3; j += 6){
                    unsigned char b = color_table[(buff[k] >> 4) * 4];
                    unsigned char g = color_table[(buff[k] >> 4) * 4 + 1];
                    unsigned char r = color_table[(buff[k] >> 4) * 4 + 2];
                    new_buff[i * (width * 3 + width % 4) + j] = b;
                    new_buff[i * (width * 3 + width % 4) + j + 1] = g;
                    new_buff[i * (width * 3 + width % 4) + j + 2] = r;
                    printf("%d\n", k);
                    b = color_table[(buff[k] & 0xF) * 4];
                    g = color_table[(buff[k] & 0xF) * 4 + 1];
                    r = color_table[(buff[k] & 0xF) * 4 + 2];
                    new_buff[i * (width * 3 + width % 4) + j + 3] = b;
                    new_buff[i * (width * 3 + width % 4) + j + 4] = g;
                    new_buff[i * (width * 3 + width % 4) + j + 5] = r;
                    ++k;
                }
            }

            image = CreateImageFromBuffer(dis, new_buff, width, height);
            break;
        }
        case 8:{
            unsigned char * color_table;
            size_t cl_size;
            cl_size = header.bfOffBits - sizeof(header) - sizeof (inf);

            color_table = (unsigned char*) malloc(cl_size);

            if(color_table == NULL){
                perror("Color table allocation error");
                exit(1);
            }

            if(read(bmp, color_table, cl_size) != cl_size){
                perror("Error while reading color table");
                exit(1);
            }

            long int width = inf.biWidth;
            long int height = inf.biHeight;
            unsigned long size;

            if(inf.biSizeImage == 0){
                size = width + width % 4;
                size *= height;
            }else
                size = inf.biSizeImage;


            unsigned char * buff = malloc(size);
            if(buff == NULL){
                perror("Buffer allocation error");
                exit(1);
            }

            lseek(bmp, header.bfOffBits, SEEK_SET);
            if(read(bmp, buff, size) != size){
                perror("Possible error while reading pixel table");
                exit(1);
            }
            close(bmp);

            unsigned long new_size;
            new_size = width * 3 + width % 4;
            new_size *= height;

            unsigned char* new_buff = (unsigned char*)malloc(new_size);

            if(new_buff == NULL){
                perror("Error while allocate new pixel buffer");
                exit(1);
            }


            for(size_t i = 0; i < height; ++i){
                for(size_t j = 0; j < width * 3; j += 3){
                    unsigned char b = color_table[(buff[i * (width + width % 4) + j/3]) * 4];
                    unsigned char g = color_table[(buff[i * (width + width % 4) + j/3]) * 4 + 1];
                    unsigned char r = color_table[(buff[i * (width + width % 4) + j/3]) * 4 + 2];
                    new_buff[i * (width * 3 + width % 4) + j] = b;
                    new_buff[i * (width * 3 + width % 4) + j + 1] = g;
                    new_buff[i * (width * 3 + width % 4) + j + 2] = r;
                }
            }
            image = CreateImageFromBuffer(dis, new_buff, width, height);
            break;
        }
        case 24:{
            long int width = inf.biWidth;
            long int height = inf.biHeight;
            unsigned long size;

            if(inf.biSizeImage == 0){
                size = width + width % 4;
                size *= height;
            }else
                size = inf.biSizeImage;


            unsigned char * buff = malloc(size);
            if(buff == NULL){
                perror("Buffer allocation error");
                exit(1);
            }

            lseek(bmp, header.bfOffBits, SEEK_SET);
            if(read(bmp, buff, size) != size){
                perror("Possible error while reading pixel table");
                exit(1);
            }
            close(bmp);
            image = CreateImageFromBuffer(dis, buff, width, height);
            break;
        }
        default:{
            perror("Incorrect BitOrder");
            exit(1);
        }

    }

    XMapWindow(dis, win);
    XSelectInput(dis, win, ExposureMask | KeyPressMask);
    while (1) {
        XNextEvent(dis, &event);
        if (event.xany.window == win) {
            switch (event.type) {
                case Expose:
                    XPutImage(dis, win, gc, image, 0, 0, 0, 0, image->width, image->height);
                    break;

                case KeyPress:
                    if (XLookupKeysym(&event.xkey, 0) == XK_q) {
                        XDestroyImage(image);
                        XCloseDisplay(dis);
                        exit(EXIT_SUCCESS);
                    }
                    break;

                default:
                    break;
            }
        }
    }
//    return 0;
}

XImage *CreateImageFromBuffer(Display * dis, const unsigned char *buf, long int width,  long int height){
    int depth, screen;
    XImage *img = NULL;
    int i;
    long int numBmpBytes;
    size_t numImgBytes;
    int32_t *imgBuf;
    int ind = 0;
    long int line;
    long int temp;
    long int ih, iw;
    long int new_ind;

    screen = DefaultScreen(dis);
    depth = DefaultDepth(dis, screen);
    temp = width * 3;
    line = temp + width % 4;
    numImgBytes = (4 * (width * height));
    imgBuf = malloc(numImgBytes);

    numBmpBytes = line * height;
    for (i = 0; i < numBmpBytes; i++) {
        unsigned int r, g, b;

        if (i >= temp && (i % line) >= temp)
            continue;

        b = buf[i];
        i++;
        g = buf[i];
        i++;
        r = buf[i];

        iw = ind % width;
        ih = ind / width;
        new_ind = iw + (height - ih - 1) * width;

        imgBuf[new_ind] = (r | g << 8 | b << 16) << 8;
        ind++;
    }

    img = XCreateImage(dis, CopyFromParent, depth, ZPixmap, 0, (char *) imgBuf, width, height, 32, 0);
    XInitImage(img);

    img->byte_order = MSBFirst;

    img->bitmap_bit_order = MSBFirst;
    return img;
}