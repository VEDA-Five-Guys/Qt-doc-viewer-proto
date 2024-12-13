#ifndef PDF_VIEWER_WIDGET_H
#define PDF_VIEWER_WIDGET_H

#include <QWidget>
#include <QPdfView>
#include <QUrl>
#include <QPoint>

#define SINGLE_PAGE 0
#define MULTI_PAGE  1

class QPdfDocument;
class QPdfPageNavigator;

class Pdf_Viewer_Widget : public QWidget
{
    Q_OBJECT

public:
    Pdf_Viewer_Widget(const QUrl &url, QWidget *parent = nullptr);
    int get_current_page_index();
    int get_total_page_index();
    qreal get_current_zoom();
    qreal get();
    qreal get_2();
    void page_changed(const int changed_page_index);
    void zoom_changed(const qreal zoom);
    void set_page_mode(QPdfView::PageMode page_mode);
    void set_zoom_mode(QPdfView::ZoomMode zoom_mode);
    void set_scroll_bar(Qt::ScrollBarPolicy policy);
    QPdfView::PageMode get_current_page_mode();
    QSize get_viewport_size();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QPdfView *pdf_view;                         // pdf를 표시
    QPdfDocument *pdf_document;                 // pdf의 데이터를 제공
    QPdfPageNavigator *pdf_page_navigator;      // pdf의 페이지 탐색
    QUrl url;
    bool using_tool_bar;
    int prev_page_index, current_page_index;
    QPdfView::PageMode page_mode;
    QPdfView::ZoomMode zoom_mode;

    void set_connects();
    void set_pdf_viewer();

    bool drag_and_drop = false;
    QPoint start_pos;
    int start_h = 0; int start_v = 0;

signals:
    void update_page_line_edit();
    void update_current_zoom(const qreal zoom);
    void set_attribute();
};
#endif // PDF_VIEWER_WIDGET_H
