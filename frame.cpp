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
    perspectiveView(false), carcasVisible(false),
    Scale(1), toMove(0),
    count(20),
    ia(0.2), id(0.6), is(0.5), ka(0.1), kd(0.8), ks(0.5), n_alpha(1),
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

/*double MakeColor(double centerX, double centerY, double centerZ, const NVector N, double h,double w) {
    NVector center = NVector();
    center.x += centerX / 3;
    center.y += centerY / 3;
    center.z += centerZ / 3;

    //QVector3D N = t->Normal();
    NVector V = NVector(); // (0.0, 0.0, 1.0);
    V.x = 0;
    V.y = 0;
    V.z = -1;
    NVector L = NVector();//(3, 3, 7); // lightX, lightY, lightZ
    L.x = w;
    L.y = h;
    L.z = -5 * w;

    L = L - center;
    //float d = L.length();
    double d = sqrt(L.x * L.x + L.y * L.y + L.z * L.z);
    //float f = 1.0 / (0.1 + 0.1 * d);
    double f = 1.0 / (0.1 + 0.1 * d);
    //L.normalize();
    if(d != 0) {
    L.x /= d;
    L.y /= d;
    L.z /= d;
    }
    //QVector3D R = 2 * (QVector3D::dotProduct(N, L)) * N - L; // скалярное
    NVector R = NVector();
    R.x = N.x * 2 * ScalarComposition(N, L);
    R.y = N.y * 2 * ScalarComposition(N, L);
    R.z = N.z * 2 * ScalarComposition(N, L);
    R = R - L;

   // R.normalize();
    d = sqrt(R.x * R.x + R.y * R.y + R.z * R.z);
    if(d != 0) {
    R.x /= d;
    R.y /= d;
    R.z /= d;
    }

    //return f * ( Ka * Ia + Kd * Id * QVector3D::dotProduct(L,N) +
            //     Ks * Is * pow (QVector3D::dotProduct(R,V), Alpha) );
    return f * ( 0.1 * 0.2 + 0.8 * 0.6 * ScalarComposition(L,N) +
                 0.5 * 0.5 * pow (ScalarComposition(R,V), 1) );

}*/


 double Frame::MakeColor(const NVector center, const NVector N, const NVector V, double K) {


    // I = Ia + Id + Is
    // Ia = Ka * ia  фоновое освещение
   //  Id = Kd * (L,N) * id  рассеянный свет
    // N нормаль, L направление из точки на источник
    // Is = Ks * (R, V)^n_alpha * is зеркальный свет
    // R = 2 * (N, L) * N - L


    double I, Ia, Id, Is;

    NVector L = NVector(); // lightX, lightY, lightZ
    L.x = 2 * K;
    L.y = 2 * K;
    L.z = -3 * K;
    L = L - center;
    double dL = sqrt(L.x * L.x + L.y * L.y + L.z * L.z);
    L.x = L.x / dL;
    L.y = L.y / dL;
    L.z = L.z / dL;

    NVector R = NVector();
    R.x = 2 * ScalarComposition(N, L) * N.x;
    R.y = 2 * ScalarComposition(N, L) * N.y;
    R.z = 2 * ScalarComposition(N, L) * N.z;
    R = R - L;
    double dR = sqrt(R.x * R.x + R.y * R.y + R.z * R.z);
    R.x = R.x / dR;
    R.y = R.y / dR;
    R.z = R.z / dR;

    Ia = ka * ia;
    Id = kd * ScalarComposition(N, L) * id / (1 +  dL);
    Is = ks * pow(ScalarComposition(R, V), n_alpha) * is / (1 + dL);

    I = Ia + Id + Is;


    return I;

}






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
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    QVector <NVector> points;
    QVector <NVector> pointsBottom;

    NMatrix SMatrix  = NMatrix();
    NMatrix ResMatrix = NMatrix();
    NVector ToCenter = NVector();
    NVector a = NVector();
    NVector b = NVector();
    NVector N = NVector();
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

    N = VectorComposition(pointsBottom[3] - pointsBottom[2], pointsBottom[1] - pointsBottom[0]);
    if(ScalarComposition(k, N) >= 0)
            bottom = true;


    /***        colour figure         ***/

    for (int i = 0; i < points.size(); i+= 4) {
        a = points[i + 1] - points[i];
        b = points[i + 3] - points[i];
        N = VectorComposition(a, b);

        if(ScalarComposition(N, k) >= 0) {

            double x1 = points[i].x, y1 = points[i].y;
            double x2 = points[i + 1].x, y2 = points[i + 1].y;
            double x3 = points[i + 2].x, y3 = points[i + 2].y;
            double x4 = points[i + 3].x, y4 = points[i + 3].y;

            NVector center = NVector();
            center.x = abs(x1 - x3)/2;
            center.y = abs(y1 - y3)/2;
            center.z = points[i].z;
            double I = MakeColor(center, N, k, K);
            //double I = 0.5;
            I = abs(I);

            if (I > 1.0) I = 1.0;
            if (I < 0.0) I = 0.0;

            double R = 100;
            double G = 50;
            double B = 200;
            painter.setPen(QColor(R * I, G * I, B * I));



            // нижний треугольник
            if (y1 > y2) { swap (y1, y2);   swap (x1, x2); }
            if (y1 > y3) { swap (y1, y3);   swap (x1, x3); }
            if (y2 > y3) { swap (y3, y2);   swap (x3, x2); }

            double dx12 = x2 - x1;
            double dy12 = y2 - y1;
            double dx32 = x2 - x3;
            double dy32 = y2 - y3;
            double dx31 = x1 - x3;
            double dy31 = y1 - y3;
            double dx13 = x3 - x1;
            double dy13 = y3 - y1;
            double dx14 = x4 - x1;
            double dy14 = y4 - y1;
            double dx34 = x4 - x3;
            double dy34 = y4 - y3;


            for (double y = y1; y < y2; y += 0.5)
                 painter.drawLine (x1 + (dx12/dy12) * (y-y1) , y, x1 + (dx13/dy13) * (y-y1), y);
            for (double y = y3; y > y2; y -= 0.5)
                 painter.drawLine (x3 + (dx32/dy32) * (y-y3) , y, x3 + (dx31/dy31) * (y-y3), y);
            if (dy13 != 0)
               painter.drawLine (x2 , y2, x1 + (dx13/dy13) * (y2-y1), y2);


            // верхний треугольник
            x1 = points[i].x, y1 = points[i].y;
            x2 = points[i + 1].x, y2 = points[i + 1].y;
            x3 = points[i + 2].x, y3 = points[i + 2].y;
            x4 = points[i + 3].x, y4 = points[i + 3].y;

            if (y1 > y3) { swap (y1, y3);   swap (x1, x3); }
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

            for (double y = y1; y < y4; y += 0.5)
                 painter.drawLine (x1 + (dx14/dy14) * (y-y1) , y, x1 + (dx13/dy13) * (y-y1), y);
            for (double y = y3; y > y4; y -= 0.5)
                 painter.drawLine (x3 + (dx34/dy34) * (y-y3) , y, x3 + (dx31/dy31) * (y-y3), y);
            if (dy13 != 0)
                painter.drawLine (x4 , y4, x1 + (dx13/dy13) * (y4-y1), y4);

        }
    }
        if(bottom) {
            QPolygonF polygon;
            QPointF temp;
            for(int i = 0; i < sizeBottom; i++) {
                temp.setX(pointsBottom[i].x);
                temp.setY(pointsBottom[i].y);
                polygon << temp;
            }
            NVector center = NVector();
            center.x = abs(pointsBottom[0].x - pointsBottom[sizeBottom / 2].x)/2;
            center.y = abs(pointsBottom[0].y - pointsBottom[sizeBottom / 2].y)/2;
            center.z = (pointsBottom[0].z + pointsBottom[sizeBottom / 2].z)/2;

            double I = MakeColor(center, N, k, K);
            //double I = 0.5;
            I = abs(I);

            if (I > 1.0) I = 1.0;
            if (I < 0.0) I = 0.0;

            double R = 100;
            double G = 50;
            double B = 200;
            painter.setPen(QColor(R * I, G * I, B * I));
            painter.setBrush(QColor(R * I, G * I, B * I));
            painter.drawPolygon(polygon);



        }



    /***        draw figure carcas        ***/
    if(carcasVisible) {
        for (int i = 0; i < size; i += 4) {
            a = points[i + 1] - points[i];
            b = points[i + 3] - points[i];
            N = VectorComposition(a, b);

            if(ScalarComposition(N, k) >= 0) {
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
            pen.setWidth(2);
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }

        if(bottom)
            for(int i = 0; i < sizeBottom - 1; i++)
                painter.drawLine(pointsBottom[i].x, pointsBottom[i].y, pointsBottom[i + 1].x, pointsBottom[i + 1].y);
    }








} // конец paint event









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

void Frame::on_Carcas_visible_toggled(bool checked) {
    if(checked)
        carcasVisible = true;
    else carcasVisible = false;
    repaint();
}

void Frame::on_ia_valueChanged(double arg1) {
    ia = arg1;
    repaint();
}

void Frame::on_id_valueChanged(double arg1) {
    id = arg1;
    repaint();
}

void Frame::on_is_valueChanged(double arg1) {
    is = arg1;
    repaint();
}

void Frame::on_ka_valueChanged(double arg1) {
    ka = arg1;
    repaint();
}

void Frame::on_kd_valueChanged(double arg1) {
    kd = arg1;
    repaint();
}

void Frame::on_ks_valueChanged(double arg1) {
    ks = arg1;
    repaint();
}

void Frame::on_fong_valueChanged(double arg1) {
    n_alpha = arg1;
    repaint();
}

void Frame::on_rFigure_valueChanged(int arg1) {
    r_figure = arg1;
    repaint();
}
