#include <QApplication>
#include <QLabel>
int main(int argc,char** argv) {
  QApplication app(argc,argv);
  QLabel label("KERobot — Sprint 01 review shell\nSynthetic offline prototype only\nSTEP viewport pending; hardware disabled");
  label.setWindowTitle("KERobot offline review"); label.resize(540,160); label.show();
  return app.exec();
}
