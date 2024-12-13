#include "event_overlay_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPainterPath>
#include <QTimer>
#include <QPointF>

Event_Overlay_Widget::Event_Overlay_Widget(QWidget *parent)
    : QWidget{parent}
    , is_dragging(false)
    , current_paint_mode(-1)
    , color_opacity(1.0)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);  // 마우스 이벤트 받기
    setAttribute(Qt::WA_NoSystemBackground, true);          // 배경 투명화
    setWindowFlags(Qt::FramelessWindowHint);                // 프레임 없는 창
    setFocusPolicy(Qt::StrongFocus);

    set_connects();
}

Event_Overlay_Widget::~Event_Overlay_Widget(){
    if(path){
        delete path;
        path = nullptr;
    }
    if(!paths.empty()){
        for(const auto *path : paths){
            delete path;
        }
    }
    paths.clear();
}

void Event_Overlay_Widget::paintEvent(QPaintEvent *event){
    // painter 설정
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 안티앨리어싱 활성화
    // pen 설정
    QPen pen;
    pen.setBrush(Qt::red);
    pen.setWidth(current_paint_mode == POINTING ? POINTING_WIDTH : DRAWING_WIDTH);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::BevelJoin);

    if(current_paint_mode == POINTING){
        painter.setPen(pen);
        painter.drawPoint(current_pos);
    }
    else if(current_paint_mode == DRAWING){
        // 투명도 설정
        QColor color = Qt::red;
        color.setAlphaF(color_opacity);
        pen.setBrush(color);
        painter.setPen(pen);

        for(const auto &path : paths){
            if(path){
                painter.drawPath(*path);
            }
        }
    }
}

void Event_Overlay_Widget::set_paint_mode(int paint_mode){
    current_paint_mode = paint_mode;
}

int Event_Overlay_Widget::get_paint_mode(){
    return current_paint_mode;
}

void Event_Overlay_Widget::set_pos(const qreal x, const qreal y){
    prev_pos = current_pos;
    current_pos.setX(x); current_pos.setY(y);

    // 드로잉 이벤트의 경우 경로 저장
    if(current_paint_mode == DRAWING){
        if(paths.empty()){
            // 새로운 드로잉 경로 추가
            path = new QPainterPath();
            path->moveTo(current_pos);
            paths.push_back(path);
        }

        QPointF mid_pos = (prev_pos + current_pos) / 2.0;
        path->quadTo(prev_pos, mid_pos);
    }

    drawing_timeout_timer->start(DRAWING_TIMEOUT_MS);

    update();
}

void Event_Overlay_Widget::set_connects(){
    // 라인 삭제 설정--------------------------------------------------------------
    // 삭제
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        if(color_opacity > 0.03){
            color_opacity -= 0.03;
        }
        else{
            timer->stop();
            paths.clear();
            color_opacity = 1.0;
        }
        update();
    });
    // 삭제를 위한 드로잉 이벤트 종료
    drawing_timeout_timer = new QTimer(this);
    connect(drawing_timeout_timer, &QTimer::timeout, this, [this](){
        if(!paths.empty()){
            timer->start(16);   // ~60fps
        }
    });
}

void Event_Overlay_Widget::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape){
        emit restore_from_full_screen();
    }
    if(event->key() == Qt::Key_D){
        set_attribute();
    }
}

void Event_Overlay_Widget::set_attribute(){
    bool flag = testAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TransparentForMouseEvents, !flag);
    if(flag){
        qDebug() << "Has Focus: " << this->hasFocus();
        setFocus();
    }
}
