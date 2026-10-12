#include "kkedplotmanager.h"


KKEDPlotManager::KKEDPlotManager() 
:  m_plotState(new KKEDDefaultPlotState())
{
  KDevLogger::getInstance().logEvent("Singleton KKEDPlotManager object constructed");

}

KKEDPlotManager::~KKEDPlotManager()
{
  KDevLogger::getInstance().logEvent("Singleton KKEDPlotManager object destructed");
  delete m_plotState;
}

KKEDPlotManager& KKEDPlotManager::getInstance()
{
  static KKEDPlotManager instance;
  return instance;
}

void KKEDPlotManager::send(int a_message)
{
}

void KKEDPlotManager::receive(int a_message)
{
}

void KKEDPlotManager::setState(int a_stateNumber)
{
  delete m_plotState;
  m_plotState = createState(a_stateNumber);
}

void KKEDPlotManager::applyState(QCustomPlot* kedPlot, bool a_isFitted)
{
  if(m_plotState)
  {
    connect(m_plotState, &KVKEDPlotState::plot_fit_too, this, &KKEDPlotManager::plot_fit_too_slot);
    m_plotState->apply(kedPlot, m_sampColor, m_bkgdColor, m_refColor,
                                m_sampDraw,  m_bkgdDraw,  m_refDraw,
                                                          a_isFitted);
  }
}

KVKEDPlotState* KKEDPlotManager::createState(int a_stateNumber) 
{
  switch (a_stateNumber) 
  {
    case 0:
      return new KKEDDefaultPlotState();
    case 1:
      return new KKEDRefPlotState();
    case 2:
      return new KKEDBkgdPlotState();
    case 3:
      return new KKEDBkgdRefPlotState();
    case 4:
      return new KKEDSampPlotState();
    case 5:
      return new KKEDSampRefPlotState();
    case 6:
      return new KKEDSampBkgdPlotState();
    case 7:
      return new KKEDAllPlotState();
    default:
      return new KKEDDefaultPlotState(); 
  }
}

KVKEDPlotState* KKEDPlotManager::get_plot_state() const
{
  return m_plotState;
}

void KKEDPlotManager::set_widget_status(int  a_sampColor, int  a_bkgdColor, int  a_refColor,
                                        bool a_sampDraw,  bool a_bkgdDraw,  bool a_refDraw)
{
  m_sampColor = a_sampColor;
  m_bkgdColor = a_bkgdColor;
  m_refColor = a_refColor;

  m_sampDraw = a_sampDraw;
  m_bkgdDraw = a_bkgdDraw;
  m_refDraw = a_refDraw;
}

void KKEDPlotManager::plot_fit_too_slot()
{
  emit plot_fit_too_signal();
}