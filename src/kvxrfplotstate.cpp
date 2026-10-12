#include "kvxrfplotstate.h"

//LETS REFACOTR TO MOVE REPEATED CODE INTO BASE CLASS. 
//CAN DO NOW THAT EVERTYHING IS BASED ON graphIndex
KVXRFPlotState::KVXRFPlotState()
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

KVXRFPlotState::~KVXRFPlotState()
{
}

void KVXRFPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                 int  a_bkgdColor, 
                                                 int  a_refColor,
                                                 bool a_sampDraw, 
                                                 bool a_bkgdDraw, 
                                                 bool a_refDraw)
{ 
  //nothing to do
}

//----------Default state-----------------------------------------------------//
KXRFDefaultPlotState::KXRFDefaultPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFDefaultPlotState object constructed");
}

KXRFDefaultPlotState::~KXRFDefaultPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFDefaultPlotState object destructed");
}

void KXRFDefaultPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                       int  a_bkgdColor, 
                                                       int  a_refColor,
                                                       bool a_sampDraw, 
                                                       bool a_bkgdDraw, 
                                                       bool a_refDraw)
{ 
  if (xrfPlot) 
  {
    xrfPlot->clearGraphs();
    xrfPlot->replot();
  }
}

//----------Sample only state-------------------------------------------------//
KXRFSampPlotState::KXRFSampPlotState()
: m_qSampCounts(KFileManager::getInstance().m_xrfSampData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfSampData.m_counts.end()),
  m_qSampChannels(KFileManager::getInstance().m_xrfSampData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfSampData.m_channels.end()),
  m_qSampEnergies(KFileManager::getInstance().m_xrfSampData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfSampData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KXRFSampPlotState object constructed");
}

KXRFSampPlotState::~KXRFSampPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFSampPlotState object destructed");
}

void KXRFSampPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                    int  a_bkgdColor, 
                                                    int  a_refColor,
                                                    bool a_sampDraw, 
                                                    bool a_bkgdDraw, 
                                                    bool a_refDraw)
{ 
  //Need max counts to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_energies);
  if (xrfPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    xrfPlot->clearGraphs();
    xrfPlot->addGraph();
    xrfPlot->graph(0)->setData(m_qSampEnergies, m_qSampCounts);
    xrfPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(0)->setPen(m_colorMap[a_sampColor]);
    xrfPlot->graph(0)->setVisible(a_sampDraw);
    
    //set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = xrfPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = xrfPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        xrfPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        xrfPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    xrfPlot->replot();
  }
  else
  {
    //should change this to send error back to runamangaer?
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Background only state---------------------------------------------//
KXRFBkgdPlotState::KXRFBkgdPlotState()
: m_qBkgdCounts(KFileManager::getInstance().m_xrfBkgdData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfBkgdData.m_counts.end()),
  m_qBkgdChannels(KFileManager::getInstance().m_xrfBkgdData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfBkgdData.m_channels.end()),
  m_qBkgdEnergies(KFileManager::getInstance().m_xrfBkgdData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfBkgdData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KXRFBkgdPlotState object constructed");
}

KXRFBkgdPlotState::~KXRFBkgdPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFBkgdPlotState object constructed");
}

void KXRFBkgdPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                    int  a_bkgdColor, 
                                                    int  a_refColor,
                                                    bool a_sampDraw, 
                                                    bool a_bkgdDraw, 
                                                    bool a_refDraw)
{ 
  //Need max counts to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfBkgdData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfBkgdData.m_energies);
  if (xrfPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    xrfPlot->clearGraphs();
    xrfPlot->addGraph();
    xrfPlot->graph(0)->setData(m_qBkgdEnergies, m_qBkgdCounts);
    xrfPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(0)->setPen(m_colorMap[a_bkgdColor]);
    xrfPlot->graph(0)->setVisible(a_bkgdDraw);
    
    //set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = xrfPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = xrfPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        xrfPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        xrfPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    xrfPlot->replot();
  }

  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Reference only state----------------------------------------------//
KXRFRefPlotState::KXRFRefPlotState()
: m_qRefCounts(KFileManager::getInstance().m_xrfRefData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfRefData.m_counts.end()),
  m_qRefChannels(KFileManager::getInstance().m_xrfRefData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfRefData.m_channels.end()),
  m_qRefEnergies(KFileManager::getInstance().m_xrfRefData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfRefData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KXRFRefPlotState object constructed");
}

KXRFRefPlotState::~KXRFRefPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFRefPlotState object destructed");
}

void KXRFRefPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                   int  a_bkgdColor, 
                                                   int  a_refColor,
                                                   bool a_sampDraw, 
                                                   bool a_bkgdDraw, 
                                                   bool a_refDraw)
{ 
  //Need max counts to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfRefData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfRefData.m_energies);
  if (xrfPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    xrfPlot->clearGraphs();
    xrfPlot->addGraph();
    xrfPlot->graph(0)->setData(m_qRefEnergies, m_qRefCounts);
    xrfPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(0)->setPen(m_colorMap[a_refColor]);
    xrfPlot->graph(0)->setVisible(a_refDraw);
    
    //set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = xrfPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = xrfPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        xrfPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        xrfPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    xrfPlot->replot();
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Sample & background state-----------------------------------------//
KXRFSampBkgdPlotState::KXRFSampBkgdPlotState()
: m_qSampCounts(KFileManager::getInstance().m_xrfSampData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfSampData.m_counts.end()),
  m_qSampChannels(KFileManager::getInstance().m_xrfSampData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfSampData.m_channels.end()),
  m_qSampEnergies(KFileManager::getInstance().m_xrfSampData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfSampData.m_energies.end()),
  m_qBkgdCounts(KFileManager::getInstance().m_xrfBkgdData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfBkgdData.m_counts.end()),
  m_qBkgdChannels(KFileManager::getInstance().m_xrfBkgdData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfBkgdData.m_channels.end()),
  m_qBkgdEnergies(KFileManager::getInstance().m_xrfBkgdData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfBkgdData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KXRFSampBkgdPlotState object constructed");
}

KXRFSampBkgdPlotState::~KXRFSampBkgdPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFSampBkgdPlotState object destructed");
}

void KXRFSampBkgdPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                        int  a_bkgdColor, 
                                                        int  a_refColor,
                                                        bool a_sampDraw, 
                                                        bool a_bkgdDraw, 
                                                        bool a_refDraw)
{ 
  //Need max counts and energy to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_counts,
                               KFileManager::getInstance().m_xrfBkgdData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_energies,
                               KFileManager::getInstance().m_xrfBkgdData.m_energies);
  if (xrfPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    xrfPlot->clearGraphs();
    xrfPlot->addGraph();
    xrfPlot->graph(0)->setData(m_qSampEnergies, m_qSampCounts);
    xrfPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(0)->setPen(m_colorMap[a_sampColor]);
    xrfPlot->graph(0)->setVisible(a_sampDraw);
    xrfPlot->addGraph();
    xrfPlot->graph(1)->setData(m_qBkgdEnergies, m_qBkgdCounts);
    xrfPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(1)->setPen(m_colorMap[a_bkgdColor]);
    xrfPlot->graph(1)->setVisible(a_bkgdDraw);
    
    //set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = xrfPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = xrfPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        xrfPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        xrfPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    xrfPlot->replot();
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Sample & reference state------------------------------------------//
KXRFSampRefPlotState::KXRFSampRefPlotState()
: m_qSampCounts(KFileManager::getInstance().m_xrfSampData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfSampData.m_counts.end()),
  m_qSampChannels(KFileManager::getInstance().m_xrfSampData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfSampData.m_channels.end()),
  m_qSampEnergies(KFileManager::getInstance().m_xrfSampData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfSampData.m_energies.end()),
  m_qRefCounts(KFileManager::getInstance().m_xrfRefData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfRefData.m_counts.end()),
  m_qRefChannels(KFileManager::getInstance().m_xrfRefData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfRefData.m_channels.end()),
  m_qRefEnergies(KFileManager::getInstance().m_xrfRefData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfRefData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KXRFSampRefPlotState object constructed");
}

KXRFSampRefPlotState::~KXRFSampRefPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFSampRefPlotState object destructed");
}

void KXRFSampRefPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                       int  a_bkgdColor, 
                                                       int  a_refColor,
                                                       bool a_sampDraw, 
                                                       bool a_bkgdDraw, 
                                                       bool a_refDraw)
{ 
  //Need max counts and energy to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_counts,
                               KFileManager::getInstance().m_xrfRefData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_energies,
                               KFileManager::getInstance().m_xrfRefData.m_energies);
  if (xrfPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    xrfPlot->clearGraphs();
    xrfPlot->addGraph();
    xrfPlot->graph(0)->setData(m_qSampEnergies, m_qSampCounts);
    xrfPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(0)->setPen(m_colorMap[a_sampColor]);
    xrfPlot->graph(0)->setVisible(a_sampDraw);
    xrfPlot->addGraph();
    xrfPlot->graph(1)->setData(m_qRefEnergies, m_qRefCounts);
    xrfPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(1)->setPen(m_colorMap[a_refColor]);
    xrfPlot->graph(1)->setVisible(a_refDraw);
    
    //set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = xrfPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = xrfPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        xrfPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        xrfPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    xrfPlot->replot();
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------Background & reference state--------------------------------------//
KXRFBkgdRefPlotState::KXRFBkgdRefPlotState()
: m_qBkgdCounts(KFileManager::getInstance().m_xrfBkgdData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfBkgdData.m_counts.end()),
  m_qBkgdChannels(KFileManager::getInstance().m_xrfBkgdData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfBkgdData.m_channels.end()),
  m_qBkgdEnergies(KFileManager::getInstance().m_xrfBkgdData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfBkgdData.m_energies.end()),
  m_qRefCounts(KFileManager::getInstance().m_xrfRefData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfRefData.m_counts.end()),
  m_qRefChannels(KFileManager::getInstance().m_xrfRefData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfRefData.m_channels.end()),
  m_qRefEnergies(KFileManager::getInstance().m_xrfRefData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfRefData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KXRFBkgdRefPlotState object constructed");
}

KXRFBkgdRefPlotState::~KXRFBkgdRefPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFBkgdRefPlotState object destructed");
}

void KXRFBkgdRefPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                       int  a_bkgdColor, 
                                                       int  a_refColor,
                                                       bool a_sampDraw, 
                                                       bool a_bkgdDraw, 
                                                       bool a_refDraw)
{ 
  //Need max counts and energy to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfBkgdData.m_counts,
                               KFileManager::getInstance().m_xrfRefData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfBkgdData.m_energies,
                               KFileManager::getInstance().m_xrfRefData.m_energies);
  if (xrfPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    xrfPlot->clearGraphs();
    xrfPlot->addGraph();
    xrfPlot->graph(0)->setData(m_qBkgdEnergies, m_qBkgdCounts);
    xrfPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(0)->setPen(m_colorMap[a_bkgdColor]);
    xrfPlot->graph(0)->setVisible(a_bkgdDraw);
    xrfPlot->addGraph();
    xrfPlot->graph(1)->setData(m_qRefEnergies, m_qRefCounts);
    xrfPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(1)->setPen(m_colorMap[a_refColor]);
    xrfPlot->graph(1)->setVisible(a_refDraw);
    
    //set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = xrfPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = xrfPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        xrfPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        xrfPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }

    xrfPlot->replot();
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

//----------All three state---------------------------------------------------//
KXRFAllPlotState::KXRFAllPlotState()
: m_qSampCounts(KFileManager::getInstance().m_xrfSampData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfSampData.m_counts.end()),
  m_qSampChannels(KFileManager::getInstance().m_xrfSampData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfSampData.m_channels.end()),
  m_qSampEnergies(KFileManager::getInstance().m_xrfSampData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfSampData.m_energies.end()),
  m_qBkgdCounts(KFileManager::getInstance().m_xrfBkgdData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfBkgdData.m_counts.end()),
  m_qBkgdChannels(KFileManager::getInstance().m_xrfBkgdData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfBkgdData.m_channels.end()),
  m_qBkgdEnergies(KFileManager::getInstance().m_xrfBkgdData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfBkgdData.m_energies.end()),
  m_qRefCounts(KFileManager::getInstance().m_xrfRefData.m_counts.begin(), 
                KFileManager::getInstance().m_xrfRefData.m_counts.end()),
  m_qRefChannels(KFileManager::getInstance().m_xrfRefData.m_channels.begin(), 
                  KFileManager::getInstance().m_xrfRefData.m_channels.end()),
  m_qRefEnergies(KFileManager::getInstance().m_xrfRefData.m_energies.begin(), 
                  KFileManager::getInstance().m_xrfRefData.m_energies.end())
{
  KDevLogger::getInstance().logEvent("KXRFAllPlotState object constructed");
}

KXRFAllPlotState::~KXRFAllPlotState()
{
  KDevLogger::getInstance().logEvent("KXRFAllPlotState object destructed");
}

void KXRFAllPlotState::apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                                   int  a_bkgdColor, 
                                                   int  a_refColor,
                                                   bool a_sampDraw, 
                                                   bool a_bkgdDraw, 
                                                   bool a_refDraw)
{ 
  //Need max counts and energy to set plot scale.
  //KMath kmath;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_counts,
                               KFileManager::getInstance().m_xrfBkgdData.m_counts,
                               KFileManager::getInstance().m_xrfRefData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_energies,
                               KFileManager::getInstance().m_xrfBkgdData.m_energies,
                               KFileManager::getInstance().m_xrfRefData.m_energies);
  if (xrfPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    xrfPlot->clearGraphs();
    xrfPlot->addGraph();
    xrfPlot->graph(0)->setData(m_qSampEnergies, m_qSampCounts);
    xrfPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(0)->setPen(m_colorMap[a_sampColor]);
    xrfPlot->graph(0)->setVisible(a_sampDraw);
    xrfPlot->addGraph();
    xrfPlot->graph(1)->setData(m_qBkgdEnergies, m_qBkgdCounts);
    xrfPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(1)->setPen(m_colorMap[a_bkgdColor]);
    xrfPlot->graph(1)->setVisible(a_bkgdDraw);
    xrfPlot->addGraph();
    xrfPlot->graph(2)->setData(m_qRefEnergies, m_qRefCounts);
    xrfPlot->graph(2)->setLineStyle(QCPGraph::lsStepCenter);
    xrfPlot->graph(2)->setPen(m_colorMap[a_refColor]);
    xrfPlot->graph(2)->setVisible(a_refDraw);
    
    //set axes ranges, so we see all data. Scalers add a little whitespace.
    QCPRange currentXRange = xrfPlot->xAxis->range();
    double lowerXBound = currentXRange.lower;
    double upperXBound = currentXRange.upper;
    QCPRange currentYRange = xrfPlot->yAxis->range();
    double lowerYBound = currentYRange.lower;
    double upperYBound = currentYRange.upper;
    //check of the x-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerXBound != 0 || upperXBound != 5)
    {
      if(upperXBound > maxXValue/m_xScaler)
      {
        xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
      }
      else
      {
        xrfPlot->xAxis->setRange(lowerXBound, upperXBound);
      }
    }
    else
    {
      xrfPlot->xAxis->setRange(0, maxXValue*m_xScaler);
    }
    //check of the y-axis has been zoomed. Set new axis range if yes,
    //default if no
    if(lowerYBound != 0 || upperYBound != 5 )
    {
      if(upperYBound > maxYValue/m_yScaler)
      {
        xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
      }
      else
      {
        xrfPlot->yAxis->setRange(lowerYBound, upperYBound);
      }
    }
    else
    {
      xrfPlot->yAxis->setRange(0, maxYValue*m_yScaler);
    }
    
    xrfPlot->replot();
  }
  else
  {
    //should change this to send error back to runamangaer
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}