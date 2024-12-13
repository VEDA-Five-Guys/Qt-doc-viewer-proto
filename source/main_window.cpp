#include "main_window.h"
#include "ui_main_window.h"

#include "pdf_viewer_widget.h"
#include "event_overlay_widget.h"

#include <QLineEdit>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPointF>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMetaObject>
#include <QMetaMethod>
#include <algorithm>

Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_Window)
    , event_overlay_widget(new Event_Overlay_Widget(nullptr))
    , focused_widget(nullptr), focused_pdf_viewer_widget(nullptr)
    , pdf_dialog(nullptr)
    , zoom_list{0.10, 0.25, 0.33, 0.50, 0.67, 0.75, 0.80, 0.90, 1.00,
                1.10, 1.25, 1.50, 1.75, 2.00, 2.50, 3.00, 4.00, 5.00}{
    ui->setupUi(this);
    ui->widget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->widget->layout()->setSpacing(0);
    ui->widget->layout()->setAlignment(Qt::AlignTop);
    set_tool_bar();
    set_connects();
}

Main_Window::~Main_Window(){
    delete ui;
}

bool Main_Window::eventFilter(QObject *obj, QEvent *event){
    if(event->type() == QEvent::Enter){
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if(widget){
            hash_2.value(widget)->setVisible(true);
        }
    }
    else if(event->type() == QEvent::Leave){
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if(widget){
            hash_2.value(widget)->setVisible(false);
        }
    }
    emit f();

    return QWidget::eventFilter(obj, event);
}

void Main_Window::set_tool_bar(){
    // 아래 정렬을 위한 스페이서
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 페이지
    QWidget *widget = new QWidget(this);
    QVBoxLayout *vertical_layout = new QVBoxLayout(widget);
    vertical_layout->setContentsMargins(0, 0, 0, 0);

    page_line_edit = new QLineEdit(this);
    page_line_edit->setFixedSize(40, 40);
    page_line_edit->setFrame(false);
    page_line_edit->setAlignment(Qt::AlignCenter);
    page_line_edit->setToolTip("특정 페이지 숫자로 이동");

    total_page_label = new QLabel(this);
    total_page_label->setFixedSize(40, 40);
    total_page_label->setAlignment(Qt::AlignCenter);
    total_page_label->setToolTip("현재 PDF의 총 페이지 수");

    vertical_layout->addWidget(page_line_edit, 0, Qt::AlignCenter);
    vertical_layout->addWidget(total_page_label, 0, Qt::AlignCenter);

    action_prev_page = new QAction(this);
    action_prev_page->setText("▲");
    action_prev_page->setToolTip("이전 페이지로 이동");

    action_next_page = new QAction(this);
    action_next_page->setText("▼");
    action_next_page->setToolTip("다음 페이지로 이동");

    // 줌
    zoom_out = new QAction(this);
    zoom_out->setText("-");

    QWidget *widget_2 = new QWidget(this);
    QVBoxLayout *vertical_layout_2 = new QVBoxLayout(widget_2);
    vertical_layout_2->setContentsMargins(0, 0, 0, 0);

    current_zoom = new QLineEdit(this);
    current_zoom->setFixedSize(40, 40);
    current_zoom->setFrame(false);
    current_zoom->setAlignment(Qt::AlignCenter);

    vertical_layout_2->addWidget(current_zoom, 0, Qt::AlignCenter);

    zoom_in = new QAction(this);
    zoom_in->setText("+");

    // 전체 화면
    action_full_screen = new QAction(this);
    action_full_screen->setText("발표");
    action_full_screen->setToolTip("전체 화면");

    // 순서에 맞게 툴 바에 추가
    ui->tool_bar->addWidget(spacer);
    ui->tool_bar->addWidget(widget);
    ui->tool_bar->addAction(action_prev_page);
    ui->tool_bar->addAction(action_next_page);
    ui->tool_bar->addSeparator();
    ui->tool_bar->addAction(zoom_out);
    ui->tool_bar->addWidget(widget_2);
    ui->tool_bar->addAction(zoom_in);
    ui->tool_bar->addSeparator();
    ui->tool_bar->addAction(action_full_screen);
}

void Main_Window::set_connects(){
    // pdf 목록 불러오기
    connect(ui->load_push_button, &QPushButton::clicked, this, &Main_Window::load_push_button_clicked);

    // 툴바
    // pdf 추가 및 변경 시 정보 업데이트
    connect(this, &Main_Window::current_widget_changed, this, [this](const QString &name){
        if(name == "page"){
            page_line_edit->clear();
            total_page_label->clear();
            set_name("");
            qDebug() << "focused_widget and focused_pdf_viewer_widgte are null";
            return;
        }

        focused_widget = hash.value(name).first;
        if(focused_widget){
            focused_pdf_viewer_widget = focused_widget->findChild<Pdf_Viewer_Widget*>();
            if(focused_pdf_viewer_widget){
                current_page_index = focused_pdf_viewer_widget->get_current_page_index();
                page_line_edit->setText(QString::number(current_page_index + 1));
                total_page_index = focused_pdf_viewer_widget->get_total_page_index();
                total_page_label->setText(QString::number(total_page_index + 1));
                // 비율 수동 계산-----
                zoom = focused_pdf_viewer_widget->get();
                int ratio = zoom * 100;
                current_zoom->setText(QString::number(ratio).append('%'));
                // -----------------
                set_name(name);
            }
            else{
                qDebug() << "focused_pdf_viewer_widget is invalid";
                return;
            }
        }
        else{
            qDebug() << "focused_widget is invalid";
            return;
        }
    });
    // 페이지
    connect(page_line_edit, &QLineEdit::returnPressed, this, [this](){
        QString str = page_line_edit->text();
        page_line_edit_return_pressed(str);
    });
    connect(action_prev_page, &QAction::triggered, this, &Main_Window::action_prev_page_triggered);
    connect(action_next_page, &QAction::triggered, this, &Main_Window::action_next_page_triggered);
    connect(action_full_screen, &QAction::triggered, this, &Main_Window::action_full_screen_triggered);
    // 줌
    connect(zoom_out, &QAction::triggered, this, &Main_Window::zoom_out_triggered);
    connect(current_zoom, &QLineEdit::returnPressed, this, [this](){
        QString str = current_zoom->text();
        current_zoom_return_pressed(str);
    });
    connect(zoom_in, &QAction::triggered, this, &Main_Window::zoom_in_triggered);
    // 전체화면
    connect(event_overlay_widget, &Event_Overlay_Widget::restore_from_full_screen, this, &Main_Window::restore_from_full_screen);
}

void Main_Window::load_push_button_clicked(){
    if(pdf_dialog == nullptr){
        pdf_dialog = new QFileDialog(this, tr("open pdf"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));   // 기본 경로 : 다운로드
        pdf_dialog->setAcceptMode(QFileDialog::AcceptOpen);   // 열기 모드
        pdf_dialog->setMimeTypeFilters({"application/pdf"});  // pdf 문서만 표시

        if(pdf_dialog->exec() == QDialog::Accepted){
            const QUrl url = pdf_dialog->selectedUrls().constFirst();
            if(url.isValid()){
                open_pdf(url);
            }
        }
        else{
            qDebug() << "pdf_dialog not accepted";
            return;
        }

        pdf_dialog->deleteLater();
        pdf_dialog = nullptr;
    }
    else{
        qDebug() << "pdf_dialog already exists";
        return;
    }
}

void Main_Window::open_pdf(const QUrl &url){
    if(url.isLocalFile()){
        Pdf_Viewer_Widget *pdf_viewer_widget = new Pdf_Viewer_Widget(url, this);

        int num = 2;
        QString original_name = url.fileName().remove(".pdf");
        QString name = original_name;
        while(hash.contains(name)){
            name = QString("%1_%2").arg(original_name).arg(num++);
        }

        hash.insert(name, {make_widget(pdf_viewer_widget, name), make_button(name)});


        emit current_widget_changed(name);

        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::update_page_line_edit, this, [this, pdf_viewer_widget](){
            current_page_index = pdf_viewer_widget->get_current_page_index();
            page_line_edit->setText(QString::number(current_page_index + 1));
        });
        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::update_current_zoom, this, [this, pdf_viewer_widget](const qreal zoom){
            this->zoom = zoom;
            int ratio = zoom * 100;
            current_zoom->setText(QString::number(ratio).append('%'));
        });
        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::set_attribute, this, [this, pdf_viewer_widget](){
            if(event_overlay_widget->parent() != nullptr){
                event_overlay_widget->set_attribute();
            }
        });
    }
    else{
        qDebug() << "can't open pdf";
    }
}

QWidget *Main_Window::make_widget(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name){
    QWidget *widget = new QWidget(ui->stacked_widget);
    widget->setObjectName(name);

    QStackedLayout *stacked_layout = new QStackedLayout(widget);
    stacked_layout->setStackingMode(QStackedLayout::StackAll);
    stacked_layout->setContentsMargins(0, 0, 0, 0);
    stacked_layout->addWidget(pdf_viewer_widget);

    widget->setLayout(stacked_layout);

    ui->stacked_widget->addWidget(widget);
    ui->stacked_widget->setCurrentWidget(widget);

    return widget;
}

QWidget *Main_Window::make_button(const QString &name){
    QWidget *widget = new QWidget(ui->widget);
    widget->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *button = new QPushButton(widget);
    button->setMinimumWidth(113);
    button->setFixedHeight(30);

    QPushButton *button_2= new QPushButton(widget);
    button_2->setText("✕");
    button_2->setFixedSize(30, 30);
    button_2->setStyleSheet(
        "text-align: center;"
        );
    button_2->setVisible(false);

    layout->addWidget(button);
    layout->addWidget(button_2);

    widget->setLayout(layout);

    ui->widget->layout()->addWidget(widget);

    QMetaObject::invokeMethod(this, [this, name, button]{
        int width = button->width();
        QString elided_name = button->fontMetrics().elidedText(name, Qt::ElideRight, width - 5);
        button->setText(elided_name);
    }, Qt::QueuedConnection);

    QMetaObject::Connection connection_f = connect(this, &Main_Window::f, this, [this, name, button]{
        int width = button->width();
        QString elided_name = button->fontMetrics().elidedText(name, Qt::ElideRight, width - 5);
        button->setText(elided_name);
    });
    connect(button, &QPushButton::clicked, this, [this, name]{
        QWidget *named_widget = hash.value(name).first;
        QWidget *current_widget = ui->stacked_widget->currentWidget();

        if(named_widget && current_widget){
            if(current_widget != named_widget){
                ui->stacked_widget->setCurrentWidget(named_widget);

                emit current_widget_changed(name);
            }
            else{
                qDebug() << "named_widget and current_widget are the same";
                return;
            }
        }
        else{
            qDebug() << "named_widget or current_widget or both are null or invalid";
            return;
        }
    });
    connect(button_2, &QPushButton::clicked, this, [this, name, connection_f](){
        disconnect(connection_f);

        QWidget *named_widget = hash.value(name).first;
        if(named_widget){
            ui->stacked_widget->removeWidget(named_widget);
            named_widget->deleteLater();

            focused_widget = nullptr;
            focused_pdf_viewer_widget = nullptr;

            const QString name = ui->stacked_widget->currentWidget()->objectName();
            emit current_widget_changed(name);
        }
        else{
            qDebug() << "named_widget is null or invalid";
            return;
        }

        QWidget *named_widget_2 = hash.value(name).second;
        if(named_widget_2){
            ui->widget->layout()->removeWidget(named_widget_2);
            named_widget_2->removeEventFilter(this);
            named_widget_2->hide();
            named_widget_2->deleteLater();
        }
        else{
            qDebug() << "named_widget_2 is null or invalid";
            return;
        }

        hash.remove(name);
        hash_2.remove(named_widget_2);
    });

    hash_2.insert(widget, button_2);

    return widget;
}

void Main_Window::set_name(const QString &name){
    this->name = name;
}

void Main_Window::page_line_edit_return_pressed(const QString &input_text){
    if(focused_pdf_viewer_widget){
        bool ok = false;
        int index = input_text.toInt(&ok) - 1;
        if(ok && index >= 0 && index <= total_page_index){
            focused_pdf_viewer_widget->page_changed(index);
        }
        page_line_edit->setText(QString::number(focused_pdf_viewer_widget->get_current_page_index() + 1));
    }
    else{
        qDebug() << "focused_pdf_viewer_widget is null(page_line_edit_return_pressed)";
        return;
    }
}

void Main_Window::action_prev_page_triggered(){
    if(focused_pdf_viewer_widget){
        if(current_page_index > 0){
            int prev_page_index = current_page_index - 1;
            focused_pdf_viewer_widget->page_changed(prev_page_index);
        }
    }
    else{
        qDebug() << "focused_pdf_viewer_widget is null(action_prev_page_triggered)";
        return;
    }
}

void Main_Window::action_next_page_triggered(){
    if(focused_pdf_viewer_widget){
        if(current_page_index < total_page_index){
            int next_page_index = current_page_index + 1;
            focused_pdf_viewer_widget->page_changed(next_page_index);
        }
    }
    else{
        qDebug() << "focused_pdf_viewer_widget is null(action_next_page_triggered)";
        return;
    }
}

void Main_Window::zoom_out_triggered(){
    if(focused_pdf_viewer_widget){
        zoom_list_idx = find_nearest_zoom(zoom);
        if(zoom_list_idx <= 0){
            focused_pdf_viewer_widget->zoom_changed(zoom_list.first());
            return;
        }
        focused_pdf_viewer_widget->zoom_changed(zoom_list[zoom_list_idx - 1]);
    }
    else{
        qDebug() << "zoom_out error";
        return;
    }
}

void Main_Window::current_zoom_return_pressed(const QString &input_text){
    if(focused_pdf_viewer_widget){
        QString without_percent(input_text);
        without_percent.remove(QLatin1Char('%'));

        bool ok = false;
        const int ratio = without_percent.toInt(&ok);
        if(ok){
            const qreal zoom = static_cast<qreal>(ratio) / 100;
            focused_pdf_viewer_widget->zoom_changed(
                zoom < zoom_list.first()
                ? zoom_list.first()
                : (zoom > zoom_list.last()) ? zoom_list.last() : zoom
            );
        }
    }
    else{
        qDebug() << "current_zoom error";
        return;
    }
}

void Main_Window::zoom_in_triggered(){
    if(focused_pdf_viewer_widget){
        zoom_list_idx = find_nearest_zoom(zoom);
        if(zoom_list_idx >= zoom_list.size() - 1){
            focused_pdf_viewer_widget->zoom_changed(zoom_list.last());
            return;
        }
        focused_pdf_viewer_widget->zoom_changed(zoom_list[zoom_list_idx + 1]);
    }
    else{
        qDebug() << "zoom_in error";
        return;
    }
}

int Main_Window::find_nearest_zoom(const qreal zoom){
    auto it = std::lower_bound(zoom_list.begin(), zoom_list.end(), zoom);
    if( *it > zoom){
        it--;
    }
    int num = it - zoom_list.begin();
    return num;
}

void Main_Window::action_full_screen_triggered(){
    if(focused_widget){
        if(focused_pdf_viewer_widget){
            prev_zoom = zoom;

            focused_pdf_viewer_widget->set_page_mode(QPdfView::PageMode::SinglePage);
            focused_pdf_viewer_widget->set_scroll_bar(Qt::ScrollBarAlwaysOff);
            focused_pdf_viewer_widget->set_zoom_mode(QPdfView::ZoomMode::FitInView);

            QMetaObject::invokeMethod(this, [this](){
                // 비율 수동 계산-----
                zoom = focused_pdf_viewer_widget->get_2();
                int ratio = zoom * 100;
                current_zoom->setText(QString::number(ratio).append('%'));
                // -----------------

            }, Qt::QueuedConnection);

            focused_widget->layout()->addWidget(event_overlay_widget);
            event_overlay_widget->show();
            event_overlay_widget->raise();

            ui->stacked_widget->removeWidget(focused_widget);
            ui->stacked_widget->setCurrentIndex(0);
            this->hide();
            focused_widget->setParent(nullptr);
            focused_widget->showFullScreen();
            event_overlay_widget->setFocus();
        }
        else{
            qDebug() << "focus_pdf_viwwer_widget is null(action_full_screen_triggered)";
            return;
        }
    }
    else{
        qDebug() << "focus_widget is null(action_full_screen_triggered)";
        return;
    }
}

void Main_Window::restore_from_full_screen(){
    if(focused_widget){
        if(focused_pdf_viewer_widget){
            focused_pdf_viewer_widget->set_page_mode(QPdfView::PageMode::MultiPage);
            focused_pdf_viewer_widget->set_scroll_bar(Qt::ScrollBarAsNeeded);

            focused_widget->layout()->removeWidget(event_overlay_widget);
            event_overlay_widget->setParent(nullptr);
            event_overlay_widget->set_paint_mode(-1);
            event_overlay_widget->hide();

            this->show();
            ui->stacked_widget->addWidget(focused_widget);
            ui->stacked_widget->setCurrentWidget(focused_widget);

            emit current_widget_changed(name);
            focused_pdf_viewer_widget->zoom_changed(prev_zoom);
        }
        else{
            qDebug() << "focus_pdf_viwwer_widget is null(restore_from_full_screen)";
            return;
        }
    }
    else{
        qDebug() << "focus_widget is null(restore_from_full_screen)";
        return;
    }
}

void Main_Window::set_paint_mode(int paint_mode){
    if(event_overlay_widget->parent() == focused_widget){
       event_overlay_widget->set_paint_mode(paint_mode);
    }
    else{
        qDebug() << "event_overlay_widget is yet added on focused_widget(set_paint_mode)";
    }
}

void Main_Window::set_pos(const int x, const int y){
    if(event_overlay_widget->parent() == focused_widget){
        qreal scaleX = static_cast<qreal>(focused_pdf_viewer_widget->get_viewport_size().width()) / 640;
        qreal scaleY = static_cast<qreal>(focused_pdf_viewer_widget->get_viewport_size().height()) / 480;

        qreal max_scale = std::max(scaleX, scaleY);

        qreal scaled_x = x * max_scale;
        qreal scaled_y = y * max_scale;

        event_overlay_widget->set_pos(scaled_x, scaled_y);
    }
    else{
        qDebug() << "event_overlay_widget is yet added on focused_widget(set_pos)";
    }
}
