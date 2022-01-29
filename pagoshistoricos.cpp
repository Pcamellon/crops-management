#include "pagoshistoricos.h"
#include "ui_pagoshistoricos.h"
#include "controlcultivos.h"
#include <QSqlTableModel>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QSqlQuery>
#include <QDebug>
#include <QDir>

PagosHistoricos::PagosHistoricos(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PagosHistoricos)
{
  ui->setupUi(this);

  tablaPagosHist = new QSqlTableModel;
  tablaPagosHist->setTable("Pagos_Historicos_Tabaco");
  tablaPagosHist->setEditStrategy(QSqlTableModel::OnManualSubmit);
  tablaPagosHist->select();
  ui->tableView->setModel(tablaPagosHist);

  /// Ocultamos la seccion del calendario
  ui->calendarWidget->hide();
  ui->label_selecFecha->hide();
}

PagosHistoricos::~PagosHistoricos(){
  delete ui;
}

void PagosHistoricos::on_calendarWidget_clicked(const QDate &date){
  //  falta   senalar dias en el calendario

  tablaPagosHist->setFilter(tablaPagosHist->headerData(0,Qt::Horizontal).toString().toLower()
                            + "='"
                            + date.toString(Qt::ISODate)
                            + "'");
  qDebug() << tablaPagosHist->headerData(0,Qt::Horizontal).toString() + " = '" + date.toString(Qt::ISODate);
}

void PagosHistoricos::on_pushButton_calendario_clicked(){
  if(calendarioVisible){
      ui->calendarWidget->hide();
      ui->label_selecFecha->hide();
      calendarioVisible = false;
    } else {
      ui->calendarWidget->setHidden(false);
      ui->label_selecFecha->setHidden(false);
      calendarioVisible = true;
    }
}

void PagosHistoricos::on_pB_tablaCompleta_clicked(){
  tablaPagosHist->setFilter("");
}

void PagosHistoricos::marcarCalendario(){
//  QPainter painter;
//  QRect *rect = new QRect;
//  qint32 i = 0;
//  while(i < tablaPagosHist->rowCount()) {
//      if(tablaPagosHist->data(tablaPagosHist->index(i,0)) != ""){
//          ui->calendarWidget->paintCell(painter,rect,
//                                        QDate::fromString(tablaPagosHist->data(tablaPagosHist->index(i,0)))
//                                        );

//        }
//    }
//  ui->calendarWidget->hide();
}
