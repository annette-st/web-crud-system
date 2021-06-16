//
// Created by igor on 20/12/2020.
//

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "inspector_widget.h"
#include "transactions_model.h"

int main(int argc, char* argv[])
{
    QApplication app (argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();


    QCommandLineOption transactionsFileOption(QStringList() << "t" << "transactions",
            "Transactions archive <zip file>",
            "zip");
    QCommandLineOption scriptFileOption(QStringList() << "s" << "script",
                                              "java script",
                                              "js");
    parser.addOption(transactionsFileOption);
    parser.addOption(scriptFileOption);

    // Process the actual command line arguments given by the user
    parser.process(app);

    QString tr_file = parser.value(transactionsFileOption);

    transactions_model model(tr_file.toStdString());

    inspector_widget inspector;
    inspector.setDataModel(&model);
    inspector.showMaximized();

    return app.exec();
}
