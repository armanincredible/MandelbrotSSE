#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <immintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mandelbrot (void);
protected:
    void keyPressEvent (QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    float xC = 0.f;
    float yC = 0.f;
    float scale = 1.f;
    QImage src = QImage(QSize(800, 600), QImage::Format_RGB888);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
