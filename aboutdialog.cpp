#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AboutDialog)
{
  ui->setupUi(this);

  /// disable resizing
  setFixedSize(size());

  /// scale logo image
  QPixmap logo = QPixmap::fromImage(QImage(":/resources/logo_128x128.png"));
  ui->appLogo->setPixmap(logo.scaledToWidth(61));

  /// add an action for OK button
  connect(ui->okButton, SIGNAL(clicked()), this, SLOT(okButtonClick()));

  /// add an action for Help button
  connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(helpButtonClick()));
}

void AboutDialog::okButtonClick()
{
  this->close();
}

void AboutDialog::helpButtonClick()
{
  /// TODO: show help window here !!!
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
