#ifndef EVENT_OVERLAY_WIDGET_H
#define EVENT_OVERLAY_WIDGET_H

#include <QWidget>
#include <QObject>

#define POINTING 0
#define POINTING_WIDTH 10
#define DRAWING 1
#define DRAWING_WIDTH 7
#define DRAWING_TIMEOUT_MS 1000 // 1000ms

class QPainterPath;
class QTimer;

class Event_Overlay_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Event_Overlay_Widget(QWidget *parent = nullptr);
    ~Event_Overlay_Widget();

    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void set_paint_mode(int paint_mode);
    int get_paint_mode();

    void set_pos(const qreal x, const qreal y);

private:
    QPainterPath *path;
    QTimer *timer;
    QTimer *drawing_timeout_timer;
    QPointF prev_pos, current_pos;
    bool is_dragging;
    int current_paint_mode;
    QList<QPainterPath*> paths;
    qreal color_opacity;

    void set_connects();

signals:
    void drawing_finished();
    void restore_from_full_screen();

public slots:
    void set_attribute();
};

#endif // EVENT_OVERLAY_WIDGET_H
