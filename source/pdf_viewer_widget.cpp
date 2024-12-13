#include "pdf_viewer_widget.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QScrollBar>
#include <QMetaObject>
#include <QMetaMethod>
#include <QTimer>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QSize>
#include <QSizeF>
#include <QTransform>

Pdf_Viewer_Widget::Pdf_Viewer_Widget(const QUrl &url, QWidget *parent)
    : url(url), QWidget(parent)
    , pdf_view(new QPdfView(this)), pdf_document(new QPdfDocument(this)), pdf_page_navigator(new QPdfPageNavigator(this))
    , using_tool_bar(false)
{
    set_pdf_viewer();
    set_connects();
    setFocusPolicy(Qt::StrongFocus);
}

bool Pdf_Viewer_Widget::eventFilter(QObject *obj, QEvent *event){
    QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>(event);
    if(mouse_event){
        QScrollBar *h = pdf_view->horizontalScrollBar();
        QScrollBar *v = pdf_view->verticalScrollBar();
        if(mouse_event->type() == QEvent::MouseButtonPress){
            start_pos = mouse_event->pos();
            start_h = h->value();
            start_v = v->value();

            drag_and_drop = true;
        }

        if(mouse_event->type() == QEvent::MouseMove && drag_and_drop){
            QPoint delta = mouse_event->pos() - start_pos;

            h->setValue(start_h - delta.x());
            v->setValue(start_v - delta.y());
        }

        if(mouse_event->type() == QEvent::MouseButtonRelease){
            drag_and_drop = false;
        }
    }

    // 다른 이벤트는 부모 클래스의 이벤트 필터로 전달
    return QWidget::eventFilter(obj, event);
}

void Pdf_Viewer_Widget::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_D){
        emit set_attribute();
    }
}

int Pdf_Viewer_Widget::get_current_page_index(){
    return current_page_index;
}

int Pdf_Viewer_Widget::get_total_page_index(){
    return pdf_document->pageCount() - 1;
}

qreal Pdf_Viewer_Widget::get_current_zoom(){
    return pdf_view->zoomFactor();
}

qreal Pdf_Viewer_Widget::get(){
    int x = pdf_document->pagePointSize(current_page_index).width();
    qreal point_to_pixel = 96.0 / 72.0;
    qreal scaled_x = x * point_to_pixel;

    return get_viewport_size().width() / scaled_x;
}

qreal Pdf_Viewer_Widget::get_2(){
    int x = pdf_document->pagePointSize(current_page_index).width();
    int y = pdf_document->pagePointSize(current_page_index).height();

    qreal point_to_pixel = 96.0 / 72.0;

    qreal scaled_x = x * point_to_pixel;
    qreal scaled_y = y * point_to_pixel;

    if(x >= y){
        return get_viewport_size().width() / scaled_x;
    }
    else{
        return get_viewport_size().height() / scaled_y;
    }
}

void Pdf_Viewer_Widget::page_changed(const int changed_page_index){
    using_tool_bar = true;

    pdf_page_navigator->jump(changed_page_index, {}, pdf_page_navigator->currentZoom());
    current_page_index = changed_page_index;
    emit update_page_line_edit();

    using_tool_bar = false;
}

void Pdf_Viewer_Widget::zoom_changed(const qreal zoom){
    if(pdf_view->zoomMode() == QPdfView::ZoomMode::FitInView || pdf_view->zoomMode() == QPdfView::ZoomMode::FitToWidth){
        pdf_view->setZoomMode(QPdfView::ZoomMode::Custom);
    }
    pdf_view->setZoomFactor(zoom);
    emit update_current_zoom(get_current_zoom());
}

void Pdf_Viewer_Widget::set_page_mode(QPdfView::PageMode page_mode){
    pdf_view->setPageMode(page_mode);
}

void Pdf_Viewer_Widget::set_zoom_mode(QPdfView::ZoomMode zoom_mode){
    pdf_view->setZoomMode(zoom_mode);
}

void Pdf_Viewer_Widget::set_scroll_bar(Qt::ScrollBarPolicy policy){
    pdf_view->setHorizontalScrollBarPolicy(policy);
    pdf_view->setVerticalScrollBarPolicy(policy);
}

QPdfView::PageMode Pdf_Viewer_Widget::get_current_page_mode(){
    return pdf_view->pageMode();
}

QSize Pdf_Viewer_Widget::get_viewport_size(){
    return pdf_view->viewport()->size();
}

void Pdf_Viewer_Widget::set_connects(){
    // 스크롤 바 사용시 페이지 표시 업데이트
    connect(pdf_view->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](){
        if(using_tool_bar){
            return;
        }
        prev_page_index = current_page_index;
        current_page_index = pdf_page_navigator->currentPage();
        if(prev_page_index != current_page_index){
            prev_page_index = current_page_index;
            emit update_page_line_edit();
        }
    });


    connect(pdf_view, &QPdfView::pageModeChanged, this, [this](QPdfView::PageMode changed_page_mode){
        if(changed_page_mode == QPdfView::PageMode::MultiPage){
            QMetaObject::invokeMethod(this, [this, changed_page_mode]{
                pdf_page_navigator->jump(current_page_index, {}, pdf_page_navigator->currentZoom());
            }, Qt::QueuedConnection);
        }
    });
}

void Pdf_Viewer_Widget::set_pdf_viewer(){
    // pdf에 표시할 문서
    pdf_view->setDocument(pdf_document);
    pdf_document->load(url.toLocalFile());

    // pdf
    pdf_page_navigator = pdf_view->pageNavigator();
    current_page_index = pdf_page_navigator->currentPage();
    pdf_page_navigator->jump(current_page_index, {}, pdf_page_navigator->currentZoom());
    pdf_view->setDocumentMargins(QMargins(0, 0, 0, 0));
    set_page_mode(QPdfView::PageMode::MultiPage);
    set_zoom_mode(QPdfView::ZoomMode::FitToWidth);
    pdf_view->viewport()->installEventFilter(this);

    // 레이아웃
    QVBoxLayout *vertical_layout = new QVBoxLayout(this);
    vertical_layout->setContentsMargins(0, 0, 0, 0);
    vertical_layout->addWidget(pdf_view);
    setLayout(vertical_layout);
}

