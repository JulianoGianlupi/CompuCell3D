# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'parvaldlg.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_ParValDlg(object):
    def setupUi(self, ParValDlg):
        ParValDlg.setObjectName("ParValDlg")
        ParValDlg.resize(269, 215)
        self.verticalLayout = QtWidgets.QVBoxLayout(ParValDlg)
        self.verticalLayout.setObjectName("verticalLayout")
        self.gridLayout = QtWidgets.QGridLayout()
        self.gridLayout.setObjectName("gridLayout")
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtWidgets.QLabel(ParValDlg)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.gridLayout.addLayout(self.horizontalLayout, 0, 0, 1, 1)
        self.minLE = QtWidgets.QLineEdit(ParValDlg)
        self.minLE.setObjectName("minLE")
        self.gridLayout.addWidget(self.minLE, 0, 1, 1, 1)
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.label_2 = QtWidgets.QLabel(ParValDlg)
        self.label_2.setObjectName("label_2")
        self.horizontalLayout_2.addWidget(self.label_2)
        spacerItem1 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem1)
        self.gridLayout.addLayout(self.horizontalLayout_2, 1, 0, 1, 1)
        self.maxLE = QtWidgets.QLineEdit(ParValDlg)
        self.maxLE.setObjectName("maxLE")
        self.gridLayout.addWidget(self.maxLE, 1, 1, 1, 1)
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.label_3 = QtWidgets.QLabel(ParValDlg)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_3.addWidget(self.label_3)
        spacerItem2 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_3.addItem(spacerItem2)
        self.gridLayout.addLayout(self.horizontalLayout_3, 2, 0, 1, 1)
        self.stepsLE = QtWidgets.QLineEdit(ParValDlg)
        self.stepsLE.setObjectName("stepsLE")
        self.gridLayout.addWidget(self.stepsLE, 2, 1, 1, 1)
        self.horizontalLayout_4 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.label_4 = QtWidgets.QLabel(ParValDlg)
        self.label_4.setObjectName("label_4")
        self.horizontalLayout_4.addWidget(self.label_4)
        spacerItem3 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_4.addItem(spacerItem3)
        self.gridLayout.addLayout(self.horizontalLayout_4, 3, 0, 1, 1)
        self.distrCB = QtWidgets.QComboBox(ParValDlg)
        self.distrCB.setObjectName("distrCB")
        self.distrCB.addItem("")
        self.distrCB.addItem("")
        self.distrCB.addItem("")
        self.gridLayout.addWidget(self.distrCB, 3, 1, 1, 1)
        self.horizontalLayout_5 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_5.setObjectName("horizontalLayout_5")
        self.label_6 = QtWidgets.QLabel(ParValDlg)
        self.label_6.setObjectName("label_6")
        self.horizontalLayout_5.addWidget(self.label_6)
        spacerItem4 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_5.addItem(spacerItem4)
        self.gridLayout.addLayout(self.horizontalLayout_5, 4, 0, 1, 1)
        self.typeCB = QtWidgets.QComboBox(ParValDlg)
        self.typeCB.setObjectName("typeCB")
        self.typeCB.addItem("")
        self.typeCB.addItem("")
        self.typeCB.addItem("")
        self.gridLayout.addWidget(self.typeCB, 4, 1, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        self.horizontalLayout_6 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_6.setObjectName("horizontalLayout_6")
        self.label_5 = QtWidgets.QLabel(ParValDlg)
        self.label_5.setObjectName("label_5")
        self.horizontalLayout_6.addWidget(self.label_5)
        self.valuesLE = QtWidgets.QLineEdit(ParValDlg)
        self.valuesLE.setObjectName("valuesLE")
        self.horizontalLayout_6.addWidget(self.valuesLE)
        self.verticalLayout.addLayout(self.horizontalLayout_6)
        self.horizontalLayout_7 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_7.setObjectName("horizontalLayout_7")
        self.generatePB = QtWidgets.QPushButton(ParValDlg)
        self.generatePB.setObjectName("generatePB")
        self.horizontalLayout_7.addWidget(self.generatePB)
        spacerItem5 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_7.addItem(spacerItem5)
        self.pushButton_2 = QtWidgets.QPushButton(ParValDlg)
        self.pushButton_2.setObjectName("pushButton_2")
        self.horizontalLayout_7.addWidget(self.pushButton_2)
        self.pushButton = QtWidgets.QPushButton(ParValDlg)
        self.pushButton.setObjectName("pushButton")
        self.horizontalLayout_7.addWidget(self.pushButton)
        self.verticalLayout.addLayout(self.horizontalLayout_7)

        self.retranslateUi(ParValDlg)
        self.pushButton_2.clicked.connect(ParValDlg.reject)
        self.pushButton.clicked.connect(ParValDlg.accept)
        QtCore.QMetaObject.connectSlotsByName(ParValDlg)

    def retranslateUi(self, ParValDlg):
        _translate = QtCore.QCoreApplication.translate
        ParValDlg.setWindowTitle(_translate("ParValDlg", "Generate Parameter Values"))
        self.label.setText(_translate("ParValDlg", "Min"))
        self.label_2.setText(_translate("ParValDlg", "Max"))
        self.label_3.setText(_translate("ParValDlg", "Number of Steps"))
        self.stepsLE.setText(_translate("ParValDlg", "5"))
        self.label_4.setText(_translate("ParValDlg", "Distribution"))
        self.distrCB.setItemText(0, _translate("ParValDlg", "linear"))
        self.distrCB.setItemText(1, _translate("ParValDlg", "log"))
        self.distrCB.setItemText(2, _translate("ParValDlg", "random"))
        self.label_6.setText(_translate("ParValDlg", "Type"))
        self.typeCB.setItemText(0, _translate("ParValDlg", "float"))
        self.typeCB.setItemText(1, _translate("ParValDlg", "int"))
        self.typeCB.setItemText(2, _translate("ParValDlg", "string"))
        self.label_5.setText(_translate("ParValDlg", "Values"))
        self.generatePB.setText(_translate("ParValDlg", "Generate"))
        self.pushButton_2.setText(_translate("ParValDlg", "Cancel"))
        self.pushButton.setText(_translate("ParValDlg", "OK"))

