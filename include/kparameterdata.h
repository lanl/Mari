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

#ifndef KPARAMETERDATA_H
#define KPARAMETERDATA_H

//External includes
#ifdef isinf
#undef isinf
#endif
#ifdef isnan
#undef isnan
#endif
#include <nlohmann/json.hpp>
//C++ includes
using json = nlohmann::json;
/**
 * Class to hold parameter set data
 **/
namespace parameter_data 
{
  struct KParameterData
  {
    //GENERAL tab
    std::string m_setName;
    std::string m_authorName;
    std::string m_facilityName;
    std::string m_date;
    std::string m_comment;
    
    //Inst tab design const subtab
    double m_generatorHVSetpoint;
    double m_beamIncidenceAngle;
    double m_takeoffAngle;
    double m_minBackscatterAngle;
    double m_beWindowThickness;
    double m_cdFilterThickness;
    double m_feFilterThickness;
    double m_xfrEntraceThickness;
    double m_xfrExitThickness;
    
    //Inst tab KED empirical const subtab
    double m_KEDCdCorr;
    double m_KEDBkgdNorm;
    double m_KEDTempCorr;
    
    //Inst tab XRF empirical const subtab
    double m_backScattC;
    double m_backScattb3;
    double m_backScattb4;
    double m_backScattb5;
    double m_backScattb6;
    double m_backScattb7;
    double m_backScattb8;
    double m_backScattb9;
    double m_backScattb10;
    double m_backScattb11;
    double m_backScattb12;
    double m_backScattb13;
    double m_backScattb14;

    //HPGE DET. tab
    double m_KEDA1;
    double m_KEDA2;
    double m_KEDA3;
    double m_KEDT1;
    double m_KEDT2;
    double m_KEDT3;
    double m_KEDT4;
    double m_XRFA1;
    double m_XRFA2;
    double m_XRFA3;
    double m_XRFT1;
    double m_XRFT2;
    double m_XRFT3;
    double m_XRFT4;
    
    //SAMPLE tab
    double m_vialID;
    double m_vialOD;
    int m_vialMaterial;
    int m_matrixMaterial;
    
    //KED ROI tab
    double m_KEDFitLowerBound;
    double m_KEDFitUpperBound;
    double m_KEDBkgdLowLowBound;
    double m_KEDBkgdLowHighBound;
    double m_KEDBkgdHighLowBound;
    double m_KEDBkgdHighHighBound;
    
    //XRF ROI tab
    double m_XRFFitLowerBound;
    double m_XRFFitUpperBound;
    double m_XRFBkgdLowLowBound;
    double m_XRFBkgdLowHighBound;
    double m_XRFBkgdHighLowBound;
    double m_XRFBkgdHighHighBound;

  };

  //Required for automatic conversion
  //Because the header is included in every translation unit that needs the 
  //conversion functions, marking them inline (or defining them in a source 
  //file and declaring them extern) prevents multiple‑definition errors.
  inline void from_json(const json& j, KParameterData& i) 
  {
    j.at("set_name").get_to(i.m_setName);
    j.at("author_name").get_to(i.m_authorName);
    j.at("facility_name").get_to(i.m_facilityName);
    j.at("date").get_to(i.m_date);
    j.at("comment").get_to(i.m_comment);
    j.at("gen_hv_setpoint").get_to(i.m_generatorHVSetpoint);
    j.at("incidence_angle").get_to(i.m_beamIncidenceAngle);
    j.at("takeoff_angle").get_to(i.m_takeoffAngle);
    j.at("backscatt_angle").get_to(i.m_minBackscatterAngle);
    j.at("be_window_dx").get_to(i.m_beWindowThickness);
    j.at("cd_filter_dx").get_to(i.m_cdFilterThickness);
    j.at("fe_filter_dx").get_to(i.m_feFilterThickness);
    j.at("xfr_entrance_dx").get_to(i.m_xfrEntraceThickness);
    j.at("xfr_exit_dx").get_to(i.m_xfrExitThickness);
    j.at("KED_A1").get_to(i.m_KEDA1);
    j.at("KED_A2").get_to(i.m_KEDA2);
    j.at("KED_A3").get_to(i.m_KEDA3);
    j.at("KED_T1").get_to(i.m_KEDT1);
    j.at("KED_T2").get_to(i.m_KEDT2);
    j.at("KED_T3").get_to(i.m_KEDT3);
    j.at("KED_T4").get_to(i.m_KEDT4);
    j.at("XRF_A1").get_to(i.m_XRFA1);
    j.at("XRF_A2").get_to(i.m_XRFA2);
    j.at("XRF_A3").get_to(i.m_XRFA3);
    j.at("XRF_T1").get_to(i.m_XRFT1);
    j.at("XRF_T2").get_to(i.m_XRFT2);
    j.at("XRF_T3").get_to(i.m_XRFT3);
    j.at("XRF_T4").get_to(i.m_XRFT4);
    j.at("vial_id").get_to(i.m_vialID);
    j.at("vial_od").get_to(i.m_vialOD);
    j.at("vial_material").get_to(i.m_vialMaterial);
    j.at("matrix_material").get_to(i.m_matrixMaterial);
    j.at("KED_fit_lower_bound").get_to(i.m_KEDFitLowerBound);
    j.at("KED_fit_upper_bound").get_to(i.m_KEDFitUpperBound);
    j.at("KED_Bkgd_Low_Low_Bound").get_to(i.m_KEDBkgdLowLowBound);
    j.at("KED_Bkgd_Low_High_Bound").get_to(i.m_KEDBkgdLowHighBound);
    j.at("KED_Bkgd_High_Low_Bound").get_to(i.m_KEDBkgdHighLowBound);
    j.at("KED_Bkgd_High_High_Bound").get_to(i.m_KEDBkgdHighHighBound);
    j.at("XRF_fit_lower_bound").get_to(i.m_XRFFitLowerBound);
    j.at("XRF_fit_upper_bound").get_to(i.m_XRFFitUpperBound);
    j.at("XRF_Bkgd_Low_Low_Bound").get_to(i.m_XRFBkgdLowLowBound);
    j.at("XRF_Bkgd_Low_High_Bound").get_to(i.m_XRFBkgdLowHighBound);
    j.at("XRF_Bkgd_High_Low_Bound").get_to(i.m_XRFBkgdHighLowBound);
    j.at("XRF_Bkgd_High_High_Bound").get_to(i.m_XRFBkgdHighHighBound);
    j.at("KED_Cd_Correction").get_to(i.m_KEDCdCorr);
    j.at("KED_Bkgd_Normalization").get_to(i.m_KEDBkgdNorm);
    j.at("KED_Temp_Correction").get_to(i.m_KEDTempCorr);
    j.at("XRF_brem_C").get_to(i.m_backScattC);
    j.at("XRF_brem_b3").get_to(i.m_backScattb3);
    j.at("XRF_brem_b4").get_to(i.m_backScattb4);
    j.at("XRF_brem_b5").get_to(i.m_backScattb5);
    j.at("XRF_brem_b6").get_to(i.m_backScattb6);
    j.at("XRF_brem_b7").get_to(i.m_backScattb7);
    j.at("XRF_brem_b8").get_to(i.m_backScattb8);
    j.at("XRF_brem_b9").get_to(i.m_backScattb9);
    j.at("XRF_brem_b10").get_to(i.m_backScattb10);
    j.at("XRF_brem_b11").get_to(i.m_backScattb11);
    j.at("XRF_brem_b12").get_to(i.m_backScattb12);
    j.at("XRF_brem_b13").get_to(i.m_backScattb13);
    j.at("XRF_brem_b14").get_to(i.m_backScattb14);
  }

  inline void to_json(nlohmann::json& j, const KParameterData& i) 
  {
    j = nlohmann::json{
                        {"set_name", i.m_setName},
                        {"author_name", i.m_authorName},
                        {"facility_name", i.m_facilityName},
                        {"date", i.m_date},
                        {"comment", i.m_comment},
                        {"gen_hv_setpoint", i.m_generatorHVSetpoint},
                        {"incidence_angle", i.m_beamIncidenceAngle},
                        {"takeoff_angle", i.m_takeoffAngle},
                        {"backscatt_angle", i.m_minBackscatterAngle},
                        {"be_window_dx", i.m_beWindowThickness},
                        {"cd_filter_dx", i.m_cdFilterThickness},
                        {"fe_filter_dx", i.m_feFilterThickness},
                        {"xfr_entrance_dx", i.m_xfrEntraceThickness},
                        {"xfr_exit_dx", i.m_xfrExitThickness},
                        {"KED_A1", i.m_KEDA1},
                        {"KED_A2", i.m_KEDA2},
                        {"KED_A3", i.m_KEDA3},
                        {"KED_T1", i.m_KEDT1},
                        {"KED_T2", i.m_KEDT2},
                        {"KED_T3", i.m_KEDT3},
                        {"KED_T4", i.m_KEDT4},
                        {"XRF_A1", i.m_XRFA1},
                        {"XRF_A2", i.m_XRFA2},
                        {"XRF_A3", i.m_XRFA3},
                        {"XRF_T1", i.m_XRFT1},
                        {"XRF_T2", i.m_XRFT2},
                        {"XRF_T3", i.m_XRFT3},
                        {"XRF_T4", i.m_XRFT4},
                        {"vial_id", i.m_vialID},
                        {"vial_od", i.m_vialOD},
                        {"vial_material", i.m_vialMaterial},
                        {"matrix_material", i.m_matrixMaterial},
                        {"KED_fit_lower_bound", i.m_KEDFitLowerBound},
                        {"KED_fit_upper_bound", i.m_KEDFitUpperBound},
                        {"KED_Bkgd_Low_Low_Bound", i.m_KEDBkgdLowLowBound},
                        {"KED_Bkgd_Low_High_Bound", i.m_KEDBkgdLowHighBound},
                        {"KED_Bkgd_High_Low_Bound", i.m_KEDBkgdHighLowBound},
                        {"KED_Bkgd_High_High_Bound", i.m_KEDBkgdHighHighBound},
                        {"XRF_fit_lower_bound", i.m_XRFFitLowerBound},
                        {"XRF_fit_upper_bound", i.m_XRFFitUpperBound},
                        {"XRF_Bkgd_Low_Low_Bound", i.m_XRFBkgdLowLowBound},
                        {"XRF_Bkgd_Low_High_Bound", i.m_XRFBkgdLowHighBound},
                        {"XRF_Bkgd_High_Low_Bound", i.m_XRFBkgdHighLowBound},
                        {"XRF_Bkgd_High_High_Bound", i.m_XRFBkgdHighHighBound},
                        {"KED_Cd_Correction", i.m_KEDCdCorr},
                        {"KED_Bkgd_Normalization", i.m_KEDBkgdNorm},
                        {"KED_Temp_Correction", i.m_KEDTempCorr},
                        {"XRF_brem_C", i.m_backScattC},
                        {"XRF_brem_b3", i.m_backScattb3},
                        {"XRF_brem_b4", i.m_backScattb4},
                        {"XRF_brem_b5", i.m_backScattb5},
                        {"XRF_brem_b6", i.m_backScattb6},
                        {"XRF_brem_b7", i.m_backScattb7},
                        {"XRF_brem_b8", i.m_backScattb8},
                        {"XRF_brem_b9", i.m_backScattb9},
                        {"XRF_brem_b10", i.m_backScattb10},
                        {"XRF_brem_b11", i.m_backScattb11},
                        {"XRF_brem_b12", i.m_backScattb12},
                        {"XRF_brem_b13", i.m_backScattb13},
                        {"XRF_brem_b14", i.m_backScattb14}
                      };
  }
}//end namespace

#endif 