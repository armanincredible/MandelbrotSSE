#include "mainwindow.h"
#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "math.h"
#include <ctime>
#include <QDebug>

const float ROI_X = -1.325f;
const float ROI_Y = 0;

const int LEFT  = 0x41;
const int RIGHT = 0x44;
const int DOWN  = 0x53;
const int UP    = 0x57;
const int PLUS  = 0x50; //P
const int MINUS = 0x4f; //O

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

const int    nMax  = 256;
const float  dx    = 1/800.f;
const float  dy    = 1/800.f;
const int size_x = 800;
const int size_y = 600;

const __m256 r2Max = _mm256_set_ps (100.f, 100.f, 100.f, 100.f,
                                    100.f, 100.f, 100.f, 100.f);

const __m256 _255  = _mm256_set_ps (255.f, 255.f, 255.f, 255.f,
                                    255.f, 255.f, 255.f, 255.f);

const __m256 _100  = _mm256_set_ps (100.f, 100.f, 100.f, 100.f,
                                    100.f, 100.f, 100.f, 100.f);

const __m256 _7to0 = _mm256_set_ps  (7.f, 6.f, 5.f, 4.f,
                                     3.f, 2.f, 1.f, 0.f);

const __m256 nmax  = _mm256_set_ps (nMax, nMax, nMax, nMax,
                                    nMax, nMax, nMax, nMax);

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case DOWN:
        {
            yC += dy * 100.f * scale;
            break;
        }
        case UP:
        {
            yC -= dy * 100.f * scale;
            break;
        }
        case LEFT:
        {
            xC -= dx * 100.f * scale;
            break;
        }
        case RIGHT:
        {
            xC += dx * 100.f * scale;
            break;
        }
        case PLUS:
        {
            scale -= scale * 0.5;
            break;
        }
        case MINUS:
        {
            scale += scale * 0.5;
            break;
        }
        default:
        {
            break;
        }
    }

    unsigned int start_time = clock ();

    /*for (int i = 0; i < 100; i++)
    {*/
        mandelbrot ();
    //}

    unsigned int delta_time = clock () - start_time;

    qDebug() << "FPS = " << double ((1000 * 100 / delta_time));

    repaint();
}

void  MainWindow::mandelbrot (void)
{
    uchar* collor_buffer = src.bits();
    const __m256 dx_vec = _mm256_set_ps (dx * scale, dx * scale, dx * scale, dx * scale,
                                         dx * scale, dx * scale, dx * scale, dx * scale);
    __m256 CollorBuf = _mm256_set_ps (0, 0, 0, 0, 0, 0, 0, 0);

    for (int scr_y = 0; scr_y < size_y; scr_y++)
        {

        float x0 = ( (- 400.f) * dx) * scale + ROI_X + xC;
        float y0 = ( ((float)scr_y - 300.f) * dy) * scale + ROI_Y + yC;

        for (int scr_x = 0; scr_x < size_x; scr_x += 8, x0 += dx * 8 * scale)
            {
                __m256 x0_vec = _mm256_set_ps (x0, x0, x0, x0,
                                               x0, x0, x0, x0);

                __m256 X0_Buf = _mm256_add_ps (x0_vec, _mm256_mul_ps (_7to0, dx_vec));
                __m256 Y0_Buf = _mm256_set_ps (y0, y0, y0, y0,
                                               y0, y0, y0, y0);

                __m256 X_Buf = X0_Buf;
                __m256 Y_Buf = Y0_Buf;
                __m256i NBuf = _mm256_setzero_si256();

                for (int n = 0; n < nMax; n++)
                {
                    __m256 x2_Buf = _mm256_mul_ps (X_Buf, X_Buf);
                    __m256 y2_Buf = _mm256_mul_ps (Y_Buf, Y_Buf);

                    __m256 r2_Buf = _mm256_add_ps (x2_Buf, y2_Buf);

                    const int imm8 = _CMP_LE_OS;
                    __m256 cmp = _mm256_cmp_ps (r2_Buf, r2Max, imm8);
                    int mask   = _mm256_movemask_ps (cmp);
                    if (!mask)
                    {
                        break;
                    }

                    NBuf = _mm256_sub_epi32 (NBuf, _mm256_castps_si256 (cmp));

                    __m256 xy = _mm256_mul_ps (X_Buf, Y_Buf);

                    X_Buf = _mm256_add_ps (_mm256_sub_ps (x2_Buf, y2_Buf), X0_Buf);
                    Y_Buf = _mm256_add_ps (_mm256_add_ps (xy, xy), Y0_Buf);
                 }

                CollorBuf = _mm256_mul_ps (_mm256_mul_ps (_mm256_sqrt_ps (_mm256_sqrt_ps (_mm256_div_ps (_mm256_cvtepi32_ps (NBuf), nmax))), _255), _100);

                for (int i = 0; i < 8; i++)
                {
                    int*   ptr_NBuf = (int*) &NBuf;
                    float* ptr_I = (float*) &CollorBuf;

                    uchar c = ptr_I[i];

                    if (ptr_NBuf[i] < nMax)
                    {
                        collor_buffer [scr_x * 3 + i * 3 + 0 + scr_y * 800 * 3] = 255 - c;
                        collor_buffer [scr_x * 3 + i * 3 + 1 + scr_y * 800 * 3] = c % 2 * 64;
                        collor_buffer [scr_x * 3 + i * 3 + 2 + scr_y * 800 * 3] = c;
                        //image.setPixel(scr_x + i, scr_y, qRgb(255-c, c%2 * 64, c));
                    }
                    else
                    {
                        collor_buffer [scr_x * 3 + i * 3 + 0 + scr_y * 800 * 3] = 0;
                        collor_buffer [scr_x * 3 + i * 3 + 1 + scr_y * 800 * 3] = 0;
                        collor_buffer [scr_x * 3 + i * 3 + 2 + scr_y * 800 * 3] = 0;
                    }
                }
            }
       }
    return;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect target(0, 0, size_x, size_y);
    QPixmap pmap(size_x, size_y);
    pmap.convertFromImage(src);

    painter.drawPixmap(target, pmap, target);
}
