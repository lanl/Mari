/*
© 2026. Triad National Security, LLC. All rights reserved.

This program was produced under U.S. Government contract 89233218CNA000001 for 
Los Alamos National Laboratory (LANL), which is operated by Triad National 
Security, LLC for the U.S. Department of Energy/National Nuclear Security 
Administration. 
All rights in the program are reserved by Triad National Security, LLC, 
and the U.S. Department of Energy/National Nuclear Security Administration. 
The Government is granted for itself and others acting on its behalf a 
nonexclusive, paid-up, irrevocable worldwide license in this material to 
reproduce, prepare. derivative works, distribute copies to the public, 
perform publicly and display publicly, and to permit others to do so.

                          (End of Notice)
*/

#ifndef BROWSEBUTTON_H
#define BROWSEBUTTON_H

//Qt includes
#include <QPushButton>
#include <QFileDialog>
#include <QString>

class BrowseButton : public QPushButton
{
    Q_OBJECT

public:
    explicit BrowseButton(QWidget *parent = nullptr);

private:

signals:
  void foundFile(std::string a_filename);

private slots:
    void on_browseButton_clicked();
};


#endif