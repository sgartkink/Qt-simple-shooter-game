#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

//TODO:
/*
 * 4. granaty - DONE
 * 5. wiecej broni i amunicji - DONE
 * 6. wchodzenie do budynkow
 * 7. skrzynie - DONE
 * 8. snajperka
 * 9. AI dla botow
 * 10. prostsze generowanie mapy
 * 11. menu gracza, paski życia, amunicji itp - DONE
 * 12. poprawa granatów: animacja wybuchu, zmiana obrażeń, zasięgu, inaczej liczony tor ruchu, możliwość ustalania prędkości - DONE
 * 13. pole widzenia gracza
 * 14. shotgun wystrzeliwuje kilka naboi na raz
 * 15. playerbarwidget jest zawsze w rogu
 * 16. odbijanie się granatu od ściany
 */
