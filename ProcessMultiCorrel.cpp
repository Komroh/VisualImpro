/**
 *  \file    ProcessMultiCorrel.cpp
 *  \author  Jérémy LIXANDRE
 *  \date    July 2017
 *
 *  \brief Processing object composed by function pointers members.
 *
 *  Here are the constructor and all the process functions used in our program.
 *  The render.cpp file will call the process function, as an auxiliary task
 *  accomplished by another thread than the master thread, every time the buffer
 *  is filled with signals of length given in parameter of the program.
 *  The process function will call sequentially every processing function one
 *  by one.
 *
 */

#include <ctime>
#include <iomanip>
#include <sys/time.h>
#include <AuxTaskRT.h>
#include "Matrix.hpp"
#include "ProcessMultiCorrel.hpp"

struct timeval tv10;
struct timeval tv20;
struct timeval tv11;
struct timeval tv12;
struct timeval tv13;
struct timeval tv14;
struct timeval tv15;


ProcessMultiCorrel::ProcessMultiCorrel(
    vector<float> (*coeffcorrel)(const vector<float>&, const vector<float>&),
    RGB (*colorscale)(float),
    Matrix<float> (*preproc)(const Matrix<float>&),
    vector<float> (*mixLevel)(const Matrix<float>&))
    : _coeffcorrel(coeffcorrel), _colorscale(colorscale), _preprocess(preproc),
    _mixLevel(mixLevel) { matrixfile.open(string("log/log"), ios::app); }

/**
 * Destructor for the ProcessMultiCorrel object. It will also close the
 * logfile called matrixfile.
 */
 
ProcessMultiCorrel::~ProcessMultiCorrel() { matrixfile.close(); }

/**
 * Correlation process function.
 */
 
Matrix<vector<float>> ProcessMultiCorrel::calcul_correl(const Matrix<float>& buffer){

  int size = buffer.getSize();
  Matrix<vector<float>> correlMatrix(size);
  vector<float> coeffcorrel;
	
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < i; j++) {
    	
      coeffcorrel = this->_coeffcorrel(buffer.getRow(i), buffer.getRow(j));
      correlMatrix.setCase(i, j, coeffcorrel);
      correlMatrix.setCase(j, i, coeffcorrel);
      //matrixfile << to_string(coeffcorrel) << " ";
    }
  }
  matrixfile << endl;
  return correlMatrix;
}

/**
 * Mix volume process function.
 */
 
void ProcessMultiCorrel::process_volume(const Matrix<float>& correlMatrix,
                                        vector<float>& meanCorrelations){
  meanCorrelations = this->_mixLevel(correlMatrix);
}

/**
 * Coloration process function.
 */
 
Matrix<RGB> ProcessMultiCorrel::color_matrix(const Matrix<float>& correlMatrix){
  int size = correlMatrix.getSize();
  Matrix<RGB> RGBmatrix(size);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < i; j++) {
      RGB color = this->_colorscale(correlMatrix.getCase(i, j));
      RGBmatrix.setCase(i, j, color);
      RGBmatrix.setCase(j, i, color);
    }
  }
  return RGBmatrix;
}

/**
 * Calling every processing function sequentially.
 */

string toJson(Matrix<vector<float>> mat)
{
	JSONObject root;
	JSONArray array;
	int size = mat.getSize();
	//cout << size << endl;
	for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
    	JSONArray arr;
    	for(float l : mat.getCase(i,j))
			arr.push_back(new JSONValue(l));
		//vector<float> vec = mat.getCase(i,j);
		array.push_back(new JSONValue(arr));
		}
	}
	
	root[L"Tensor"] = new JSONValue(array);
	JSONValue *value = new JSONValue(root);
	std::wstring wide = value->Stringify().c_str();
	std::string str( wide.begin(), wide.end() );
	delete value;
	return str;
}

void ProcessMultiCorrel::process(const Matrix<float>& buffer,
                                 vector<float>& meanCorrelations,
                                 Connection conn){
  Matrix<float> copy = buffer;

  // Processing functions
  copy = _preprocess(buffer);
  Matrix<vector<float>> correlMatrix = calcul_correl(copy);
  //process_volume(correlMatrix, meanCorrelations);
  //Matrix<RGB> mat = color_matrix(correlMatrix);

  // Send data
  string str = toJson(correlMatrix);
  send_task =  std::unique_ptr<AuxTaskRT>(new AuxTaskRT());
  send_task->create("send-task", [conn, str]() mutable {conn.send(str);},75);
  send_task->schedule();
}
