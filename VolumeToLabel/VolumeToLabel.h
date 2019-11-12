#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_VolumeToLabel.h"
#include "SourceVolume.h"
#include <fstream>
#include <iostream>
#include "ParameterControlWidget.h"
#include <QDockWidget>
#include "SliceView.h"

class VolumeToLabel : public QMainWindow
{
	Q_OBJECT

public:
	VolumeToLabel(QWidget *parent = Q_NULLPTR);

	void setConnectionState();
	bool getDrawedState() const { return is_drawed; }

public slots:
	void slot_ImportVifoFile();
	void slot_ExportNetAndNodeFile();

private:
	Ui::VolumeToLabelClass	ui;
	std::string				infoFileName =  "F:\\atmosphere\\timestep21_float\\_SPEEDf21.vifo";
	int						data_number;
	std::string				datatype;
	hxy::my_int3			dimension;
	hxy::my_double3			space;
	vector<string>			file_list;
	vector<unsigned char>	volume_data;
	std::string				file_path;


	ParameterControlWidget*	parameter_control_widget;
	QDockWidget *			parameter_dock;
	SliceView *				slice_view;
	int						plane_mode = 0;	//0 for xy-plane, 1 for yz-plane, 2 for xz-plane;
	int						slice_id = 0;
	bool					is_drawed = false;
};
