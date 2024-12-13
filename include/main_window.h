#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QHash>

class Pdf_Viewer_Widget;
class Event_Overlay_Widget;

class QFileDialog;
class QLineEdit;
class QLabel;
class QAction;
class QPushButton;
class QHBoxLayout;

namespace Ui {
class Main_Window;
}

class Main_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_Window(QWidget *parent = nullptr);
    ~Main_Window();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::Main_Window *ui;
    QWidget *focused_widget;
    Pdf_Viewer_Widget *focused_pdf_viewer_widget;
    Event_Overlay_Widget *event_overlay_widget;
    QFileDialog *pdf_dialog;
    QLineEdit *page_line_edit, *current_zoom;
    QLabel *total_page_label;
    QAction *action_prev_page, *action_next_page, *action_full_screen, *zoom_in, *zoom_out;
    int current_page_index, total_page_index;
    QHash<QString, QPair<QWidget*, QWidget*>> hash;
    QHash<QWidget*, QPushButton*> hash_2;
    QString name;
    qreal prev_zoom, zoom;
    const QVector<qreal> zoom_list;
    int zoom_list_idx;

    void set_tool_bar();
    void set_connects();
    void open_pdf(const QUrl &url);
    QWidget *make_widget(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name);
    QWidget *make_button(const QString &name);
    void set_name(const QString &name);
    int find_nearest_zoom(const qreal zoom);

signals:
    void current_widget_changed(const QString &name);
    void f();

public slots:
    void load_push_button_clicked();
    void page_line_edit_return_pressed(const QString &input_text);
    void action_prev_page_triggered();
    void action_next_page_triggered();
    void zoom_out_triggered();
    void current_zoom_return_pressed(const QString &input_text);
    void zoom_in_triggered();
    void action_full_screen_triggered();
    void restore_from_full_screen();
    void set_paint_mode(int paint_mode);
    void set_pos(const int x, const int y);
};

#endif // MAIN_WINDOW_H
