#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "requesthandler_global.h"
#include <CompuCellModel.h>
#include <QFileInfo>
#include <cc3dreader.h>

class REQUESTHANDLERSHARED_EXPORT RequestHandler
{

private:

    Model compuCellModel;

public:

    RequestHandler();

    /**
     * This function reads the input file specified and open the CompuCell3D model.
     * @param filePath Path of CompuCell3D project file (.cc3d)
     * @return True if Open simulation is successful else False
     */
    bool openCompuCellModel(QString cc3dFilePath);

    ~RequestHandler();

};

#endif // REQUESTHANDLER_H