#pragma once
#include <qwidget.h>
#include "ui_ParameterControlWidget.h"
class ParameterControlWidget :
	public QWidget
{
	Q_OBJECT
public:
	ParameterControlWidget(QWidget *parent = nullptr);
	~ParameterControlWidget();

public:
	Ui::Form ui;
};

