// neural-net-tutorial.cpp
// David Miller, http://millermattson.com/dave
// See the associated video for instructions: http://vimeo.com/19569529


#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <jni.h>

using namespace std;

class MinMaxFeatureScaling;

class MinMaxFeatureScaling {
public:
    MinMaxFeatureScaling(const std::string path);
private:
    std::string fileName;
    std::string normalizeFileName;
    std::vector<double> minMax;
    inline std::string as_string(double value);
    std::ifstream m_loginFile;
    std::ofstream m_loginFileNormalized;
    void readFileMinMaxFeatureScaling();
    void getTrainingMinMax();
    void normalizeData();
};
inline string MinMaxFeatureScaling::as_string(double value) {
    char buf[32];
    return std::string(buf, std::snprintf(buf, sizeof buf, "%.16g", value));
};

void MinMaxFeatureScaling::readFileMinMaxFeatureScaling() {
    ifstream m_minMaxFile;

    m_minMaxFile.open(normalizeFileName.c_str());

    while (!m_minMaxFile.eof()) {
        string line;
        getline(m_minMaxFile, line);

        stringstream ss(line);

        double oneValue;
        while (ss >> oneValue) {
            minMax.push_back(oneValue);
        }
    }
}

void MinMaxFeatureScaling::normalizeData() {
    m_loginFile.open(fileName.c_str());

    vector<string> fileData;
    vector<string> newFileData;
    vector<double> totalTime;
    vector<double> sumTimePerTrajectory;
    vector<double> fingersUp;
    vector<double> sumTrajectoryInX;
    vector<double> sumTrajectoryInY;
    vector<double> sumTrajectoryOutX;
    vector<double> sumTrajectoryOutY;

    while (!m_loginFile.eof()) {
        string line;
        getline(m_loginFile, line);

        stringstream ss(line);
        string newLine;

        ss >> newLine;

        int valuesCount = 1;
        double oneValue;
        while (ss >> oneValue) {
            if (valuesCount < 7) {
                newLine += " " + as_string(oneValue);
            }
            else {
                switch (valuesCount)
                {
                    case 7:
                        totalTime.push_back(oneValue);
                        break;
                    case 8:
                        sumTimePerTrajectory.push_back(oneValue);
                        break;
                    case 9:
                        fingersUp.push_back(oneValue);
                        break;
                    case 10:
                        sumTrajectoryInX.push_back(oneValue);
                        break;
                    case 11:
                        sumTrajectoryInY.push_back(oneValue);
                        break;
                    case 12:
                        sumTrajectoryOutX.push_back(oneValue);
                        break;
                    case 13:
                        sumTrajectoryOutY.push_back(oneValue);
                        break;
                }
            }

            ++valuesCount;
        }
        newFileData.push_back(newLine);
    }
    m_loginFile.close();

    double max = minMax[0];
    double min = minMax[1];
    for (int i = 0; i < totalTime.size(); ++i) {
        totalTime[i] = (totalTime[i] - min) / (max - min);
    }

    max = minMax[2];
    min = minMax[3];
    for (int i = 0; i < sumTimePerTrajectory.size(); ++i) {
        sumTimePerTrajectory[i] = (sumTimePerTrajectory[i] - min) / (max - min);
    }

    max = minMax[4];
    min = minMax[5];
    for (int i = 0; i < fingersUp.size(); ++i) {
        fingersUp[i] = (fingersUp[i] - min) / (max - min);
    }

    max = minMax[6];
    min = minMax[7];
    for (int i = 0; i < sumTrajectoryInX.size(); ++i) {
        sumTrajectoryInX[i] = (sumTrajectoryInX[i] - min) / (max - min);
    }

    max = minMax[8];
    min = minMax[9];
    for (int i = 0; i < sumTrajectoryInY.size(); ++i) {
        sumTrajectoryInY[i] = (sumTrajectoryInY[i] - min) / (max - min);
    }

    max = minMax[10];
    min = minMax[11];
    for (int i = 0; i < sumTrajectoryOutX.size(); ++i) {
        sumTrajectoryOutX[i] = (sumTrajectoryOutX[i] - min) / (max - min);
    }

    max = minMax[12];
    min = minMax[13];
    for (int i = 0; i < sumTrajectoryOutY.size(); ++i) {
        sumTrajectoryOutY[i] = (sumTrajectoryOutY[i] - min) / (max - min);
    }


    for (int i = 0; i < totalTime.size() * 2; i += 2) {
        string newLine = "";
        if (i > 0)
            newLine += " " + as_string(totalTime[i / 2]) + " " + as_string(sumTimePerTrajectory[i / 2]) + " " + as_string(fingersUp[i / 2]) + " " +
                       as_string(sumTrajectoryInX[i / 2]) + " " + as_string(sumTrajectoryInY[i / 2]) + " " + as_string(sumTrajectoryOutX[i / 2]) + " " + as_string(sumTrajectoryOutY[i / 2]);
        else
            newLine += " " + as_string(totalTime[0]) + " " + as_string(sumTimePerTrajectory[0]) + " " + as_string(fingersUp[0]) + " " +
                       as_string(sumTrajectoryInX[0]) + " " + as_string(sumTrajectoryInY[0]) + " " + as_string(sumTrajectoryOutX[0]) + " " + as_string(sumTrajectoryOutY[0]);
        newFileData[i] += newLine;
    }


    string FileData = "";
    for (int i = 0; i < newFileData.size(); i++)
        FileData += newFileData[i] + "\n";


    m_loginFileNormalized.open(fileName.c_str(), fstream::in | fstream::out | fstream::trunc);

    m_loginFileNormalized << FileData;

    m_loginFileNormalized.close();
}

MinMaxFeatureScaling::MinMaxFeatureScaling(const string path){
    fileName = path + "login.txt";
    normalizeFileName = path + "minMaxFile.txt";
    readFileMinMaxFeatureScaling();

    normalizeData();
}


// Silly class to read training data from a text file -- Replace This.
// Replace class TrainingData with whatever you need to get input data into the
// program, e.g., connect to a database, or take a stream of data from stdin, or
// from a file specified by a command line argument, etc.

class TrainingData
{
public:
    TrainingData(const string filename);
    bool isEof(void) { return m_trainingDataFile.eof(); }
    vector<vector<double>> readWeightFile(string path);

    // Returns the number of input values read from the file:
    unsigned getNextInputs(vector<double> &inputVals);
    unsigned getTargetOutputs(vector<double> &targetOutputVals);

private:
    ifstream m_trainingDataFile;
};

TrainingData::TrainingData(const string filename)
{
    m_trainingDataFile.open(filename.c_str());
}

unsigned TrainingData::getNextInputs(vector<double> &inputVals)
{
    inputVals.clear();

    string line;
    getline(m_trainingDataFile, line);
    stringstream ss(line);

    string label;
    ss>> label;
    if (label.compare("in:") == 0) {
        double oneValue;
        while (ss >> oneValue) {
            inputVals.push_back(oneValue);
        }
    }

    return inputVals.size();
}

unsigned TrainingData::getTargetOutputs(vector<double> &targetOutputVals)
{
    targetOutputVals.clear();

    string line;
    getline(m_trainingDataFile, line);
    stringstream ss(line);

    string label;
    ss>> label;
    if (label.compare("out:") == 0) {
        double oneValue;
        while (ss >> oneValue) {
            targetOutputVals.push_back(oneValue);
        }
    }

    return targetOutputVals.size();
}


vector<vector<double>> TrainingData::readWeightFile(string path) {
    //Basically read the file of the weights and return a vector of vector of double
    //Where vector[0][n] is the topology
    //  and the vector[1][n] is the weights
    vector<vector<double>> weight;
    string fileName = path + "bestWeight.txt";

    ifstream m_weightFile;
    m_weightFile.open(fileName.c_str());

    string line;
    getline(m_weightFile, line);
    stringstream ss(line);

    string top;
    ss >> top;
    double oneValue;
    vector<double> topology;
    while (ss >> oneValue) {
        topology.push_back(oneValue);
    }
    weight.push_back(topology);


    getline(m_weightFile, line);
    stringstream ss2(line);
    vector<double> weightsvector;
    while (ss2 >> oneValue) {
        weightsvector.push_back(oneValue);
    }
    weight.push_back(weightsvector);
    return weight;
}

struct Connection
{
    double weight;
    double deltaWeight;
};


class Neuron;

typedef vector<Neuron> Layer;

// ****************** class Neuron ******************
class Neuron
{
public:
    Neuron(unsigned numOutputs, unsigned myIndex);
    void setOutputVal(double val) { m_outputVal = val; }
    double getOutputVal(void) const { return m_outputVal; }
    void feedForward(const Layer &prevLayer);
    void calcOutputGradients(double targetVal);
    void calcHiddenGradients(const Layer &nextLayer);
    void updateInputWeights(Layer &prevLayer);
    double getWeight(int index) { return m_outputWeights[index].weight; };
    void setWeight(int index, double val) { m_outputWeights[index].weight = val; };

private:
    static double eta;   // [0.0..1.0] overall net training rate
    static double alpha; // [0.0..n] multiplier of last weight change (momentum)
    static double transferFunction(double x);
    static double transferFunctionDerivative(double x);
    static double randomWeight(void) { return rand() / double(RAND_MAX); }
    double sumDOW(const Layer &nextLayer) const;
    double m_outputVal;
    double m_inputVal;
    vector<Connection> m_outputWeights;
    unsigned m_myIndex;
    double m_gradient;
};

double Neuron::eta = 0.4;    // overall net learning rate, [0.0..1.0]
double Neuron::alpha = 0.0;   // momentum, multiplier of last deltaWeight, [0.0..1.0]


void Neuron::updateInputWeights(Layer &prevLayer)
{
    // The weights to be updated are in the Connection container
    // in the neurons in the preceding layer

    for (unsigned n = 0; n < prevLayer.size(); ++n) {
        Neuron &neuron = prevLayer[n];
        double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

        double newDeltaWeight =
                // Individual input, magnified by the gradient and train rate:
                eta
                * neuron.getOutputVal()
                * m_gradient
                // Also add momentum = a fraction of the previous delta weight;
                + alpha
                  * oldDeltaWeight;

        neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
        neuron.m_outputWeights[m_myIndex].weight -= newDeltaWeight;
    }
}

double Neuron::sumDOW(const Layer &nextLayer) const
{
    double sum = 0.0;

    // Sum our contributions of the errors at the nodes we feed.

    for (unsigned n = 0; n < nextLayer.size() - 1; ++n) {
        sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
    }

    return sum;
}

void Neuron::calcHiddenGradients(const Layer& nextLayer)
{
    double dow = sumDOW(nextLayer);
    m_gradient = dow * Neuron::transferFunctionDerivative(m_inputVal);
}

void Neuron::calcOutputGradients(double targetVals)
{
    double delta = -(targetVals - m_outputVal);
    m_gradient = delta * Neuron::transferFunctionDerivative(m_inputVal);
}

double Neuron::transferFunction(double x)
{
    // sigmoid
    // 1 / (1 + e^-x)
    return 1 / (1 + exp(-x));
}

double Neuron::transferFunctionDerivative(double x)
{
    // tanh derivative
    //return 1.0 - tanh(x) * tanh(x);
    // sigmoid derivative
    // sigmoid(x)[1-sigmoid(x)]
    return transferFunction(x) * (1 - transferFunction(x));
}

void Neuron::feedForward(const Layer &prevLayer)
{
    double sum = 0.0;

    // Sum the previous layer's outputs (which are our inputs)
    // Include the bias node from the previous layer.

    for (unsigned n = 0; n < prevLayer.size(); ++n) {
        sum += prevLayer[n].getOutputVal() *
               prevLayer[n].m_outputWeights[m_myIndex].weight;
    }

    m_inputVal = sum;
    m_outputVal = Neuron::transferFunction(sum);
}

Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
    for (unsigned c = 0; c < numOutputs; ++c) {
        m_outputWeights.push_back(Connection());
        m_outputWeights.back().weight = randomWeight();
    }

    m_myIndex = myIndex;
}


// ****************** class Net ******************
class Net
{
public:
    Net(const vector<double> &topology);
    vector<double> saveWeight();
    void feedForward(const vector<double> &inputVals);
    void backProp(const vector<double> &targetVals);
    void getResults(vector<double> &resultVals) const;
    void resetRecentAverageError(void) { m_recentAverageError = 0; }
    double getRecentAverageError(void) const { return m_recentAverageError; }
    void setWeightNet(vector<double> weights);

private:
    vector<Layer> m_layers; // m_layers[layerNum][neuronNum]
    double m_error;
    double m_recentAverageError;
    static double m_recentAverageSmoothingFactor;
};

double Net::m_recentAverageSmoothingFactor = 300.0; // Number of training samples to average over


void Net::setWeightNet(vector<double> weights) {
    //Run thought the the network and set the weights
    int w = 0;
    for (unsigned layerNum = 0; layerNum < m_layers.size() - 1; ++layerNum) {
        for (unsigned neuronNum = 0; neuronNum < m_layers[layerNum].size(); ++neuronNum) {
            for (unsigned n = 0; n < m_layers[layerNum + 1].size() - 1; ++n) {
                m_layers[layerNum][neuronNum].setWeight(n, weights[w]);
                ++w;
            }
        }
    }
}

void Net::getResults(vector<double> &resultVals) const
{
    resultVals.clear();

    for (unsigned n = 0; n < m_layers.back().size() - 1; ++n) {
        resultVals.push_back(m_layers.back()[n].getOutputVal());
    }
}

void Net::backProp(const vector<double> &targetVals)
{
    // Calculate overall net error (RMS of output neuron errors)

    Layer &outputLayer = m_layers.back();
    m_error = 0.0;

    for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
        double delta = targetVals[n] - outputLayer[n].getOutputVal();
        m_error += delta * delta;
    }
    m_error /= outputLayer.size() - 1; // get average error squared
    m_error = sqrt(m_error); // RMS

    // Implement a recent average measurement

    m_recentAverageError =
            (m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
            / (m_recentAverageSmoothingFactor + 1.0);

    // Calculate output layer gradients

    for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
        outputLayer[n].calcOutputGradients(targetVals[n]);
    }

    // Calculate hidden layer gradients

    for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum) {
        Layer &hiddenLayer = m_layers[layerNum];
        Layer &nextLayer = m_layers[layerNum + 1];

        for (unsigned n = 0; n < hiddenLayer.size(); ++n) {
            hiddenLayer[n].calcHiddenGradients(nextLayer);
        }
    }

    // For all layers from outputs to first hidden layer,
    // update connection weights

    for (unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
        Layer &layer = m_layers[layerNum];
        Layer &prevLayer = m_layers[layerNum - 1];

        for (unsigned n = 0; n < layer.size() - 1; ++n) {
            layer[n].updateInputWeights(prevLayer);
        }
    }
}

void Net::feedForward(const vector<double> &inputVals)
{
    assert(inputVals.size() == m_layers[0].size() - 1);

    // Assign (latch) the input values into the input neurons
    for (unsigned i = 0; i < inputVals.size(); ++i) {
        m_layers[0][i].setOutputVal(inputVals[i]);
    }

    // forward propagate
    for (unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
        Layer &prevLayer = m_layers[layerNum - 1];
        for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
            m_layers[layerNum][n].feedForward(prevLayer);
        }
    }
}

Net::Net(const vector<double> &topology)
{
    unsigned numLayers = topology.size();
    for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum) {
        m_layers.push_back(Layer());
        unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];

        // We have a new layer, now fill it with neurons, and
        // add a bias neuron in each layer.
        for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {
            m_layers.back().push_back(Neuron(numOutputs, neuronNum));
            cout << "Made a Neuron!" << endl;
        }

        // Force the bias node's output to 1.0 (it was the last neuron pushed in this layer):
        m_layers.back().back().setOutputVal(1.0);
    }
}

vector<double> Net::saveWeight()
{
    vector<double> weights;
    for (unsigned layerNum = 0; layerNum < m_layers.size() - 1; ++layerNum) {
        for (unsigned neuronNum = 0; neuronNum < m_layers[layerNum].size(); ++neuronNum) {
            for (unsigned n = 0; n < m_layers[layerNum + 1].size() - 1; ++n) {
                weights.push_back(m_layers[layerNum][neuronNum].getWeight(n));
            }
        }
    }
    return weights;
}


void showVectorVals(string label, vector<double>& v)
{
    cout << label << " ";
    for (unsigned i = 0; i < v.size(); ++i) {
        cout << v[i] << " ";
    }

    cout << endl;
}

void saveWeightFile(vector<double> weights, vector<unsigned> topology) {

    string FileData = "";
    //Get all the weights and save them into a file
    FileData += "topology: ";
    for (int i = 0; i < topology.size(); ++i)
        FileData += to_string(topology[i]) + " ";

    FileData += "\n";
    for (int n = 0; n < weights.size(); ++n)
        FileData += to_string(weights[n]) + " ";

    ofstream m_minMaxFile;

    m_minMaxFile.open("DataSet\\bestWeight.txt", fstream::in | fstream::out | fstream::trunc);

    m_minMaxFile << FileData;

    m_minMaxFile.close();
}

double Login(string path){
    MinMaxFeatureScaling minMaxFeatureScaling(path);

    TrainingData trainData(path + "login.txt");

    // e.g., { 3, 2, 1 }
    vector<double> topology;
    vector<double> finalWeights;
    //Read the weights
    //This file have to be put manually in your device.
    vector<vector<double>> weights = trainData.readWeightFile(path);

    //Obtain the optimal topology that the weight file contains and define the actual neural network topology
    topology = weights[0];
    Net myNet(topology);
    myNet.setWeightNet(weights[1]);
    vector<double> inputVals, targetVals, resultVals;

    // Get new input data and feed it forward:
    if (trainData.getNextInputs(inputVals) != topology[0]) {
        return false;
    }
    //showVectorVals("Inputs:", inputVals);
    myNet.feedForward(inputVals);

    // Collect the net's actual output results:
    myNet.getResults(resultVals);
    //showVectorVals("Outputs:", resultVals);

    return resultVals[0];
}

std::string jstring2string(JNIEnv *env, jstring jStr) {
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char *)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

extern "C"
JNIEXPORT jint Java_com_example_assinatura_LoginActivity_InitiateLogin(JNIEnv* env, jobject obj, jstring path) {

    //Call the testMethod
    double result = Login(jstring2string(env, path));
    jint jresult = 0;
    if(result > 0.5) {
        jresult = 1;
    }
    return jresult;
}