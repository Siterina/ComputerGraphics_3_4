#ifndef FRAME_H
#define FRAME_H

#include <QWidget>
#include <QPainter>
#include <QPointF>
#include <QDebug>
#include <QMouseEvent>
#include <cmath>
#include <QMouseEvent>
#include "nvector.h"
#include "NMatrix.h"

namespace Ui {
class Frame;
}

class Frame : public QWidget
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = 0);
    ~Frame();

    double r_figure;
    double alpha;
    double beta;
    double lastX, lastY;
    bool visible;
    bool axleVisible;
    bool perspectiveView;
    bool carcasVisible;
    int count;
    QPen pen;
    NVector Scale;
    NVector toMove;
    double ka, kd, ks;
    double n_alpha;
    double lightX, lightY, lightZ;
    double ILightR, ILightG, ILightB;
    double IFigureR, IFigureG, IFigureB;
    double IAmbientR, IAmbientG, IAmbientB;




private:
    Ui::Frame *ui;

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *mEvent);
    void mouseMoveEvent(QMouseEvent *mEvent);
    NVector Frame::MakeColor(const NVector center, const NVector N, const NVector V, double K);
private slots:
    void on_exitButton_clicked();
    void on_checkBox_toggled(bool checked);
    void on_ScaleX_valueChanged(int arg1);
    void on_ScaleY_valueChanged(int arg1);
    void on_ScaleZ_valueChanged(int arg1);
    void on_LeftRight_sliderMoved(int position);
    void on_UpDown_sliderMoved(int position);
    void on_axleVisible_toggled(bool checked);
    void on_perspectiveView_toggled(bool checked);
    void on_zoom_sliderMoved(int position);
    void on_Approximation_valueChanged(int arg1);
    void on_Carcas_visible_toggled(bool checked);
    void on_ka_valueChanged(double arg1);
    void on_kd_valueChanged(double arg1);
    void on_ks_valueChanged(double arg1);
    void on_fong_valueChanged(double arg1);
    void on_rFigure_valueChanged(int arg1);
    void on_lightX_sliderMoved(int position);
    void on_lightY_sliderMoved(int position);
    void on_lightZ_sliderMoved(int position);

    void on_ILightR_valueChanged(int arg1);
    void on_ILightG_valueChanged(int arg1);
    void on_ILightB_valueChanged(int arg1);
    void on_IFigureR_valueChanged(int arg1);
    void on_IFigureG_valueChanged(int arg1);
    void on_IFigureB_valueChanged(int arg1);
    void on_IAmbientR_valueChanged(int arg1);
    void on_IAmbientG_valueChanged(int arg1);
    void on_IAmbientB_valueChanged(int arg1);
};

#endif // FRAME_H
