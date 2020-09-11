#ifndef OXFOLD_WRAPPER_H
#define OXFOLD_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <QDir>
#include <QString>
#include <QNetworkInterface>
#include <QHostAddress>

void init_bft_env();
void close_bft_env();
int start_oxfold();
int stop_oxfold();
QString getNodeIPV4();
QString getNodeIPV6();
uint64_t getNodeNWID();

QString converFileSizeToKBMBGB(qint64 filesize);

#endif // OXFOLD_WRAPPER_H




