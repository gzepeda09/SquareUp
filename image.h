#ifndef IMAGE_H
#define IMAGE_H

class Image {
    public:
        int width, height;
        unsigned char *data;
        Image(const char *fname);
        ~Image();
};
#endif // IMAGE_H

