#ifndef PAGOSHISTORICOS_H
#define PAGOSHISTORICOS_H

#include <QDialog>

namespace Ui {
  class PagosHistoricos;
}

class QSqlTableModel;

class PagosHistoricos : public QDialog
{
  Q_OBJECT

public:
  explicit PagosHistoricos(QWidget *parent = 0);
  ~PagosHistoricos();

private slots:
  void on_calendarWidget_clicked(const QDate &date);
  void on_pushButton_calendario_clicked();
  void on_pB_tablaCompleta_clicked();
  void marcarCalendario();

private:
  Ui::PagosHistoricos *ui;
  QSqlTableModel *tablaPagosHist;
  bool calendarioVisible = false;
};

#endif // PAGOSHISTORICOS_H
