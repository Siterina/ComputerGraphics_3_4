#include "frame.h"
#include "ui_frame.h"
#include "nvector.h"
#include "nmatrix.h"

#define pi 3.1415

Frame::Frame(QWidget *parent) :
    QWidget(parent),
    r_figure(2),
    alpha(30), beta(0),
    lastX(0), lastY(0),
    visible(false), axleVisible(false),
    perspectiveView(false),
    Scale(1), toMove(0),
    count(20),
    ui(new Ui::Frame) {
    toMove.z = 100;
    ui->setupUi(this);
}

Frame::~Frame() {
    delete ui;
}

NVector figurePoint(double phi, double psi, double r_figure) {
    NVector result = NVector();
    result.x = r_figure * sin(phi) * cos(psi);
    result.y = r_figure * sin(phi) * sin(psi);
    result.z = r_figure * cos(phi);
    return result;
}

NVector bottomPoint(double phi, double r_figure) {
    NVector result = NVector();
    result.x = r_figure * cos(phi);
    result.y = r_figure * sin(phi);
    result.z = 0;
    return result;
}

NVector VectorComposition(const NVector a, const NVector b) {
    NVector result = NVector();
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

double ScalarComposition(const NVector a, const NVector b) {
    double result;
    result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

/*float Widget::MakeColor(Triangle *t)
{
    QVector3D center(0.0, 0.0, 0.0);
    center += t->vert1;
    center += t->vert2;
    center += t->vert3;
    center /= 3;
    QVector3D N = t->Normal();
    QVector3D V (0.0, 0.0, 1.0);
    QVector3D L (lightX, lightY, lightZ);
    L -= center;
    float d = L.length();
    float f = 1.0 / (0.1 + 0.1 * d);
    L.normalize();
    QVector3D R = 2 * (QVector3D::dotProduct(N, L)) * N - L;
    R.normalize();
    return f * ( Ka * Ia + Kd * Id * QVector3D::dotProduct(L,N) +
                 Ks * Is * pow (QVector3D::dotProduct(R,V), Alpha) );
}*/

void swap(double &a, double &b)
{
    double tmp = a;
    a = b;
    b = tmp;
}

void Frame::paintEvent(QPaintEvent*) {

    double max = std::max(1.5 * r_figure * Scale.z, std::max(1.5 * r_figure * Scale.y, 1.5 * r_figure * Scale.x));
    double Ky = height()/(2 * max);
    double Kx = (width())/(2 * max);
    double K = std::min(Kx, Ky);
    double w  = width() / 2.0;
    double h = height() / 2.0 + 30;
    bool bottom = false;


    QPainter painter(this);

    if(axleVisible) {
        pen.setColor(Qt::darkMagenta);
        pen.setWidth(2);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
        painter.drawLine(w, h, (2 * w - 20), h);
        painter.drawLine(w, h, 20, height() - h / 8);
        painter.drawLine(w, h, w, 60);
    }

    pen.setColor(Qt::black);
    pen.setWidth(3);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    QVector <NVector> points;
    QVector <NVector> pointsBottom;

    NMatrix SMatrix  = NMatrix();
    NMatrix ResMatrix = NMatrix();
    NVector ToCenter = NVector();
    NVector a = NVector();
    NVector b = NVector();
    NVector n = NVector();
    NVector k = NVector();
    NMatrix toCheck = NMatrix();
    ToCenter.x = w;
    ToCenter.y = h;
    toCheck.data[3][2] = 0.07;
    k.z = -1;

    SMatrix.SetScale(K, Scale);
    ResMatrix.RotateAll(beta, alpha);

    /***        points         ***/

    double step_phi = pi / count / 2;
    double step_psi = pi / count * 2;
    double phi = pi/360;
    double psi = pi/360;
    for(int i = 0; i < count; phi += step_phi, i++) {
        psi = 0;
        for(int j = 0; j < count; psi += step_psi, j++) {

         points.push_back(figurePoint(phi, psi, r_figure));
         points.push_back(figurePoint(phi + step_phi, psi, r_figure));
         points.push_back(figurePoint(phi + step_phi, psi + step_psi, r_figure));
         points.push_back(figurePoint(phi, psi + step_psi, r_figure));
        }
    }

    int size = points.size();
    phi = pi/360;

    for(int i = 0; i < count + 1; phi += step_psi, i++) {
        pointsBottom.push_back(bottomPoint(phi, r_figure));
    }
    int sizeBottom = pointsBottom.size();

    for (int i = 0; i < size; i++) {

        if(perspectiveView) {
            points[i].x *= (Scale.x * toMove.z / 100);
            points[i].y *= (Scale.y * toMove.z / 100);
            points[i].z *= (Scale.z * toMove.z / 100);
            points[i] = ResMatrix * points[i];
            points[i] = toCheck * points[i];
            points[i].x = points[i].x / points[i].t;
            points[i].y = points[i].y / points[i].t;
            points[i].x *= K;
            points[i].y *= K;
            points[i].z *= K;
            if(i < sizeBottom) {
                pointsBottom[i].x *= (Scale.x * toMove.z / 100);
                pointsBottom[i].y *= (Scale.y * toMove.z / 100);
                pointsBottom[i].z *= (Scale.z * toMove.z / 100);
                pointsBottom[i] = ResMatrix * pointsBottom[i];
                pointsBottom[i] = toCheck * pointsBottom[i];
                pointsBottom[i].x = pointsBottom[i].x / pointsBottom[i].t;
                pointsBottom[i].y = pointsBottom[i].y / pointsBottom[i].t;
                pointsBottom[i].x *= K;
                pointsBottom[i].y *= K;
                pointsBottom[i].z *= K;
            }
        }
        else {
             points[i] = SMatrix * points[i];
             points[i].x *= (toMove.z / 100);
             points[i].y *= (toMove.z / 100);
             points[i].z *= (toMove.z / 100);
             points[i] = ResMatrix * points[i];
             if(i < sizeBottom) {
                 pointsBottom[i] = SMatrix * pointsBottom[i];
                 pointsBottom[i].x *= (toMove.z / 100);
                 pointsBottom[i].y *= (toMove.z / 100);
                 pointsBottom[i].z *= (toMove.z / 100);
                 pointsBottom[i] = ResMatrix * pointsBottom[i];
             }
        }
        points[i] = points[i] + ToCenter;
        points[i] = points[i] + toMove;
        if(i < sizeBottom) {
            pointsBottom[i] = pointsBottom[i] + ToCenter;
            pointsBottom[i] = pointsBottom[i] + toMove;
        }
}

    n = VectorComposition(pointsBottom[3] - pointsBottom[2], pointsBottom[1] - pointsBottom[0]);
    if(ScalarComposition(k, n) >= 0)
            bottom = true;


    /***        draw figure carcas        ***/

   /* for (int i = 0; i < size; i += 4) {
        a = points[i + 1] - points[i];
        b = points[i + 3] - points[i];
        n = VectorComposition(a, b);

        if(ScalarComposition(n, k) >= 0) {
            painter.drawLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
            painter.drawLine(points[i + 1].x, points[i + 1].y, points[i + 2].x, points[i + 2].y);
            painter.drawLine(points[i + 2].x, points[i + 2].y, points[i + 3].x, points[i + 3].y);
            painter.drawLine(points[i + 3].x, points[i + 3].y, points[i].x, points[i].y);
        }
        else if(visible) {
            pen.setColor(Qt::darkGray);
            pen.setWidth(1);
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
            painter.drawLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
            painter.drawLine(points[i + 1].x, points[i + 1].y, points[i + 2].x, points[i + 2].y);
            painter.drawLine(points[i + 2].x, points[i + 2].y, points[i + 3].x, points[i + 3].y);
            painter.drawLine(points[i + 3].x, points[i + 3].y, points[i].x, points[i].y);
        }
        pen.setColor(Qt::black);
        pen.setWidth(3);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
    }
*/
    if(bottom)
        for(int i = 0; i < sizeBottom - 1; i++)
            painter.drawLine(pointsBottom[i].x, pointsBottom[i].y, pointsBottom[i + 1].x, pointsBottom[i + 1].y);


    /***        colour figure         ***/


    for (int i = 0; i < points.size(); i+= 4) {
        a = points[i + 1] - points[i];
        b = points[i + 3] - points[i];
        n = VectorComposition(a, b);

        if(ScalarComposition(n, k) >= 0) {


           /* double x12 = abs(points[i].x - points[i + 1].x);
            double y12 = abs(points[i].y - points[i + 1].y);
            double x23 = abs(points[i + 1].x - points[i + 2].x);
            double y23 = abs(points[i + 1].y - points[i + 2].y);
            double x34 = abs(points[i + 2].x - points[i + 3].x);
            double y34 = abs(points[i + 2].y - points[i + 3].y);
            double x41 = abs(points[i + 3].x - points[i].x);
            double y41 = abs(points[i + 3].y - points[i].y);*/

            double x1 = points[i].x, y1 = points[i].y;
            double x2 = points[i + 1].x, y2 = points[i + 1].y;
            double x3 = points[i + 2].x, y3 = points[i + 2].y;
            double x4 = points[i + 3].x, y4 = points[i + 3].y;

           /* if(x1 > x2) {
                swap(y1, y2);
                swap(x1, x2);
            }
            if(x4 > x3) {
                swap(y3, y4);
                swap(x3, x4);
            }
            if(x1 > x4) {
                swap(y1, y4);
                swap(x1, x4);
            }
            if(x2 > x3) {
                swap(y3, y2);
                swap(x3, x2);
            }
            */
           // float I = MakeColor(figure->triangles[i]);// переписать функцию
            double I = 0.5;
            if (I > 1.0) I = 1.0;
            if (I < 0.0) I = 0.0;

            //float R = (mColorR + lColorR) / 2;
           // float G = (mColorG + lColorG) / 2;
           // float B = (mColorB + lColorB) / 2;

            double R = 0;
            double G = 0;
            double B = 255;

            painter.setPen(QColor(R * I, G * I, B * I));

           // painter.drawLine(x1, y1, x2, y2);
           // painter.drawLine(x2, y2, x3, y3);
           // painter.drawLine(x1, y1, x3, y3);

           // if(points[i].x < points[i + 1].x)
            //    for(double x = x1; x < x2; x+= 0.5)
              //      painter.drawLine(x, y12/x12 * (x - x1) + y1, x - x41, y34/x34 * (x - x41 - x4) + y4);

            if (y1 > y2) { swap (y1, y2);   swap (x1, x2); }
            if (y1 > y3) { swap (y1, y3);   swap (x1, x3); }
            if (y2 > y3) { swap (y3, y2);   swap (x3, x2); }
            //if (y1 > y4) { swap (y1, y4);   swap (x1, x4); }
            //if (y4 > y3) { swap (y3, y4);   swap (x3, x4); }





            float dx12 = x2 - x1;
            float dy12 = y2 - y1;
            float dx32 = x2 - x3;
            float dy32 = y2 - y3;
            float dx31 = x1 - x3;
            float dy31 = y1 - y3;
            float dx13 = x3 - x1;
            float dy13 = y3 - y1;

            float dx14 = x4 - x1;
            float dy14 = y4 - y1;
            float dx34 = x4 - x3;
            float dy34 = y4 - y3;


            //double x12 = abs(x2 - x1);
            //double y12 = abs(y2 - y1);
            //double x23 = abs(x3 - x2);
            //double y23 = abs(y3 - y2);
            //double x13 = abs(x3 - x1);
            //double y13 = abs(y3 - y1);

            //painter.drawLine(x1, y1, x2, y2);
            //painter.drawLine(x2, y2, x3, y3);
            //painter.drawLine(x3, y3, x4, y4);
            //painter.drawLine(x4, y4, x1, y1);



            for (float y = y1; y < y2; y += 0.5)
                 painter.drawLine (x1 + (dx12/dy12) * (y-y1) , y, x1 + (dx13/dy13) * (y-y1), y);

            for (float y = y3; y > y2; y -= 0.5)
                 painter.drawLine (x3 + (dx32/dy32) * (y-y3) , y, x3 + (dx31/dy31) * (y-y3), y);

            if (dy13 != 0)
               painter.drawLine (x2 , y2, x1 + (dx13/dy13) * (y2-y1), y2);
            if (dy13 != 0)
                painter.drawLine (x3 + dx32 , y2, x3 + (dx13/dy13) * (y2-y3), y2);






            x1 = points[i].x, y1 = points[i].y;
            x2 = points[i + 1].x, y2 = points[i + 1].y;
            x3 = points[i + 2].x, y3 = points[i + 2].y;
            x4 = points[i + 3].x, y4 = points[i + 3].y;


            //if (y1 > y2) { swap (y1, y2);   swap (x1, x2); }
            if (y1 > y3) { swap (y1, y3);   swap (x1, x3); }
            //if (y2 > y3) { swap (y3, y2);   swap (x3, x2); }
            if (y1 > y4) { swap (y1, y4);   swap (x1, x4); }
            if (y4 > y3) { swap (y3, y4);   swap (x3, x4); }

            dx12 = x2 - x1;
            dy12 = y2 - y1;
            dx32 = x2 - x3;
            dy32 = y2 - y3;
            dx31 = x1 - x3;
            dy31 = y1 - y3;
            dx13 = x3 - x1;
            dy13 = y3 - y1;

             dx14 = x4 - x1;
            dy14 = y4 - y1;
            dx34 = x4 - x3;
            dy34 = y4 - y3;



            for (float y = y1; y < y4; y += 0.5)
                 painter.drawLine (x1 + (dx14/dy14) * (y-y1) , y, x1 + (dx13/dy13) * (y-y1), y);

            for (float y = y3; y > y4; y -= 0.5)
                 painter.drawLine (x3 + (dx34/dy34) * (y-y3) , y, x3 + (dx31/dy31) * (y-y3), y);


            if (dy13 != 0)
                painter.drawLine (x4 , y4, x1 + (dx13/dy13) * (y4-y1), y4);
            if (dy13 != 0)
                painter.drawLine (x3 + dx34 , y4, x3 + (dx13/dy13) * (y4-y3), y4);





           // else
              //  for(double x = points[i + 1].x; x < points[i].x; x+= 0.5)
                  //  painter.drawLine(x, points[i].y, x, points[i + 3].y);


           /* for (float y = y1; y < y2; y += 0.5)
                painter.drawLine (x1 + (dx12/dy12) * (y-y1) , y, x1 + (dx13/dy13) * (y-y1), y);

            for (float y = y3; y > y2; y -= 0.5)
                painter.drawLine (x3 + (dx32/dy32) * (y-y3) , y, x3 + (dx31/dy31) * (y-y3), y);

            if (dy13 != 0)
                painter.drawLine (x2 , y2, x1 + (dx13/dy13) * (y2-y1), y2);
            if (dy31 != 0)
                painter.drawLine (x3 + dx32 , y2, x3 + (dx31/dy31) * (y2-y3), y2);
            */

        }
    }



















}









/******************************************************/
/*                   form                             */
/******************************************************/


void Frame::mousePressEvent(QMouseEvent *mEvent) {
    lastX = mEvent->x();
    lastY = mEvent->y();
}

void Frame::mouseMoveEvent(QMouseEvent *mEvent) {
    beta  -= (mEvent->x() - lastX) * pi / 360;
    alpha += (mEvent->y() - lastY) * pi / 360;
    lastX = mEvent->x();
    lastY = mEvent->y();
    repaint();
}

void Frame::on_exitButton_clicked() {
    close();
}

void Frame::on_checkBox_toggled(bool checked) {
    if(checked)
        visible = true;
    else visible = false;
    repaint();
}

void Frame::on_ScaleX_valueChanged(int arg1) {
    Scale.x = (double)arg1;
    repaint();
}

void Frame::on_ScaleY_valueChanged(int arg1) {
    Scale.y = (double)arg1;
    repaint();
}

void Frame::on_ScaleZ_valueChanged(int arg1) {
    Scale.z = (double)arg1;
    repaint();
}

void Frame::on_LeftRight_sliderMoved(int position) {
    toMove.x = (double)position;
    repaint();
}

void Frame::on_UpDown_sliderMoved(int position) {
    toMove.y = (double)position;
    repaint();
}

void Frame::on_zoom_sliderMoved(int position) {
    toMove.z = (double)position;
    repaint();
}

void Frame::on_axleVisible_toggled(bool checked) {
    if(checked)
        axleVisible = true;
    else axleVisible = false;
    repaint();
}

void Frame::on_perspectiveView_toggled(bool checked) {
    if(checked)
        perspectiveView = true;
    else perspectiveView = false;
    repaint();
}

void Frame::on_Approximation_valueChanged(int arg1) {
    count = arg1;
    repaint();
}
