#include "kxrfplotmanager.h"


KXRFPlotManager::KXRFPlotManager() 
:  m_plotState(new KXRFDefaultPlotState())
{
  KDevLogger::getInstance().logEvent("Singleton KXRFPlotManager object constructed");
}

KXRFPlotManager::~KXRFPlotManager()
{
  KDevLogger::getInstance().logEvent("Singleton KXRFPlotManager object destructed");
  delete m_plotState;
}

KXRFPlotManager& KXRFPlotManager::getInstance()
{
  static KXRFPlotManager instance;
  return instance;
}

void KXRFPlotManager::send(int a_message)
{
}

void KXRFPlotManager::receive(int a_message)
{
}

void KXRFPlotManager::setState(int a_stateNumber)
{
  delete m_plotState;
  m_plotState = createState(a_stateNumber);
}

void KXRFPlotManager::applyState(QCustomPlot* xrfPlot)
{
  if(m_plotState)
  {
    m_plotState->apply(xrfPlot, m_sampColor, m_bkgdColor, m_refColor,
                                m_sampDraw,  m_bkgdDraw,  m_refDraw);
  }
}

KVXRFPlotState* KXRFPlotManager::createState(int a_stateNumber) 
{
  switch (a_stateNumber) 
  {
    case 0:
      return new KXRFDefaultPlotState();
    case 1:
      return new KXRFRefPlotState();
    case 2:
      return new KXRFBkgdPlotState();
    case 3:
      return new KXRFBkgdRefPlotState();
    case 4:
      return new KXRFSampPlotState();
    case 5:
      return new KXRFSampRefPlotState();
    case 6:
      return new KXRFSampBkgdPlotState();
    case 7:
      return new KXRFAllPlotState();
    default:
      return new KXRFDefaultPlotState(); 
  }
}

KVXRFPlotState* KXRFPlotManager::get_plot_state() const
{
  return m_plotState;
}

void KXRFPlotManager::set_widget_status(int  a_sampColor, int  a_bkgdColor, int  a_refColor,
                                        bool a_sampDraw,  bool a_bkgdDraw,  bool a_refDraw)
{
  m_sampColor = a_sampColor;
  m_bkgdColor = a_bkgdColor;
  m_refColor = a_refColor;

  m_sampDraw = a_sampDraw;
  m_bkgdDraw = a_bkgdDraw;
  m_refDraw = a_refDraw;
}