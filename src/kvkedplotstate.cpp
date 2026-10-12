#include "kvkedplotstate.h"

//LETS REFACOTR TO MOVE REPEATED CODE INTO BASE CLASS. 
//CAN DO NOW THAT EVERTYHING IS BASED ON graphIndex
KVKEDPlotState::KVKEDPlotState()
{
  QPen bluePen(Qt::blue);
  QPen blackPen(Qt::black);
  QPen redPen(Qt::red);
  QPen greenPen(Qt::green);
  QPen cyanPen(Qt::cyan);

  m_colorMap[0] = bluePen;
  m_colorMap[1] = blackPen;
  m_colorMap[2] = redPen;
  m_colorMap[3] = greenPen;
  m_colorMap[4] = cyanPen;
}

KVKEDPlotState::~KVKEDPlotState()
{
}

void KVKEDPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                 int  a_bkgdColor, 
                                                 int  a_refColor,
                                                 bool a_sampDraw, 
                                                 bool a_bkgdDraw, 
                                                 bool a_refDraw,
                                                 bool a_isFitted)
{ 
  //nothing to do
}

//----------Default state-----------------------------------------------------//
KKEDDefaultPlotState::KKEDDefaultPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDDefaultPlotState object constructed");
}

KKEDDefaultPlotState::~KKEDDefaultPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDDefaultPlotState object destructed");
}

void KKEDDefaultPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                       int  a_bkgdColor, 
                                                       int  a_refColor,
                                                       bool a_sampDraw, 
                                                       bool a_bkgdDraw, 
                                                       bool a_refDraw,
                                                       bool a_isFitted)
{ 
  if (kedPlot) 
  {
    kedPlot->clearGraphs();
    if(a_isFitted)
    {
      kedPlot->replot();
      emit plot_fit_too();
    }
    else
    {
      kedPlot->replot();
    }
  }
}

//----------Sample only state-------------------------------------------------//
KKEDSampPlotState::KKEDSampPlotState()
: m_qSampCounts(KFileManager::getInstance().m_kedSampData.m_counts.begin(), 
                KFileManager::getInstance().m_kedSampData.m_counts.end()),
  m_qSampChannels(KFileManager::getInstance().m_kedSampData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedSampData.m_channels.end()),
  m_qSampEnergies(KFileManager::getInstance().m_kedSampData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedSampData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KKEDSampPlotState object constructed");
}

KKEDSampPlotState::~KKEDSampPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDSampPlotState object destructed");
}

void KKEDSampPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                    int  a_bkgdColor, 
                                                    int  a_refColor,
                                                    bool a_sampDraw, 
                                                    bool a_bkgdDraw, 
                                                    bool a_refDraw,
                                                    bool a_isFitted)
{ 
  //Need max counts to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_energies);
  if (kedPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    kedPlot->clearGraphs();
    kedPlot->addGraph();
    kedPlot->graph(0)->setData(m_qSampEnergies, m_qSampCounts);
    kedPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(0)->setPen(m_colorMap[a_sampColor]);
    kedPlot->graph(0)->setVisible(a_sampDraw);
    
    //set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = kedPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = kedPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        kedPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        kedPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    //If the data has been fitted, we need to draw this too
    if(a_isFitted)
    {
      kedPlot->replot();
      emit plot_fit_too();
    }
    else
    {
      kedPlot->replot();
    }
  }
  else
  {
    //should change this to send error back to runamangaer?
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Background only state---------------------------------------------//
KKEDBkgdPlotState::KKEDBkgdPlotState()
: m_qBkgdCounts(KFileManager::getInstance().m_kedBkgdData.m_counts.begin(), 
                KFileManager::getInstance().m_kedBkgdData.m_counts.end()),
  m_qBkgdChannels(KFileManager::getInstance().m_kedBkgdData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedBkgdData.m_channels.end()),
  m_qBkgdEnergies(KFileManager::getInstance().m_kedBkgdData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedBkgdData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KKEDBkgdPlotState object constructed");
}

KKEDBkgdPlotState::~KKEDBkgdPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDBkgdPlotState object constructed");
}

void KKEDBkgdPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                    int  a_bkgdColor, 
                                                    int  a_refColor,
                                                    bool a_sampDraw, 
                                                    bool a_bkgdDraw, 
                                                    bool a_refDraw,
                                                    bool a_isFitted)
{ 
  //Need max counts to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedBkgdData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_kedBkgdData.m_energies);
  if (kedPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    kedPlot->clearGraphs();
    kedPlot->addGraph();
    kedPlot->graph(0)->setData(m_qBkgdEnergies, m_qBkgdCounts);
    kedPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(0)->setPen(m_colorMap[a_bkgdColor]);
    kedPlot->graph(0)->setVisible(a_bkgdDraw);
    // set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = kedPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = kedPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        kedPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        kedPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    if(a_isFitted)
    {
      kedPlot->replot();
      emit plot_fit_too();
    }
    else
    {
      kedPlot->replot();
    }
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Reference only state----------------------------------------------//
KKEDRefPlotState::KKEDRefPlotState()
: m_qRefCounts(KFileManager::getInstance().m_kedRefData.m_counts.begin(), 
                KFileManager::getInstance().m_kedRefData.m_counts.end()),
  m_qRefChannels(KFileManager::getInstance().m_kedRefData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedRefData.m_channels.end()),
  m_qRefEnergies(KFileManager::getInstance().m_kedRefData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedRefData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KKERefPlotState object constructed");
}

KKEDRefPlotState::~KKEDRefPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDRefPlotState object destructed");
}

void KKEDRefPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                   int  a_bkgdColor, 
                                                   int  a_refColor,
                                                   bool a_sampDraw, 
                                                   bool a_bkgdDraw, 
                                                   bool a_refDraw,
                                                   bool a_isFitted)
{ 
  //Need max counts to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedRefData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_kedRefData.m_energies);
  if (kedPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    kedPlot->clearGraphs();
    kedPlot->addGraph();
    kedPlot->graph(0)->setData(m_qRefEnergies, m_qRefCounts);
    kedPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(0)->setPen(m_colorMap[a_refColor]);
    kedPlot->graph(0)->setVisible(a_refDraw);
    // set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = kedPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = kedPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        kedPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        kedPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    if(a_isFitted)
    {
      kedPlot->replot();
      emit plot_fit_too();
    }
    else
    {
      kedPlot->replot();
    }
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Sample & background state-----------------------------------------//
KKEDSampBkgdPlotState::KKEDSampBkgdPlotState()
: m_qSampCounts(KFileManager::getInstance().m_kedSampData.m_counts.begin(), 
                KFileManager::getInstance().m_kedSampData.m_counts.end()),
  m_qSampChannels(KFileManager::getInstance().m_kedSampData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedSampData.m_channels.end()),
  m_qSampEnergies(KFileManager::getInstance().m_kedSampData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedSampData.m_energies.end()),
  m_qBkgdCounts(KFileManager::getInstance().m_kedBkgdData.m_counts.begin(), 
                KFileManager::getInstance().m_kedBkgdData.m_counts.end()),
  m_qBkgdChannels(KFileManager::getInstance().m_kedBkgdData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedBkgdData.m_channels.end()),
  m_qBkgdEnergies(KFileManager::getInstance().m_kedBkgdData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedBkgdData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KKEDSampBkgdPlotState object constructed");
}

KKEDSampBkgdPlotState::~KKEDSampBkgdPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDSampBkgdPlotState object destructed");
}

void KKEDSampBkgdPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                        int  a_bkgdColor, 
                                                        int  a_refColor,
                                                        bool a_sampDraw, 
                                                        bool a_bkgdDraw, 
                                                        bool a_refDraw,
                                                        bool a_isFitted)
{ 
  //Need max counts and energy to set plot scale.
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_counts,
                               KFileManager::getInstance().m_kedBkgdData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_energies,
                               KFileManager::getInstance().m_kedBkgdData.m_energies);
  if (kedPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    kedPlot->clearGraphs();
    kedPlot->addGraph();
    kedPlot->graph(0)->setData(m_qSampEnergies, m_qSampCounts);
    kedPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(0)->setPen(m_colorMap[a_sampColor]);
    kedPlot->graph(0)->setVisible(a_sampDraw);
    kedPlot->addGraph();
    kedPlot->graph(1)->setData(m_qBkgdEnergies, m_qBkgdCounts);
    kedPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(1)->setPen(m_colorMap[a_bkgdColor]);
    kedPlot->graph(1)->setVisible(a_bkgdDraw);
    // set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = kedPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = kedPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        kedPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        kedPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    if(a_isFitted)
    {
      kedPlot->replot();
      emit plot_fit_too();
    }
    else
    {
      kedPlot->replot();
    }
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Sample & reference state------------------------------------------//
KKEDSampRefPlotState::KKEDSampRefPlotState()
: m_qSampCounts(KFileManager::getInstance().m_kedSampData.m_counts.begin(), 
                KFileManager::getInstance().m_kedSampData.m_counts.end()),
  m_qSampChannels(KFileManager::getInstance().m_kedSampData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedSampData.m_channels.end()),
  m_qSampEnergies(KFileManager::getInstance().m_kedSampData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedSampData.m_energies.end()),
  m_qRefCounts(KFileManager::getInstance().m_kedRefData.m_counts.begin(), 
                KFileManager::getInstance().m_kedRefData.m_counts.end()),
  m_qRefChannels(KFileManager::getInstance().m_kedRefData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedRefData.m_channels.end()),
  m_qRefEnergies(KFileManager::getInstance().m_kedRefData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedRefData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KKEDSampRefPlotState object constructed");
}

KKEDSampRefPlotState::~KKEDSampRefPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDSampRefPlotState object destructed");
}

void KKEDSampRefPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                       int  a_bkgdColor, 
                                                       int  a_refColor,
                                                       bool a_sampDraw, 
                                                       bool a_bkgdDraw, 
                                                       bool a_refDraw,
                                                       bool a_isFitted)
{ 
  //Need max counts and energy to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_counts,
                               KFileManager::getInstance().m_kedRefData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_energies,
                               KFileManager::getInstance().m_kedRefData.m_energies);
  if (kedPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    kedPlot->clearGraphs();
    kedPlot->addGraph();
    kedPlot->graph(0)->setData(m_qSampEnergies, m_qSampCounts);
    kedPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(0)->setPen(m_colorMap[a_sampColor]);
    kedPlot->graph(0)->setVisible(a_sampDraw);
    kedPlot->addGraph();
    kedPlot->graph(1)->setData(m_qRefEnergies, m_qRefCounts);
    kedPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(1)->setPen(m_colorMap[a_refColor]);
    kedPlot->graph(1)->setVisible(a_refDraw);
    // set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = kedPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = kedPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        kedPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        kedPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    if(a_isFitted)
    {
      kedPlot->replot();
      emit plot_fit_too();
    }
    else
    {
      kedPlot->replot();
    }
  }
  else
  {
    //xxxshould change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Background & reference state--------------------------------------//
KKEDBkgdRefPlotState::KKEDBkgdRefPlotState()
: m_qBkgdCounts(KFileManager::getInstance().m_kedBkgdData.m_counts.begin(), 
                KFileManager::getInstance().m_kedBkgdData.m_counts.end()),
  m_qBkgdChannels(KFileManager::getInstance().m_kedBkgdData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedBkgdData.m_channels.end()),
  m_qBkgdEnergies(KFileManager::getInstance().m_kedBkgdData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedBkgdData.m_energies.end()),
  m_qRefCounts(KFileManager::getInstance().m_kedRefData.m_counts.begin(), 
                KFileManager::getInstance().m_kedRefData.m_counts.end()),
  m_qRefChannels(KFileManager::getInstance().m_kedRefData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedRefData.m_channels.end()),
  m_qRefEnergies(KFileManager::getInstance().m_kedRefData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedRefData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KKEDBkgdRefPlotState object constructed");
}

KKEDBkgdRefPlotState::~KKEDBkgdRefPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDBkgdRefPlotState object destructed");
}

void KKEDBkgdRefPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                       int  a_bkgdColor, 
                                                       int  a_refColor,
                                                       bool a_sampDraw, 
                                                       bool a_bkgdDraw, 
                                                       bool a_refDraw,
                                                       bool a_isFitted)
{ 
  //Need max counts and energy to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedBkgdData.m_counts,
                                      KFileManager::getInstance().m_kedRefData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_kedBkgdData.m_energies,
                                      KFileManager::getInstance().m_kedRefData.m_energies);
  if (kedPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    kedPlot->clearGraphs();
    kedPlot->addGraph();
    kedPlot->graph(0)->setData(m_qBkgdEnergies, m_qBkgdCounts);
    kedPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(0)->setPen(m_colorMap[a_bkgdColor]);
    kedPlot->graph(0)->setVisible(a_bkgdDraw);
    kedPlot->addGraph();
    kedPlot->graph(1)->setData(m_qRefEnergies, m_qRefCounts);
    kedPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(1)->setPen(m_colorMap[a_refColor]);
    kedPlot->graph(1)->setVisible(a_refDraw);
    // set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = kedPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = kedPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        kedPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        kedPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    if(a_isFitted)
    {
      kedPlot->replot();
      emit plot_fit_too();
    }
    else
    {
      kedPlot->replot();
    }
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------All three state---------------------------------------------------//
KKEDAllPlotState::KKEDAllPlotState()
: m_qSampCounts(KFileManager::getInstance().m_kedSampData.m_counts.begin(), 
                KFileManager::getInstance().m_kedSampData.m_counts.end()),
  m_qSampChannels(KFileManager::getInstance().m_kedSampData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedSampData.m_channels.end()),
  m_qSampEnergies(KFileManager::getInstance().m_kedSampData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedSampData.m_energies.end()),
  m_qBkgdCounts(KFileManager::getInstance().m_kedBkgdData.m_counts.begin(), 
                KFileManager::getInstance().m_kedBkgdData.m_counts.end()),
  m_qBkgdChannels(KFileManager::getInstance().m_kedBkgdData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedBkgdData.m_channels.end()),
  m_qBkgdEnergies(KFileManager::getInstance().m_kedBkgdData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedBkgdData.m_energies.end()),
  m_qRefCounts(KFileManager::getInstance().m_kedRefData.m_counts.begin(), 
                KFileManager::getInstance().m_kedRefData.m_counts.end()),
  m_qRefChannels(KFileManager::getInstance().m_kedRefData.m_channels.begin(), 
                  KFileManager::getInstance().m_kedRefData.m_channels.end()),
  m_qRefEnergies(KFileManager::getInstance().m_kedRefData.m_energies.begin(), 
                  KFileManager::getInstance().m_kedRefData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KKEDAllPlotState object constructed");
}

KKEDAllPlotState::~KKEDAllPlotState()
{
  KDevLogger::getInstance().logEvent("KKEDAllPlotState object destructed");
}

void KKEDAllPlotState::apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                                   int  a_bkgdColor, 
                                                   int  a_refColor,
                                                   bool a_sampDraw, 
                                                   bool a_bkgdDraw, 
                                                   bool a_refDraw,
                                                   bool a_isFitted)
{ 
  //Need max counts and energy to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_counts,
                                      KFileManager::getInstance().m_kedBkgdData.m_counts,
                                      KFileManager::getInstance().m_kedRefData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_energies,
                                      KFileManager::getInstance().m_kedBkgdData.m_energies,
                                      KFileManager::getInstance().m_kedRefData.m_energies);
  if (kedPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    kedPlot->clearGraphs();
    kedPlot->addGraph();
    kedPlot->graph(0)->setData(m_qSampEnergies, m_qSampCounts);
    kedPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(0)->setPen(m_colorMap[a_sampColor]);
    kedPlot->graph(0)->setVisible(a_sampDraw);
    kedPlot->addGraph();
    kedPlot->graph(1)->setData(m_qBkgdEnergies, m_qBkgdCounts);
    kedPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(1)->setPen(m_colorMap[a_bkgdColor]);
    kedPlot->graph(1)->setVisible(a_bkgdDraw);
    kedPlot->addGraph();
    kedPlot->graph(2)->setData(m_qRefEnergies, m_qRefCounts);
    kedPlot->graph(2)->setLineStyle(QCPGraph::lsStepCenter);
    kedPlot->graph(2)->setPen(m_colorMap[a_refColor]);
    kedPlot->graph(2)->setVisible(a_refDraw);
    // set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = kedPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = kedPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    
    //check if the plot has the initial default axes (0,5) in x and y
    if(lowerXBound != 0 || upperXBound != 5)
    {
      //upper bound is greater than data (i.e. extra whitespace) so set axes to 
      //data values. For a plot with data that has not been zoomed or scaled,
      //the upper axis is dataMax*scaler
      if(upperXBound > maxXValue/m_xScaler)
      {
        kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      //upper bound is less than data (i.e. zoomed) so set axes to zoom values
      else
      {
        kedPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    //plot is empty (0,5) so set axes to data
    else
    {
      kedPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        kedPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      kedPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    
    if(a_isFitted)
    {
      kedPlot->replot();
      emit plot_fit_too();
    }
    else
    {
      kedPlot->replot();
    }
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}