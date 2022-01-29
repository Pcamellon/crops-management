/* LEYENDA (buscar):
 * falta: sin implementar
 * error:
 * nf: no funciona
 * cambiar
 
     nf   Estmos usando QSqlQueryModel por lo que las transacciones
          se hacen directamente con la bd
* Cuadro de Dialogo: cuanto X dia --- cant de dias
* Cant de trabajadores +- cte.
*
* QComboBox "cultivos" eliminado -> version para tabaco
* leer y guardar labores de un txt
*
*/
#include <QObject>
#include "ui_controlcultivos.h"
#include "controlcultivos.h"
#include "pagoshistoricos.h"
#include "compras.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
//#include <QFileDialog>
#include <QDebug>
#include <QDate>
#include <QDir>

//#include <QMouseEvent>

ControlCultivos::ControlCultivos(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ControlCultivos)
{
  ui->setupUi(this);

  connect(this,&ControlCultivos::pagosTotalesChanged,
          this,&ControlCultivos::calcPagosTotales);
  
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  QString dirDB = QDir::currentPath();
  db.setDatabaseName(dirDB + "/bd.sqlite");
  
  if(!db.open()){
    QMessageBox::critical(this,"Error!",
                          "<h1>Hola!</h1><br>"
                          "No puedo cargar la base de datos",
                          QMessageBox::Cancel);

  }
  
  tabla = new QSqlTableModel(this,db);
  
  /* + (ui->cB_selecCultivo->currentText() + 1)*/
  tabla->setTable("Pago_Semanal_Tabaco"); //Pago_Semanal_Tabaco
  //  if (tabla->lastError()){
  //        qDebug() << tabla->lastError();
  //  }
  tabla->setEditStrategy(QSqlTableModel::OnManualSubmit);
  tabla->select();
  ui->tableView->setModel(tabla);
  
  /// ComboBox "Labores"
  QFile laboresTxt;
  QTextStream labTxtStr(&laboresTxt);
  qDebug() << QDir::currentPath();
  if(QFile::exists(QDir::currentPath() + "/labores.txt")){
    laboresTxt.setFileName(QDir::currentPath() + "/labores.txt");
    laboresTxt.open(QIODevice::ReadWrite | QIODevice::Text);
  } else {
    QMessageBox::warning(this, "No encuentro tus labores",
                         "Al parecer labores.txt ha desaparecido.<br>"
                         "No te preocupes, crearé uno nuevo con <br>"
                         "algunas labores por defecto.",
                         QMessageBox::Ok);
    laboresTxt.setFileName(QDir::currentPath() + "/labores.txt");
    laboresTxt.open(QIODevice::ReadWrite | QIODevice::Text);
    
    labTxtStr << "Labor: Chapea\nSiembra\nRiego\n";
  }
  
  /// Estableciendo labores desde labores.txt
  while(!labTxtStr.atEnd()){
    QString lab = labTxtStr.readLine();
    qDebug() << lab;
    ui->cB_Labor->addItem(lab);
  }

  // no funciona
  //  ui->tableView->setSizeAdjustPolicy(ui->tableView->AdjustToContents);
  
  //  error
  //  ui->cB_diaSemana->setModel(tabla);
  //  ui->cB_diaSemana->setModelColumn(tabla->fieldIndex("Obrero"));
  
  //    connect(&ui->cB_selecCultivo, &QComboBox::currentTextChanged,
  //            &ControlCultivos::actualizarVista);
}

ControlCultivos::~ControlCultivos(){
  delete ui;
}

void ControlCultivos::on_actionSalir_triggered(){
  close();
}

void ControlCultivos::on_actionNueva_Cosecha_triggered(){ //falta
  /// Nueva Cosecha:
  /// 1. Tabla nueva para la cosecha
  /// 1. ** ej: guardamos la semana actual a la tabla de "Pagos Hist"
  /// y vaciamos "Tabla Semanal Tabaco"
  
  // llamamos a Cierra Semanal para que guarde a Pagos Hist
  //  ui->actionCierre_Semanal->trigger();
  
  if(tabla->rowCount() > 3){
    tabla->removeRows(3, tabla->rowCount()-3);
    qDebug() << "rows eliminadas";
  }
  
  for(int fila = 0; fila < 3; ++fila){
    for(int col = 0; col < tabla->columnCount(); ++col){
      tabla->setData(tabla->index(fila,col),"");
    }
  }
  
  emit ui->actionGuardar_Cosecha->triggered();
  
  qDebug() << "end on_actionNueva_Cosecha_triggered";
}

void ControlCultivos::on_actionEliminarCultivo_triggered(){ // falta
  /* OK
    // Eliminar tabla de db
    QString q = "DROP TABLE '" + ui->cB_selecCultivo->currentText()
            + "'";
    tabla->database().exec(q);
    ui->cB_selecCultivo->removeItem(ui->cB_selecCultivo->currentIndex());
*/
  qDebug("Del Tabla");
}

void ControlCultivos::on_actionAdd_Labor_triggered(){ //falta
  bool ok;
  QString laborNueva = QInputDialog::getText(this, "Añadir labor",
                                             "Nombre:", QLineEdit::Normal,
                                             "Labor",&ok);
  
  if(laborNueva != "" /*|| trabjNuevo != "Trabajador Nuevo"*/){
    ui->cB_Labor->addItem(laborNueva);
  } else {
    int reintentar = QMessageBox::warning(this,"Labor sin nombre",
                                          "Debe indicar un nombre para la nueva labor",
                                          QMessageBox::Retry
                                          | QMessageBox::Cancel);
    if(reintentar == QMessageBox::Retry){
      on_actionAdd_Labor_triggered();
    }
  }
  qDebug()<< "end on_actionAdd_Labor_triggered";
}

void ControlCultivos::on_actionEliminar_Labor_triggered(){ //falta
  ui->cB_Labor->removeItem(ui->cB_Labor->currentIndex());
}

void ControlCultivos::on_actionAdd_Obrero_triggered(){ // beta
  bool ok;
  QString trabjNuevo = QInputDialog::getText(this, "Añadir trabajador",
                                             "Nombre:", QLineEdit::Normal,
                                             "Trabajador Nuevo",&ok);
  
  while (trabjNuevo == "" || trabjNuevo == "Trabajador Nuevo") {
    int reintentar = QMessageBox::warning(this,"Revise el nombre",
                                          "Debe indicar un nombre para<br>"
                                          "el nuevo trabajador",
                                          QMessageBox::Retry
                                          | QMessageBox::Cancel);
    if(reintentar == QMessageBox::Retry){
     /// error (mensaje sale doble)
      emit ui->actionAdd_Obrero->triggered();
    } else if(reintentar == QMessageBox::Cancel){
      return;
    }
  }
  
  tabla->insertRow(tabla->rowCount());
  tabla->setData(tabla->index(tabla->rowCount()-1, 0), trabjNuevo);
  
  // guardamos a la db
  emit ui->actionGuardar_Cosecha->triggered();
  
  qDebug()<<"end on_actionAdd_Obrero_triggered";
}

void ControlCultivos::on_actionEliminar_Obrero_triggered(){
  tabla->removeRow(filaActual);
  
  // guardamos a la db
  emit ui->actionGuardar_Cosecha->triggered();
  
  qDebug()<<"end on_actionEliminar_Obrero_triggered";
}

void ControlCultivos::on_actionGuardar_Cosecha_triggered(){
  /// Cambiar esto:
  /// Primero definir que es un cambio en el programa:
  /// - Tablas con valores modificados
  /// - Nuevos cultivos anadidos
  /// - Cant Obreros y labores modificados
  /// ** Cada uno de estos modificaría una variable o
  /// emitiria una senal que refleje que se han realizado
  /// cambios. **
  /// Luego:
  /// Si detecta cambios que guarde automaticamente NO
  
  if(!tabla->submitAll()){
    QMessageBox::warning(this,"Error al guardar",
                         "No se han podido guardar sus cambios<br>"
                         "en la base de datos");
    return;
  }
  
  ui->statusBar->showMessage("Guardado!!",1500);
}

void ControlCultivos::on_cB_selecCultivo_currentTextChanged(/*const QString &textCBox*/){
  //falta
}

//void ControlCultivos::actualizarVista(){
//  // Esto cambia segun el cultivo selec
//  QString text = ui->cB_selecCultivo->currentText();
//  tabla->setTable("Pago_Semanal");
//  tabla->select();
//  ui->tableView->setModel(tabla);
//  qDebug("Vista Actualizada");
//}

void ControlCultivos::on_tableView_clicked(const QModelIndex &index){
  setFilaColActual(index.row(),index.column());
  qDebug()<<QString("Fila, Col : %1, %2").arg(filaActual).arg(colActual);
  
  ui->label_nombreObrero->setText(tabla->data(tabla->index(filaActual,0))
                                  .toString());
  
//  emit this->pagosTotalesChanged();
}

void ControlCultivos::on_actionAnadir_Cultivo_triggered(){ //error
  /*  bool ok;
  QString cultivoNuevo;
  
  cultivoNuevo = QInputDialog::getText(this, "Añadir cultivo",
                                       "Nombre:", QLineEdit::Normal,
                                       "Nuevo_Cultivo",&ok);
                                       
  /// Comprobacion de valor de cultivoNuevo
  if(ok && !cultivoNuevo.isEmpty()){
      /// Anadir cultivo al ComboBox "cB_selecCultivo"
      ui->cB_selecCultivo->addItem(cultivoNuevo);
      
      // error creando tablas
      /// Creamos las tablas para el nuevo cultivo
      QString q = "CREATE TABLE '" + cultivoNuevo + "'";
      QSqlQuery encuesta(q);
      if(!encuesta.exec()){
          qDebug()<<"Problemas creando tabla nueva";
        } else {
          qDebug()<<"tabla nueva creada";
        }
      q = "CREATE TABLE 'Pagos_Historicos_" + cultivoNuevo + "'";
      encuesta.prepare(q);
      if(!encuesta.exec()){
          qDebug()<<"Problemas creando tabla nueva";
        } else {
          qDebug()<<"tabla nueva creada";
        }
    }
    
  //selecc en cB al nuevo cultivo
  ui->cB_selecCultivo->setCurrentIndex(ui->cB_selecCultivo->count());
*/
}

void ControlCultivos::on_pB_AddPago_clicked(){
  // Advertir segun el caso
  /*&& ui->cB_selecCultivo->currentIndex() > 0*/
  if(ui->cB_diaSemana->currentIndex() == 0){
    ui->label_errorEnDatos->setText("Selecciona un dia de la semana");
    return;
  }
  if(ui->cB_Labor->currentIndex() == 0){
    ui->label_errorEnDatos->setText("Selecciona una labor");
    return;
  }

  /// Borrar aviso de error en datos
  ui->label_errorEnDatos->clear();

//  if(ui->cB_diaSemana->currentIndex() == 0
//     && ui->cB_Labor->currentIndex() == 0
//     /*&& ui->cB_selecCultivo->currentIndex() > 0*/
//     ){
//    QMessageBox::warning(this,"Error","Selecciona valores correctos,<br>"
//                                      "luego de clic en Añadir.",
//                         QMessageBox::Cancel);
//    return;
//  }
  
  QString dia = ui->cB_diaSemana->currentText();
  QString labor = ui->cB_Labor->currentText();
  int pago = ui->sB_Pago->value();
  qDebug() << dia << ";" << labor << ": " << QString::number(pago);

  if(dia == "Lunes"){
    tabla->setData(tabla->index(filaActual,1), labor);
    tabla->setData(tabla->index(filaActual,2), pago);
  }
  else if(dia == "Martes"){
    tabla->setData(tabla->index(filaActual,3), labor);
    tabla->setData(tabla->index(filaActual,4), pago);
  }
  else if(dia == "Miercoles"){
    tabla->setData(tabla->index(filaActual,5), labor);
    tabla->setData(tabla->index(filaActual,6), pago);
  }
  else if(dia == "Jueves"){
    tabla->setData(tabla->index(filaActual,7), labor);
    tabla->setData(tabla->index(filaActual,8), pago);
  } else if(dia == "Viernes"){
    tabla->setData(tabla->index(filaActual,9), labor);
    tabla->setData(tabla->index(filaActual,10), pago);
  }else if(dia == "Sabado"){
    tabla->setData(tabla->index(filaActual,11), labor);
    tabla->setData(tabla->index(filaActual,12), pago);
  }
  /*
  else
    QMessageBox::warning(this,"Error en datos","El dia de la semana seleccionado<br>"
                                               "no es valido.",QMessageBox::Cancel);
  */
}

void ControlCultivos::setFilaColActual(int fila, int col){
  filaActual = fila;
  colActual = col;
}

void ControlCultivos::calcPagosTotales(){
    /// Para calcular el pago total por obrero
    QVector<int> totales;
    int fila, col;
    for(fila = 0; fila <= tabla->rowCount(); ++fila){
      int total = 0;
      for(col = 2; col < tabla->columnCount(); col += 2){
        total += tabla->data(tabla->index(fila, col)).toInt();
      }
      totales << total;
    }

    for(fila = 0; fila < tabla->rowCount(); ++fila){
      tabla->setData(tabla->index(fila,tabla->columnCount()-1), totales.at(fila));
    }
}

void ControlCultivos::on_actionCierre_Semanal_triggered(){
  // error Falta guardar las modificaciones a "Pago_Semanal_Tabaco" antes
  // de llamar a CierreSemanal
  
  // 1. leemos los totales de la tabla mostrada de pagos semanales
  // 2. Añadimos obrero, fecha, pago total
  
  /// Evitar registros duplicados
  /// Comprobamos si hay cambios antes de guardar con isDirty()
  
  QList<QString> nombres; // contiene los obreros
  QList<int> totales; // contiene los pagos
  int numFilas = tabla->rowCount(); // de la tabla "Pago Semanal"
  
  for(qint8 i = 0; i < numFilas; ++i){
    nombres << tabla->data(tabla->index(i,0)).toString();
    totales << tabla->data(tabla->index(i,tabla->columnCount()-1)).toInt();
    qDebug() << QString("%1, %2").arg(nombres.at(i)).arg(totales.at(i));
  }

  if(nombres.length() == 0 || totales.length() == 0){
    QMessageBox::warning(this,"Error al procesar datos","¡Lo sentimos! Hubo problemas al recoger<br>"
                                                        "los datos de esta semana. "
                                                        "Por favor intentalo de nuevo. Si el problema<br>"
                                                        "persiste ponte en contacto con el autor del<br>"
                                                        "programa.");
    return;
  }

  /// Trabajando con "Pagos_Historicos"
  tabla->setTable("Pagos_Historicos_Tabaco");
  tabla->select();
  
  /// Rellenamos con los datos de arriba
  for(qint8 i = 0; i < numFilas; ++i){
    tabla->insertRow(tabla->rowCount());
    tabla->setData(tabla->index(tabla->rowCount()-1, 0),
                   QDate::currentDate().toString(Qt::ISODate));
    tabla->setData(tabla->index(tabla->rowCount()-1, 1),
                   nombres.at(i));
    tabla->setData(tabla->index(tabla->rowCount()-1, 2),
                   totales.at(0));
  }

  emit ui->actionGuardar_Cosecha->triggered();

  tabla->setTable("Pago_Semanal_Tabaco");
  tabla->select();
  /// Limpiamos la tabla semanal
  emit ui->actionNueva_Cosecha->triggered();

  /// Modificando Lab_Ob

  QSqlTableModel *tabla_Lab_Ob = new QSqlTableModel;
  tabla_Lab_Ob->setTable("Lab_Ob_Tabaco");
  QSqlTableModel *tabla_Pagos_Hist = new QSqlTableModel;
  tabla_Pagos_Hist->setTable("Pagos_Historicos_Tabaco");

  QString nombreObrero, nomObPagHist;
  qint8 total;

  for(qint8 i = 0; i < tabla_Lab_Ob->rowCount() - 1; ++i){
    nombreObrero = tabla_Lab_Ob->data(QSqlTableModel::index(i,0)).toString();

    for(qint8 j = 0; j < tabla_Pagos_Hist->rowCount(); ++i){
      nomObPagHist = tabla_Pagos_Hist->data(QSqlTableModel::index(j,0)).toString();

      if(nombreObrero != nomObPagHist){

      }
    }
  }

  qDebug() << "end on_actionCierre_Semanal_triggered";
}
//  error
//  while(i < numFilas){
//      //      bool dirty;
//      //      /// iteramos en cada fila buscando algun cambio,
//      //      for(int j = 0; j < tabla->columnCount(); ++j){
//      //          if(tabla->isDirty(tabla->index(i, j))){
//      //             dirty = true;
//      //            }
//      //          else dirty = false;
//      //        }

//      /// si hubo cambios guardamos
//      //      if(dirty){
//      nombres << tabla->data(tabla->index(i, 0)).toString();
//      totales << tabla->data(tabla->index(i, tabla->columnCount()-1)).toInt();
//      //              }
//      i++;
//    }
//  else {
//      QMessageBox::information(this, "sin cambios","sin cambios",
//                               QMessageBox::Ok);
//    }
//
//}

void ControlCultivos::on_actionImprimirCierreSemanal_triggered(){
  //  QMessageBox::StandardButton imprimir;
  //  imprimir = QMessageBox::question(
  //        this, tr("Imprimir Pago"),
  //        tr("¿Desea imprimir un resumen del pago de esta semana?"),
  //        QMessageBox::Yes | QMessageBox::Cancel);
  
  //    if(imprimir == QMessageBox::Yes){
  //        /// Guardar a un txt un resumen
  //        QString nombreArchivo = QFileDialog::getSaveFileName
  //                (this, tr("Guardar"), QString(), tr("Archivo de Texto (*.txt)"));
  //        if (!nombreArchivo.isEmpty()){
  //            QFile file(nombreArchivo);
  //            if(!file.open(QIODevice::WriteOnly)){
  //                QMessageBox::critical(this,tr("Problemas guardando"),
  //                                      tr("No se puede abrir el archivo para guardar"));
  //                return;
  //            } else {
  //                QTextStream stream(&file);
  //                stream << "Obrero:\n==============";
  
  ////                int f = 0;
  ////                while(f < tabla->rowCount()){
  ////                    float total = 0;
  ////                    for(int col = 1; col < tabla->columnCount(); ++col){
  ////                        total += tabla->data(tabla->index(f, col)).toFloat();
  ////                    }
  ////                    /// Añadimos los totales por obrero
  ////                    stream << tabla->data(tabla->index(f,0)).toString() << ": "
  ////                           << QString::number(total) << '\n';
  
  ////                    f++;
  //                }
  //            }
  //        }
  //    } else if (respuesta == QMessageBox::No) {
  //        // falta si dio en salir debe salir ahora...
  //        return;
  //    } else {
  //        return;
  //    }
  
  qDebug()<< "Imprimiendo...";
  qDebug()<< "Imprimir LISTO!";
}

void ControlCultivos::on_actionModoNocturno_triggered(){ // falta
  //  QString estiloNocturno = " QTableView { background: #aaf;} "
  //                           " QWidget { background: #88f; } "
  //                           " QMenuBar { background: #008; color: #FFF; } "
  //                           " QPushButton { background-color: blue; color: white; "
  //                           " border-radius: 2px; padding: 5px; } ";
  
  //  if(modoNoctON){
  //      ui->centralWidget->setStyleSheet("");
  //    } else {
  //      ui->centralWidget->setStyleSheet(estiloNocturno);
  //    }
}
/*
                                   QMainWindow {
                                         width: 10px;
                                         height: 10px;
                                         background: #00F;
                                     }
                                   */

void ControlCultivos::on_actionCompras_triggered(){
  Compras c;
  c.show();
}

void ControlCultivos::on_actionPagosHistoricos_triggered(){
  PagosHistoricos p;
  p.exec();
}

void ControlCultivos::on_pB_tablaSem_clicked(){
  if(tabla->tableName().contains("Pago_Semanal_")){
    return;
  }
  /// Mostrando barra horizontal
  ui->label_nombreObrero->show();
  ui->cB_diaSemana->show();
  ui->cB_Labor->show();
  ui->label_pago->show();
  ui->pB_AddPago->show();
  ui->sB_Pago->show();

  tabla->setTable("Pago_Semanal_Tabaco");
  tabla->select();
  ui->tableView->setModel(tabla);
}

void ControlCultivos::on_pB_tablaLabOb_clicked(){
  if(tabla->tableName().contains("Lab_Ob_")){
    return;
  }
  /// Ocultando barra horizontal
  ui->label_nombreObrero->hide();
  ui->cB_diaSemana->hide();
  ui->cB_Labor->hide();
  ui->label_pago->hide();
  ui->pB_AddPago->hide();
  ui->sB_Pago->hide();

  tabla->setTable("Lab_Ob_Tabaco");
  tabla->select();
  ui->tableView->setModel(tabla);
}
