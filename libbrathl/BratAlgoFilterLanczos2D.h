/*
* 
*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#if !defined(_BratAlgoFilterLanczos2D_h_)
#define _BratAlgoFilterLanczos2D_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "Tools.h" 
#include "BratAlgoFilterLanczos.h" 


using namespace brathl;

namespace brathl
{
//-------------------------------------------------------------
//------------------- CBratAlgoFilterLanczos2D class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgoFilterLanczos2D : public CBratAlgoFilterLanczos 
{
public:
  /** Default contructor */
  CBratAlgoFilterLanczos2D();
  /** Copy contructor */
  CBratAlgoFilterLanczos2D(const CBratAlgoFilterLanczos2D	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterLanczos2D();

  virtual std::string GetName() const override { return "BratAlgoFilterLanczosGrid"; }

  virtual std::string GetDescription() const override { return "Lanczos filter for two-dimensional data (e.g. gridded data) as input data source. "
                                           "Lanczos filter is windowed filter. The Lanczos window is the central lobe of a horizontally-stretched sinc, sinc(X/a) for -a <= X <= a, with (2 x a) = window length."
                                           "Thus the Lanczos filter, on its interval, is a product of two sinc functions. The resulting function is then used as a convolution kernel to resample the input field. "
                                           "The output value unit depends on the variable (data) filtered" ; }

  virtual double Run(CVectorBratAlgorithmParam& args) override;


    /** Overloads operator '=' */
  CBratAlgoFilterLanczos2D& operator=(const CBratAlgoFilterLanczos2D &copy);

  virtual uint32_t GetDataWindowSize() override { return m_dataWindowLength * m_dataWindowLength; }

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr) override;
 

protected:

  void Init();
  void Set(const CBratAlgoFilterLanczos2D &copy);

  virtual void OpenProductFile() override;
  void CheckProduct();
  void CheckVarExpression(uint32_t index) override;

  virtual void SetBeginOfFile() override;
  virtual void SetEndOfFile() override;
  
  double ComputeSingle();
  double ComputeMean();

  virtual double ComputeLanczos(CExpressionValue& exprValue);



protected:


};


/** @} */
}


#endif // !defined(_BratAlgoFilterLanczos2D_h_)
