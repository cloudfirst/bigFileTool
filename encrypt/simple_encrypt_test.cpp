# include <cstdlib>
# include <iostream>

# include <QtGui/QApplication>
# include <QtCore/QDebug>

# include "simplecrypt.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString FreeTrialStartDate ;

  std::cout << "\n";
  std::cout << "SIMPLECRYPT_TEST:\n";
  std::cout << "  Simple test of QT\n";

    //Set The Encryption And Decryption Key
    SimpleCrypt processSimpleCrypt(89473829);

    QString FreeTrialStartsOn("22/11/2011");

    //Encrypt
    FreeTrialStartDate = processSimpleCrypt.encryptToString(FreeTrialStartsOn);

    qDebug() << "Encrypted 22/11/2011 to" << FreeTrialStartDate;

    //Decrypt
    QString decrypt = processSimpleCrypt.decryptToString(FreeTrialStartDate);

    qDebug() << "Decrypted 22/11/2011 to" << decrypt;

  std::cout << "\n";
  std::cout << "SIMPLECRYPT_TEST:\n";
  std::cout << "  Normal end of execution.\n";

  return 0;
//  return a.exec();
}

