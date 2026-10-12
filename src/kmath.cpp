#include "kmath.h"

KMath::KMath()
{
  KDevLogger::getInstance().logEvent("KMath object constructed");
}

KMath::~KMath()
{
  KDevLogger::getInstance().logEvent("KMath object destructed");
}

std::vector<std::pair<double, double>> KMath::sort_data(std::vector<std::pair<double, double>> a_data)
{
  std::sort(a_data.begin(), a_data.end(),
                            [](const std::pair<double, double>& a, const std::pair<double, double>& b) 
                               {return a.first < b.first;});
  return a_data;
}

std::vector<std::pair<double, double>> KMath::lorentz_broadening_step(std::vector<std::pair<double, double>>& a_data,
                                                                      const double a_stepLocation,
                                                                      const double a_width)
{
  if (a_data.size() < 2 || a_width <= 0.0)
  return a_data;

  std::vector<std::pair<double, double>> broadenedData;
  int stepIndex = -1;
  //find the index of the step
  for (int i=0; i<a_data.size()-1; i++) 
  {
    if((a_data.at(i).first < a_stepLocation && a_data.at(i+1).first > a_stepLocation) ||
        a_data.at(i).first == a_stepLocation)
    {
      stepIndex = i;
      break;
    }
  }
  //If there is no step, simply return the original data
  if(stepIndex == -1) return a_data;
  if (stepIndex == a_data.size()) return a_data;

  constexpr double PI = 3.14159265358979323846;
  //calculate the step height
  double stepHeight = a_data.at(stepIndex).second - a_data.at(stepIndex-1).second;
  //for data before step, implement Lorenztian broadening
  for (std::size_t i = 0; i < stepIndex; ++i)
  {
    double yNoStep = a_data.at(i).second;
    if (i >= stepIndex)
    {
      yNoStep -= stepHeight;
    }
    double stepShape = 0.5 + (1.0 / PI) *
          std::atan((a_data.at(i).first - a_stepLocation) / a_width);
    double yBroad = yNoStep + stepHeight * stepShape;
    broadenedData.emplace_back(a_data.at(i).first, yBroad);
  }
  for(std::size_t i = stepIndex; i < a_data.size(); ++i)
  {
    broadenedData.emplace_back(a_data.at(i).first, a_data.at(i).second);
  }
  return broadenedData;
}

std::vector<std::pair<double, double>> KMath::gaussian_broaden_table(
                                              const std::vector<std::pair<double, double>>& table,
                                              double A1,
                                              double A2,
                                              double A3,
                                              double n_sigma)
{
  std::vector<std::pair<double, double>> out;
  out.reserve(table.size());

  if (table.empty())
  {
    return table;
  }

  for (std::size_t j = 0; j < table.size(); ++j)
  {
    const double Ej = table[j].first;
    const double fwhm = calc_fwhm(Ej, A1, A2, A3);
    if (fwhm <= 0.0)
    {
      out.emplace_back(Ej, table[j].second);
      continue;
    }

    const double sigma = fwhm / (2.0 * std::sqrt(2.0 * std::log(2.0)));
    double weighted_sum = 0.0;
    double weight_sum = 0.0;
    
    for (std::size_t k = 0; k < table.size(); ++k)
    {
      const double Ek = table[k].first;
      const double dE = Ek - Ej;
      if (std::abs(dE) > n_sigma * sigma)
      {
        continue;
      }
      double deltaE = 1.0;
      if (table.size() == 1)
      {
        deltaE = 1.0;
      }
      else if (k == 0)
      {
        deltaE = table[1].first - table[0].first;
      }
      else if (k == table.size() - 1)
      {
        deltaE = table[k].first - table[k - 1].first;
      }
      else
      {
        deltaE = 0.5 * (table[k + 1].first - table[k - 1].first);
      }

      const double w = std::exp(-(dE * dE) / (2.0 * sigma * sigma)) * deltaE;
      weighted_sum += w * table[k].second;
      weight_sum += w;
    }

    const double broadened_mu = (weight_sum > 0.0) ? weighted_sum / weight_sum : table[j].second;
    out.emplace_back(Ej, broadened_mu);
  }

    return out;
}

double KMath::calc_fwhm(double a_energy, double a_A1, double a_A2, double a_A3)
{
  return std::sqrt(a_A1 + a_A2*a_A2 + a_A3/a_energy);
}

std::vector<std::pair<double, double>> KMath::empirical_mu_adjust(const std::vector<std::pair<double, double>>& a_table,
                                                                  const double a_l0,
                                                                  const double a_l1,
                                                                  const double a_l2,
                                                                  const double a_l3,
                                                                  const double a_h1,
                                                                  const double a_h2,
                                                                  const double a_h3,
                                                                  const double a_kedge)
{
  std::vector<std::pair<double, double>> adjData;
  double adjMu;
  for (std::size_t i = 0; i < a_table.size(); ++i)
  {
    if(a_table.at(i).first < a_kedge)
    { 
      adjMu = pow(10.0, a_l0*std::log10(a_table.at(i).first)*std::log10(a_table.at(i).first)*std::log10(a_table.at(i).first) +
                        a_l1*std::log10(a_table.at(i).first)*std::log10(a_table.at(i).first) +
                        a_l2*std::log10(a_table.at(i).first) +
                        a_l3);
      adjData.push_back(std::make_pair(a_table.at(i).first, adjMu));
    }
    else if(a_table.at(i).first > a_kedge)
    { 
      adjMu = pow(10.0, a_h1*std::log10(a_table.at(i).first)*std::log10(a_table.at(i).first) +
                        a_h2*std::log10(a_table.at(i).first) +
                        a_h3);
      adjData.push_back(std::make_pair(a_table.at(i).first, adjMu));
    }
    else
    {
      adjData.push_back(std::make_pair(a_table.at(i).first, a_table.at(i).second));
    }

    //if(i%10)
    //{
    //  std::cout << a_table.at(i).second << " " << adjData.at(i).second << std::endl;
    //}

  }
  return adjData;
}