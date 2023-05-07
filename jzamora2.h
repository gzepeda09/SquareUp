#ifndef _JOSE
#define _JOSE

#include <GL/glx.h>

// --- Jose ---
extern void displayBackground(float x0, float x1,
                              float y0, float y1,
                              int xres, int yres,
                              int width, int height,
                              GLuint texture, GLvoid* data);

extern void displayCharacter(float x0, float x1,
                             float y0, float y1,
                             double px, double px2,
                             double py, double py2,
                             int xres, int yres,
                             int width, int height,
                             GLuint texture, GLvoid* data);

extern void display_map(float x0, float x1,
                        float y0, float y1,
                        int xres, int yres);

extern void player_hitbox(int w, int h, float x, float y);

extern void punch_hitbox(int w1, int w2, int h, float x, float y);

extern void fmBorder(int xres, int yres);

extern void display_controls(int wf, int yres);

extern void movePlayerLeft(double* pos_x, double* pos2_x,
                           double* pos_y, double* pos2_y,
                           double vel_x,  float p2_w,
                           float width,   float* lft,     float* rgt);


extern void movePlayerRight(double* pos_x, double* pos2_x, 
                            double* pos_y, double* pos2_y,
                            double vel_x,  float p2_w,
                            float width,   float* lft, 
                            float* rgt,    int xres);

extern void jumpPlayer(double* vel_down, double* vel_up);

extern void movePlayerUp(double* vel_up, double* pos_y);

extern void movePlayerDown(double* vel_down, double* pos_y);

extern void punchAbilityPlayer1(int* p1_punch,int* sPunch, int jeflag,
                                double* p1_pos_x, double* p1_y, float p1_pw2,
                                double* p2_pos_x, double* p2_pos_y, float p2_w,
                                double* p2_vel_1, int* p2_dead,
                                int p2_block, int* p2_health, int weapon);

extern void punchAbilityPlayer2(int* p2_punch, int* sPunch, int jeflag,
                                double* p2_pos_x, double* p2_y, float p2_pw2,
                                double* p1_pos_x, double* p1_pos_y, float p1_w,
                                double* p1_vel_1, int* p1_dead,
                                int p1_block, int* p1_health, int weapon);

extern void punchCooldownPlayer(float* cooldown, int* punch);

#endif
